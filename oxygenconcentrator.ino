#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define backgroundReading 12.4 // mV
#define LCD
#define I2C_SLAVE_LCD 0x27
#define I2C_SLAVE_ADC 0x48

LiquidCrystal_I2C lcd(I2C_SLAVE_LCD,16,2);
unsigned long time_ms;
unsigned int time_s;
unsigned int time_min;
unsigned int time_hr;
char time_string[14];
float scaleFactor;
int sensorValue, sensorValueL, sensorValueH;
float sensorVoltage;
float adcResolution;
float O2_level;
char O2_level_str[6];

int Sieve_A_Valve = 5;
int Sieve_B_Valve = 6;
int PreCharge_Valve = 8;
//int Compressor = 8;

unsigned long Relay_Test_Delay = 0;
unsigned long Startup_Purge_Delay = 1000;
unsigned long Production_Delay = 4000;
unsigned long Flush_Delay = 450;
unsigned long PreCharge_Delay = 700;

void setup() {
  #ifdef LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.home();
  #endif
  
  Serial.begin(9600);
  Serial.println("O2 Concentrator");

  #ifdef LCD
  lcd.setCursor(0,0);
  lcd.print("O2 Concentrator");
  #endif

  scaleFactor = (float) backgroundReading / 21.0;
  adcResolution = (float) (256.0 / 32767.0);

  Wire.begin();
  Wire.beginTransmission (I2C_SLAVE_ADC);
  Wire.write(0x11);
  Wire.write(0x0E);
  Wire.write(0x85);
  Wire.endTransmission();

  pinMode(Sieve_A_Valve, OUTPUT);
  pinMode(Sieve_B_Valve, OUTPUT);
  pinMode(PreCharge_Valve, OUTPUT);
  //pinMode(Compressor, OUTPUT);

  // STARTUP PURGE
  Serial.println("Relay Test Sequence");
  digitalWrite(Sieve_A_Valve, HIGH); //Turn on relay
  digitalWrite(Sieve_B_Valve, HIGH); //Turn on relay
  digitalWrite(PreCharge_Valve, HIGH); //Turn on relay
  delay(Startup_Purge_Delay);

}

void loop() {
  time_ms = millis();
  time_s = time_ms/1000%60;
  time_min = time_ms/1000/60%60;
  time_hr = time_ms/1000/60/60;
  sprintf(time_string, "%02d:%02d", time_hr, time_min);
  #ifdef LCD
  lcd.setCursor(0,1);
  lcd.print(time_string);
  #endif

  Wire.beginTransmission (I2C_SLAVE_ADC);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(I2C_SLAVE_ADC,2);
  sensorValueH = Wire.read();
  sensorValueL = Wire.read();
  sensorValue = (sensorValueH*256) + sensorValueL;
  sensorVoltage = (float) sensorValue * adcResolution;
  O2_level = sensorVoltage / scaleFactor;

  Serial.print(sensorValue);
  Serial.print("  ");
  Serial.print(sensorVoltage);
  Serial.print("  ");
  Serial.println(O2_level);
  #ifdef LCD
  lcd.setCursor(6,1);
  lcd.print(int(sensorVoltage));
  lcd.print(" ");
  lcd.print(String(O2_level,2));
  lcd.print("%");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  #endif

  //CYCLE 1
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Purge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, LOW);
  digitalWrite(PreCharge_Valve, LOW);
  delay(Production_Delay);


  //CYCLE 2
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Purge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, LOW);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(Flush_Delay) ;


  //CYCLE 3
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Charge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(PreCharge_Delay);

  //CYCLE 4
  //**************************************************************************
  Serial.println("Sieve A Purge / Sieve B Charge");
  digitalWrite(Sieve_A_Valve, LOW);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, LOW);
  delay(Production_Delay);

  //CYCLE 5
  //**************************************************************************
  Serial.println("Sieve A Purge / Sieve B Charge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, LOW);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(Flush_Delay);


  //CYCLE 6
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Charge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(PreCharge_Delay) ;
  
}
