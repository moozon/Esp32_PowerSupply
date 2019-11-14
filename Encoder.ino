void taskEncoder(void* pvParameters)
{
	(void)pvParameters;

#ifdef DEBUG
	Serial.println("TaskEncoder is running");
#endif // DEBUG

	for (;;)
	{
		vTaskDelay(1000);
	}
}
// Encoder Init
void encoderInit() {
	pinMode(PIN_SW, INPUT_PULLUP);
	pinMode(PIN_A, INPUT_PULLUP);
	pinMode(PIN_B, INPUT_PULLUP);
	digitalWrite(PIN_A, HIGH);
	digitalWrite(PIN_B, HIGH);
	attachInterrupt(PIN_A, handler_a, CHANGE);
#ifdef DEBUG
	Serial.println("encoderInit");
#endif // DEBUG
}
void oneButtonInit() {
	oneButton.attachClick(click);
	oneButton.attachDoubleClick(doubleClick);
	oneButton.attachLongPressStart(longPressStart); // ������ ����������� �������
	//oneButton.attachLongPressStop(longPressStop); // ����� ����������� �������	
	//oneButton.attachDuringLongPress(duringLongPress); // ������������ ������� ��� ���������	
	//oneButton.setDebounceTicks(100000);

#ifdef DEBUG
	Serial.println("oneButtonInit");
#endif // DEBUG
}
// Encoder methods BEGIN
void handler_a() {
	byte A = digitalRead(PIN_A);
	//if (!fl) { // ���� �� ���������� ������ ������ �� ������
	if (prevA && !A) { // ����� ����
		value_b = digitalRead(PIN_B); // ���������� �����������, �� ���� ������ "��������� ���"
		failingTime = millis(); // � ��������� ����� �� "����� � ����", ������ ������ ����� ��������
	}
	if (!prevA && A && failingTime) { // ���������� ����� � �� � ������ "������ ������� ��������
		pulseLen = millis() - failingTime;
		if (pulseLen > PULSE_PIN_LEN) { // ������� �� ���������� ������� ���-�� �������� ���� ��� �� ��������� � ��� ������
			//Serial.println("Encoder");
			updLcd = true;
			fl = true; // �������� �������, ��� ����� �������� ����� 
			if (value_b) { // ����������
				//Serial.println("+++");
				if (screen == 1) {
					if (edit) {	// ���� ������� ����� ��������������
						if (cursor_pos == 0)  incU(0);
						if (cursor_pos == 1)  incU(1);
						if (cursor_pos == 2)  incU(2);
						if (cursor_pos == 3)  incI(0);
						if (cursor_pos == 4)  incI(1);
						if (cursor_pos == 5)  incI(2);
						//if (menu[0].valueSet < 30) { //�������� ���������������� I �� ����������
						//	if (cursor_pos == 3) {
						//		double tmp = menu[1].valueSet;
						//		tmp++;
						//		if (tmp >= 10) menu[1].valueSet = 10;
						//		else menu[1].valueSet = tmp;
						//	}
						//	if (cursor_pos == 4) menu[1].valueSet += 0.1;
						//	if (cursor_pos == 5) menu[1].valueSet += 0.01;
						//}
						//if (menu[1].valueSet < 10) { //�������� ���������������� I �� ����������
						//	if (cursor_pos == 3) {
						//		double tmp = menu[1].valueSet;
						//		tmp++;
						//		if (tmp >= 10) menu[1].valueSet = 10;
						//		else menu[1].valueSet = tmp;
						//	}
						//	if (cursor_pos == 4) menu[1].valueSet += 0.1;
						//	if (cursor_pos == 5) menu[1].valueSet += 0.01;
						//}
					}
					else {	// ����������� ������� ������
						if (cursor_pos < (CUR_POSITIONS - 1)) {
							cursor_pos++;
						}
						else {
							cursor_pos = 0;
						}
					}
				}
				//if (move && !edit) {
				//	if (cursor_pos < MENU_ELEMENTS - 1) {
				//		cursor_pos++;
				//	}
				//	else {
				//		cursor_pos = 0;
				//	}
				//	current_pos = cursor_pos;
				//	//return;
				//}
				//if (edit) {
				//	//elements[cursor_pos]++;
				//	menu[cursor_pos].value = menu[cursor_pos].value + 0.01;						

				//	/*if (cursor_pos == 0) Value0++;
				//	if (cursor_pos == 1) Value1++;
				//	if (cursor_pos == 2) Value2++;
				//	if (cursor_pos == 3) Value3++;*/
				//}
			}

			else { // ����������
				//Serial.println("---");
				if (screen == 1) {
					if (edit) {	// ���� ������� ����� ��������������
						if (cursor_pos == 0)  decU(0);
						if (cursor_pos == 1)  decU(1);
						if (cursor_pos == 2)  decU(2);
						if (cursor_pos == 3)  decI(0);
						if (cursor_pos == 4)  decI(1);
						if (cursor_pos == 5)  decI(2);
						//if (menu[0].valueSet > 0) { //�������� ���������������� U �� 0
						//	if (cursor_pos == 0) {// ��������� U �� ����� 
						//		double tmp = menu[0].valueSet;
						//		tmp--;
						//		if (tmp <= 0) menu[0].valueSet = 0;
						//		else menu[0].valueSet = tmp;
						//	}
						//	if (cursor_pos == 1) menu[0].valueSet -= 0.1;
						//	if (cursor_pos == 2) menu[0].valueSet -= 0.01;
						//}
						//if (menu[1].valueSet > 0) { // �������� ���������������� I �� 0
						//	if (cursor_pos == 3) {	// ��������� I �� ����� 
						//		double tmp = menu[1].valueSet;
						//		tmp--;
						//		if (tmp <= 0) menu[1].valueSet = 0;
						//		else menu[1].valueSet = tmp;
						//	}
						//	if (cursor_pos == 4) menu[1].valueSet -= 0.1;
						//	if (cursor_pos == 5) menu[1].valueSet -= 0.01;
						//}
					}
					else {	// ����������� ������� �����
						if (cursor_pos > 0) {
							cursor_pos--;
						}
						else {
							cursor_pos = (CUR_POSITIONS - 1);
						}
					}
				}
			}
		}
		failingTime = 0; // ������ �� ����� ����� ������� ��������
	}
	//}

	prevA = A;
}
void click() {
	/*if (move) {
		if (!edit) {
			edit = true;
			lcd.blink();
		}
		else {
			edit = false;
			lcd.noBlink();
		}
	}*/

	if (screen == 1 && !edit) {
		edit = true;
		lcd.blink();
	}
	else if (screen == 1 && edit) {
		edit = false;
		lcd.noBlink();
	}
}
void doubleClick() {
	if (screen == 0) {
		if (ON) { ON = false; }
		else { ON = true; }
	}
	updLcd = true;
	//if (!edit) {
	//	lcd.noBlink();
	//	lcd.noCursor();
	//	if (screen < (screen_cnt - 1)) {
	//		screen++;
	//	}
	//	else screen = 0;
	//	//updScr = true;
	//	fl = true;
	//	//lcd.clear();
	//	//lcd.print("Double Click");
	//}
}
void longPressStart() {
	updLcd = true;
	changeScr = true;
	if (screen == 0) {
		//Serial.println("Long press");
		screen = 1;
		//lcd.cursor();
		//cursor_pos = 0;
		//fl = true;
		//updLcd = true;
		/*if (move) {
			move = false;
			edit = false;
			blink = false;
			lcd.noBlink();
			lcd.noCursor();
		}
		else {
			move = true;
			blink = true;
			lcd.home();
			lcd.cursor();
		}*/
	}
	else if (screen == 1) {
		screen = 0;
		//lcd.noCursor();
		//fl = true;
		//updLcd = true;
	}

}
void longPressStop() {
	lcd.clear();
	lcd.print("Long Press Stop");
}
void duringLongPress() {
	lcd.clear();
	lcd.print("Multiple Long Press");
}
// Encoder methods END