#include <Arduino.h>

namespace Olon {

typedef std::function<void()> NtpEventCb;

class NtpClass {
 public:
  NtpClass();
  void setServer(const char* ntpServer1, const char* ntpServer2);
  void setLocation(const char* location);
  void setTimezone(const char* timezone);
  const char* getTimeZoneFromLocation(const char* location);
  uint16_t timeZonesCount() { return _timezoneCount; };
  const char* getLocationAtIndex(uint32_t index);
  String getFullDateTimePretty();
  void registerEventCallback(NtpEventCb callback);
  bool synced = false;
 private:
  void onSync();
  static NtpClass* _instance;
  uint16_t _timezoneCount = 0;
  static void timeSyncCallback(struct timeval *tv);
  std::vector<NtpEventCb> _cbEventList;
};

} // namespace Olon
