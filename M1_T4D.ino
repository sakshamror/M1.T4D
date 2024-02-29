#include <avr/io.h>
#include <avr/interrupt.h>

const int pirSensorPin1 = 2;          // Digital input pin for PIR sensor 1
const int pirSensorPin2 = 3;          // Digital input pin for PIR sensor 2
const int soilMoisturePin = A0;       // Analog input pin for soil moisture sensor
const int pirLedPin = 13;              // Digital output pin for PIR sensor 1 LED
const int pirLedPin2 = 12;             // Digital output pin for PIR sensor 2 LED
const int moistureLedPin = 11;         // Digital output pin for soil moisture LED

volatile int motionFlag1 = 0;
volatile int motionFlag2 = 0;
volatile int lastMoistureLevel = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pirSensorPin1, INPUT);
  pinMode(pirSensorPin2, INPUT);
  pinMode(soilMoisturePin, INPUT);
  pinMode(pirLedPin, OUTPUT);
  pinMode(pirLedPin2, OUTPUT);
  pinMode(moistureLedPin, OUTPUT);

  // Enable pin change interrupt for PIR sensor pins
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);  // Enable interrupts for pins 2 and 3
  PCICR |= (1 << PCIE2); // Enable PCINT for port D

  // Enable pin change interrupt for soil moisture pin
  PCMSK0 |= (1 << PCINT0); // Enable interrupt for pin A0
  PCICR |= (1 << PCIE0); // Enable PCINT for port B

  // Timer interrupt configuration
  cli(); // Disable interrupts
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1 = 0; // Initialize counter value to 0
  OCR1A = 15624; // Set compare match register for 1s interrupt at 16MHz
  TCCR1B |= (1 << WGM12); // Turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024 and start the timer
  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
  sei(); // Enable interrupts
}

void loop() {
  // Check soil moisture
  int moistureLevel = analogRead(soilMoisturePin);
  if (moistureLevel > 200 && moistureLevel != lastMoistureLevel) {
    Serial.print("High Soil Moisture Detected: ");
    Serial.println(moistureLevel);
    lastMoistureLevel = moistureLevel;
    digitalWrite(moistureLedPin, HIGH); // Turn on moisture LED
    delay(1000); // Keep LED on for 1 second
    digitalWrite(moistureLedPin, LOW); // Turn off moisture LED
  }
}

ISR(PCINT2_vect) {
  if (digitalRead(pirSensorPin1)) {
    motionFlag1 = 1;
  }
  if (digitalRead(pirSensorPin2)) {
    motionFlag2 = 1;
  }
}

ISR(PCINT0_vect) {
 
}

void checkAndHandleFlags() {
  if (motionFlag1) {
    digitalWrite(pirLedPin, HIGH);  // Turn on PIR sensor 1 LED
    Serial.println("Motion detected by PIR sensor 1!");

    delay(1000);  // Delay

    digitalWrite(pirLedPin, LOW);  // Turn off PIR sensor 1 LED
    Serial.println("PIR sensor 1 LED turned off");

    motionFlag1 = 0;
  }

  if (motionFlag2) {
    digitalWrite(pirLedPin2, HIGH);  // Turn on PIR sensor 2 LED
    Serial.println("Motion detected by PIR sensor 2!");

    delay(1000);  // Delay

    digitalWrite(pirLedPin2, LOW);  // Turn off PIR sensor 2 LED
    Serial.println("PIR sensor 2 LED turned off");

    motionFlag2 = 0;
  }
}

ISR(TIMER1_COMPA_vect) {
  checkAndHandleFlags();
}
