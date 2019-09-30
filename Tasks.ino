void taskMain(void* pvParameters)
{
	//Serial.println("TaskMain is running");
	for (;;)
	{
		//Serial.println("taskMain - " + String(xPortGetCoreID()) + "  Free: " + String(xPortGetFreeHeapSize()));
		oneButton.tick();
		ticks1++;
		vTaskDelay(1);
	}
}
void taskMeasure(void* pvParameters)
{
	(void)pvParameters;
	Serial.println("TaskMeasure is running");
	for (;;)
	{
		vTaskDelay(1000);
	}
	Serial.println("Ending task 1");
	vTaskDelete(NULL);
}
void taskTest(void* pvParameters)
{
	(void)pvParameters;
	//Serial.println("TaskTest is running");
	for (;;)
	{
		vTaskDelay(1000);
	}
}
void taskCalc(void* pvParameters)
{
	(void)pvParameters;
	//Serial.println("TaskTest is running");
	//vTaskSuspend(xHandleTaskCalc);
	for (;;)
	{
		menu[2].value = menu[0].value * menu[1].value;
		vTaskDelay(10);
	}
}
void taskBlink(void* pvParameters)
{
	if (!config.blink) vTaskSuspend(xHandleTaskBlink);
	//Serial.println(F("TaskBlink is running"));
	//pinMode(LED_BUILTIN, OUTPUT);
	bool ledState = false;
	for (;;)
	{
		ledState = !ledState;
		digitalWrite(LED_BUILTIN, ledState);
		vTaskDelay(ledBlinkDelay);
	}
}
void taskSerial(void* pvParameters)  // This is a task.
{
	(void)pvParameters;
	//Serial.println("TaskSerial is running");
	for (;;)
	{
		if (Serial.available())
		{
			//Serial.println("Serial.available");
			int rec = Serial.read();
			Serial.read();	// Дочитываем пустые данные
			Serial.read();	// Дочитываем пустые данные
			if (rec == '0') {
				Serial.println("TaskBlink is Suspend");
				vTaskSuspend(xHandleTaskBlink);
			}
			if (rec == '1') {
				Serial.println("TaskBlink is Resume");
				vTaskResume(xHandleTaskBlink);
			}
			if (rec == '2') {
				Serial.println("TaskDebug is Suspend");
				vTaskSuspend(xHandleTaskDebug);
			}
			if (rec == '3') {
				Serial.println("TaskDebug is Resume");
				vTaskResume(xHandleTaskDebug);
			}
			if (rec == '4') {
				Serial.println();
				Serial.print("Print Serial");
				Serial.println();
				//vTaskList(pcWriteBuffer);
				//Serial.println(pcWriteBuffer);
				//vTaskDelay(100 / portTICK_PERIOD_MS);
				//Serial.println("vTaskList2");

			}
			if (rec == '5') {
				if (xHandleTaskTest)
				{
					vTaskDelete(xHandleTaskTest);
					Serial.println("TaskTest has been deleted");
					xHandleTaskTest = NULL;
				}
				else
				{
					Serial.println("TaskTest has been deleted already");
				}
			}
		}
		vTaskDelay(100);
	}
}
