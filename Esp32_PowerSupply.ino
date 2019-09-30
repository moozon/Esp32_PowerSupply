/*
    Name:       Esp32_PowerSupply.ino
    Created:	18.09.2019 1:38:59
    Author:     NETBOOK\Michael
*/

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiGeneric.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SPIFFS.h"
#include <OneButton.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// FTP Server
// To define FTP_DEBUG in ESP8266FtpServer.h for debug
#define FTP_SERVER
#ifdef FTP_SERVER
#include <WiFi.h>
#include "SPIFFS.h"
#include <ESP8266FtpServer.h>
#endif // FTP_SERVER


// Ping
#include <ESP32Ping.h>

// TimeNTP
#include <TimeLib.h>
#include <NTPClientLib.h>
#include "AsyncUDP.h"


// User Define
#define PING_AVERAGE_SIZE 10
#define TASK_STACK_SIZE 2048
#define LED_BUILTIN 22
#define PIN_DAC1 25
#define PIN_DAC2 26
#define PIN_A 32 
#define PIN_B 33
#define PIN_SW 12
#define PIN_SDA 0
#define PIN_SCL 4
#define PULSE_PIN_LEN 5 //	8 минимальная длинна импульса в миллесекундах на которую мы обращаем внимание
#define MENU_ELEMENTS 4	// Колличество элементов на главном экране(0)
#define CUR_POSITIONS 6	// Количество позиций курсора на экране редактирования(1)
#define PATH_CONFIG_FILE "/config.json"

// FreeRTOS Init Begin
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
//#define CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
//#define INCLUDE_vTaskDelete 1
//#define INCLUDE_vTaskSuspend 1
//#define INCLUDE_xTaskResumeFromISR 1
//#define configUSE_TRACE_FACILITY 1
//#define configUSE_STATS_FORMATTING_FUNCTIONS 1
//#define configGENERATE_RUN_TIME_STATS 1 
//#define configUSE_STATS_FORMATTING_FUNCTIONS 1
//#define configGENERATE_RUN_TIME_STATS 1

TaskHandle_t xHandleTaskMain;
TaskHandle_t xHandleTaskBlink;
TaskHandle_t xHandleTaskSerial;
TaskHandle_t xHandleTaskWebServer;
TaskHandle_t xHandleTaskLCD;
TaskHandle_t xHandleTaskDebug;
TaskHandle_t xHandleTaskMeasure;
TaskHandle_t xHandleTaskTest;
TaskHandle_t xHandleTaskCalc;
TaskHandle_t xHandleTaskAPmode;


void taskMain(void *pvParameters);
void taskBlink(void *pvParameters);
void taskSerial(void *pvParameters);
void taskWebServer(void *pvParameters);
void taskLCD(void *pvParameters);
void taskEncoder(void *pvParameters);
void taskDebug(void *pvParameters);
void taskMeasure(void *pvParameters);
void taskTest(void *pvParameters);
void taskCalc(void *pvParameters);
void taskAPmode(void* pvParameters);
// FreeRTOS Init END

// FTP Server Init BEGIN
#ifdef FTP_SERVER
TaskHandle_t xHandleTaskFTPServer;
void taskFTPServer(void* pvParameters);
FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
#endif // FTP_SERVER

// FTP Server Init END

// NTP Init BEGIN
AsyncUDP udp;
// NTP Init END

unsigned int localPort = 8888;  // local port to listen for UDP packets
//const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
//byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
//bool noNTP = false;
//time_t getNtpTime();
//void digitalClockDisplay();
//void printDigits(int digits);
//void sendNTPpacket(IPAddress& address);
//time_t prevDisplay = 0; // when the digital clock was displayed
#define NTP_TIMEOUT 1500
int8_t timeZone = 4;
int8_t minutesTimeZone = 0;
const PROGMEM char* ntpServer = "pool.ntp.org";
//const PROGMEM char* ntpServer = "ru.pool.ntp.org";
//const PROGMEM char* ntpServer = "ntp1.stratum2.ru";
//const PROGMEM char* ntpServer = "us.pool.ntp.org";
//const PROGMEM char* ntpServer = "time.nist.gov";
//const PROGMEM char* ntpServer = "time.windows.com";
//const PROGMEM char* ntpServer = "time-a.timefreq.bldrdoc.gov";
//const PROGMEM char* ntpServer = "time-b.timefreq.bldrdoc.gov";
//const PROGMEM char* ntpServer = "time-c.timefreq.bldrdoc.gov";

struct DateTime
{
	String time;
	String date;
	/*int hour;
	int minute;
	int second;
	int day;
	int month;
	int year;*/
}dateTime;

// NTP Init END


// LCD Init BEGIN
//LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
// LCD Init END

// OneButton Init BEGIN
OneButton oneButton(PIN_SW, true);
// OneButton Init END



