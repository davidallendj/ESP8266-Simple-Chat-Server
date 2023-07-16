#pragma once

#include <SPI.h>
#include "types.hpp"

namespace rfid{

	// first data that is sent from tag is UID and PICC type
	// which identifies the tag

	// uid: unique identifier such as "BD 31 15 2B". it is saved
	// in the first memory block of the tag and read-only
	// picc: proximity integrated circuit card. Tag type.

	// for use with 13.56 MHz (HF), ISM band cards
	// supported tag formats: S50, S70 Ultralight, Pro, DESFire
	// --->> Look up info on tag transponders such as Mifare transponder

	void dump_byte_array(byte *buffer, byte buffer_size);
	void get_uid(string_t content, const rfid_t& rfid);
	void init(rfid_t& rfid);
	void update(rfid_t& rfid);
}