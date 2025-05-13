#include <Arduino.h>
#include <functional>  // Needed for std::function

namespace Olon {

class NtpClass {
 public:
  using OnSyncCallback = std::function<void()>;

  NtpClass();
  void setServer(const char* ntpServer1, const char* ntpServer2);
  void setLocation(const char* location);
  void setTimezone(const char* timezone);
  const char* getTimeZoneFromLocation(const char* location);
  uint16_t timeZonesCount() { return _timezoneCount; };
  const char* getLocationAtIndex(uint32_t index);
  String getFullDateTimePretty();

  void onSync(OnSyncCallback callback) {
    _onSyncCallback = callback;
  }

  bool synced = false;

 private:
  OnSyncCallback _onSyncCallback = nullptr;
  void syncCallbackTrigger();
  static NtpClass* _instance;
  uint16_t _timezoneCount = 0;
  static void timeSyncCallback(struct timeval *tv);
};

} // namespace Olon
