#pragma once

#include <SPI.h>
#include <MFRC522.h>
#include <string>

// #define BAUD_RATE							= 9600
constexpr int BAUD_RATE							= 115200;
constexpr int DELAY_INTERVAL					= 1000;
constexpr int MAX_MESSAGES						= 32;
constexpr int MAX_CLIENTS						= 8;
constexpr int JSON_BUFFER_SIZE					= 1024;
constexpr int DNS_PORT							= 53;
constexpr int SERVER_PORT						= 80;
constexpr const char *PARAM_USER_NAME		= "user";
constexpr const char *PARAM_USER_MESSAGE	= "message";
constexpr const char *PARAM_USER_IP		= "ip";
constexpr const char *PARAM_TIMESTAMP		= "timestamp";
constexpr const char *PARAM_MESSAGE_COUNT	= "count";

// LCD constants
constexpr bool LCD_ENABLE			= false;
constexpr int LCD_ADDRESS			= 0x27;
constexpr int LCD_ROW_COUNT			= 2;
constexpr int LCD_COL_COUNT			= 16;

// RFID constants
constexpr bool RFID_ENABLE 			= false;
constexpr int RFID_RC522_PIN_RST 	= 16;	// GPIO 16 (D0) -- has built-in pull down
constexpr int RFID_RC522_PIN_IRQ 	= -1;	// not set	
constexpr int RFID_RC522_PIN_MOSI 	= 7	;	// GPIO 13 (D7)
constexpr int RFID_RC522_PIN_MISO 	= 6;	// GPIO 12 (D6)
constexpr int RFID_RC522_PIN_SDA	= 15;	// GPIO 15 (D8) and also SS/CS -- Fails if pulled up!!
constexpr int RFID_RC522_PIN_SS 	= RFID_RC522_PIN_SDA;
constexpr int RFID_RC522_PIN_SCK	= 5;	// GPIO 5 (D5)

constexpr std::array 	names{
	"cat", "dog", "giraffe", "donkey", "elephant", "snake", "owl",
	"roach", "racoon", "zebra", "bear", "fish", "fox", "weasel", "worm",
	"mole", "kangaroo", "lion", "hyena", "eel", "snail", "whale", 
	"dolphin", "rabbit", "anteater", "armadillo", "salamander"
};
