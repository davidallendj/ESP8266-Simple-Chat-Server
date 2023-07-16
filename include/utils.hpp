
#pragma once

#include <Arduino.h>
// #include <ESP8266WiFi.h>
// #include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include <vector>
#include <algorithm>
#include "types.hpp"

namespace utils {
	template <
		typename Container, 
		typename Predicate = std::function<void()>
	>
	auto find_if(Container c, Predicate p){
		return std::find_if(c.begin(), c.end(), p);
	}

	template <
		typename Container, 
		typename Predicate = std::function<void()>
	>
	auto for_each(Container c, Predicate p){
		return std::for_each(c.begin(), c.end(), p);
	}

	template <
		typename Container, 
		typename Predicate = std::function<void()>
	>
	auto remove_if(Container c, Predicate p){
		return std::remove_if(c.begin(), c.end(), p);
	}

	template <
		typename Container, 
		typename Replace,
		typename Predicate = std::function<void()>
	>
	auto replace_if(Container c, Predicate p, Replace r){
		return std::replace_if(c.begin(), c.end(), p, r);
	}


	string_t load_file(const char *path);
	void save_file(const char *path, string_t contents, bool append=false);
	string_t load_html(const char* path);

	std::vector<wlan_network_t> scan_networks();
	std::vector<ip_address_t> scan_clients();
	void scan_i2c();
	const char* get_random_name();
}
