#include <VirtualWire.h>
#include <LiquidCrystal.h>

#define MESSAGE_TIMEOUT_MS 21000
#define WEATHER_STATION_ID 155

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
  lcd.begin(16, 2);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(2);
  vw_setup(4000);
  lcd.setCursor(0, 0);
  lcd.print("Waiting for the");
  lcd.setCursor(0, 1);
  lcd.print("weather station");
  vw_rx_start();       // Start the receiver PLL running
}

const char* myPrintf(const char *fmt, ... ) {
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  return buf;
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  vw_wait_rx_max(MESSAGE_TIMEOUT_MS); // let's block until there's a message
  if (vw_get_message(buf, &buflen)) {// && (buf[0] == WEATHER_STATION_ID)) {
    lcd.clear();
    int id = buf[0];
    int t = buf[1];
    int h = buf[2];
    lcd.setCursor(0, 0);
    lcd.print(myPrintf("T: %d, H: %d%%", t, h, id));
    lcd.setCursor(0, 1);
    lcd.print(myPrintf("%d", id));
  } else {
    lcd.setCursor(0, 1);
    lcd.print("No response...");
  }
}
