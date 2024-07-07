#define BUZZER 7 // Arduino pin D7, AVR pin PD7
// Buzzer frequency (Hz) and duration (milliseconds). 
unsigned int frequency = 8000;
unsigned long duration = 12;
// Additional global variables used by the buzzer function
volatile long timer1_toggle_count;
volatile uint8_t *timer1_pin_port;
volatile uint8_t timer1_pin_mask;

unsigned long millisVal;

//-------------------------------------------------------------------------
// Function beepbuzzer()
// This function uses TIMER1 to toggle the BUZZER pin to drive a piezo 
// buzzer. The frequency and duration of the noise are defined as global
// variables at the top of the program. This function exists in place of
// the normal Arduino tone() function because tone() uses TIMER2, which 
// interferes with the 32.768kHz timer used to clock the data logging. 
void beepbuzzer(void){
  uint8_t prescalarbits = 0b001;
  long toggle_count = 0;
  uint32_t ocr = 0;
  int8_t _pin = BUZZER;
  
  // Reset the 16 bit TIMER1 Timer/Counter Control Register A
    TCCR1A = 0;
  // Reset the 16 bit TIMER1 Timer/Counter Control Register B
    TCCR1B = 0;
  // Enable Clear Timer on Compare match mode by setting the bit WGM12 to 
  // 1 in TCCR1B
    bitWrite(TCCR1B, WGM12, 1);
  // Set the Clock Select bit 10 to 1, which sets no prescaling
    bitWrite(TCCR1B, CS10, 1);
  // Establish which pin will be used to run the buzzer so that we
  // can do direct port manipulation (which is fastest). 
    timer1_pin_port = portOutputRegister(digitalPinToPort(_pin));
    timer1_pin_mask = digitalPinToBitMask(_pin);
  
    // two choices for the 16 bit timers: ck/1 or ck/64
    ocr = F_CPU / frequency / 2 - 1;
  
  prescalarbits = 0b001; // 0b001 equals no prescalar 
      if (ocr > 0xffff)
      {
        ocr = F_CPU / frequency / 2 / 64 - 1;
        prescalarbits = 0b011; // 0b011 equal ck/64 prescalar
      }

  // For TCCR1B, zero out any of the upper 5 bits using AND that aren't already 
  // set to 1, and then do an OR with the prescalarbits to set any of the
  // lower three bits to 1 wherever prescalarbits holds a 1 (0b001).
    TCCR1B = (TCCR1B & 0b11111000) | prescalarbits;

  // Calculate the toggle count
    if (duration > 0)
    {
      toggle_count = 2 * frequency * duration / 1000;
    }
  
  // Set the OCR for the given timer,
    // set the toggle count,
    // then turn on the interrupts
    OCR1A = ocr; // Store the match value (ocr) that will trigger a TIMER1 interrupt
    timer1_toggle_count = toggle_count;
    bitWrite(TIMSK1, OCIE1A, 1); // Set OCEIE1A bit in TIMSK1 to 1.
  // At this point, TIMER1 should now be actively counting clock pulses, and
  // throwing an interrupt every time the count equals the value of ocr stored
  // in OCR1A above. The actual toggling of the pin to make noise happens in the 
  // TIMER1_COMPA_vect interrupt service routine. 
}


void setup() {
  Serial.begin(57600); // adjust your serial monitor baud to 57600 to match
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
    beepbuzzer();
  }
  Serial.println(F("Tadam serial connected"));
}

void loop() {
  if (millis() > millisVal + 1000){
    millisVal = millis(); // update millisVal
    Serial.print(F("Test serial monitor"));
  }
}
