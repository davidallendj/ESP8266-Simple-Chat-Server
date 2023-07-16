
#include "user.hpp"

namespace user {

	void user_list::add_user(
		size_t client_id, 
		string_t name, 
		ip_address_t ip,
		mac_address_t mac
	){
		user_t user{
			.client_id 	= client_id,
			.name 		= name, 
			.ip		 	= ip,
			.mac		= mac
		};
		add_user(user);
	}


	void user_list::add_user(const user_t& user){
		auto size = m_users.size();
		if(size >= MAX_CLIENTS)
			Serial.println("Cannot add new user to server (max limit reached)!");
		m_users.emplace_back(user);
		// m_users.insert(user);
	}


	optional_ref user_list::get_user(size_t index){
		return std::ref(m_users.at(index));
	}


	optional_ref user_list::find_user(const user_t& user){
		return find_user(user.client_id, user.ip);
	}


	optional_ref user_list::find_user(string_t name){
		auto it = find_if(m_users, [&name](user_t& user){
			return user.name == name;
		});
		if(it != m_users.end())
			return std::ref(*it);
		return {};
	}


	optional_ref user_list::find_user(ip_address_t ip){
		auto it = find_if(m_users, [&ip](user_t& user){
			return user.ip == ip;
		});
		if(it != m_users.end())
			return std::ref(*it);
		return {};
	}


	optional_ref user_list::find_user(mac_address_t mac){
		auto it = find_if(m_users, [&mac](user_t& user){
			return user.mac == mac;
		});
		if(it != m_users.end())
			return std::ref(*it);
		return {};
	}


	optional_ref user_list::find_user(
		size_t client_id, 
		ip_address_t ip
	){
		auto it = find_if(m_users, [&client_id, &ip](user_t& user){
			return 	user.client_id == client_id && 
					user.ip == ip;
		});
		if(it != m_users.end())
			return std::ref(*it);
		return {};
	}


	optional_ref user_list::find_user(size_t client_id){
		auto it = find_if(m_users, [&client_id](user_t& user){
			return user.client_id == client_id;
		});
		if(it != m_users.end())
			return std::ref(*it);
		return {};
	}


	optional_ref user_list::find_and_replace_user(const user_t& user){
		auto it = find_if(m_users, [&user](user_t& other){
			return user.ip == other.ip;
		});
		if(it != m_users.end()){
			(*it) = user;
			return std::ref(*it);
		}
		return {};
	}


	bool user_list::has_user(const user_t& user) const {
		return has_user(user.name, user.ip);
	}

	bool user_list::has_user(string_t name, ip_address_t ip) const {
		auto it = find_if(m_users, 
			[&name, &ip](const user_t& user){
				return 	user.name 	== name &&
						user.ip 	== ip;
		});
		return it != m_users.end();
	}


	void user_list::remove_user(ip_address_t ip){
		m_users.erase(remove_if(m_users,
			[&ip](const user_t& user){
				return 	user.ip[0] == ip[0] &&
						user.ip[1] == ip[1] &&
						user.ip[2] == ip[2] &&
						user.ip[3] == ip[3];
		}));
	}


	void user_list::remove_user(size_t client_id){
		m_users.erase(remove_if(m_users,
			[&client_id](const user_t& user){
				return user.client_id == client_id;
		}));
	}


	void user_list::replace(const user_t& user){
		replace_if(m_users,
			[&user](const user_t& other){
				return other.ip == user.ip;
			},
			user
		);
	}


	string_t user_list::to_json() const{
		string_t json("{\n\t\"type\": \"user\",\n\t\"users\":[\n");
		for(const auto& user : m_users){
			json += "\t\t{\n";
			json += "\t\t\"client_id\": " + string_t(user.client_id) + ",\n";
			json += "\t\t\"name\": \"" + user.name + "\",\n";
			json += "\t\t\"ip\":["
				+ string_t(user.ip[0]) + ", "
				+ string_t(user.ip[1]) + ", "
				+ string_t(user.ip[2]) + ", "
				+ string_t(user.ip[3]) + "]\n"
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
		json += "\t\"ip\": ["
			+ string_t(user.ip[0]) + ", "
			+ string_t(user.ip[1]) + ", "
			+ string_t(user.ip[2]) + ", "
			+ string_t(user.ip[3]) + "]\n";
		json += "}\n";
		return json;
	}
}