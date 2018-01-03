#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   
#include <PubSubClient.h>
#include "var.h"

char thingsboardServer[] = IP_SERVER;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
String data, sensor[MAXSENSOR * 2];

//
// reconnect
//
void reconnect() {
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection... ");
		if (client.connect(DEVICE1, TOKEN1, NULL)) {
			Serial.print("Connected MQTT: ");
			Serial.println(IP_SERVER);
		}
		else {
			Serial.print("Failed, error:");
			Serial.print(client.state());
			Serial.println(" Try again in 5 seconds");
			delay(5000);
		}
	}
}

//
// setup
//
void setup()
{
	Serial.begin(SERIALBAUDS);

	WiFiManager wifiManager;
	//wifiManager.resetSettings();
	if (!wifiManager.autoConnect("Temp/Hum Device")) {
		Serial.println("Failed to connect");
		delay(1000);
		ESP.reset();
		delay(5000);
	}
	Serial.println("Connected");
	// Connect to MQTT broker
	client.setServer(thingsboardServer, MQTTPORT);
	reconnect();
	//Send connexion to WiFi OK or KO
	sendWiFiStateConnexion();
	//Init sensor
	for (int i = 0; i < (MAXSENSOR * 2); i++)
		sensor[i] = "";
}

//
// loop
//
void loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	// Check if data available
	if (Serial.available())
	{
		data = Serial.readString();
		// get data
		if (data.startsWith("T", 0)) {
			sensor[int(data[1]) - 48 - 1] = data.substring(3, data.length() - 1);
#ifdef INFO
			Serial.println(data);
#endif 
		}
		if (data.startsWith("H", 0)) {
			sensor[int(data[1]) - 48] = data.substring(3, data.length() - 1);
#ifdef INFO
			Serial.println(data);
#endif 
		}
	}

	// Transmit Data
	if (sensor[int(data[1]) - 48 - 1].length() > 0 && sensor[int(data[1]) - 48].length() > 0) {
		sendMQTT(data[1], sensor[int(data[1]) - 48 - 1], sensor[int(data[1]) - 48]);
		sensor[int(data[1]) - 48 - 1] = "";
		sensor[int(data[1]) - 48] = "";
	}
}

//
// sendMQTT
//
void sendMQTT(char sensor, String temp, String hum) {
	// Prepare a JSON payload string
	String payload = "{";
	payload += "\"temperature";
	payload += sensor;
	payload += "\":";
	payload += temp;
	payload += ",";

	payload += "\"humidity";
	payload += sensor;
	payload += "\":";
	payload += hum;
	payload += "}";

	// Send payload
	char attributes[100];
	payload.toCharArray(attributes, 100);
	client.publish("v1/devices/me/telemetry", attributes);
	client.publish("v1/devices/me/attributes", attributes);

	//Send connexion to WiFi OK or KO
	sendWiFiStateConnexion();
#ifdef INFO
	Serial.println("Send to MQTT broker:");
	Serial.println(payload);
#endif 

}

//
// sendWiFiconnexion
//
void sendWiFiStateConnexion()
{
	if (client.connected())
		Serial.println("WOK");
	else
		Serial.println("WKO");
}