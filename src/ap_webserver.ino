// TODO: Make communication through microphone and speaker
// TODO: Replace "Submit" with asynchronous AJAX calls (DONE!)
// TODO: Send minimal data when make a request from server
// TODO: Use sockets to only push data to clients instead of constant polling
#include "include/message.h"
#include "include/rfid.h"
#include "include/utils.h"


//WiFiServer web_server(SERVER_PORT);
AsyncWebServer 			web_server(SERVER_PORT);
AsyncWebSocket			web_sockets("/sockets");
AsyncEventSource		web_events("/web_events");
DNSServer				dns_server;
LiquidCrystal_I2C		lcd(LCD_ADDRESS, LCD_COL_COUNT, LCD_ROW_COUNT); // set lcd address to 0x27 for 16x2 chars
rfid_t 					rfid_reader(RFID_RC522_PIN_SS, RFID_RC522_PIN_RST);
rfid_key_t				rfid_key;
// user_list 			users;
message_queue 			sent_messages;
message_queue			unsent_messages;
unsigned long 			start_time	= millis();
constexpr const char 	ssid[]	= "david's esp8266 192.168.4.1";
constexpr const char 	pass[]	= "";
uint8_t					cursor_pos = 0;


void setup() {
	// Initialize LCD and turn on backlight
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.autoscroll();
	lcd.leftToRight();

	// Initialize access point
	Serial.begin(BAUD_RATE);
	Serial.println(F("Setting up access point..."));
	(pass) ? WiFi.softAP(ssid) : WiFi.softAP(ssid, pass);
	WiFi.hostname(DOMAIN_NAME);

	ip_address_t ip_addr = WiFi.softAPIP();
	//ip_address_t dns(192, 168, 4, 1);
	//ip_address_t gateway(192, 168, 1, 1);
	//ip_address_t subnet(255, 255, 0, 0);
	Serial.println("ip address: " + ip_addr.toString());

	// Print the ESP8266 local ip address
	Serial.println(WiFi.localIP());

	// Set DNS server params
	dns_server.setTTL(300);
	dns_server.setErrorReplyCode(DNSReplyCode::ServerFailure);
	// dns_server.start(DNS_PORT, DOMAIN_NAME, WiFi.localIP());
	dns_server.start(DNS_PORT, "*", WiFi.softAPIP());

	// Initialize LittleFS
	if (!LittleFS.begin()) {
		Serial.println(F("An error has occurred mounting SPIFFS"));
		return;
	}

	web_events.onConnect([](AsyncEventSourceClient *client){
		if(client->lastId()){
			Serial.printf("Client connected. Last message ID: %u", client->lastId());
		}
		client->send("hello", NULL, millis(), 1000);
	});

	// basic http authentication
	// web_events.setAuthentication("user", "pass");
	web_server.addHandler(&web_events);

	// Route for root/web page
	web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(LittleFS, "/index.html", "text/html");
	});

	web_server.on("/functions.js", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(LittleFS, "/functions.js", "text/javascript");
	});

	web_server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(LittleFS, "/styles.css", "text/css");
	});

	// Load "sent_messages.html" to webpage
	web_server.on("/load_messages", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(LittleFS, MESSAGES_FILE, "application/json");
	});

	// Send message from user to web_server and send "sent_messages.html" back
	web_server.on("/send_message", HTTP_POST, [&sent_messages](AsyncWebServerRequest *request) {
		message_t	message;

		// Get and process incoming message value on <ESP_IP>/send
		if (request->hasParam(PARAM_USER_MESSAGE, true) && request->hasParam(PARAM_USER_NAME, true)) {
			Serial.println(F("Processing POST request..."));
			message.sender.name		= request->getParam(PARAM_USER_NAME, true)->value();
			message.sender.ip_addr	= request->client()->remoteIP();
			message.contents		= request->getParam(PARAM_USER_MESSAGE, true)->value();
			message.timestamp		= request->getParam(PARAM_TIMESTAMP, true)->value();

			// Get user from list and add message to message list
			if (!message.sender.name.isEmpty() && !message.contents.isEmpty()){
				Serial.println(F("Adding message to list..."));
				Serial.println("message.sender.name="+message.sender.name);
				Serial.println("message.sender.ip_addr="+message.sender.ip_addr.toString());
				Serial.println("message.contents='"+message.contents+"'");
				Serial.println("message.timestamp="+message.timestamp);
				unsent_messages.add_message(message);
			}
		}

		// Save message list HTML to file using LittleFS
		Serial.println(F("Saving sent message list to file system..."));
		save_to_file(MESSAGES_FILE, sent_messages.to_json());
		request->send(200, "OK", "text/plain");
  	});
	
	// WARNING: Do not request more often than 3-5 seconds!
	web_server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
		string_t json = "[";
		int n = WiFi.scanComplete();
		if(n == -2){
			WiFi.scanNetworks(true);
		} else if(n){
			for(int i = 0; i < n; ++i){
				if(i)
					json += ",";
				json += "{";
				json += "\"rssi\":"+String(WiFi.RSSI(i));
				json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
				json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
				json += ",\"channel\":"+String(WiFi.channel(i));
				json += ",\"secure\":"+String(WiFi.encryptionType(i));
				json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
				json += "}";
			}
			WiFi.scanDelete();
			if(WiFi.scanComplete() == -2){
				WiFi.scanNetworks(true);
			}
		}
		json += "]";
		request->send(200, "application/json", json);
		json = string_t();
	});

	web_server.onNotFound([](AsyncWebServerRequest *request){
		request->send(404, "text/plain", "Error 404: Page not found!");
	});
	web_server.begin();
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

	if(unsent_messages.has_messages()){
		string_t json = unsent_messages.to_json();
		web_events.send(json.c_str(), "update_message");
		sent_messages.add_messages(unsent_messages);
		unsent_messages.clear();
	}
	
	// MDNS.update();
	dns_server.processNextRequest();
	rfid_update(rfid_reader);
	web_sockets.cleanupClients();
	delay(1000);
}
