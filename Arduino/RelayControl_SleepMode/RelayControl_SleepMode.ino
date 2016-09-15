
#define __AVR_ATtiny85__
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define PTT_SENSE 5
#define TX_SENSE 4
#define TX_AN_SENSE 2
#define RX_SENSE 3
#define RX_AN_SENSE 3
#define TX_CTL 1
#define RX_CTL 2
#define RELAY_ENA 0


int PTT_state; 
int RX_now;
 
int rx_sense; 
int tx_sense; 
int saw_int;

void system_sleep()
{
  MCUCR = 0x20 | 0x10; 
  sleep_cpu();
}


void setup() {
  // put your setup code here, to run once:
  pinMode(PTT_SENSE, INPUT);
  pinMode(RX_SENSE, INPUT);
  pinMode(TX_SENSE, INPUT);
  digitalWrite(RX_SENSE, HIGH);
  digitalWrite(TX_SENSE, HIGH);
  pinMode(TX_CTL, OUTPUT);
  pinMode(RX_CTL, OUTPUT);
  pinMode(RELAY_ENA, OUTPUT);
  digitalWrite(RELAY_ENA, LOW);
  digitalWrite(TX_CTL, HIGH);
  digitalWrite(RX_CTL, LOW);

  RX_now = 0; 


  // set in RX mode to start
  digitalWrite(RELAY_ENA, LOW); 
  digitalWrite(TX_CTL, HIGH); 
  digitalWrite(RX_CTL, LOW);
  digitalWrite(RELAY_ENA, HIGH); 
  delay(100);
  digitalWrite(RELAY_ENA, LOW); 
  RX_now = 1; 
       
  // turn on interrupts
  GIMSK = 0x20; // PinChangeInterrupt enable
  PCMSK = 0x20; // select input pin 5 (PTT_SENSE)
  // enable interrupts
  sei();
  // clear the flag
  saw_int = 0; 
  // now go to sleep. 
  system_sleep(); 
} 

ISR(PCINT0_vect) {
  saw_int = 1; 
}
void loop() {
  // put your main code here, to run repeatedly:
  PTT_state = digitalRead(PTT_SENSE);
  if((PTT_state == LOW) && (RX_now == 1)) {
    // switch to TX mode. 
    RX_now = 0; 
    // energize the TX coil
    digitalWrite(RELAY_ENA, LOW); 
    digitalWrite(RX_CTL, HIGH); 
    digitalWrite(TX_CTL, LOW);  
     //   digitalWrite(RELAY_ENA, HIGH); 
  }
  else if((PTT_state == HIGH) && (RX_now == 0)) {
    // switch to RX mode
    RX_now = 1; 
    // energize the RX coil
    digitalWrite(RELAY_ENA, LOW); 
    digitalWrite(TX_CTL, HIGH); 
    digitalWrite(RX_CTL, LOW);
   //     digitalWrite(RELAY_ENA, HIGH); 
  }


  // now test the inputs and energize the appropriate coil
  // depending on what we're doing
  tx_sense = analogRead(TX_AN_SENSE); 
  rx_sense = analogRead(RX_AN_SENSE);
  
  if(RX_now == 1) {
    // we should be in RX mode.  test rx_sense
    if(rx_sense > 128) {
      // the relay hasn't closed yet.
      digitalWrite(RELAY_ENA, HIGH);
      delay(10);
    }
    else {
      digitalWrite(RELAY_ENA, LOW); 
      system_sleep();
    }
  }
  else {
    if(tx_sense > 128) {
      // the relay hasn't closed yet.
      digitalWrite(RELAY_ENA, HIGH);
      delay(10);
    }
    else {
      digitalWrite(RELAY_ENA, LOW); 
      system_sleep();
    }   
  }
}
