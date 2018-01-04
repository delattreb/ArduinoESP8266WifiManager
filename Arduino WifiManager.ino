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
const int CHAR = 48;

//
// reconnect
//
void callback(char* topic, byte* payload, unsigned int length) {
#ifdef INFO
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();
#endif
}

//
// reconnect
//
void reconnect() {
	while (!client.connected()) {
#ifdef INFO
		Serial.print("Attempting MQTT connection... ");
#endif
		if (client.connect(DEVICE, TOKEN, NULL)) {
#ifdef INFO
			Serial.print("Connected MQTT: ");
			Serial.println(IP_SERVER);
#endif
		}
		else {
#ifdef INFO
			Serial.print("Failed, error:");
			Serial.print(client.state());
			Serial.println(" Try again in 10 seconds");
#endif
			delay(10000);
		}
	}
	Serial.println("WOK");
}

//
// setup
//
void setup()
{
	Serial.begin(SERIALBAUDS);
	//digitalWrite(BUILTIN_LED, LOW);
	WiFiManager wifiManager;
	wifiManager.setAPStaticIPConfig(IPAddress(IPLOWA, IPLOWB, IPLOWC, IPLOWD), IPAddress(IPHIGHA, IPHIGHB, IPHIGHC, IPHIGHD), IPAddress(255, 255, 255, 0));
	if (!wifiManager.autoConnect("Temp/Hum Device")) {
		Serial.println("Failed to connect");
		delay(1000);
		ESP.reset();
		delay(5000);
	}
	client.setServer(thingsboardServer, MQTTPORT);
	client.setCallback(callback);
	reconnect();
#ifdef INFO
	Serial.print("Core version: ");
	Serial.println(ESP.getCoreVersion());
	Serial.print("Sdk version: ");
	Serial.println(ESP.getSdkVersion());
#endif // INFO	//Init sensor
	for (int i = 0; i < (MAXSENSOR * 2); i++)
		sensor[i] = "";
}

//
// loop
//
void loop()
{
	if (!client.connected()) {
		Serial.println("WKO");
		reconnect();
	}
	client.loop();

	// Check if data available
	if (Serial.available())
	{
		data = Serial.readString();
		// get data
		if (data.startsWith("T", 0)) {
			sensor[int(data[1]) - CHAR - 1] = data.substring(3, data.length() - 1);
#ifdef INFO
			Serial.println(data);
#endif 
		}
		if (data.startsWith("H", 0)) {
			sensor[int(data[1]) - CHAR] = data.substring(3, data.length() - 1);
#ifdef INFO
			Serial.println(data);
#endif 
		}
	}

	// Transmit Data
	if (sensor[int(data[1]) - CHAR - 1].length() > 0 && sensor[int(data[1]) - CHAR].length() > 0) {
		sendMQTT(data[1], sensor[int(data[1]) - CHAR - 1], sensor[int(data[1]) - CHAR]);
		sensor[int(data[1]) - CHAR - 1] = "";
		sensor[int(data[1]) - CHAR] = "";
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

#ifdef INFO
	Serial.println("Send to MQTT broker:");
	Serial.println(payload);
#endif 

}

