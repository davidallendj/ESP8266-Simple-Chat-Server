#pragma once

#include <optional>
#include <vector>
#include <algorithm>

#include "typedefs.h"
#include "constants.h"
#include "utils.h"


typedef struct _user{
	string_t		name;
	ip_address_t	ip_addr;
} user_t;


class user_list{
public:
	user_list();
	void add_user(string_t name, ip_address_t ip_addr);
	void add_user(const user_t& user);
	opt<user_t> get_user(int index);
	opt<user_t> find_user(const user_t& user);
	opt<user_t> find_user(string_t name);
	opt<user_t> find_user(ip_address_t ip_addr);
	opt<user_t> find_user(string_t name, ip_address_t ip_addr);
	int find_user_index(string_t name);
	bool has_user_name(string_t name);
	bool has_user_ip(ip_address_t ip_addr);
	bool has_user(string_t name, ip_address_t ip_addr);
	bool has_user(const user_t& user);
	
private:
	std::vector<opt<user_t>> m_users;
};

user_list::user_list()
: m_users(MAX_CLIENTS)
{}

void user_list::add_user(string_t name, ip_address_t ip_addr){
	user_t user{ .name = name, .ip_addr = ip_addr };
	add_user(user);
}


void user_list::add_user(const user_t& user){
	auto size = m_users.size();
	if(size >= MAX_CLIENTS)
		Serial.println("Cannot add new user to server");
	m_users.push_back(user);
}


opt<user_t> user_list::get_user(int index){
	return m_users.at(index);
}


opt<user_t> user_list::find_user(const user_t& user){
	return find_user(user.name, user.ip_addr);
}

opt<user_t> user_list::find_user(string_t name){
	auto it = find_if(m_users, [&name](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return user.name == name;
		}
		return false;
	});
	if(it != m_users.end())
		return *it;
	return {};
}


opt<user_t> user_list::find_user(ip_address_t ip_addr){
	auto it = find_if(m_users, [&ip_addr](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return user.ip_addr == ip_addr;
		}
		return false;
	});
	if(it != m_users.end())
		return *it;
	return {};
}

opt<user_t> user_list::find_user(string_t name, ip_address_t ip_addr){
	auto it = find_if(m_users, [&name, &ip_addr](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return user.name == name && user.ip_addr == ip_addr;
		}
		return false;
	});
	if(it != m_users.end())
		return *it;
	return {};
}


int user_list::find_user_index(string_t name){
	auto it = find_if(m_users, [&name](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return user.name == name;
		}
		return false;
	});
	return std::distance(m_users.begin(), it);
}


bool user_list::has_user_name(string_t name){
	// auto it = std::find_if(m_users.begin(), m_users.end(), [&name](user_t user){
	// 	return user.name == name;
	// });
	auto it = find_if(m_users, [&name](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return user.name == name;
		}
		return false;
	});
	return it != m_users.end();
}


bool user_list::has_user_ip(ip_address_t ip_addr){
	auto it = find_if(m_users, [&ip_addr](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return user.ip_addr == ip_addr;
		}
		return false;
	});
	return it != m_users.end();
}


bool user_list::has_user(string_t name, ip_address_t ip_addr){
	// This only checks to see if the user has the same values
	// and does not use the actual address of the user
	auto it = find_if(m_users, [&name, &ip_addr](opt<user_t> opt_user){
		if(opt_user.has_value()){
			auto user = opt_user.value();
			return (user.name == name && user.ip_addr == ip_addr);
		}
		return false;
	});
	return it != m_users.end();
}


bool user_list::has_user(const user_t& user){
	return has_user(user.name, user.ip_addr);
}