#pragma once

#include <optional>
#include <set>
#include <algorithm>

#include "types.h"
#include "constants.h"
#include "utils.h"


typedef struct _user{
	size_t 			client_id;
	string_t		name;
	ip_address_t	ip_addr;
} user_t;


class user_list{
public:
	using optional_ref = std::optional<std::reference_wrapper<user_t>>;
public:
	void add_user(size_t client_id, string_t name, ip_address_t ip_addr);
	void add_user(const user_t& user);
	optional_ref get_user(size_t index);
	optional_ref find_user(const user_t& user);
	optional_ref find_user(string_t name);
	optional_ref find_user(ip_address_t ip_addr);
	optional_ref find_user(size_t client_id, ip_address_t ip_addr);
	optional_ref find_user(size_t client_id);
	optional_ref find_and_replace_user(const user_t& user);
	bool has_user(const user_t& user);
	bool has_user(string_t name, ip_address_t ip_addr);
	void remove_user(ip_address_t ip_addr);
	void remove_user(size_t client_id);
	void replace(const user_t& user);
	string_t to_json() const;
	size_t get_count() const;
	
private:
	std::vector<user_t> m_users;	// TODO: user a set for unique instead??
};


void user_list::add_user(size_t client_id, string_t name, ip_address_t ip_addr){
	user_t user{
		.client_id 	= client_id,
		.name 		= name, 
		.ip_addr 	= ip_addr 
	};
	add_user(user);
}


void user_list::add_user(const user_t& user){
	auto size = m_users.size();
	if(size >= MAX_CLIENTS)
		Serial.println("Cannot add new user to server (max limit reached)!");
	m_users.emplace_back(user);
}


user_list::optional_ref user_list::get_user(size_t index){
	return std::ref(m_users.at(index));
}


user_list::optional_ref user_list::find_user(const user_t& user){
	return find_user(user.client_id, user.ip_addr);
}


user_list::optional_ref user_list::find_user(string_t name){
	auto it = find_if(m_users, [&name](user_t user){
		return user.name == name;
	});
	if(it != m_users.end())
		return std::ref(*it);
	return {};
}


user_list::optional_ref user_list::find_user(ip_address_t ip_addr){
	auto it = find_if(m_users, [&ip_addr](user_t& user){
		return user.ip_addr == ip_addr;
	});
	if(it != m_users.end())
		return std::ref(*it);
	return {};
}


user_list::optional_ref user_list::find_user(size_t client_id, ip_address_t ip_addr){
	auto it = find_if(m_users, [&client_id, &ip_addr](user_t& user){
		return 	user.client_id == client_id && 
				user.ip_addr == ip_addr;
	});
	if(it != m_users.end())
		return std::ref(*it);
	return {};
}


user_list::optional_ref user_list::find_user(size_t client_id){
	auto it = find_if(m_users, [&client_id](user_t& user){
		return user.client_id == client_id;
	});
	if(it != m_users.end())
		return std::ref(*it);
	return {};
}


user_list::optional_ref user_list::find_and_replace_user(const user_t& user){
	auto it = find_if(m_users, [&user](user_t& other){
		return user.ip_addr == other.ip_addr;
	});
	if(it != m_users.end()){
		(*it) = user;
		return std::ref(*it);
	}
	return {};
}


bool user_list::has_user(const user_t& user){
	return has_user(user.name, user.ip_addr);
}

bool user_list::has_user(string_t name, ip_address_t ip_addr){
	auto it = find_if(m_users, [&name, &ip_addr](const user_t& user){
		return 	user.name == name &&
				user.ip_addr == ip_addr;
	});
	return it != m_users.end();
}


void user_list::remove_user(ip_address_t ip_addr){
	m_users.erase(std::remove_if(m_users.begin(), m_users.end(), [&ip_addr](const user_t& user){
		return 	user.ip_addr[0] == ip_addr[0] &&
				user.ip_addr[1] == ip_addr[1] &&
				user.ip_addr[2] == ip_addr[2] &&
				user.ip_addr[3] == ip_addr[3];
	}));
}


void user_list::remove_user(size_t client_id){
	m_users.erase(std::remove_if(m_users.begin(), m_users.end(), [&client_id](const user_t& user){
		return user.client_id == client_id;
	}));
}


void user_list::replace(const user_t& user){
	std::replace_if(m_users.begin(), m_users.end(),[&user](const user_t& other){
		return other.ip_addr == user.ip_addr;
	}, user);
}


string_t user_list::to_json() const{
	string_t json("{\n\t\"type\": \"user\",\n\t\"users\":[\n");
	for(const auto& user : m_users){
		json += "\t\t{\n";
		json += "\t\t\"client_id\": " + string_t(user.client_id) + ",\n";
		json += "\t\t\"name\": \"" + user.name + "\",\n";
		json += "\t\t\"ip_addr\":["
			+ string_t(user.ip_addr[0]) + ", "
			+ string_t(user.ip_addr[1]) + ", "
			+ string_t(user.ip_addr[2]) + ", "
			+ string_t(user.ip_addr[3]) + "]\n"
			+ "\t\t},\n";
	}
	if(!json.isEmpty())
		json.remove(json.length()-2, 2);
	json += "\n\t]\n}\n";
	return json;
}


size_t user_list::get_count() const{
	return m_users.size();
}


string_t to_json(const user_t& user){
	string_t json = "{\n";
	json += "\t\"client_id\": " + string_t(user.client_id) + ",\n";
	json += "\t\"name\": \"" + user.name +"\",\n";
	json += "\t\"ip_addr\": ["
		+ string_t(user.ip_addr[0]) + ", "
		+ string_t(user.ip_addr[1]) + ", "
		+ string_t(user.ip_addr[2]) + ", "
		+ string_t(user.ip_addr[3]) + "]\n";
	json += "}\n";
	return json;
}