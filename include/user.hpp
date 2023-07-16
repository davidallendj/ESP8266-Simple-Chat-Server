#pragma once

#include <optional>
#include <set>
#include <algorithm>

#include "types.hpp"
#include "constants.hpp"
#include "utils.hpp"


namespace user {

	struct user_t{
		size_t 			client_id;
		string_t		name;
		ip_address_t	ip;
		mac_address_t	mac;
	};
	
	using namespace utils;
	using optional_ref = std::optional<std::reference_wrapper<user_t>>;
	
	class user_list{
	public:
	public:
		void add_user(size_t client_id, string_t name, ip_address_t ip, mac_address_t mac = 0);
		void add_user(const user_t& user);
		optional_ref get_user(size_t index);
		optional_ref find_user(const user_t& user);
		optional_ref find_user(string_t name);
		optional_ref find_user(ip_address_t ip);
		optional_ref find_user(size_t client_id, ip_address_t ip);
		optional_ref find_user(mac_address_t mac);
		optional_ref find_user(size_t client_id);
		optional_ref find_and_replace_user(const user_t& user);
		bool has_user(const user_t& user) const;
		bool has_user(string_t name, ip_address_t ip) const;
		void remove_user(ip_address_t ip);
		void remove_user(size_t client_id);
		void replace(const user_t& user);
		string_t to_json() const;
		size_t get_count() const;
		
	private:
		std::vector<user_t> m_users;	// TODO: user a set for unique instead??
	};

	string_t to_json(const user_t& user);
}