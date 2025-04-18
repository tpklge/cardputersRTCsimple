
#include <M5Cardputer.h>
#include <Wire.h>
#include <RTClib.h>

RTC_PCF8563 rtc;
bool editing = false;
int selectedField = 0;
int inputBuffer = -1;
bool menuNeedsUpdate = true;

int tempHour, tempMinute, tempSecond, tempDay, tempMonth, tempYear;

const char* diasSemana[] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};
const char* meses[] = {"Janeiro", "Fevereiro", "Marco", "Abril", "Maio", "Junho",
                       "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};

void drawClock() {
  M5Cardputer.Display.clear();
  DateTime now = rtc.now();

  M5Cardputer.Display.setTextSize(4);
  char timeStr[16];
  sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  int w = M5Cardputer.Display.textWidth(timeStr);
  int h = M5Cardputer.Display.fontHeight();
  int x_center = (M5Cardputer.Display.width() - w) / 2;
  int y_center = (M5Cardputer.Display.height() - h) / 3;
  M5Cardputer.Display.setCursor(x_center, y_center);
  M5Cardputer.Display.println(timeStr);

  char dateStr[64];
  sprintf(dateStr, "%s, %02d de %s de %d",
    diasSemana[now.dayOfTheWeek()],
    now.day(),
    meses[now.month() - 1],
    now.year());

  M5Cardputer.Display.setTextSize(2);
  int dw = M5Cardputer.Display.textWidth(dateStr);
  int dh = M5Cardputer.Display.fontHeight();
  M5Cardputer.Display.setCursor((M5Cardputer.Display.width() - dw) / 2, y_center + 50);
  M5Cardputer.Display.println(dateStr);
}

void drawMenu() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setCursor(10, 10);
  M5Cardputer.Display.println("Digite o valor (2 dig):");
  M5Cardputer.Display.setCursor(10, 40);
  M5Cardputer.Display.printf("%s HORA   : %02d\n", selectedField == 0 ? ">" : " ", tempHour);
  M5Cardputer.Display.printf("%s MINUTO : %02d\n", selectedField == 1 ? ">" : " ", tempMinute);
  M5Cardputer.Display.printf("%s SEGUNDO: %02d\n", selectedField == 2 ? ">" : " ", tempSecond);
  M5Cardputer.Display.printf("%s DIA    : %02d\n", selectedField == 3 ? ">" : " ", tempDay);
  M5Cardputer.Display.printf("%s MES    : %02d\n", selectedField == 4 ? ">" : " ", tempMonth);
  M5Cardputer.Display.printf("%s ANO    : 20%02d\n", selectedField == 5 ? ">" : " ", tempYear);
  M5Cardputer.Display.setCursor(10, 200);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.print("w/s: Navega | 0-9: Digita | Enter: Salvar");
}

void applyChanges() {
  rtc.adjust(DateTime(2000 + tempYear, tempMonth, tempDay, tempHour, tempMinute, tempSecond));
}

void processDigitInput(int digit) {
  if (inputBuffer == -1) {
    inputBuffer = digit;
  } else {
    int fullValue = inputBuffer * 10 + digit;
    switch (selectedField) {
      case 0: tempHour   = constrain(fullValue, 0, 23); break;
      case 1: tempMinute = constrain(fullValue, 0, 59); break;
      case 2: tempSecond = constrain(fullValue, 0, 59); break;
      case 3: tempDay    = constrain(fullValue, 1, 31); break;
      case 4: tempMonth  = constrain(fullValue, 1, 12); break;
      case 5: tempYear   = constrain(fullValue, 0, 99); break;
    }
    inputBuffer = -1;
    menuNeedsUpdate = true;
  }
}

void setup() {
  M5Cardputer.begin();
  Wire.begin();
  rtc.begin();

  DateTime now = rtc.now();
  tempHour = now.hour();
  tempMinute = now.minute();
  tempSecond = now.second();
  tempDay = now.day();
  tempMonth = now.month();
  tempYear = now.year() % 100;

  menuNeedsUpdate = true;
}

void loop() {
  M5Cardputer.update();
  auto status = M5Cardputer.Keyboard.keysState();

  if (editing) {
    if (menuNeedsUpdate) {
      drawMenu();
      menuNeedsUpdate = false;
    }

    if (M5Cardputer.Keyboard.isKeyPressed('w')) {
      selectedField = (selectedField + 5) % 6;
      inputBuffer = -1;
      menuNeedsUpdate = true;
      delay(200);
    }

    if (M5Cardputer.Keyboard.isKeyPressed('s')) {
      selectedField = (selectedField + 1) % 6;
      inputBuffer = -1;
      menuNeedsUpdate = true;
      delay(200);
    }

    for (char c = '0'; c <= '9'; ++c) {
      if (M5Cardputer.Keyboard.isKeyPressed(c)) {
        processDigitInput(c - '0');
        delay(200);
      }
    }

    if (status.enter) {
      applyChanges();
      editing = false;
      inputBuffer = -1;
      delay(500);
    }
  } else {
    drawClock();

    if (status.enter) {
      editing = true;
      inputBuffer = -1;
      menuNeedsUpdate = true;
      delay(500);
    }

    delay(500);
  }
}