// User
volatile unsigned long failingTime = 0;
volatile bool fl = true; // флаг, что нужно вывести на дисплей
volatile bool value_b = 0;
volatile byte prevA = 0;
volatile int Value0 = 0;
volatile int Value1 = 1;
volatile int Value2 = 2;
volatile int Value3 = 3;
volatile unsigned long pulseLen = 0;
volatile bool blink = false;
volatile bool edit = false;
volatile bool move = false;
volatile uint8_t cursor_pos = 0;
volatile uint8_t current_pos = 0;
volatile uint8_t screen = 0;
const uint8_t screen_cnt = 3;
//volatile bool updScr = false;
volatile int ticks1 = 0;
volatile int ticks2 = 0;
volatile char buf[1024];
volatile uint32_t ledBlinkDelay = 1000;
const char* hostName = "esp32";
volatile bool ON = false;
volatile bool updLcd = true;
volatile bool changeScr = false;
volatile bool debug = false;

volatile double u = 5.0;
volatile double uSet = 0.0;
volatile double i = 1.0;
volatile double iSet = 0.0;
volatile int test = 1;

volatile bool uSetOver = false;
volatile bool iSetOver = false;
volatile bool pOver = false;

String debugString = "";


// WiFiSetup
//const char* ssid = "HTC Portable Hotspot";
//const char* password = "1234567890";
//const char* ssid = "MyHomeMts";
//const char* password = "ripas2005";
char ssid[25] = "";
char password[15] = "";
WebServer server(80);
IPAddress ipSTA;
IPAddress ipAP;
bool staInitOk;
String checkClientNetStr;
String APmacStr;
String APmacID;
String STAmacStr;
String STAmacID;
String ipAPstr;
String ipSTAstr;
char AP_NameChar[32];
char AP_PassChar[32];
#define	 WL_MAC_ADDR_LENGTH 6
#define NAME_TAG "ESP"
#define AP_SSID NAME_TAG "32-"
#define AP_PASSWORD_PREFIX "SECRET" // + mac XXXX
#define AP_PASSWORD_SUFFIX ""

struct Config {
	bool APmode;
	bool debug;
	bool blink;
	bool ftpServer;
	double uSet;
	double iSet;
	char ssid[17] = "MyHomeMts";
	char password[17] = "ripas2005";
	//String ssid;
	//String password;
} config;

struct  itemMenu {
	//String name;
	volatile double value;
	volatile double valueSet;
	volatile char name2;
	volatile uint8_t position;
};
//itemMenu Volt = { 4, 1.0, 'V', 0 };
//itemMenu Amp = { 1, 1.0, 'A', 1 };
//itemMenu Watt = { 3, 1.0, 'W', 2 };
//itemMenu Mode = { 0, 1.0, NULL, 3 };

itemMenu menu[MENU_ELEMENTS] = { { 4, 1.0, 'V', 0 }		// Volt
										, { 1, 1.0, 'A', 1 }		// Amp
										, { 3, 1.0, 'W', 2 }		// Watt
										, { 0, 1.0, NULL, 3 } };	// Mode

//volatile boolean ledState = true;
//uint8_t elements[];


void setup() {
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);
	delay(100);
	/*while (!Serial) { // Need for Arduino Micro
	}*/
	Serial.println("Setup Begin");

	//	SPIFFS Setup
	if (!SPIFFS.begin(true)) {
		while (true) {
			Serial.println("Error while mounting SPIFFS");
			delay(500);
		}
	}
	
	// Load configuration from config file from SPIFFS
	//saveConfigsToFile();
	loadConfigsFromFile();

	uSet = config.uSet;
	iSet = config.iSet;
	debug = config.debug;
	blink = config.blink;	

	// AP Mode
	if (config.APmode) {
		createTaskAP();
	}
	else {// STA Mode
		/*if (! config.APmode)
		{
			Serial.print("STA enabled, checking : ");  Serial.println(config.ssid);

			if (checkAPinair(String(config.ssid)))
			{
				WiFi.mode(WIFI_AP_STA);
				softAPinit();
				STAinit();
				Serial.print(millis() / 1000); Serial.println(": AP found, AP_STA mode");
			}
			else
			{
				WiFi.mode(WIFI_AP);
				softAPinit();
				Serial.print(millis() / 1000); Serial.println(": AP not found, AP mode");
			}
		}

		else
		{
			WiFi.mode(WIFI_AP);
			softAPinit();
		}*/
		
		//Serial.println("2");

		// WiFi Setup
		WiFiInit();

		// MDNS Setup
		MDNSInit();

		// LCD Setup
		LCDInit();

		// NTP Setup
		NTPInit();

		// OneButton Setup
		oneButtonInit();

		// Encoder Setup
		encoderInit();

		// FreeRTOS tasks create
		createTasks();
		
		// Обовление задач основываясь на конфиг файл.
		updateConfigs();

		Serial.println("Setup STA End");

		//vTaskGetRunTimeStats(buf);
	}// AP / STA End 
	delay(100); //Чутка ждем
}
// Empty Task runs on Core 1
void loop()
{
	//server.handleClient();
	//delay(1);
	//Serial.println("loop   - " + String(xPortGetCoreID()) + "  Free: " + String(xPortGetFreeHeapSize()));
	//delay(1000);
	//ticks2++;
	//Serial.println(NTP.getTimeDateString());
	timeUpdate();
	Serial.print(".");
	vTaskDelay(1000);
}


// Other methods BEGIN
// Other methods BEGIN