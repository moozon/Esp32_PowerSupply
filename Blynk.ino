//#define BLYNK_DEFAULT_DOMAIN     "moozon.ddns.info"
//#define BLYNK_DEFAULT_PORT       8080
//#define BLYNK_DEFAULT_PORT_SSL   443

void blynkInit() {
	char auth[] = "96a99b2e59c044f49a432c88ee2d6546";

	// Attach virtual serial terminal to Virtual Pin V1
	WidgetTerminal terminal(V0);

	Blynk.begin(auth);
	//Blynk.begin(auth, config.ssid, config.password);
}

void taskBlynk(void* pvParameters)
{
#ifdef DEBUG
	Serial.println("TaskBlynk is running");
#endif // DEBUG
	for (;;)
	{
		Blynk.run();
		vTaskDelay(1);
	}
}
BLYNK_WRITE(V0)
{
	String s = param.asStr();
	Serial.println(s);
	Serial.flush();
}
BLYNK_WRITE(V1)
{
	int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
	// String i = param.asStr();
	// double d = param.asDouble();

	digitalWrite(LED_BUILTIN, (uint8_t)pinValue);
	Serial.println("Blynk Pin Value: " + (String)pinValue);	
}
