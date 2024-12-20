#include <OlonNtp.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

Olon::NtpClass ntp;

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.begin("Support", "1234567890");

  ntp.setServer("time.google.com", "time.ics.forth.gr");
  ntp.setLocation("Europe/Athens");

  // Example usage of getTimeZone
  const char* tz = ntp.getTimeZoneFromLocation("Europe/Athens");
  if (tz) {
    Serial.printf("Timezone: %s\n", tz);
  } else {
    Serial.printf("TimeZone not found\n");
  }

  // Example usage of getLocationAtIndex
  for (uint16_t i = 0; i < ntp.timeZonesCount(); i++) {
    Serial.println(
        ntp.getLocationAtIndex(i));  // Prints each location in the list
  }
}

void loop() {
  Serial.println(ntp.getFullDateTimePretty());
  delay(10000);
}