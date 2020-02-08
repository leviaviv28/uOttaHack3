#include "WiFi.h"
#include <HTTPClient.h>

#define MAX_RETRY 3

const char* wifiPrefix = "InnovaChamps"
const char* serverAddress = "http://192.168.137.109:5000/update_location";
//const char* ssid = "ArduinoNet";
const char* password =  "nohackplz";

bool isValidWiFi(ssid) {
	return strlen(ssid) > strlen(wifiPrefix) && ssid.startsWith(wifiPrefix);
}

void scanWiFi() {
	Serial.println("scan start");
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0) {
		Serial.println("no networks found");
	} else {
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i) {
			// Print SSID and RSSI for each network found
			Serial.println((i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") " + (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
			// Check if WiFi is valid InnovaPost address, and that signal is stronger than current connection
			if wifi.isValidWiFi(WiFi.SSID(i) && WiFi.RSSI(i) < WiFi.RSSI()) {
				WiFi.begin(WiFi.SSID(i), password);
				while (WiFi.status() != WL_CONNECTED) //Check for the connection
					Serial.println("Connecting to " + WiFi.SSID(i));
				Serial.println("Connected to " + WiFi.SSID(i));
			}
		}
	}
}

void sendUpdate(int retries) {
	HTTPClient http;

	http.begin(serverAddress);  //Specify destination for HTTP request
	http.addHeader("Content-Type", "text/plain");             //Specify content-type header

	int httpResponseCode = http.POST("tracking=" + tracking + "&node=" + WiFi.SSID());   //Send the actual POST request

	if(httpResponseCode>0){
		String response = http.getString(); //Get the response to the request
		Serial.println(httpResponseCode);   //Print return code
		Serial.println(response);           //Print request answer
	} else {
		Serial.print("Error on sending POST: ");
		Serial.println(httpResponseCode);
		Serial.println("Retrying " + retries + " times")
		if(retries > 0)
			sendUpdate(retries) - 1;
	}

	http.end();  //Free resources
}

void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);
	Serial.println("Setup done");
	delay(4000);   //Delay needed before calling the WiFi.begin
	// Connect to nearest InnovaPost WiFi 
	scanWiFi();
 }

void loop() {
	if(WiFi.status() == WL_CONNECTED) {   //Check WiFi connection status
		sendUpdate(MAX_RETRY);
		delay(5000)
	} else {
		scanWiFi();
	}
}
