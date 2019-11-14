void taskMain(void* pvParameters)
{
#ifdef DEBUG
	Serial.println("TaskMain is running");
#endif // DEBUG
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

#ifdef DEBUG
	Serial.println("TaskMeasure is running");
#endif // DEBUG

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
	
#ifdef DEBUG
	Serial.println("TaskTest is running");
#endif // DEBUG

	for (;;)
	{
		vTaskDelay(1000);
	}
}
void taskCalc(void* pvParameters)
{
	(void)pvParameters;

#ifdef DEBUG
	Serial.println("TaskCalc is running");
#endif // DEBUG

	//vTaskSuspend(xHandleTaskCalc);
	for (;;)
	{
		menu[2].value = menu[0].value * menu[1].value;
		vTaskDelay(10);
	}
}
void taskBlink(void* pvParameters)
{
#ifdef DEBUG
	Serial.println("TaskBlink is running");
#endif // DEBUG
	if (!config.blink) vTaskSuspend(xHandleTaskBlink);
	
	//pinMode(LED_BUILTIN, OUTPUT);
	bool ledState = false;
	for (;;)
	{
		ledState = !ledState;
		digitalWrite(LED_BUILTIN, ledState);
		vTaskDelay(config.blinkDelay);
	}
}
void taskSerial(void* pvParameters)  // This is a task.
{
	(void)pvParameters;

#ifdef DEBUG
	Serial.println("TaskSerial is running");
#endif // DEBUG
	
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
