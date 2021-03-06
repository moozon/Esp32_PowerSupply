// WebServer methods BEGIN
void taskWebServer(void* pvParameters)
{
#ifdef DEBUG
	Serial.println("TaskWebServer is running");
#endif // DEBUG
	// ��������� ��������
 
	//server.on("/update_firmware_page", HTTP_GET, updateFirmwarePageHandler);
	server.on("/update_firmware", HTTP_POST, postUpdateFirmwareHandler, updateFirmwareHandler);
	server.on("/get_debug_wifi", debugWifiHandler);
	server.on("/led_switch", ledSwitchHandler);
	server.on("/get_values", getValuesHandler);
	server.on("/set_u", setUHandler);
	server.on("/set_i", setIHandler);
	server.on("/inc_u", incUHandler);
	server.on("/dec_u", decUHandler);
	server.on("/inc_i", incIHandler);
	server.on("/dec_i", decIHandler);
	server.on("/on", onHandler);
	server.on("/reset", resetHandler);
	server.on("/sleep", sleepHandler);
	server.on("/save_settings", saveSettingsHandler);
	server.on("/get_config", getConfigHandler);
	server.on("/save_to_file", saveConfigsToFile);
	server.on("/save_config", HTTP_POST, saveConfigPostHandler);
	server.on("/save_config", saveConfigHandler);
	/*server.on("/settings.html", []() {
		if (fileReadHandler(server.uri())) Serial.println("File sent to client: " + server.uri().substring(1));
		else Serial.println("File not sent to client: " + server.uri().substring(1));
		});*/
	server.on("/style.css", []() {
		if (fileReadHandler(server.uri())) Serial.println("File sent to client: " + server.uri().substring(1));
		else Serial.println("File not sent to client: " + server.uri().substring(1));
		});
	server.on("/script.js", []() {
		if (fileReadHandler(server.uri())) Serial.println("File sent to client: " + server.uri().substring(1));
		else Serial.println("File not sent to client: " + server.uri().substring(1));
		});
	server.onNotFound([]() {                                                 // ��������� �������� ��� ������� "�� �������"
		if (!fileReadHandler(server.uri()))                                       // ���� ������� handleFileRead (������� ����) ���������� �������� false � ����� �� ����� ����� � �������� �������
			server.send(404, "text/plain", "Not Found");                        // ���������� �� ������ ��������� ��������� "File isn't found" � ����� 404 (�� �������)
		});

	server.begin();

	for (;;)
	{
		server.handleClient();
		vTaskDelay(1);
	}
}

//void updateFirmwarePageHandler() {
//	server.sendHeader("Connection", "close");
//	server.send(200, "text/html", updateIndex);
//}
void updateFirmwareHandler() {
	uint32_t blinkDelayOld = config.blinkDelay;
	config.blinkDelay = 50;
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		Serial.setDebugOutput(true);
		Serial.printf("Update: %s\n", upload.filename.c_str());
		if (!Update.begin()) { //start with max available size
			Update.printError(Serial);
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
			Update.printError(Serial);
		}
	}
	else if (upload.status == UPLOAD_FILE_END) {
		if (Update.end(true)) { //true to set the size to the current progress
			Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
		}
		else {
			Update.printError(Serial);
		}
		Serial.setDebugOutput(false);
	}
	else {
		Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
	}
	config.blinkDelay = blinkDelayOld;
}

void postUpdateFirmwareHandler() {
	if (Update.hasError()) {
		server.sendHeader("Connection", "close");
		server.send(200, "text/plain", "FAIL");
	}
	else { 
		server.sendHeader("Connection", "close");
		server.send(200, "text/plain", "Update completed successfully. Please reload the webpage(Press F5)");
	
	}
	//delay(500);
	//Serial.println(F("Esp32 will now is rebooting"));
	ESP.restart();

	/*server.sendHeader("Connection", "close");
	server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
	ESP.restart();*/
}

void debugWifiHandler() {
	server.send(200, "text/plain", debugWiFiStr);
}

