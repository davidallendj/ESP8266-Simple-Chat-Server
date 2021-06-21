
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
#include "types.h"


template <typename Container, typename Predicate = std::function<void()>>
auto find_if(Container c, Predicate p){
	return std::find_if(c.begin(), c.end(), p);
}

template <typename Container, typename Predicate = std::function<void()>>
auto for_each(Container c, Predicate p){
	return std::for_each(c.begin(), c.end(), p);
}

string_t load_from_file(const char *path);
void save_to_file(const char *path, string_t contents, bool append=false);
string_t load_html(const char* path);
void not_found(AsyncWebServerRequest *request);
std::vector<wlan_network_t> scan_networks();
std::vector<ip_address_t> scan_clients();
void scan_i2c();
void handle_message(AsyncWebSocket *server, AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);
void on_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void notify_all(AsyncWebSocket *server, char *text);
const char* get_random_name();


string_t load_from_file(const char *path){
	string_t contents("");

	// Open file in read mode
	File file = LittleFS.open(path, "r");
	if(!file){
		Serial.print("Could not open file at ");
		Serial.println(path);
	}

	// Read contents of file, close, and return string
	while(file.available())
		contents += (char)file.read();
	file.close();
	return contents;
}


void save_to_file(const char *path, string_t contents, bool append){
	// Open file in write mode
	File file = LittleFS.open(path, (append) ? "a" : "w+");
	if(!file){
		Serial.print("Could not open file at ");
		Serial.println(path);
	}

	// Write contents to file and close
	file.write(contents.c_str());
	file.close();
}


// Shows list of available networks
std::vector<wlan_network_t> scan_networks(){
	std::vector<wlan_network_t> networks;
	int scan_result = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
	if(scan_result == 0){
		Serial.println("No networks found...");
	}
	else if(scan_result > 0){
		Serial.printf(PSTR("%d networks found:\n"), scan_result);

		for(int8_t i = 0; i < scan_result; i++){
			wlan_network_t n;
			WiFi.getNetworkInfo(i, n.ssid, n.encryption_type, n.rssi, n.bssid, n.channel, n.is_hidden);
			Serial.printf(PSTR(" %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %s\n"),
							i,
							n.channel,
							n.bssid[0], n.bssid[1], n.bssid[2],
							n.bssid[3], n.bssid[4], n.bssid[5],
							n.rssi,
							(n.encryption_type == ENC_TYPE_NONE) ? ' ' : '*',
							n.is_hidden ? 'H' : 'V',
							n.ssid.c_str()
			);
			delay(0);
			networks.push_back(n);
		}
	}
	else{
		Serial.printf(PSTR("WiFi scan error %d"), scan_result);
	}
	return networks;
}


std::vector<ip_address_t> scan_clients(){
	// struct ip_addr 			*ip;
	std::vector<ip_address_t>	ip_addrs;
	struct station_info 		*stat_info		= wifi_softap_get_station_info();
	// unsigned char 			station_count	= wifi_softap_get_station_num();

	while(stat_info != nullptr){
		// ip = stat_info->ip;
		ip_addrs.push_back(ip_address_t(stat_info->ip.addr));
		stat_info = STAILQ_NEXT(stat_info, next);
	}
	return ip_addrs;
}


void scan_i2c(){
	byte error, address;
	int devices = 0;
	for(address = 1; address < 127; address++){
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if(error == 0){
			if(address < 16){
				Serial.print(address, HEX);
				devices++;
			}
			else if(error == 4){
				if(address < 16){
					Serial.print("0");
					Serial.println(address, HEX);
				}
			}
			if(devices == 0){
				Serial.println("No I2C devices found\n");
			}
			else{
				Serial.println("I2C device found");
				delay(5000);
			}
		}
	}
}


void notify_all(AsyncWebSocket *server, const char *text){
	server->textAll(text);
}


const char* get_random_name(){
	return names[random(0, names.size()-1)];
}
