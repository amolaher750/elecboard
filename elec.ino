#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	Serial.println();

	StaticJsonDocument<200> doc;
	DeserializationError error = deserializeJson(doc, payload);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	const char* command = doc["cmd"];
	long button = doc["button"];
	const char* action = doc["action"];

	Serial.print("[");
	Serial.print(command);
	Serial.print(", ");
	Serial.print(button);
	Serial.print(", ");
	Serial.print(action);
	Serial.print("]");
	Serial.println();
	// add toggle logic here
}

void reconnect() {
	Serial.println("reconnecting");
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.println("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("b123")) {
			Serial.println("connected");
			client.subscribe("elec/boards/123");
		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void setup(void){ 
	WiFi.mode(WIFI_STA);

	Serial.begin(9600);

	WiFiManager wm;

	bool res;
	res = wm.autoConnect("Elec Board 123", "qwertyui");

	if (!res) {
		Serial.println("Failed to connect");
	} else {
		Serial.println("WiFi connected");
		client.setServer("test.mosquitto.org", 1883);
		client.setCallback(callback); 
	}
}

void loop(){
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
}