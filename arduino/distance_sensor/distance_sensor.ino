#define trigPin 7
#define echoPin 6
#define FIRST_CLK 3
#define FIRST_DATA 2
#define SECOND_CLK 12
#define SECOND_DATA 11
#define measurementMsDelay 200


// idx = number
byte digitPins[10]= {1, 1+2+4+16+32, 2+64, 2+16, 4+16+32, 8+16, 8, 1+2+16+32, 0, 16};

/* Make serial port debugging available */
void initSerial(){
  Serial.begin(9600);
}

void initLed() {
  pinMode(FIRST_CLK, OUTPUT);
  pinMode(FIRST_DATA, OUTPUT);
  pinMode(SECOND_CLK, OUTPUT);
  pinMode(SECOND_DATA, OUTPUT);
}

/* Emulate printf to serial */
#include <stdarg.h>
void serialPrintf(const char *fmt, ... ){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.print(buf);
}

// using global pin variables
long getMeasuredDistanceInCentimeters() {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  //distance = (duration/2) / 29.1;
  
  // Formula: uS / 58 = centimeters or uS / 148 =inch; or: the range = high level time * velocity (340M/S) / 2
  distance = (duration / 2) / 29.1; 
  return distance;
}

int changeNthBit(int n, int currentValue, bool newBitValue)
{
    if(currentValue & (newBitValue << n)) // if set already
        return currentValue & (~0 ^ (newBitValue << n)); // then force with and
    else // else or
        return currentValue | (newBitValue << n); 
}


void writeDigit(int clk, int data, int number, bool withHundreds = false) {
  serialPrintf("writing digit %i with hundreds %i\n", number, withHundreds);
  int numberBits = ~digitPins[number];
  int finalBits = changeNthBit(7, numberBits, !withHundreds);
  serialPrintf("Write number %i %i\n", numberBits, finalBits);
  shiftOut(data, clk, MSBFIRST, finalBits); // digitOne
}

void writeNumber(long number) {
  
  int decimalDigit = (number / 10) % 10;
  int unitDigit = number % 10;

  int isUnitDot = (number / 100) % 2;
  int isDecimalDot = int(number / 100) == 2;

//  serialPrintf("Is dot? %i %i\n", isDecimalDot, isUnitDot);
  writeDigit(FIRST_CLK, FIRST_DATA, decimalDigit, isDecimalDot);
  writeDigit(SECOND_CLK, SECOND_DATA, unitDigit, isUnitDot);
}

void setup() {
  initSerial();
  initLed();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  long distance = getMeasuredDistanceInCentimeters();
  serialPrintf("Distance: %ld\n", distance);
  writeNumber(distance);
  delay(measurementMsDelay);
}
