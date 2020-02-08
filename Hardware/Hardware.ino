#include "WiFi.h"
#include <HTTPClient.h>
#include <PubSubClient.h>

#define MAX_RETRY 3

// Setting variables for timer ISRs
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
long tracking = 123456789;
// WiFi Configuration Variables
const char* wifiPrefix = "InnovaChamps";
const char* serverAddress = "http://34.66.100.203:80/update_location";
const char* ssid = "ArduinoNet";
const char* password =  "nohackplz";
// MQTT Configuration Variables
const char* mqttServer = "mr2hd0llj3vwjx.messaging.solace.cloud";
const int mqttPort = 8883;
const char* mqttUser = "solace-cloud-client";
const char* mqttPassword = "vvngpe3cn2ss72a7425hgf1l7f";


void scanWiFi() {
	Serial.println("scan start");
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0) {
		Serial.println("no networks found");
	} else {
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; i++) {
			// Print SSID and RSSI for each network found
			// Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
            Serial.println(WiFi.RSSI(i));
            Serial.println(WiFi.RSSI());
			// Check if WiFi is valid InnovaPost address, and that signal is stronger than current connection
        char __SSID[sizeof(WiFi.SSID(i))];
        WiFi.SSID(i).toCharArray(__SSID, sizeof(__SSID));
        Serial.println(__SSID);
			if (WiFi.RSSI() == 0 || WiFi.RSSI(i) > WiFi.RSSI) {
				WiFi.begin(__SSID, password);
       Serial.println(__SSID);
       delay(3000);
				while (WiFi.status() != WL_CONNECTED) //Check for the connection
					Serial.println("Connecting to " + WiFi.SSID(i));
				Serial.println("Connected to " + WiFi.SSID(i));
			}
		}
	}
}


void sendUpdate(int retries) {
	client.publish("update_tracking/" + String(tracking), String(WiFi.SSID()));
	Serial.println("Sent: " + String(WiFi.SSID()));
}

void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	client.setServer(mqttServer, mqttPort);
  	client.setCallback(callback);

	Serial.println("Setup done");
	delay(4000);   //Delay needed before calling the WiFi.begin
	// Connect to nearest InnovaPost WiFi 
	scanWiFi();
 }


void loop() { 
  if(WiFi.status() == WL_CONNECTED) {   //Check WiFi connection status
    sendUpdate(MAX_RETRY);
  } else {
    scanWiFi();
  }
  delay(5000);
}
