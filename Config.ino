// ConfigFile methods BEGIN
bool loadConfigsFromFile() {
	File file;

	file = SPIFFS.open(PATH_CONFIG_FILE, FILE_READ);
	if (!file) {
		Serial.println(F("Failed to read file"));
		return false;
	}
	StaticJsonDocument<512> jsonDoc;
	DeserializationError error = deserializeJson(jsonDoc, file);
	if (error) {
		Serial.println(F("Failed to read file, using default configuration"));
		return false;
	}

	config.ftpServer = jsonDoc["ftpServer"];
	Serial.println("FtpServer load from config file: " + String(config.ftpServer));
	config.APmode = jsonDoc["APmode"];
	Serial.println("APmode load from config file: " + String(config.APmode));
	config.debug = jsonDoc["debug"];
	Serial.println("Debug load from config file: " + String(config.debug));
	config.blink = jsonDoc["blink"];
	Serial.println("Blink load from config file: " + String(config.blink));
	config.uSet = jsonDoc["setU"];
	Serial.println("uSet load from config file: " + String(config.uSet));
	config.iSet = jsonDoc["setI"];
	Serial.println("iSet load from config file: " + String(config.iSet));

	strlcpy(config.ssid, jsonDoc["ssid"], sizeof(config.ssid));
	//strncpy(config.ssid, jsonDoc["ssid"], sizeof(config.ssid));
	Serial.println("Ssid load from config file: " + String(config.ssid));
	strlcpy(config.password, jsonDoc["password"], sizeof(config.password));
	//strncpy(config.password, jsonDoc["password"], sizeof(config.password));
	Serial.println("Password load from config file: " + String(config.password));

	/*const char* s = jsonDoc["ssid"];
	config.ssid = s;
	const char* p = jsonDoc["password"];
	config.password = (char*)p;*/

	// Узнаем размер файла:
	//Serial.print("File size: ");
	//Serial.println(file.size());

	file.close();
	return true;
}
bool saveConfigsToFile() {
	File file;

	SPIFFS.remove(PATH_CONFIG_FILE);

	file = SPIFFS.open(PATH_CONFIG_FILE, FILE_WRITE);
	if (!file) {
		Serial.println(F("Failed to open file"));
		return false;
	}
	StaticJsonDocument<512> jsonDoc;

	jsonDoc["APmode"] = config.APmode;
	jsonDoc["debug"] = config.debug;
	jsonDoc["blink"] = config.blink;
	jsonDoc["setU"] = config.uSet;
	jsonDoc["setI"] = config.iSet;
	jsonDoc["ssid"] = config.ssid;
	jsonDoc["password"] = config.password;
	jsonDoc["ftpServer"] = config.ftpServer;

	// Serialize JSON to file
	if (serializeJson(jsonDoc, file) == 0) {
		Serial.println(F("Failed to write to file"));
		return false;
	}
	Serial.println("Save settings to file is succesfully.");
	server.send(200, "text/plain", "OK");
	file.close();
	return true;
}

void updateConfigs() {
	if (!config.APmode && (xHandleTaskAPmode == NULL)) {	// Проверка на режим AP, иначе падает т.к задачи не созданы  
		if (!config.debug) vTaskSuspend(xHandleTaskDebug);
		else vTaskResume(xHandleTaskDebug);
		if (!config.blink) vTaskSuspend(xHandleTaskBlink);
		else vTaskResume(xHandleTaskBlink);
#ifdef FTP_SERVER
		if (!config.ftpServer) vTaskSuspend(xHandleTaskFTPServer);
		else vTaskResume(xHandleTaskFTPServer);
#endif // DEBUG

		
	}
}
// ConfigFile methods END
