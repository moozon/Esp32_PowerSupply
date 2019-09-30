// WebServer methods BEGIN
void taskWebServer(void* pvParameters)
{
	// Обарботка запросов
 
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
	server.onNotFound([]() {                                                 // Описываем действия при событии "Не найдено"
		if (!fileReadHandler(server.uri()))                                       // Если функция handleFileRead (описана ниже) возвращает значение false в ответ на поиск файла в файловой системе
			server.send(404, "text/plain", "Not Found");                        // возвращаем на запрос текстовое сообщение "File isn't found" с кодом 404 (не найдено)
		});

	server.begin();

	for (;;)
	{
		server.handleClient();
		vTaskDelay(1);
	}
}

void debugWifiHandler() {
	server.send(200, "text/plain", debugString);
}

void saveConfigHandler() {

}
void saveConfigPostHandler() {
	// функция fetch с методом POST

	//WiFiClient client = server.client();

	//while (client.connected()) {            // loop while the client's connected
	//	if (client.available()) {             // if there's bytes to read from the client,
	//		char c = client.read();             // read a byte, then
	//		Serial.write(c);
	//	}
	//}

	//String jsonStr = server.client().readString();
	//StaticJsonDocument<256> jdoc;
	//serializeJson()
	/*Serial.println(server.arg("ssid"));
	config.ftpServer = server.arg("ftpServer");
	config.APmode = server.arg("APmode");
	config.debug = server.arg("debug");
	config.blink = server.arg("blink");*/
	
	config.ftpServer = (server.arg("ftpServer") == "true") ? true : false;
	config.APmode = (server.arg("APmode") == "true") ? true : false;
	config.debug = (server.arg("debug") == "true") ? true : false;
	config.blink = (server.arg("blink") == "true") ? true : false;
	String s = server.arg("blink");
	Serial.println(s);
	// Submit формы
	/*config.ftpServer = (server.arg("ftpServer") == "on") ? true : false;
	config.APmode = (server.arg("APmode") == "on") ? true : false;
	config.debug = (server.arg("debug") == "on") ? true : false;
	config.blink = (server.arg("blink") == "on") ? true : false;*/
	server.arg("ssid").toCharArray(config.ssid, 25);
	server.arg("password").toCharArray(config.password, 15);
	//config.ssid = server.arg("ssid");
	//config.password = server.arg("password");

	server.sendHeader("Location", "/settings.html");
	server.send(303);
	updLcd = true;
	updateConfigs();
}
void getConfigHandler() {
	String jsonResponse;
	StaticJsonDocument<512> jsonDoc;
	jsonDoc["debug"] = config.debug;
	jsonDoc["blink"] = config.blink;
	jsonDoc["ssid"] = config.ssid;
	jsonDoc["password"] = config.password;
	jsonDoc["APmode"] = config.APmode;
	jsonDoc["ftpServer"] = config.ftpServer;

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
bool fileReadHandler(String path) {                                       // Функция работы с файловой системой
	if (path.endsWith("/")) path += "index.html";                          // Если устройство вызывается по корневому адресу, то должен вызываться файл index.html (добавляем его в конец адреса)
	String contentType = getContentType(path);                            // С помощью функции getContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову
	if (SPIFFS.exists(path)) {                                              // Если в файловой системе существует файл по адресу обращения
		File file = SPIFFS.open(path, "r");                                 //  Открываем файл для чтения
		size_t sent = server.streamFile(file, contentType);                   //  Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
		//server.send(200, "text/plain", "OK");
		file.close();                                                       //  Закрываем файл
		//Serial.println("Spiffs read ok");
		return true;                                                        //  Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
	}
	//Serial.println("Spiffs read err");
	return false;                                                         // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
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
	jsonDoc["blink"] = config.blink;
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
void beforeReset() {
	for (size_t i = 10; i > 1; i--)
	{
		Serial.println("Rebooting after " + String(i) + " seconds.");
		vTaskDelay(1000);
	}
}
String getContentType(String filename) {                                 // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
	if (filename.endsWith(".html")) return "text/html";                   // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
	else if (filename.endsWith(".css")) return "text/css";                // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
	else if (filename.endsWith(".js")) return "application/javascript";   // Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
	else if (filename.endsWith(".png")) return "image/png";               // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
	else if (filename.endsWith(".jpg")) return "image/jpeg";              // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpg" и завершаем выполнение функции
	else if (filename.endsWith(".gif")) return "image/gif";               // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
	else if (filename.endsWith(".ico")) return "image/x-icon";            // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
	return "text/plain";                                                  // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
}
// WebServer methods END