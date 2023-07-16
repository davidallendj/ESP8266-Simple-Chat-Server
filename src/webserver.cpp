
#include "webserver.hpp"


namespace web_server{
	// Use web sockets to detect when client connects/disconnects
	void on_event(
		AsyncWebSocket *server, 
		AsyncWebSocketClient *client, 
		AwsEventType type, 
		void *arg, 
		uint8_t *data, 
		size_t len
	){
		if(type == WS_EVT_CONNECT){
			Serial.printf("%s: client %u connected to server established!\n", server->url(), client->id());
			
			// Send initial payload to client
			Serial.printf("sending initial message list to client %u: \n%s\n", client->id(), sent_messages.to_json().c_str());
			server->text(client->id(), sent_messages.to_json());
			
			// Add user with client id to list
			user_t user{
				.client_id 	= client->id(),
				.name 		= utils::get_random_name(),
				.ip			= client->remoteIP(),//{ip[0], ip[1], ip[2], ip[3]}
				.mac		= 0
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


	void handle_message(
		AsyncWebSocket *server, 
		AsyncWebSocketClient *client, 
		void *arg, 
		uint8_t *data, 
		size_t len
	){
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
				.ip 		= client->remoteIP(), //{ ip[0], ip[1], ip[2], ip[3] }
				.mac		= 0
			};

			Serial.printf("%s: received message from client %u with type '%s'\n", 
				server->url(), client->id(), type.c_str()
			);

			// Check if user provided a name...if not, give a random one
			if(sender.name.isEmpty())
				sender.name = utils::get_random_name();
			
			// Check if we need to update user list information
			auto user = users.find_user(sender);;
			if(user){
				// lol...std::optional<std::reference_wrapper<user_t>>
				users.replace(sender);
				
				// Send all clients an update list of users
				notify_all(server, users.to_json().c_str());
				Serial.println(domain_name + ": updating user list...");
				// Serial.printf("%s\n", users.to_json().c_str());
			}
			else{
				users.add_user(sender);
				string_t sender_json = to_json(sender);
				notify_all(server, users.to_json().c_str());
				Serial.println(domain_name + ": adding user to user list...");
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
					Serial.println(domain_name + ": saving sent message list to file system...");
					// Serial.printf("%s\n", unsent_messages.to_json().c_str());
					save_file(messages_file.c_str(), sent_messages.to_json());
					notify_all(server, unsent_messages.to_json().c_str());

					sent_messages.add_messages(unsent_messages);
					unsent_messages.clear();
				}
				// veprosqlloader to popupdatabase need to find attributes (channels are cached in postgres)
				// need identifier (id) for to map to asset names
			}
		}
	}


	void notify_all(AsyncWebSocket *server, const char *text){
		server->textAll(text);
	}
}