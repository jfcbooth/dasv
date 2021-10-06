#include <avr/io.h>
#include <avr/interrupt.h>
#include <AltSoftSerial.h>
#include <Arduino.h>

//IntervalTimer myTimer;
AltSoftSerial BT;

// pin mappings
const int colPins[] = {7, A0, A1, 13, A5, A4, A3, A2}; // HIGH for OFF, LOW for ON
const int rowPins[] = {11, 12, 10, 2, 3, 4, 5, 6}; // HIGH for ON, LOW for OFF
int levels[] = {1,2,3,4,5,6,7,8}; // LED level for each column
const byte numChars = 8;      //8 bytes + null
int receivedChars[numChars+1] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // an array to store the filtered numbers
boolean newData = false;      //flag new data has been received

//function prototype
void allOff(); // turns all LEDs off
void turnOnLEDs(int col); // makes the physical LEDs reflect the values in levels[]
void colOn( int anode, int level); // turns on column <anode> to <level> (0-7)
void colOff(int anode, int level); // turns off column <anode>
void recNewData(); // puts new numbers in receivedChars[]
void updateLevels(); // puts the values in receviedChars[] into levels[]

//void interr(void){
//  static int row = 0;
//  turnOnLEDs(row); // turn on the actual LEDs
//  row = (row == 8) ? 0 : row+1;
//}

void setup() {
  //myTimer.begin(interr, 1500);
    cli(); // disable interrupts
    // these two lines make an interrupt occur when the 8-bit timer reaches its max value (255)
    TCCR0A = 0b00000000;
    TCCR0B = 0b00000001;
    TIMSK0 = 0b00000001; // trigger interrupt when timer overflows
    TCNT0  = 0;//initialize counter value to 0
    sei(); // enable interrupts

//    //set timer1 interrupt at 8000Hz
//    TCCR1A = 0;// set entire TCCR1A register to 0
//    TCCR1B = 0;// same for TCCR1B
//    TCNT1  = 0;//initialize counter value to 0
//    // set compare match register for 1hz increments
//    OCR1A = 2000;// = (16*10^6) / (1*8000) - 1 (must be <65536)
//    // turn on CTC mode
//    TCCR1B |= (1 << WGM12);
//    // Set CS10 and CS12 bits for 1 prescaler
//    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
//    // enable timer compare interrupt
//    TIMSK1 |= (1 << OCIE1A);
  
  // setup serial communication
  Serial.begin(57600); // was 9600
  BT.begin(9600);  // set all pins to output mode
  
  for (int i = 0; i < 8; i++) {
    pinMode(rowPins[i], OUTPUT);
    pinMode(colPins[i], OUTPUT);
  }
  allOff(); // makes sure all pins are in output mode

}

//// Update row
//ISR(TIMER1_COMPA_vect){
////generates pulse wave of frequency 8000Hz
//  static int row = 0;
//  turnOnLEDs(row); // turn on the actual LEDs
//  row = (row == 8) ? 0 : row+1;
//}
ISR(TIMER0_OVF){
  static int overflows = 0;
  static int row = 0;
  if(overflows < 8){
    turnOnLEDs(row); // turn on the actual LEDs
    row = (row == 8) ? 0 : row+1;
    overflows = 0;
  } else {
    overflows++;
  }
  
}



void loop() {
//  recNewData(); // check if new data arrived to populate receivedChars[]
//  if(newData == true){ // if new data arrived
//    updateLevels(); // update the levels array
//    newData = false;
//  }
  turnOnLEDs(1);
  delay(1000);
}


//
//int isValid(char receivedChar){
//  return ((receivedChar > 32 && receivedChar < 126)); //&& (receivedChar != '\n') && (receivedChar != ';'));
//}
//
//// puts new numbers in receivedChars[]
//void recNewData() {
//  static int ndx = 0; // number of chars received
//  char receivedChar;
//
//  if(BT.available() < 1) return;
//  // if data is available
//  cli();//stop interrupts
//  receivedChar = BT.read();
//  if(BT.overflow()){
//    Serial.println("Overflow occured\n");
//  }
//  if(isValid(receivedChar)){// && ndx < numChars){
//    Serial.println(receivedChar);
//    receivedChars[ndx] = (int)(receivedChar - '0'); // found a received character
//    ndx++; // number of chars received
//  }
//  if(ndx >= numChars){
//    receivedChars[ndx] = '\0'; // terminate the string
//    ndx = 0;
//    newData = true;
//    Serial.println("Finished row\n");
//  }
//  sei(); //enable interrupts
//}
//
//
//// updates the values in levels[] if all values have been fulfilled
//void updateLevels() {
//  for (int i = 0; i < 8; i++) {
//    levels[i] = receivedChars[i];
//  }
//  for (int i = 0; i < 8; i++) {
//    Serial.print(levels[i], DEC);
//    Serial.print("\t");    // prints a tab
//  }
//  Serial.print("\n");
//}

// makes the physical LEDs reflect the values in levels[]
void turnOnLEDs(int col) {
  // colOff statements turn off the previous column of LEDs
  if (col == 0) {
    colOff(colPins[7]);
  }
  else {
    colOff(colPins[col - 1]);
  }
  colOn(colPins[col], levels[col]);
}

// turns off all LEDs
void allOff() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(rowPins[i], LOW);
    digitalWrite(colPins[i], HIGH);
  }
}

// turns on column <anode> to <level> (0-7)
void colOn(int anode, int level) {
  if (level) {
    for (int i = 0; i < level; i++) {
      digitalWrite(rowPins[i], HIGH);
    }
    digitalWrite(anode, LOW);
  }
}

// turns off column <anode>
void colOff(int anode) {
  digitalWrite(anode, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(rowPins[i], LOW);
  }
}
