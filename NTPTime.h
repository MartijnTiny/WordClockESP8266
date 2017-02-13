
//#define DEFAULT_TIMESERVER "time.nist.gov"
#define DEFAULT_TIMESERVER "de.pool.ntp.org"
#define MINIMUM_INTERVAL 60
#define TIMEZONE 0


#include <Time.h>


TimeChangeRule myDST = {"EDT", Last, Sun, Mar, 1, 120};    //Daylight time = UTC + 2 hours
TimeChangeRule mySTD = {"EST", Last, Sun, Oct, 2, 60};     //Standard time = UTC + 1 hours
Timezone myTZ(myDST, mySTD);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev

unsigned int localPort = 4097;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
byte sendBuffer[] = {
  0b11100011,          // LI, Version, Mode.
  0x0,                 // Stratum unspecified.
  0x6,                 // Polling interval
  0xEC,                // Clock precision.
  0x0, 0x0, 0x0, 0x0}; // Reference ...
time_t getNtpTime();


void sendNTPpacket(WiFiUDP *u) {
  // Zeroise the buffer.
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  memcpy(packetBuffer, sendBuffer, 16);

  if (u->beginPacket(DEFAULT_TIMESERVER, 123)) {
    u->write(packetBuffer, NTP_PACKET_SIZE);
    u->endPacket();
  }
}

time_t getNtpTime()
{
  WiFiUDP udp;
  udp.begin(localPort);
  while (udp.parsePacket() > 0) ; // discard any previously received packets
  for (int i = 0 ; i < 5 ; i++) { // 5 retries.
    sendNTPpacket(&udp);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
      if (udp.parsePacket()) {
         udp.read(packetBuffer, NTP_PACKET_SIZE);
         // Extract seconds portion.
         unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
         unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
         unsigned long secSince1900 = highWord << 16 | lowWord;
         udp.flush();
         return myTZ.toLocal(secSince1900 - 2208988800UL + TIMEZONE * SECS_PER_HOUR,&tcr);
      }
      delay(10);
    }
  }
  return 0; // return 0 if unable to get the time
}

time_t returnTime() {
  time_t curtime = getNtpTime();
  if(curtime!=0)
  {
    return myTZ.toLocal(curtime,&tcr);
  }

  return 0;

}
