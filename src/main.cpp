
#include "webserver.hpp"

AsyncWebServer 		server(SERVER_PORT);
AsyncWebSocket		sockets("/web_sockets");
AsyncEventSource		events("/web_events");
DNSServer			dns_server;
LiquidCrystal_I2C	lcd(LCD_ADDRESS, LCD_COL_COUNT, LCD_ROW_COUNT); // set lcd address to 0x27 for 16x2 chars
rfid_t 				rfid_reader(RFID_RC522_PIN_SS, RFID_RC522_PIN_RST);
rfid_key_t			rfid_key;
user_list 			users;
message_queue 		sent_messages;
message_queue		unsent_messages;
unsigned long 		start_time				= millis();
string_t 			ssid					= "esp8266->192.168.4.1";
string_t 			pass					= "";
string_t 			domain_name				= "esp8266";
string_t 			messages_file			= "/messages.json";
uint8_t				cursor_pos 				= 0;
size_t				message_queue::counter 	= 0;
ip_address_t			ap_ip(192, 168, 4, 1);

void setup() {
	// Read config file and parse contents

	// Initialize LCD and turn on backlight
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.autoscroll();
	lcd.leftToRight();

	// Initialize access point
	Serial.begin(BAUD_RATE);
	Serial.println(domain_name + ": setting up access point...");
	WiFi.mode(WIFI_AP);
	// WiFi.softAPConfig(ap_ip, ap_ip, ip_address_t(255, 255, 255, 0), ap_ip);
	WiFi.config(ap_ip, ap_ip, ip_address_t(255, 255, 255, 0), ip_address_t(8,8,8,8));

	(pass) ? WiFi.softAP(ssid) : WiFi.softAP(ssid, pass);
	WiFi.hostname(domain_name);
	
	if(WiFi.status() != WL_CONNECTED){
		Serial.println("Could not connect to Wi-Fi access point");
	}

	// Initialize SPI and rfid
	SPI.begin();
	rfid::init(rfid_reader);
	
	// Print the ESP8266 local ip address
	Serial.print("ip address: \nsoftap: " + WiFi.softAPIP().toString() + "\ndns ip: ");
	Serial.println(WiFi.dnsIP().toString());


	// Set up mDNS server
	if(MDNS.begin("towk")){
		Serial.println("mDNS started successfully");
	}

	// Set DNS server params
	// dns_server.setTTL(300);
	// dns_server.setErrorReplyCode(DNSReplyCode::ServerFailure);
	// dns_server.start(DNS_PORT, DOMAIN_NAME, ip_address_t(8,8,8,8));

	// Initialize LittleFS
	if (!LittleFS.begin()) {
		Serial.println(F("An error has occurred mounting LittleFS"));
		return;
	}

	string_t json = load_file(messages_file.c_str());
	if (!json.isEmpty()){
		DynamicJsonDocument doc(1024);
		deserializeJson(doc, json);
		JsonArray messages = doc["messages"];
		for(JsonVariant v : messages){
			JsonObject s 	= v["sender"];
			JsonArray i 	= s["ip_addr"];
			user_t sender{
				.client_id	= s["client_id"],
				.name 		= s["name"],
				.ip 		= { i[0], i[1], i[2], i[3] },
				.mac		= 0
			};
			message_t message{
				.id 		= s["id"],
				.sender 	= sender,
				.contents 	= s["contents"],
				.timestamp	= s["timestamp"]
			};
		}
	}

	// Route for root/web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(LittleFS, "/index.html", "text/html");
	});

	server.on("/functions.js", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(LittleFS, "/functions.js", "text/javascript");
	});

	server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(LittleFS, "/styles.css", "text/css");
	});

	server.on("/background.css", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(LittleFS, "/background.css", "text/css");
	});

	server.onNotFound([](AsyncWebServerRequest *request){
		request->send(404, "text/plain", "Error 404: Page not found!");
		AsyncWebServerResponse *response = request->beginResponse(302);
		response->addHeader("Location", "http://192.168.4.1/");
		request->send(response);
	});

	// basic http authentication
	// events.setAuthentication("user", "pass");
	sockets.onEvent(web_server::on_event);
	server.addHandler(&sockets);
	server.addHandler(&events);
	server.begin();
}


void loop() {
	// Read all available characters
	if(Serial.available() > 0){
		// Display characters on LCD
		int ch = Serial.read();

		// 'enter' key press event ch==0x0A
		if(ch == 0x0A){
			lcd.clear();
			lcd.setCursor(0, 0);
		} 
		else
			lcd.write(ch);
	}
	
	// MDNS.update();
	dns_server.processNextRequest();
	rfid::update(rfid_reader);// show the UID and PICC type when scanning and send a message to clients
	sockets.cleanupClients();
	delay(1000);
}