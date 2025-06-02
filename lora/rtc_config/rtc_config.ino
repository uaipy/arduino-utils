#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Failed to initialize RTC.");
    while (1); // Halt if RTC is not found
  }

  // Set RTC date and time to the time of sketch compilation
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("RTC successfully set!");
}

void loop() {
  DateTime now = rtc.now();

  Serial.print("Current date/time: ");
  Serial.print(now.year(), DEC);
  Serial.print('-');
  Serial.print(now.month(), DEC);
  Serial.print('-');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  delay(1000);
}
