// Debug methods BEGIN
void taskDebug(void* pvParameters)
{
	(void)pvParameters;
	//Serial.println("TaskDebug is running");
	if (!config.debug)	vTaskSuspend(xHandleTaskDebug);

	char buf[1024];

	for (;;)
	{
		/*vTaskList(buf);
		Serial.printf(buf);
		Serial.println("/////////////////////////////////////");
		vTaskGetRunTimeStats(buf);
		Serial.printf(buf);
		Serial.println("/////////////////////////////////////");
		debugString = "Free heap : " + String(ESP.getFreeHeap());*/

		//debugPrint("Free heap: " + ESP.getFreeHeap());
		//Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
		//Serial.println();
		//Serial.print("Debug:");
		//Serial.print("	uSet = " + String(menu[0].valueSet));
		//Serial.print("	iSet = " + String(menu[1].valueSet));
		//Serial.print("	cursor_pos = " + String(cursor_pos));


		//debugPrint("uSet = ", String(uSet));
		//debugPrint("cursor_pos = ", String(cursor_pos));
		//Serial.println();
		//Serial.print("Debug: ");
		////vTaskGetRunTimeStats(stats_buf);
		//Serial.println("vTaskGetRunTimeStats: " + String(stats_buf));
		////vTaskList(stats_buf);
		//
		//Serial.println("vTaskList: " + String(stats_buf));
		//TickType_t _ticks = xTaskGetTickCount();
		//Serial.println("xTaskGetTickCount: " + String(_ticks));
		/*Serial.println("Ticks: " + String(ticks1));
		Serial.println("Ticks2: " + String(ticks2));
		Serial.println("Diff: " + String(ticks2 - ticks1));*/
		//debugPrint("Ticks", String(ticks1));

		//Serial.println();
		vTaskDelay(1000);
	}
}
void debugPrint(String str) {
	debugString = str;
	Serial.println();
	Serial.print("Debug:	");
	Serial.println(str);;
	Serial.println();
}
void debugPrint(String str1, String str2) {
	debugString = str1 + " " + str2;
	//Serial.println();
	Serial.print("Debug:	");
	Serial.print(str1);
	Serial.print(str2);
	//Serial.println();
}
// Debug methods END
