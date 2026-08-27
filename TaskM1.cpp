#include <Arduino.h>

#include <avr/interrupt.h>

// --- Volatile Flags for ISR Safety ---
volatile bool flag_sensor1 = false;
volatile bool flag_sensor2 = false;
volatile bool flag_sensor3 = false;
volatile bool flag_timer   = false;

// State variable to detect pin state changes on PORTB
volatile uint8_t last_portb_state = 0xFF; 

// Pin definitions
const int SENSOR1_PIN = 8;  // PB0 / PCINT0
const int SENSOR2_PIN = 9;  // PB1 / PCINT1
const int SENSOR3_PIN = 10; // PB2 / PCINT2
const int TIMER_LED   = 13; // Onboard LED

// System counter for state tracking
unsigned long timer_ticks = 0;

// --- Modular Configurations ---

void setupPCI() {
  // 1. Enable Pin Change Interrupts for PORTB (PCIE0)
  PCICR |= (1 << PCIE0); 
  
  // 2. Unmask Pins 8, 9, and 10 (PCINT0, PCINT1, PCINT2)
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2); 
  
  // Record initial hardware state
  last_portb_state = PINB;
}

void setupTimer1() {
  cli(); // Disable global interrupts during register configuration
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  
  // OCR1A = 15624 for 1Hz frequency (16MHz / (1024 prescaler * 1Hz) - 1)
  OCR1A = 15624; 
  
  // CTC Mode (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);
  
  // Set 1024 Prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  
  // Enable Timer Compare Match A Interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei(); // Re-enable global interrupts
}

// --- Modular Processing Handlers ---

void processSensorEvents() {
  if (flag_sensor1) {
    Serial.println("EVENT [PCINT0]: Sensor 1 (Pin 8) triggered!");
    flag_sensor1 = false;
  }
  
  if (flag_sensor2) {
    Serial.println("EVENT [PCINT1]: Sensor 2 (Pin 9) triggered!");
    flag_sensor2 = false;
  }

  if (flag_sensor3) {
    Serial.println("EVENT [PCINT2]: Sensor 3 (Pin 10) triggered!");
    flag_sensor3 = false;
  }
}

void processTimeBasedTasks() {
  if (flag_timer) {
    timer_ticks++;
    digitalWrite(TIMER_LED, !digitalRead(TIMER_LED));
    
    Serial.print("TIMER [Timer1]: Periodic Heartbeat #");
    Serial.print(timer_ticks);
    Serial.println(" | System Active");
    
    flag_timer = false;
  }
}

// --- Arduino Standard Core Functions ---

void setup() {
  Serial.begin(9600);
  
  pinMode(SENSOR1_PIN, INPUT_PULLUP);
  pinMode(SENSOR2_PIN, INPUT_PULLUP);
  pinMode(SENSOR3_PIN, INPUT_PULLUP);
  pinMode(TIMER_LED, OUTPUT);
  
  Serial.println("=== SYSTEM INITIALIZING ===");
  setupPCI();
  setupTimer1();
  Serial.println("=== SYSTEM READY (PCI + TIMER1 ACTIVE) ===");
}

void loop() {
  // Non-blocking processing execution
  processSensorEvents();
  processTimeBasedTasks();
}

// --- Interrupt Service Routines (ISRs) ---

// Single ISR managing Pin Change Interrupts for PORTB (Pins 8-13)
ISR(PCINT0_vect) {
  uint8_t current_portb_state = PINB;
  uint8_t changed_bits = current_portb_state ^ last_portb_state;
  
  // Check Pin 8 (PB0) - Falling Edge (Button Press)
  if ((changed_bits & (1 << PB0)) && !(current_portb_state & (1 << PB0))) {
    flag_sensor1 = true;
  }
  
  // Check Pin 9 (PB1) - Falling Edge (Button Press)
  if ((changed_bits & (1 << PB1)) && !(current_portb_state & (1 << PB1))) {
    flag_sensor2 = true;
  }

  // Check Pin 10 (PB2) - Falling Edge (Button Press)
  if ((changed_bits & (1 << PB2)) && !(current_portb_state & (1 << PB2))) {
    flag_sensor3 = true;
  }
  
  last_portb_state = current_portb_state;
}

// ISR for Timer1 Compare Match
ISR(TIMER1_COMPA_vect) {
  flag_timer = true; // Minimal ISR execution
}