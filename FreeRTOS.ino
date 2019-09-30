// FreeRTOS methods BEGIN
void createTasks() {
	xTaskCreatePinnedToCore(taskMain, "TaskMain", 2048, NULL, 1, &xHandleTaskMain, ARDUINO_RUNNING_CORE);
	xTaskCreatePinnedToCore(taskLCD, "TaskLCD", 2048, NULL, 1, &xHandleTaskLCD, ARDUINO_RUNNING_CORE);
	xTaskCreatePinnedToCore(taskWebServer, "TaskWebServer", 10000, NULL, 1, &xHandleTaskWebServer, ARDUINO_RUNNING_CORE);
	xTaskCreatePinnedToCore(taskSerial, "TaskSerial", TASK_STACK_SIZE, NULL, 1, &xHandleTaskSerial, ARDUINO_RUNNING_CORE);
	xTaskCreatePinnedToCore(taskDebug, "TaskDebug", 4096, NULL, 1, &xHandleTaskDebug, ARDUINO_RUNNING_CORE);
	xTaskCreatePinnedToCore(taskBlink, "TaskBlink", 512, NULL, 1, &xHandleTaskBlink, ARDUINO_RUNNING_CORE);

#ifdef FTP_SERVER
	xTaskCreatePinnedToCore(taskFTPServer, "TaskFTPServer", 4096, NULL, 1, &xHandleTaskFTPServer, ARDUINO_RUNNING_CORE);
#endif // FTP_SERVER

	//xTaskCreatePinnedToCore(taskEncoder, "TaskEncoder", TASK_STACK_SIZE, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
	//xTaskCreatePinnedToCore(taskMeasure, "TaskMeasure", TASK_STACK_SIZE, NULL, 1, &xHandleTaskMeasure, ARDUINO_RUNNING_CORE);
	//xTaskCreatePinnedToCore(taskTest, "TaskTest", TASK_STACK_SIZE, NULL, 2, &xHandleTaskTest, ARDUINO_RUNNING_CORE);
	//xTaskCreatePinnedToCore(taskCalc, "TaskCalc", TASK_STACK_SIZE, NULL, 2, &xHandleTaskCalc, ARDUINO_RUNNING_CORE);
}
void createTaskAP() {
	xTaskCreatePinnedToCore(taskAPmode, "TaskAPmode", 10000, NULL, 1, &xHandleTaskAPmode, ARDUINO_RUNNING_CORE);
	Serial.println("Setup softAP End");
}
// FreeRTOS methods END
