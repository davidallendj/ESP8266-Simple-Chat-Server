#include "include/message.h"
#include "include/rfid.h"
#include "include/utils.h"


//WiFiServer web_server(SERVER_PORT);
AsyncWebServer 			web_server(SERVER_PORT);
AsyncWebSocket			web_sockets("/web_sockets");
AsyncEventSource		web_events("/web_events");
DNSServer				dns_server;
LiquidCrystal_I2C		lcd(LCD_ADDRESS, LCD_COL_COUNT, LCD_ROW_COUNT); // set lcd address to 0x27 for 16x2 chars
rfid_t 					rfid_reader(RFID_RC522_PIN_SS, RFID_RC522_PIN_RST);
rfid_key_t				rfid_key;
user_list 				users;
message_queue 			sent_messages;
message_queue			unsent_messages;
unsigned long 			start_time	= millis();
constexpr const char 	ssid[]	= "esp8266 192.168.4.1";
constexpr const char 	pass[]	= "";
uint8_t					cursor_pos = 0;
size_t					message_queue::counter = 0;
ip_address_t			ap_ip(192, 168, 4, 1);


// Use web sockets to detect when client connects/disconnects
void on_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){
		Serial.printf("%s: client %u connected to server established!\n", server->url(), client->id());
		
		// Send initial payload to client
		Serial.printf("sending initial message list to client %u: \n%s\n", client->id(), sent_messages.to_json().c_str());
		server->text(client->id(), sent_messages.to_json());
		
		// Add user with client id to list
		user_t user{
			.client_id 	= client->id(),
			.name 		= get_random_name(),
			.ip_addr	= client->remoteIP()//{ip[0], ip[1], ip[2], ip[3]}
		};
		users.add_user(user);

		// Print the contents of the message
		Serial.printf("sending user data to all clients: \n%s\n", users.to_json().c_str());
		
		// Send all clients an update list of users
		notify_all(server, users.to_json().c_str());

		// Retrieve and parse user data into JSON format
		// AwsFrameInfo *info = (AwsFrameInfo*)arg;
		// if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT){
		// 	data[len] = 0;
		// }
		return;

	} else if(type == WS_EVT_DISCONNECT){
		Serial.printf("%s: client %u disconnected from server\n", server->url(), client->id());
		users.remove_user(client->id());

		// Send all clients an update list of users
		notify_all(server, users.to_json().c_str());
	} else if(type == WS_EVT_DATA){
		handle_message(server, client, arg, data, len);
	} else if(type == WS_EVT_PONG){

	} else if(type == WS_EVT_ERROR){
		Serial.printf("%s: an error has occurred\n", server->url());
	}
}


void handle_message(AsyncWebSocket *server, AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len){
	AwsFrameInfo *info = (AwsFrameInfo*)arg;
	if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT){
		data[len] = 0;
		// data[len] = '\0';

		// Parse JSON data down into a message object
		DynamicJsonDocument doc(JSON_BUFFER_SIZE);
		DeserializationError error = deserializeJson(doc, (const char*)data);
		if(error){
			Serial.printf("deserializeJson() failed: %s\n", error.c_str());
			return;
		}

		string_t type 	= doc["type"].as<string_t>();
		user_t sender{ 
			.client_id	= client->id(),
			.name 		= doc["message"]["sender"]["name"].as<string_t>(), 
			.ip_addr 	= client->remoteIP() //{ ip[0], ip[1], ip[2], ip[3] }
		};

		Serial.printf("%s: received message from client %u with type '%s'\n", 
			server->url(), client->id(), type.c_str()
		);

		// Check if user provided a name...if not, give a random one
		if(sender.name.isEmpty())
			sender.name = get_random_name();
		
		// Check if we need to update user list information
		auto user = users.find_user(sender);;
		if(user){
			// lol...std::optional<std::reference_wrapper<user_t>>
			users.replace(sender);
			
			// Send all clients an update list of users
			notify_all(server, users.to_json().c_str());
			Serial.println(F(DOMAIN_NAME ": updating user list..."));
			// Serial.printf("%s\n", users.to_json().c_str());
		}
		else{
			users.add_user(sender);
			string_t sender_json = to_json(sender);
			notify_all(server, users.to_json().c_str());
			Serial.println(F(DOMAIN_NAME ": adding user to user list..."));
			// Serial.printf("%s\n", users.to_json().c_str());
		}

		// Add user to user list
		if(type.equals("user")){
		}
		else if(type.equals("message")){ // Add message to message list
			message_t message{
				.id			= message_queue::counter++,
				.sender 	= sender,
				.contents 	= doc["message"]["contents"].as<string_t>(),
				.timestamp 	= doc["message"]["timestamp"].as<string_t>()
			};

			// Get user from list and add message to message list
			if (!message.sender.name.isEmpty() && !message.contents.isEmpty()){
				unsent_messages.add_message(message);

				// Save message list HTML to file using LittleFS
				Serial.println(F(DOMAIN_NAME ": saving sent message list to file system..."));
				// Serial.printf("%s\n", unsent_messages.to_json().c_str());
				save_to_file(MESSAGES_FILE, sent_messages.to_json());
				notify_all(server, unsent_messages.to_json().c_str());

				sent_messages.add_messages(unsent_messages);
				unsent_messages.clear();
			}
			// veprosqlloader to popupdatabase need to find attributes (channels are cached in postgres)
			// need identifier (id) for to map to asset names
		}
	}
}


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
	Serial.println(F(DOMAIN_NAME ": setting up access point..."));
	WiFi.mode(WIFI_AP);
	// WiFi.softAPConfig(ap_ip, ap_ip, ip_address_t(255, 255, 255, 0), ap_ip);
	WiFi.config(ap_ip, ap_ip, ip_address_t(255, 255, 255, 0), ip_address_t(8,8,8,8));

	(pass) ? WiFi.softAP(ssid) : WiFi.softAP(ssid, pass);
	WiFi.hostname(DOMAIN_NAME);
	
	while(WiFi.status() != WL_CONNECTED){
		Serial.println("Waiting to connect to WiFi...");
		delay(100);
	}


	// Initialize SPI and rfid
	SPI.begin();
	rfid_init(rfid_reader);
	
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

	string_t json = load_from_file(MESSAGES_FILE);
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
				.ip_addr 	= { i[0], i[1], i[2], i[3] }
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
	web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(LittleFS, "/index.html", "text/html");
	});

	web_server.on("/functions.js", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(LittleFS, "/functions.js", "text/javascript");
	});

	web_server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(LittleFS, "/styles.css", "text/css");
	});

	web_server.on("/background.css", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(LittleFS, "/background.css", "text/css");
	});

	web_server.onNotFound([](AsyncWebServerRequest *request){
		request->send(404, "text/plain", "Error 404: Page not found!");
		AsyncWebServerResponse *response = request->beginResponse(302);
		response->addHeader("Location", "http://192.168.4.1/");
		request->send(response);
	});

	// basic http authentication
	// web_events.setAuthentication("user", "pass");
	web_sockets.onEvent(on_event);
	web_server.addHandler(&web_sockets);
	web_server.addHandler(&web_events);
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
	
	// MDNS.update();
	dns_server.processNextRequest();
	rfid_update(rfid_reader);// show the UID and PICC type when scanning and send a message to clients
	web_sockets.cleanupClients();
	delay(1000);

}
