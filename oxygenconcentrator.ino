#include <LiquidCrystal_I2C.h>

#define backgroundReading 148 // mV

LiquidCrystal_I2C lcd(0x27,16,2);
unsigned long time_ms;
unsigned int time_s;
unsigned int time_min;
unsigned int time_hr;
char time_string[14];

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.home();
  
  Serial.begin(9600);
  Serial.println("O2 Concentrator");
  lcd.setCursor(0,0);
  lcd.print("O2 Concentrator");
}

void loop() {
  delay(1000);
  time_ms = millis();
  time_s = time_ms/1000%60;
  time_min = time_ms/1000/60%60;
  time_hr = time_ms/1000/60/60;
  sprintf(time_string, "%02d:%02d:%02d", time_hr, time_min, time_s);
  lcd.setCursor(0,1);
  lcd.print(time_string);

  static float scaleFactor = (float) backgroundReading / 21;
  int sensorValue = analogRead(A0);
  float sensorVoltage = sensorValue * (5000 / 1023);
  float O2_level = sensorVoltage / scaleFactor;

  Serial.print(sensorValue);
  Serial.print("  ");
  Serial.print(scaleFactor);
  Serial.print("  ");
  Serial.print(sensorVoltage);
  Serial.print("  ");
  Serial.println(O2_level);
  lcd.setCursor(10,1);
  lcd.print(String(O2_level,2));
  lcd.print("%");
}
