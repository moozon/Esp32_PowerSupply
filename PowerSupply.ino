// PS methods BEGIN
bool setU(double value) {
	if ((value >= 0) && (value <= 30)) {
		menu[0].valueSet = value;
		return true;
	}
	return false;
}
bool setI(double value) {
	if ((value >= 0) && (value <= 10)) {
		menu[1].valueSet = value;
		return true;
	}
	return false;
}
bool incU(uint8_t position) {
	//Serial.println("incU");
	if (menu[0].valueSet < 30) { //Проверка устанавливаемого U на превышение
		if (position == 0) {
			double tmp = menu[0].valueSet;
			tmp++;
			if (tmp >= 30) menu[0].valueSet = 30;
			else menu[0].valueSet = tmp;
		}
		if (position == 1) menu[0].valueSet += 0.1;
		if (position == 2) menu[0].valueSet += 0.01;
		return true;
	}
	return false;
}
bool decU(uint8_t position) {
	//Serial.println("decU");
	if (menu[0].valueSet > 0) { //Проверка устанавливаемого U на 0
		if (position == 0) {// Проверяем U на минус 
			double tmp = menu[0].valueSet;
			tmp--;
			if (tmp <= 0) menu[0].valueSet = 0;
			else menu[0].valueSet = tmp;
		}
		if (position == 1) menu[0].valueSet -= 0.1;
		if (position == 2) menu[0].valueSet -= 0.01;
		return true;
	}
	return false;
}
bool incI(uint8_t position) {
	//Serial.println("incI");
	if (menu[1].valueSet < 10) {
		if (position == 0) {
			double tmp = menu[1].valueSet;
			tmp++;
			if (tmp >= 10) menu[1].valueSet = 10;
			else menu[1].valueSet = tmp;
		}
		if (position == 1) menu[1].valueSet += 0.1;
		if (position == 2) menu[1].valueSet += 0.01;
		return true;
	}
	return false;
}
bool decI(uint8_t position) {
	//Serial.println("decI");
	if (menu[1].valueSet > 0) { // Проверка устанавливаемого I на 0
		if (position == 0) {	// Проверяем I на минус 
			double tmp = menu[1].valueSet;
			tmp--;
			if (tmp <= 0) menu[1].valueSet = 0;
			else menu[1].valueSet = tmp;
		}
		if (position == 1) menu[1].valueSet -= 0.1;
		if (position == 2) menu[1].valueSet -= 0.01;
		return true;
	}
	return false;
}
// PS methods END