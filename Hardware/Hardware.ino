#include "WiFi.h"
#include <HTTPClient.h>

#define MAX_RETRY 3

// Setting variables for timer ISRs
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
long tracking = 123456789;
const char* wifiPrefix = "InnovaChamps";
const char* serverAddress = "http://34.66.100.203:80/update_location";
const char* ssid = "ArduinoNet";
const char* password =  "nohackplz";


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
	HTTPClient http;

	http.begin(serverAddress);  //Specify destination for HTTP request
	http.addHeader("Content-Type", "text/plain");             //Specify content-type header

	int httpResponseCode = http.POST("tracking=" + String(tracking) +"&node=" + WiFi.SSID());   //Send the actual POST request

	if(httpResponseCode>0){
		String response = http.getString(); //Get the response to the request
		Serial.println(httpResponseCode);   //Print return code
		Serial.println(response);           //Print request answer
	} else {
		Serial.print("Error on sending POST: ");
		Serial.println(httpResponseCode);
		Serial.print("Retrying ");
      Serial.print(retries);
    Serial.println(" times");

		if(retries > 0)
			sendUpdate(retries - 1);
	}

	http.end();  //Free resources
}

void setup() {
	Serial.begin(115200);
/*
	// Timer ISR Setup
	// Set timer to trigger every 10 seconds: 80k/800k=10s
	timer = timerBegin(1, 800000000, true);
	timerAttachInterrupt(timer, onTimer, true);
	timerAlarmWrite(timer, 1000000, true);
	timerAlarmEnable(timer);*/
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);
	Serial.println("Setup done");
	delay(4000);   //Delay needed before calling the WiFi.begin
	// Connect to nearest InnovaPost WiFi 
	scanWiFi();
 }

/*void IRAM_ATTR onTime() {
	portENTER_CRITICAL_ISR(&timerMux);
	interrupts++;
	if(WiFi.status() == WL_CONNECTED) {   //Check WiFi connection status
		sendUpdate(MAX_RETRY);
	} else {
		scanWiFi();
	}
	portEXIT_CRITICAL_ISR(&timerMux);
}*/

void loop() {
 
	/*if (interrupts > 0) {
		portENTER_CRITICAL(&timerMux);
		interrupts--;
		portEXIT_CRITICAL(&timerMux);
		totalInterrupts++;
		Serial.print("totalInterrupts");
		Serial.println(totalInterrupts);
	}*/

  if(WiFi.status() == WL_CONNECTED) {   //Check WiFi connection status
    sendUpdate(MAX_RETRY);
  } else {
    scanWiFi();
  }
  delay(5000);
}
