#pragma once

#include <SPI.h>
#include "typedefs.h"

// first data that is sent from tag is UID and PICC type
// which identifies the tag

// uid: unique identifier such as "BD 31 15 2B". it is saved
// in the first memory block of the tag and read-only
// picc: proximity integrated circuit card. Tag type.

// for use with 13.56 MHz (HF), ISM band cards
// supported tag formats: S50, S70 Ultralight, Pro, DESFire
// --->> Look up info on tag transponders such as Mifare transponder

void dump_byte_array(byte *buffer, byte buffer_size){
	for(byte i = 0; buffer_size; i++){
		Serial.print(buffer[i] < 0x10 ? " 0":"");
		Serial.print(buffer[i], HEX);
	}
}

void get_uid(string_t content, const rfid_t& rfid){
	for(byte i = 0; i < rfid.uid.size; i++){
		content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0":""));
		content.concat(String(rfid.uid.uidByte[i], HEX));
	}
	content.toUpperCase();

	if(content.substring(1) == "E3 26 92 03")
		Serial.println("Correct tag");
	else
		Serial.println("Wrong tag");
}

void rfid_update(rfid_t& rfid){
	// Reset loop if no new card present
	if(!rfid.PICC_IsNewCardPresent())
		return;
	
	// Select one of the cards
	if(!rfid.PICC_ReadCardSerial())
		return;
	
	// Show some details of the PICC (that is: the tag/card)
	Serial.print(F("Card UID:"));
	rfid_picc_t picc_type = rfid.PICC_GetType(rfid.uid.sak);
	Serial.println(rfid.PICC_GetTypeName(picc_type));

	// Check for compatibility
	if(picc_type != MFRC522::PICC_TYPE_MIFARE_MINI
		&& picc_type != MFRC522::PICC_TYPE_MIFARE_1K
		&& picc_type != MFRC522::PICC_TYPE_MIFARE_4K){
		Serial.println(F("No valid tag"));
		return;
	}

	rfid_status_code_t status;

	String content = "";
	get_uid(content, rfid);

	// Halt PICC
	rfid.PICC_HaltA();

	// Stop encryption on PCD
	rfid.PCD_StopCrypto1();

}