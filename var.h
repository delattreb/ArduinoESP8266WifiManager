// VAR

//#define INFO
//#define DEBUG
//#define WIFIDEBUG

// Device selection
#define SERIALBAUDS 9600

//#define NETWORKNAME "ESP Bureau"
//#define NETWORKNAME "ESP Sejour"
//#define NETWORKNAME "ESP Cave"
#define NETWORKNAME "ESP Nomade"
#define ATTENPTING 2000

#pragma region IPDEFINITION
// IP Start
#define IPLOWA 10
#define IPLOWB 0
#define IPLOWC 0
#define IPLOWD 1

// IP End
#define IPHIGHA 10
#define IPHIGHB 0
#define IPHIGHC 0
#define IPHIGHD 1
#pragma endregion

#define MQTTPORT 1883
#define MAXSENSOR 5
#define DB_FREQUENCY 3000
#define IP_SERVER "192.169.1.46"

// MQTT authentification
#define MQTT_USER "dietpi"
#define MQTT_PWD "infected"

// MQTT Topic
#define TOPIC_IOT "data"