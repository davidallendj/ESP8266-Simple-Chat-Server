#pragma once

#include <deque>
#include <functional>
#include "user.h"
#include "reverse.h"
#include "constants.h"
#include "typedefs.h"
#include "utils.h"

/* Example JSON file storing messages to disk
	{
		"count": 2,
		"messages":[
			{
				"sender": {
					"name": 		"elementary.os",
					"ip addr": 		[192, 168, 4, 2]		// save to init IPAddress
				},
				"contents": 		"hello world",
				"timestamp": 		"2021-6-8 06:43:21 PM"
			},
			{
				"sender": {
					"name":			"manjaro"
					"ip addr":		[192, 168, 4, 3]
				},
				"contents":			"goodbye world",
				"timestamp":		"2021-6-8 06:43:34 PM"
			}
		]
	}
	*/


typedef struct _message{
	user_t 			sender;
	string_t		contents;
	string_t		timestamp;
} message_t;


class message_queue{
public:
	void add_message(const message_t& message);
	void add_message(const user_t& sender, string_t contents, string_t timestamp);
	void add_messages(const message_queue& messages);
	void remove_message(size_t index);
	int get_count() const;
	string_t to_json() const;
	string_t to_json(size_t from) const;
	string_t to_json(size_t from, size_t to) const;
	string_t get_message(size_t index) const;
	bool has_messages() const;
	const message_t& get_message_ref(size_t index) const;
	string_t get_last_message() const;
	std::deque<message_t> get_messages() const;
	void load_from_json(string_t path);
	void load_from_json_file(string_t json);
	void set_message_status(size_t index, bool status);
	void clear();
	
private:
	// FIFO queue to push new messages and pop old ones
	std::deque<message_t> m_messages;
};


void message_queue::add_message(const message_t& message){
	// Check if we've reach our message limit
	if(m_messages.size() > MAX_MESSAGES){
		// Pop the message at the front of the queue
		m_messages.pop_front();
	}
	
	// Then, push new message to the back
	m_messages.push_back(message);
}


void message_queue::add_message(const user_t& sender, string_t contents, string_t timestamp){
	message_t m { 
		.sender		= sender, 
		.contents	= contents, 
		.timestamp	= timestamp
	};
	add_message(m);
}


void message_queue::add_messages(const message_queue& messages){
	for(const auto& message : messages.get_messages())
		add_message(message);
}


void message_queue::remove_message(size_t index){
	m_messages.erase(m_messages.begin() + index);
}


int message_queue::get_count() const{
	return m_messages.size();
}


string_t message_queue::to_json() const{
	return to_json(0, get_count());
}


string_t message_queue::to_json(size_t from) const{
	return to_json(from, get_count());
}


string_t message_queue::to_json(size_t from, size_t to) const{
	string_t json("{\n\t\"messages\":[\n");
	for(size_t i = from; i < to; i++){
		message_t message = m_messages[i];
		json += "\t\t{\n";
		json += "\t\t\"sender\":{\n";
		json += "\t\t\t\"name\": \"" + message.sender.name + "\",\n";
		json += "\t\t\t\"ip_addr\":[" 
			+ string_t(message.sender.ip_addr[0]) + ", "
			+ string_t(message.sender.ip_addr[1]) + ", "
			+ string_t(message.sender.ip_addr[2]) + ", "
			+ string_t(message.sender.ip_addr[3]) + "]\n"
			+ "\t\t},\n";
		json += "\t\t\"contents\":\"" + message.contents + "\",\n";
		json += "\t\t\"timestamp\":\"" + message.timestamp + "\"\n";
		json += "\t\t},";
	}
	json.remove(json.length()-1, 1);
	json += "\n\t]\n}\n";
	return json;
}


bool message_queue::has_messages() const{
	return m_messages.size() > 0;
}


string_t message_queue::get_message(size_t index) const{
	message_t message = m_messages.at(index);
	if(message.sender.name.isEmpty())
		return "invalid sender";
	return message.sender.name + "@" 
		+ message.sender.ip_addr.toString() 
		+ "[" + message.timestamp + "]: " 
		+ message.contents;
}


const message_t& message_queue::get_message_ref(size_t index) const{
	return m_messages.at(index);
}


string_t message_queue::get_last_message() const{
	return get_message(m_messages.size()-1);
}


std::deque<message_t> message_queue::get_messages() const{
	return m_messages;
}


void message_queue::load_from_json(string_t json){
	DynamicJsonDocument doc(512);
	DeserializationError err = deserializeJson(doc, json);
	if(err){
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(err.c_str());
	}
	// Fetch messages from JSON object
	JsonObject 	object 			= doc.to<JsonObject>();
	JsonArray 	array 			= object["message"];
	for(JsonObject _message : array){
		message_t message;
		JsonObject sender 		= _message["sender"];
		JsonArray ip 			= sender["ip addr"];
		message.sender.name 	= sender["name"].as<string_t>();
		message.sender.ip_addr 	= ip_address_t(ip[0], ip[1], ip[2], ip[3]);
		message.contents 		= _message["contents"].as<string_t>();
		message.timestamp 		= _message["timestamp"].as<string_t>();
		add_message(message);
	}
}


void message_queue::load_from_json_file(string_t path){
	string_t json = load_from_file(path.c_str());
	load_from_json(json);
}


void message_queue::clear(){
	m_messages.clear();
}