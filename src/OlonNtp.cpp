#include "OlonNtp.h"

#include <time.h>

#include "OlonTimezones.h"
#if defined(ESP8266)
  #include <coredecls.h>  // settimeofday_cb()
  // #include <sntp.h>
#elif defined(ESP32)
  #include "esp_sntp.h"  // sntp_set_time_sync_notification_cb()
  // #include <lwip/apps/sntp.h>
#endif

#define TAG "NTP"

#ifdef OLON_DEBUG
  #define LOGD(tag, format, ...) {                              \
      Serial.printf("\033[0;36m%6lu [D] [%s] ", millis(), tag); \
      Serial.printf(format "\033[0m\n", ##__VA_ARGS__); }
  #define LOGI(tag, format, ...) {                              \
      Serial.printf("\033[0;32m%6lu [I] [%s] ", millis(), tag); \
      Serial.printf(format "\033[0m\n", ##__VA_ARGS__); }
  #define LOGW(tag, format, ...) {                              \
      Serial.printf("\033[0;33m%6lu [W] [%s]", millis(), tag);  \
      Serial.printf(format "\033[0m\n", ##__VA_ARGS__); }
  #define LOGE(tag, format, ...) {                              \
      Serial.printf("\033[0;31m%6lu [E] [%s] ", millis(), tag); \
      Serial.printf(format "\033[0m\n", ##__VA_ARGS__); }
  #define LOGBULK(...) Serial.printf(PSTR("%s"), ##__VA_ARGS__);
#else
  #define LOGD(tag, format, ...)
  #define LOGI(tag, format, ...)
  #define LOGW(tag, format, ...)
  #define LOGE(tag, format, ...)
  #define LOGBULK(...)
#endif

Olon::NtpClass* Olon::NtpClass::_instance = nullptr;

void Olon::NtpClass::timeSyncCallback(struct timeval* tv) {
  if (_instance) {
    _instance->onSync();  // Call the member function onSync() directly
  }
}

Olon::NtpClass::NtpClass() { _instance = this; }

void Olon::NtpClass::onSync() {
  synced = true;
  String tm = getFullDateTimePretty();
  LOGI(TAG, "Time synced: %s", tm.c_str());
  for (uint8_t i = 0; i < _cbEventList.size(); i++) {
    NtpEventCb event = _cbEventList[i];
    if (event) {
      event();
    }
  }
}

void Olon::NtpClass::setServer(const char* ntpServer1, const char* ntpServer2) {
// configure callbacks
#ifdef ESP8266
  settimeofday_cb([this]() { onSync(); });
#else
  // https://techtutorialsx.com/2021/09/03/esp32-sntp-additional-features/
  sntp_set_time_sync_notification_cb(timeSyncCallback);
  // sntp_set_time_sync_notification_cb([](struct timeval *tv) { ((NtpClass
  // *)tv)->onSync();});
#endif

  LOGI(TAG, "Setup server 1: %s Server 2: %s", ntpServer1, ntpServer2);
  configTime(0, 0, ntpServer1, ntpServer2);
  // sntp_init();
  _timezoneCount = sizeof(timezones) / sizeof(timezones[0]);
}

const char* Olon::NtpClass::getTimeZoneFromLocation(const char* location) {
  for (uint16_t i = 0; i < sizeof(timezones) / sizeof(timezones[0]); i++) {
    const TimezoneEntry* tz = &timezones[i];
    if (strcmp_P(location, (const char*)pgm_read_ptr(&tz->location)) == 0) {
      return (const char*)pgm_read_ptr(&tz->timezone);
    }
  }
  return nullptr;
}

void Olon::NtpClass::setLocation(const char* location) {
  const char* timezone = getTimeZoneFromLocation(location);
  if (timezone) {
    LOGI(TAG, "Set location %s", location);
    setTimezone(timezone);
  } else {
    LOGE(TAG, "Set location %s Not Found", location);
  }
}

void Olon::NtpClass::setTimezone(const char* timezone) {
  LOGI(TAG, "Set timezone %s", timezone);
  setenv("TZ", timezone, 1);
  tzset();
}

const char* Olon::NtpClass::getLocationAtIndex(uint32_t index) {
  if (index >= 0 && index < sizeof(timezones) / sizeof(timezones[0])) {
    return (const char*)pgm_read_ptr(&timezones[index].location);
  }
  return nullptr;  // Return nullptr if index is out of bounds
}

String Olon::NtpClass::getFullDateTimePretty() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5)) {
    char timeStringBuff[50];
    // change date format https://cplusplus.com/reference/ctime/strftime/
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %d %B %G %T",
             &timeinfo);
    return timeStringBuff;
    // return asctime(&timeinfo);
  };
  return "Time not set";
}

void Olon::NtpClass::registerEventCallback(NtpEventCb callback) {
  if (callback) {
    _cbEventList.push_back(callback);
  }
}

// void NtpClass::loop() {
//   // using callback instead. Do nothing.
//   return;

//   if (synced) return;

//   struct tm * timeinfo;
//   time_t now;

//   time(&now);
//   localtime_r(&now, timeinfo);
//   if (timeinfo->tm_year > (2022 - 1900)) { // tm_year = years since 1900
//     if (!synced) {
//       synced = true;
//       onSync();
//     }
//   }
// }
