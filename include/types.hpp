#pragma once

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <ESP8266WiFi.h>
// #include <ESP8266mDNS.h>
#include <DNSServer.h>


namespace {
	using string_t 					= String;
	using ip_address_t 				= IPAddress;
	using ip_addresses_t			= std::vector<IPAddress>;
	using mac_address_t				= uint8_t;
	using rfid_t					= MFRC522;
	using rfid_key_t				= MFRC522::MIFARE_Key;
	using rfid_picc_t				= MFRC522::PICC_Type;
	using rfid_status_code_t		= MFRC522::StatusCode;
	using request_function			= std::function<void(AsyncWebServerRequest*)>;
	template <typename T> using opt = std::optional<T>;
}


struct wlan_network_t{
	string_t		ssid;
	int32_t			rssi;
	uint8_t			encryption_type;
	uint8_t			*bssid;
	int32_t			channel;
	bool			is_hidden;
};


struct datetime_t{
	unsigned long	years;
	unsigned long	months;
	unsigned long	days;
	unsigned long	hours;
	unsigned long	minutes;
	float			seconds;
};
