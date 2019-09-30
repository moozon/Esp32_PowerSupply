// FTP Server methods BEGIN
void taskFTPServer(void* pvParameters) {
#ifdef FTP_SERVER		

	if (!config.ftpServer) vTaskSuspend(xHandleTaskFTPServer);

	ftpSrv.begin("esp32", "esp32");

	for (;;)
	{
		ftpSrv.handleFTP();
		vTaskDelay(1);
	}
#endif // FTP_SERVER
}
// FTP Server methods END