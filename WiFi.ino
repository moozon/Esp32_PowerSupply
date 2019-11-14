void taskAPmode(void* pvParameters) {
#ifdef DEBUG
	Serial.println("TaskAPmode is running");
#endif // DEBUG

	WiFi.mode(WIFI_AP);
	softAPinit();
	Serial.print("1");
	server.on("/", []() {
		server.sendHeader("Location", "/settings.html");
		server.send(303);
		});
	server.on("/get_config", getConfigHandler);
	server.on("/save_to_file", saveConfigsToFile);
	server.on("/save_config", HTTP_POST, saveConfigPostHandler);
	server.on("/reset", resetHandler);
	//server.on("/a", handleActions);
	//server.on("/c", handleConfig);
	//server.on("/reboot", handleReboot);
	//server.on("/serialcheck", handleSerialCheck);
	server.onNotFound([]() {
		server.send(404, "text/plain", "Not Found\n\n");
		});
	server.on("/settings.html", []() {
		if (fileReadHandler(server.uri())) Serial.println("File sent to client: " + server.uri().substring(1));
		else Serial.println("File not sent to client: " + server.uri().substring(1));
		});

	server.begin();
	for (;;)
	{
		server.handleClient();
		//MDNS..update();
		vTaskDelay(1);
	}
}

void WiFiInit() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(config.ssid, config.password);
	// Wait for connection
	int pause = 0;
	while (WiFi.status() != WL_CONNECTED) {
		if (pause >= 60) {	// Если не удалось подключиться к роутеру в течении 60 сек. Перегружаемся в режим AP
			config.APmode = true;
			saveConfigsToFile();
			Serial.println("Rebooting to AP Mode");
			resetHandler();
		}
		delay(1000);
		Serial.print(".");
		pause++;
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(config.ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

#ifdef DEBUG
	Serial.println("WiFiInit");
#endif // DEBUG

}

void MDNSInit() {
	if (MDNS.begin(hostName)) {
		MDNS.addService("http", "tcp", 80);
		Serial.println("MDNS responder started");
		Serial.print("You can now connect to http://");
		Serial.print(hostName);
		Serial.println(".local");
#ifdef DEBUG
		Serial.println("MDNSInit");
#endif // DEBUG
	}
}

// WiFi methods BEGIN
bool checkAPinair(String name) {
	name.toUpperCase();

	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	//WiFi.mode(WIFI_STA);
	//WiFi.disconnect();
	//Serial.println(name);

	int n = WiFi.scanNetworks();
	//Serial.println("scan done");
	if (n == 0)
	{
		//Serial.println("no networks found");
		return false;
	}

	else
	{
		String nnn;
		//Serial.print(n);    Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
			nnn = WiFi.SSID(i);
			nnn.toUpperCase();
			//Serial.println(nnn);

			if (nnn == name)
			{
				//Serial.print(name);    Serial.print(" network found, RSSI:"); Serial.println(WiFi.RSSI(i));;
				return true;
			}
		}
	}
	return false;

}
void softAPinit() {

	//Serial.println();

	uint8_t mac[WL_MAC_ADDR_LENGTH];
	WiFi.softAPmacAddress(mac);



	APmacStr = String(mac[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 5], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 4], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);


	APmacID = "";

	if (mac[WL_MAC_ADDR_LENGTH - 2] < 0x10)  APmacID = APmacID + "0";
	APmacID = APmacID + String(mac[WL_MAC_ADDR_LENGTH - 2], HEX);

	if (mac[WL_MAC_ADDR_LENGTH - 1] < 0x10)  APmacID = APmacID + "0";
	APmacID = APmacID + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);


	APmacStr.toUpperCase();
	APmacID.toUpperCase();


	String AP_NameString = AP_SSID + APmacID;

	//char AP_NameChar[AP_NameString.length() + 1];
	//char AP_PassChar[AP_PassString.length() + 1];

	for (int i = 0; i < AP_NameString.length(); i++)
		AP_NameChar[i] = AP_NameString.charAt(i);
	AP_NameChar[AP_NameString.length()] = 0;


	/*String AP_PassString = AP_PASSWORD_PREFIX + APmacID + AP_PASSWORD_SUFFIX;

	for (int i = 0; i < AP_PassString.length(); i++)
		AP_PassChar[i] = AP_PassString.charAt(i);
	AP_PassChar[AP_PassString.length()] = 0;*/
	char* AP_PassChar = "1234567890";
	WiFi.softAP(AP_NameChar, AP_PassChar);
	Serial.print("AP SSID: ");  Serial.println(AP_NameChar);
	Serial.print("AP pass: ");  Serial.println(AP_PassChar);

	ipAP = WiFi.softAPIP();

	ipAPstr = String(ipAP[0]) + '.' + String(ipAP[1]) + '.' + String(ipAP[2]) + '.' + String(ipAP[3]);
	checkClientNetStr = String(ipAP[0]) + '.' + String(ipAP[1]) + '.' + String(ipAP[2]) + '.';

	Serial.print("AP IP address: ");
	Serial.println(ipAPstr);

	Serial.print("AP MAC address: ");
	Serial.println(APmacStr);


}
void STAinit() {

	WiFi.begin(config.ssid, config.password);


	uint8_t mac[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(mac);



	STAmacStr = String(mac[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 5], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 4], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + ":" +
		String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);


	STAmacID = "";

	if (mac[WL_MAC_ADDR_LENGTH - 2] < 0x10)  STAmacID = STAmacID + "0";
	STAmacID = STAmacID + String(mac[WL_MAC_ADDR_LENGTH - 2], HEX);

	if (mac[WL_MAC_ADDR_LENGTH - 1] < 0x10)  STAmacID = STAmacID + "0";
	STAmacID = STAmacID + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);



	STAmacStr.toUpperCase();
	STAmacID.toUpperCase();

	Serial.print("STA MAC address: ");
	Serial.println(STAmacStr);

	Serial.print("STA SSID: ");
	Serial.println(config.ssid);
}
// WiFi methods END