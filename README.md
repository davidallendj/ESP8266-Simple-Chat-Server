# ESP8266 Simple Chat Server

A local, simple chat server built for the ESP8266 written in primarily in C++ 17. The server creates an access point that allows other to connect. The chat server uses the ESPAsyncWebServer library for communication and is built to be self-contained (i.e. does not connect to other access points nor the Internet). The project is built with PlatformIO in Visual Studio Code. 

The project also includes code for other devices, including a RFID RC522 card reader and a LCD screen with I2C adapter module. This code may be removed in the future to minimize code bloat.



## Future Work

- Add a user list of connections

- Add compression to messages (Unishox??)

- Configure DNS service to *actually* work

- Remove unused code
