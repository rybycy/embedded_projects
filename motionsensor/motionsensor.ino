#include <VirtualWire.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define MOTION_CAPTURE_DEVICE_ID 156
#define MOTION_DETECTED 15
//#define HEARTBEAT 31
//#define LED_PORT 8
#define PIR_PORT 2
#define PIR_INTERRUPT_PORT 0

int currentPirState = 0;                    // variable for reading the pin status

void setup() {
  pinMode(PIR_PORT, INPUT);
//  pinMode(LED_PORT, OUTPUT);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(1);
  vw_setup(2000); // speed of data transfer Kbps

//  digitalWrite(LED_PORT, HIGH);
//  delay(3000); // boot PIR
//  digitalWrite(LED_PORT, LOW);
}

//void sendHeartbeat() { sendMessage(HEARTBEAT); }

void sendMessage(int messageId) {
  uint8_t values[] = {
    MOTION_CAPTURE_DEVICE_ID, messageId    };
  vw_send(values, sizeof(values));
  vw_wait_tx(); // Wait until the whole message is gone
}

void handlePirChange() { 
  currentPirState = 1; 
}

void calmDownAndRelax()         // here we put the arduino to sleep
{
  /* 
   * The 5 different modes are:
   *     SLEEP_MODE_IDLE         -the least power savings
   *     SLEEP_MODE_ADC
   *     SLEEP_MODE_PWR_SAVE
   *     SLEEP_MODE_STANDBY
   *     SLEEP_MODE_PWR_DOWN     -the most power savings
   */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

  // disable adc
  ADCSRA = ADCSRA & B01111111;
  ACSR = B10000000;

  // disabled bod

  DIDR0 = DIDR0 | B00111111;
  MCUCR |= (1<<BODS) | (1<<BODSE);
  MCUCR &= ~(1<<BODSE);  // must be done right before sleep

  sleep_enable();
  /* LOW|CHANGE|RISING|FALLING|HIGH */
  attachInterrupt(PIR_INTERRUPT_PORT, handlePirChange, RISING);

  sleep_mode(); // good night

  sleep_disable(); // wake up
  detachInterrupt(PIR_INTERRUPT_PORT);
}

void loop(){
  /* loop-based version
  currentPirState = digitalRead(PIR_PORT);  // read input value
   currentTime = millis();
   
   //  if(previousTime + 3600000UL < currentTime) { // sending heartbeat hourly
   //    sendMessage(HEARTBEAT);
   //    previousTime = currentTime;
   //  }
   
   if (currentPirState == HIGH) {
   digitalWrite(LED_PORT, HIGH);
   delay(200);
   digitalWrite(LED_PORT, LOW);
   }
   */
  calmDownAndRelax();
  if(currentPirState == 1){
//    digitalWrite(LED_PORT, HIGH);
    delay(200);
    currentPirState = 0;
//    digitalWrite(LED_PORT, LOW);
    sendMessage(MOTION_DETECTED);
  }

}


