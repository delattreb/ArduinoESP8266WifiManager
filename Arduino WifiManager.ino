#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include "var.h"

char nodeServer[] = IP_SERVER;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
String data, sensor[MAXSENSOR * 2];
const int CHAR = 48;
int32_t oldRSSI = 0;
static unsigned long previousMillis = 0;
unsigned long currentMillis;

//
// reconnect
//
void callback(char *topic, byte *payload, unsigned int length)
{
}

//
// reconnect
//
void reconnect()
{
	// Connect to MQTT
	if (!client.connected())
	{
#ifdef INFO
		Serial.println("Attempting MQTT connection...");
#endif
		while (!client.connected())
		{
#ifdef INFO
			Serial.print(".");
#endif
			Serial.println("K");
			client.connect(NETWORKNAME, MQTT_USER, MQTT_PWD);
			delay(ATTENPTING);
		}
#ifdef INFO
		Serial.println("");
		Serial.println("Connected");
#endif
		Serial.println("O");
	}
}

//
// setup
//
void setup()
{
	Serial.begin(SERIALBAUDS);
	while (!Serial) continue;
#ifdef INFO
	delay(5000);
	Serial.print("Core version: ");
	Serial.println(ESP.getCoreVersion());
	Serial.print("Sdk version: ");
	Serial.println(ESP.getSdkVersion());
#endif
	WiFiManager wifiManager;
	//Reset setting
	//wifiManager.resetSettings();

	wifiManager.setAPStaticIPConfig(IPAddress(IPLOWA, IPLOWB, IPLOWC, IPLOWD), IPAddress(IPHIGHA, IPHIGHB, IPHIGHC, IPHIGHD), IPAddress(255, 255, 255, 0));
#ifdef WIFIDEBUG
	wifiManager.setDebugOutput(true);
#else
	wifiManager.setDebugOutput(false);
#endif

	if (!wifiManager.autoConnect(NETWORKNAME))
	{
#ifdef DEBUG
		Serial.println("Failed to connect");
#endif
		delay(1000);
		ESP.reset();
		delay(5000);
	}

	client.setServer(nodeServer, MQTTPORT);
	client.setCallback(callback);
	reconnect();
	for (int i = 0; i < (MAXSENSOR * 2); i++)
		sensor[i] = "";
	oldRSSI = WiFi.RSSI();
}

//
// loop
//
void loop()
{
	currentMillis = millis();
	reconnect();
	if (currentMillis - previousMillis >= DB_FREQUENCY)
	{
		if (oldRSSI != WiFi.RSSI())
		{
			Serial.println("S" + String(WiFi.RSSI()));
			oldRSSI = WiFi.RSSI();
		}
		previousMillis = currentMillis;
	}
	if (client.connected())
		client.loop();
	// Check if data available
	if (Serial.available())
	{
		data = Serial.readString();
		// get data
		if (data.startsWith("T", 0))
		{
			sensor[int(data[1]) - CHAR - 1] = data.substring(3, data.length() - 1);
#ifdef DEBUG
			Serial.println(data);
#endif
		}
		if (data.startsWith("H", 0))
		{
			sensor[int(data[1]) - CHAR] = data.substring(3, data.length() - 1);
#ifdef DEBUG
			Serial.println(data);
#endif
		}
	}

	// Transmit Data
	if (sensor[int(data[1]) - CHAR - 1].length() > 0 && sensor[int(data[1]) - CHAR].length() > 0)
	{
		sendMQTT(data[1], sensor[int(data[1]) - CHAR - 1], sensor[int(data[1]) - CHAR]);
		sensor[int(data[1]) - CHAR - 1] = "";
		sensor[int(data[1]) - CHAR] = "";
	}
}

//
// sendMQTT
//
void sendMQTT(char sensor, String temp, String hum)
{
	// Send payload
	String strT = "iot:t";
	String strH = "iot:h";
	char attributest[100];
	char attributesh[100];
	temp.toCharArray(attributest, 100);
	hum.toCharArray(attributesh, 100);
	if (client.connected())
	{
#ifdef DEBUG
		Serial.println("Before send to MQTT broker:");
		Serial.println(temp);
		Serial.println(hum);
		Serial.println(sensor);
		Serial.println(attributest);
		Serial.println(attributesh);
#endif
		strT.concat(sensor);
		strH.concat(sensor);
		client.publish(strT.c_str(), attributest);
		client.publish(strH.c_str(), attributesh);
	}
}
