#pragma once

#include "message.hpp"
#include "rfid.hpp"
#include "utils.hpp"

namespace web_server {
	void on_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
	void handle_message(AsyncWebSocket *server, AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);
	void notify_all(AsyncWebSocket *server, const char *text);
	void not_found(AsyncWebServerRequest *request);
}

using namespace user;
using namespace message;

//WiFiServer web_server(SERVER_PORT);
extern AsyncWebServer 		server;
extern AsyncWebSocket		sockets;
extern AsyncEventSource		events;
extern DNSServer			dns_server;
extern LiquidCrystal_I2C	lcd; // set lcd address to 0x27 for 16x2 chars
extern rfid_t 				rfid_reader;
extern rfid_key_t			rfid_key;
extern user_list 			users;
extern message_queue 		sent_messages;
extern message_queue		unsent_messages;
extern unsigned long 		start_time;
extern string_t 			ssid;
extern string_t 			pass;
extern string_t 			domain_name;
extern string_t 			messages_file;
extern uint8_t				cursor_pos;
extern ip_address_t			ap_ip;