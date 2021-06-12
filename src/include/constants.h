#pragma once

// #define BAUD_RATE			9600
#define BAUD_RATE			115200
#define DELAY_INTERVAL		1000
#define MAX_MESSAGES		32
#define MAX_CLIENTS			8
#define SERVER_PORT			80
#define DNS_PORT			53
#define DOMAIN_NAME			"www.davidjallen.net"
#define MESSAGES_FILE		"/messages.json"
#define PARAM_USER_NAME		"user"
#define PARAM_USER_MESSAGE	"message"
#define PARAM_USER_IP		"ip"
#define PARAM_TIMESTAMP		"timestamp"
#define PARAM_MESSAGE_COUNT	"count"
#define LCD_ADDRESS			0x27
#define LCD_ROW_COUNT		2
#define LCD_COL_COUNT		16

#define RFID_RC522_PIN_RST	0	// GPIO 16 (D0)
#define RFID_RC522_PIN_IRQ		// not set	
#define RFID_RC522_PIN_MOSI	7	// GPIO 13 (D7)
#define RFID_RC522_PIN_MISO 6	// GPIO 12 (D6)
#define RFID_RC522_PIN_SDA	8	// GPIO 4 (D8) and also SS/CS
#define RFID_RC522_PIN_SS 	RFID_RC522_PIN_SDA
#define RFID_RC522_PIN_SCK	5	// GPIO 5 (D5)