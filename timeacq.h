#ifndef TIMEACQ
#define TIMEACQ

#include <TinyGPS++.h>

// #define T_BEAM_V07
#define T_BEAM_V10      //same v1.1 version

#include <sys/time.h>

#include <SPI.h>

#include <WiFi.h>




void setupGPS(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
int setUnixtime(int32_t unixtime);
void sntp_callback(struct timeval *tv);
void timeacq(const char* ssid, const char* passphrase, unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
void gps_timeacq(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
void ntp_timeacq(const char* ssid, const char* passphrase);

#endif