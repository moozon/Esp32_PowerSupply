// FTP Server methods BEGIN
void taskFTPServer(void* pvParameters) {
#ifdef FTP_SERVER
#ifdef DEBUG
	Serial.println("TaskFTPServer is running");
#endif // DEBUG

	if (!config.ftpServer) vTaskSuspend(xHandleTaskFTPServer);

	ftpSrv.begin("esp32", "esp32");

	for (;;)
	{
		ftpSrv.handleFTP();
		vTaskDelay(1);
	}
#endif // DEBUG
}
// FTP Server methods END