void saveConfigHandler() {

}
void saveConfigPostHandler() {
	// ������� fetch � ������� POST

	String json = server.arg("plain");
	server.sendHeader("Location", "/settings.html");
	server.send(303);
	
	StaticJsonDocument<512> jsonDoc;
	DeserializationError error = deserializeJson(jsonDoc, json);
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		return;
	}

	config.ftpServer = jsonDoc["ftpServer"];
	config.APmode = jsonDoc["APmode"];
	config.debug = jsonDoc["debug"];
	config.debugWiFi = jsonDoc["debugWiFi"];
	config.blink = jsonDoc["blink"];
	config.blinkDelay = jsonDoc["blinkDelay"];
	const char *ssid = jsonDoc["ssid"];
	const char *password = jsonDoc["password"];
	
	config.ssid = (char*)ssid;
	config.password = (char*)password;
	
	// Submit �����
	/*config.ftpServer = (server.arg("ftpServer") == "on") ? true : false;
	config.APmode = (server.arg("APmode") == "on") ? true : false;
	config.debug = (server.arg("debug") == "on") ? true : false;
	config.blink = (server.arg("blink") == "on") ? true : false;*/
	//server.arg("ssid").toCharArray(config.ssid, 25);
	//server.arg("password").toCharArray(config.password, 15);
	////config.ssid = server.arg("ssid");
	////config.password = server.arg("password");

	
	updLcd = true;
	updateConfigs();
}
void getConfigHandler() {
	String jsonResponse;
	StaticJsonDocument<512> jsonDoc;
	jsonDoc["debug"] = config.debug;
	jsonDoc["debugWiFi"] = config.debugWiFi;
	jsonDoc["blink"] = config.blink;
	jsonDoc["ssid"] = config.ssid;
	jsonDoc["password"] = config.password;
	jsonDoc["APmode"] = config.APmode;
	jsonDoc["ftpServer"] = config.ftpServer;
	jsonDoc["blinkDelay"] = config.blinkDelay;

	serializeJson(jsonDoc, jsonResponse);

	server.send(200, "text/plain", jsonResponse);
}
void saveSettingsHandler() {
	debug = (server.arg("debug") == "true") ? true : false;
	if (debug) vTaskResume(xHandleTaskDebug);
	else vTaskSuspend(xHandleTaskDebug);
	//Serial.println(String(ON));
	server.send(200, "text/plain", "OK");
	updLcd = true;
}
void rootHandler() {
	digitalWrite(LED_BUILTIN, HIGH);
	server.send(200, "text/plain", "hello from esp8266!");
	digitalWrite(LED_BUILTIN, LOW);
}
void notFoundHandler() {
	digitalWrite(LED_BUILTIN, HIGH);
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
	digitalWrite(LED_BUILTIN, LOW);
}
bool fileReadHandler(String path) {                                       // ������� ������ � �������� ��������
	if (path.endsWith("/")) path += "index.html";                          // ���� ���������� ���������� �� ��������� ������, �� ������ ���������� ���� index.html (��������� ��� � ����� ������)
	String contentType = getContentType(path);                            // � ������� ������� getContentType (������� ����) ���������� �� ���� ����� (� ������ ���������) ����� ��������� ���������� ���������� �� ��� ������
	if (SPIFFS.exists(path)) {                                              // ���� � �������� ������� ���������� ���� �� ������ ���������
		File file = SPIFFS.open(path, "r");                                 //  ��������� ���� ��� ������
		size_t sent = server.streamFile(file, contentType);                   //  ������� ���������� ����� �� HTTP, �������� ��������� ���� ����������� contentType
		//server.send(200, "text/plain", "OK");
		file.close();                                                       //  ��������� ����
		//Serial.println("Spiffs read ok");
		return true;                                                        //  ��������� ���������� �������, ��������� ����������� �� ���������� true (������)
	}
	//Serial.println("Spiffs read err");
	return false;                                                         // ��������� ���������� �������, ��������� ����������� �� ���������� false (���� �� ������������ ���������� �������)
}
void sendFileHandler(String path) {
	fileReadHandler(path);
}
void ledSwitchHandler() {
	if (digitalRead(LED_BUILTIN) == HIGH) {
		digitalWrite(LED_BUILTIN, LOW);
		server.send(200, "text/plain", "OK");
	}
	else {
		digitalWrite(LED_BUILTIN, HIGH);
		server.send(200, "text/plain", "ERR");
	}
}
void getValuesHandler() {
	//Serial.println("Fetch");
	String jsonResponse;
	StaticJsonDocument<512> jsonDoc;
	jsonDoc["U"] = (menu[0].value);
	jsonDoc["I"] = (menu[1].value);
	jsonDoc["P"] = (menu[2].value);
	jsonDoc["USet"] = (menu[0].valueSet);
	jsonDoc["ISet"] = (menu[1].valueSet);
	jsonDoc["ON"] = ON;
	jsonDoc["debug"] = config.debug;
	jsonDoc["debugWiFi"] = config.debugWiFi;
	jsonDoc["blink"] = config.blink;
	jsonDoc["blinkDelay"] = config.blinkDelay;
	//String time = (String(dateTime.hour) + ':' + String(dateTime.minute) + ':' + String(dateTime.second));
	//String date = (String(dateTime.day) + ':' + String(dateTime.month) + ':' + String(dateTime.year));
	jsonDoc["time"] = dateTime.time;
	jsonDoc["date"] = dateTime.date;

	serializeJson(jsonDoc, jsonResponse);

	server.send(200, "text/plain", jsonResponse);
}
void setUHandler() {
	if (setU(server.arg("u").toDouble()))
		server.send(200, "text/plain", "OK");
	else
		server.send(200, "text/plain", "ERROR. Failed to set voltage");
	updLcd = true;
}
void setIHandler() {
	if (setI(server.arg("i").toDouble()))
		server.send(200, "text/plain", "OK");
	else
		server.send(200, "text/plain", "ERROR. Failed to set current");
	updLcd = true;
}
void incUHandler() {
	if (incU(0))
		server.send(200, "text/plain", "OK");
	else
		server.send(200, "text/plain", "ERROR. Failed to increase voltage");
	updLcd = true;
}
void decUHandler() {
	if (decU(0))
		server.send(200, "text/plain", "OK");
	else
		server.send(200, "text/plain", "ERROR. Failed to decrease voltage");
	updLcd = true;
}
void incIHandler() {
	if (incI(0))
		server.send(200, "text/plain", "OK");
	else
		server.send(200, "text/plain", "ERROR. Failed to increase current");
	updLcd = true;
}
void decIHandler() {
	if (decI(0))
		server.send(200, "text/plain", "OK");
	else
		server.send(200, "text/plain", "ERROR. Failed to decrease current");
	updLcd = true;
}
void onHandler() {
	ON = (server.arg("on") == "true") ? true : false;
	//Serial.println(String(ON));
	server.send(200, "text/plain", "OK");
	updLcd = true;
}
void resetHandler() {
	esp_register_shutdown_handler(beforeReset);
	esp_restart();
}
void sleepHandler() {
	int seconds = server.arg("sleep").toInt();
	debugPrint("Going to deep sleep");
	esp_deep_sleep(seconds * 1000000);
}

