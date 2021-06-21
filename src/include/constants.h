#pragma once

#include <SPI.h>
#include <MFRC522.h>

// #define BAUD_RATE		9600
#define BAUD_RATE			115200
#define DELAY_INTERVAL		1000
#define MAX_MESSAGES		32
#define MAX_CLIENTS			8
#define JSON_BUFFER_SIZE	1024
#define DNS_PORT			53
#define SERVER_PORT			80
#define DOMAIN_NAME			"esp8266"
#define MESSAGES_FILE		"/messages.json"
#define PARAM_USER_NAME		"user"
#define PARAM_USER_MESSAGE	"message"
#define PARAM_USER_IP		"ip"
#define PARAM_TIMESTAMP		"timestamp"
#define PARAM_MESSAGE_COUNT	"count"

// LCD constants
#define LCD_ADDRESS			0x27
#define LCD_ROW_COUNT		2
#define LCD_COL_COUNT		16

// RFID constants (need to solder header!!)
#define RFID_RC522_PIN_RST	16	// GPIO 16 (D0) -- has built-in pull down
#define RFID_RC522_PIN_IRQ		// not set	
#define RFID_RC522_PIN_MOSI	7	// GPIO 13 (D7)
#define RFID_RC522_PIN_MISO 6	// GPIO 12 (D6)
#define RFID_RC522_PIN_SDA	15	// GPIO 15 (D8) and also SS/CS -- Fails if pulled up!!
#define RFID_RC522_PIN_SS 	RFID_RC522_PIN_SDA
#define RFID_RC522_PIN_SCK	5	// GPIO 5 (D5)

constexpr std::array 	names{
	"cat", "dog", "giraffe", "donkey", "elephant", "snake", "owl",
	"roach", "racoon", "zebra", "bear", "fish", "fox", "weasel", "worm",
	"mole", "kangaroo", "lion", "hyena", "eel", "snail", "whale", 
	"dolphin", "rabbit", "anteater", "armadillo", "salamander"
};
