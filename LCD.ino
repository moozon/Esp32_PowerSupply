void LCDInit() {
	Wire.begin(PIN_SDA, PIN_SCL);	// Для софтверного I2C в ESP32
	lcd.init();
	lcd.backlight();
	lcd.noAutoscroll();

#ifdef DEBUG
	Serial.println("LCDInit");
#endif // DEBUG
}

// LCD methods BEGIN
void taskLCD(void* pvParameters)
{
	(void)pvParameters;
#ifdef DEBUG
	Serial.println("TaskLCD is running");
#endif // DEBUG

	for (;;)
	{
		lcdUpdate();
		// Если больше 10, добавляем дополнительное место на дисплее 
		if (menu[0].valueSet >= 10) uSetOver = true;
		else uSetOver = false;
		if (menu[1].valueSet >= 10) iSetOver = true;
		else iSetOver = false;
		vTaskDelay(1);
	}
}
void lcdUpdate() {
	if (updLcd)
	{
		switch (screen)
		{
		case 0:
			if (changeScr) lcd.clear();
			lcd.home();
			lcd.noCursor();
			lcd.print("U:");
			lcd.print(menu[0].value);
			if (!uSetOver) lcd.print(' ');
			lcd.print(' ');
			lcd.print(menu[0].name2);
			lcd.setCursor(10, 0);
			lcd.print("I:");
			lcd.print(menu[1].value);
			if (!iSetOver) lcd.print(' ');
			lcd.print(' ');
			lcd.print(menu[1].name2);
			lcd.setCursor(0, 1);
			lcd.print("P:");
			lcd.print(menu[2].value);
			if (!pOver) lcd.print(' ');
			lcd.print(' ');
			lcd.print(menu[2].name2);
			lcd.setCursor(10, 1);
			lcd.print("Load: ");
			if (ON) lcd.print("ON ");
			else lcd.print("OFF");
			//lcd.print(Value3);
			//lcd.print(menu[3].name);
			//lcd.print(menu[3].value);
			//lcd.print(Mode.name2);

			/*if (cursor_pos == 0) lcd.setCursor(0, 0);
			if (cursor_pos == 1) lcd.setCursor(8, 0);
			if (cursor_pos == 2) lcd.setCursor(0, 1);
			if (cursor_pos == 3) lcd.setCursor(8, 1);*/

			//fl = false;
			updLcd = false;
			changeScr = false;
			vTaskDelay(100);
			break;
		case 1:
			//menu[0].valueSet = menu[0].value
			if (changeScr) lcd.clear();
			lcd.home();
			lcd.cursor();
			//cursor_pos = 0; 
			lcd.print("Set:");
			lcd.setCursor(6, 0);
			lcd.print("U=");
			lcd.print(menu[0].valueSet);
			//lcd.print(test);
			//lcd.print(menu[0].valueSet);
			if (!uSetOver) lcd.print(' ');
			lcd.print(' ');
			lcd.print(menu[0].name2);
			lcd.setCursor(6, 1);
			lcd.print("I=");
			lcd.print(menu[1].valueSet);
			//lcd.print(menu[1].valueSet);
			if (!iSetOver) lcd.print(' ');
			lcd.print(' ');
			lcd.print(menu[1].name2);
			if (cursor_pos == 0) {
				lcd.setCursor(8, 0);
			}
			if (cursor_pos == 1) {	//Проверка на десятичность
				if (!uSetOver) lcd.setCursor(10, 0);
				else lcd.setCursor(11, 0);
			}
			if (cursor_pos == 2) {
				if (!uSetOver) lcd.setCursor(11, 0);
				else lcd.setCursor(12, 0);
			}
			if (cursor_pos == 3) {
				lcd.setCursor(8, 1);
			}
			if (cursor_pos == 4) {	//Проверка на десятичность
				if (!iSetOver) lcd.setCursor(10, 1);
				else lcd.setCursor(11, 1);
			}
			if (cursor_pos == 5) {
				if (!iSetOver) lcd.setCursor(11, 1);
				else lcd.setCursor(12, 1);
			}

			//vTaskDelay(100);
			//vTaskSuspendAll();
			//fl = false;
			updLcd = false;
			changeScr = false;
			//xTaskResumeAll();
			break;
		case 2:
			if (changeScr) lcd.clear();
			lcd.home();
			lcd.print("Ticks1: ");
			lcd.print(ticks1);
			/*lcd.setCursor(8, 0);
			lcd.print("Diff: ");
			lcd.print(ticks2 - ticks1);*/
			lcd.setCursor(0, 1);
			lcd.print("Ticks2: ");
			lcd.print(ticks2);
			vTaskDelay(1000);

			//fl = true;
			updLcd = false;
			changeScr = false;
			break;
		default:
			lcd.clear();
			lcd.print("Exception!!!");
			//fl = false;
			break;
		}

	}
}
// LCD methods END