void beforeReset() {
	for (size_t i = 10; i > 1; i--)
	{
		Serial.println("Rebooting after " + String(i) + " seconds.");
		vTaskDelay(1000);
	}
}
String getContentType(String filename) {                                 // �������, ������������ ����������� ��������� ���� ����������� � ����������� �� ���������� �����
	if (filename.endsWith(".html")) return "text/html";                   // ���� ���� ������������� �� ".html", �� ���������� ��������� "text/html" � ��������� ���������� �������
	else if (filename.endsWith(".css")) return "text/css";                // ���� ���� ������������� �� ".css", �� ���������� ��������� "text/css" � ��������� ���������� �������
	else if (filename.endsWith(".js")) return "application/javascript";   // ���� ���� ������������� �� ".js", �� ���������� ��������� "application/javascript" � ��������� ���������� �������
	else if (filename.endsWith(".png")) return "image/png";               // ���� ���� ������������� �� ".png", �� ���������� ��������� "image/png" � ��������� ���������� �������
	else if (filename.endsWith(".jpg")) return "image/jpeg";              // ���� ���� ������������� �� ".jpg", �� ���������� ��������� "image/jpg" � ��������� ���������� �������
	else if (filename.endsWith(".gif")) return "image/gif";               // ���� ���� ������������� �� ".gif", �� ���������� ��������� "image/gif" � ��������� ���������� �������
	else if (filename.endsWith(".ico")) return "image/x-icon";            // ���� ���� ������������� �� ".ico", �� ���������� ��������� "image/x-icon" � ��������� ���������� �������
	return "text/plain";                                                  // ���� �� ���� �� ����� ����� �� ������, �� ������� ��� ���������� ����� ���������, ������ ��������������� ��������� � ��������� ���������� �������
}
// WebServer methods END