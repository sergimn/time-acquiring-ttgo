#include "timeacq.h"

bool date_set;
bool timeacq_use_gps = true;
bool timeacq_use_ntp = true;

void setupGPS(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    Serial1.begin(baud, config, rxPin, txPin);
    SPI.begin(sck, miso, mosi, ss);
    delay(1500);
}


int setUnixtime(int32_t unixtime) {
  timeval epoch = {unixtime, 0};
  return settimeofday((const timeval*)&epoch, 0);
}

void sntp_callback(struct timeval *tv){
  date_set = true;
}

void gps_timeacq(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t sck, int8_t miso, int8_t mosi, int8_t ss){
  timeacq_use_ntp = false;
  timeacq("", "", baud, config, rxPin, txPin, sck, miso, mosi, ss);
}

void ntp_timeacq(const char* ssid, const char* passphrase){
  timeacq_use_gps = false;
  timeacq(ssid, passphrase, 0, 0, 0, 0, 0, 0, 0, 0);
}

void timeacq(const char* ssid, const char* passphrase, unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
  date_set = false;
  unsigned long last = 0UL;

  if(timeacq_use_ntp) WiFi.begin(ssid, passphrase);
  if(timeacq_use_gps) setupGPS(baud, config, rxPin, txPin, sck, miso, mosi, ss);
  TinyGPSPlus gps;
  configTime(3600, 3600, "0.pool.ntp.org", "1.pool.ntp.org");
  if(timeacq_use_ntp) sntp_set_time_sync_notification_cb(sntp_callback);
  while(!date_set){
    if(millis() % 10000000 == 0) Serial.println(date_set); //Without this line the while loop never ends. Idk what's going on, probably some kind of optimisation
    if(timeacq_use_gps){
      while (Serial1.available() > 0)
          gps.encode(Serial1.read());

      if (gps.time.isUpdated() && gps.time.isValid() && (gps.time.hour() != 0 || gps.time.minute() > 22)) {
          uint8_t hour = gps.time.hour();
          uint8_t minute = gps.time.minute();
          uint8_t second = gps.time.second();
          uint32_t fake_unix_time = second + minute * 60 + hour * 3600;
          setUnixtime(fake_unix_time);
          struct  tm timeval;
          getLocalTime(&timeval, 0);
          int8_t diff = timeval.tm_hour - hour;
          setUnixtime(fake_unix_time + diff * 3600);
          
          date_set = true;
      }

      else if (millis() - last > 5000) {
          if (gps.charsProcessed() < 10)
              Serial.println(F("WARNING: No GPS data.  Check wiring."));

          last = millis();
          Serial.println();
      }
    }
  }
  struct tm now;
  getLocalTime(&now,0);
  Serial.println(&now, "%B %d %Y %H:%M:%S");
  if(timeacq_use_gps) Serial1.end();
  if(timeacq_use_ntp) sleep(10); WiFi.disconnect();
}