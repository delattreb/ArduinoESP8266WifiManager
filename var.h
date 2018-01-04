// VAR

//#define INFO

// Device selection
#define HOME
//#define CESI


#define SERIALBAUDS 115200

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
#define IPHIGHD 100
#pragma endregion

#define MQTTPORT 1883
#define MAXSENSOR 5
#define IP_SERVER  "mycube.dscloud.me"

#ifdef HOME
#define TOKEN "5XIw0pg3kLHYGFhcuyS5"
#define DEVICE "HomeTH"
#endif 
#ifdef CESI
#define TOKEN "DhkNy1JRH0lfkd0RccDl"
#define DEVICE "CESITH"
#endif 