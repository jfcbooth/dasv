#include <avr/io.h>
#include <avr/interrupt.h>

// pin mappings
const int colPins[] = {7, A0, A1, 13, A5, A4, A3, A2}; // HIGH for OFF, LOW for ON
const int rowPins[] = {11, 12, 10, 2, 3, 4, 5, 6}; // HIGH for ON, LOW for OFF
int levels[] = {1,2,3,4,5,6,7,8}; // LED level for each column
const byte numChars = 8; //8 bytes
int receivedChars[numChars] = {0, 0, 0, 0, 0, 0, 0, 0}; // an array to store the filtered numbers
boolean newDataAvailableAvailable = false;      //flag new data has been received

//function prototype
void allOff(); // turns all LEDs off
void turnOnLEDs(int col); // makes the physical LEDs reflect the values in levels[]
void colOn( int anode, int level); // turns on column <anode> to <level> (0-7)
void colOff(int anode, int level); // turns off column <anode>
void recnewDataAvailable(); // puts new numbers in receivedChars[]
void updateLevelValues(); // puts the values in receviedChars[] into levels[]

void setup() {
    cli(); // disable interrupts
    
    //set timer1 interrupt at 8000Hz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR1A = 2000;// = (16*10^6) / (1*8000) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1 prescaler
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    sei(); // enable interrupts

  // setup serial communication
  Serial.begin(57600); // was 9600

  for (int i = 0; i < 8; i++) {
    pinMode(rowPins[i], OUTPUT);
    pinMode(colPins[i], OUTPUT);
  }
  allOff(); // makes sure all pins are in output mode

}

// Update row
ISR(TIMER1_COMPA_vect){
  //generates pulse wave of frequency 8000Hz
  static int overflows = 0;
  static int row = 0;
  if(overflows < 8){
    nextCol(row); // turn on the actual LEDs
    row = (row == 8) ? 0 : row+1;
    overflows = 0;
  } else {
    overflows++;
  }

}


void loop() {
  recnewDataAvailable(); // check if new data arrived to populate receivedChars[]
  if(newDataAvailable){ // if new data arrived
    updateLevelValues(); // update the levels array
    newDataAvailable = false;
  }
}



int isValid(char receivedChar){
  return ((receivedChar > 32 && receivedChar < 126) && (receivedChar != '\n') && (receivedChar != ';'));
}

// puts new numbers in receivedChars[]
void recnewDataAvailable() {
  static int ndx = 0; // number of chars received
  char receivedChar;

  if(!Serial.available()) return;
  // if data is available
  cli();//stop interrupts
  receivedChar = Serial.read();
  if(isValid(receivedChar) && ndx < numChars){
    receivedChars[ndx] = (int)(receivedChar - '0'); // found a received character
    ndx++; // number of chars received
  }
  if(ndx >= numChars){
    ndx = 0;
    newDataAvailable = true;
  }
  sei(); //enable interrupts
}


// updates the values in levels[] if all values have been fulfilled
void updateLevelValues() {
  for (int i = 0; i < 8; i++) {
    levels[i] = receivedChars[i];
  }
}

// makes the physical LEDs reflect the values in levels[]
void nextCol(int col) {
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
