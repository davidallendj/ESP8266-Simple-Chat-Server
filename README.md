# ESP8266 Simple Chat Server

A local, simple chat server built for the ESP8266 written in primarily in C++ 17, HTML, and JavaScript. The ESP8266 creates a Wi-Fi access point that allows devices to connect to it. The chat server uses the ESPAsyncWebServer library for communication and is built to be self-contained (i.e. does not connect to other access points nor the Internet). The project is built with PlatformIO in Visual Studio Code.

The project also includes functionality for other devices, including a RFID RC522 card reader and a LCD screen with I2C adapter module. This functionality will likely be changed to be configed using these devices. A diagram will be included later to show how the circuit is wired.

## Getting Started

The project is typically ran using the PlatformIO extension in Visual Studio Code. The project should also work with Arduino IDE, but may require more effort to get working. The instruction below are only for PlatformIO + VS Code.

First clone the repository:

```bash
git clone https://github.com/davidallendj/ESP8266-Simple-Chat-Server
```

Then, open the project in VS code by going to `File -> Open Folder` and select where you cloned the project. Next, select the PlatformIO tab, click `Build` and then `Upload` to the device. Make sure that the correct device is selected (ESP8266) or else the upload may failed. This has not been tested to work on an ESP32 or its variants.

## Future Work

*   [ ] Add a user list of connections

*   [ ] Compress messages to save space (using Unishox?)

*   [ ] Configure mDNS to actually work properly

*   [ ] Modify code to enable/disable features and make configurable (RFID, LCD, etc.)

*   [ ] Read input from serial to change device state

*   [ ] Create interface for monitor/changing device state

*   [ ] Create interface to connect to another Wi-Fi access point

*   [ ] Use captive portal to redirect connections to a landing page

*   [ ] Switch to using WebSockets instead of event source to handle client connections more reliably

*   [x] Use event source to only push data to clients instead of constant polling

*   [x] Minimize data being sent when making a request from server

*   [x] Replace "Submit" call with asynchronous AJAX calls

