#include <VirtualWire.h>
#include "DHT.h"

/*
  Vcc >>>>5V
  ATAD>>>DHTPIN
  Gnd >>> Gnd
*/
#define DHTTYPE DHT11
#define DHTPIN 2
#define MEASUREMENT_INTERVAL_MS 5000
#define WEATHER_STATION_ID 155

DHT dht(DHTPIN, DHTTYPE);
uint8_t measurementId = 0; // incremental
uint8_t failureWait = 1;

void setup() {
  dht.begin();
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(1);
  vw_setup(4000); // speed of data transfer Kbps
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  // Read temperature as Celsius

  if (isnan(humidity) || isnan(temperature)) {
    // don't send anything, execute the exponential backoff
    delay(failureWait * 1000);
    failureWait = (failureWait * 2);
  } else {
    failureWait = 1;
    uint8_t values[] = {WEATHER_STATION_ID, measurementId, (uint8_t)temperature, (uint8_t)humidity};
    vw_send(values, sizeof(values));
    measurementId++;
    vw_wait_tx(); // Wait until the whole message is gone
    delay(MEASUREMENT_INTERVAL_MS);
  }

}


