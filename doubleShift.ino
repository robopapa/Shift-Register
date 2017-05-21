#define LetchPin 9
#define ClockPin 8
#define SerialClockPin 10

int seconds = 0;
int minutes = 0;
void AddValue(bool);
void DoLetch();
void setup() {
  pinMode(LetchPin,OUTPUT);
  pinMode(ClockPin,OUTPUT);
  pinMode(SerialClockPin,OUTPUT);

  Serial.begin(9600);
  Reset();
  // initialize timer1
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  //compare match register 16MHz/256/2Hz
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12) | (1<<CS10); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts
}
ISR(TIMER1_COMPA_vect)
{
  ++seconds;
  if(seconds>59)
  {
    seconds = 0;
    ++minutes;
  }
  if(minutes>59)
  {
    minutes = 0;  
  }
  
  if(minutes>0)
  {
    for(int i = 6; i>=0; --i)
    {
      //Serial.println((minutes&(0x01<<i))>>i);
      AddValue((minutes&(0x01<<i))>>i);
    }
    DoLetch();
  } 
  
  //shift(PB2,PB1,PB0,seconds);
  
/*
  for(int i = 0; i<7; ++ i)
  {
    AddValue((minutes&(0x01<<i))>>i);
  }
  AddValue(false);
  //Serial.println(seconds);
  for(int i=0; i < 7; ++i)
  {
    AddValue((seconds&(0x01<<i))>>i);
  }
  DoLetch();
  */
  
  
  //shift(SerialClockPin,LetchPin,ClockPin,data);
  //shift(PB2,PB1,PB0,data);

}
void shift(int SRCLK_Pin, int RCLK_Pin, int SER_Pin, unsigned long data){
	PORTB &= ~(1 << RCLK_Pin); 				// Set the register-clock pin low

	for (int i = 0; i < (8); i++){	// Now we are entering the loop to shift out 8+ bits

		PORTB &= ~(1 << SRCLK_Pin); 			// Set the serial-clock pin low

		PORTB |= (((data&(0x01<<i))>>i) << SER_Pin ); 	// Go through each bit of data and output it

		PORTB |= (1 << SRCLK_Pin); 			// Set the serial-clock pin high

		PORTB &= ~(((data&(0x01<<i))>>i) << SER_Pin );	// Set the datapin low again	
	}

	PORTB |= (1 << RCLK_Pin);				// Set the register-clock pin high to update the output of the shift-register
}
void loop() {
  // put your main code here, to run repeatedly:

}

void DoLetch()
{
  digitalWrite(LetchPin,HIGH);
  //delay(1);
  digitalWrite(LetchPin,LOW);
}

void AddValue(bool isOn)
{
   digitalWrite(ClockPin,isOn);
   digitalWrite(SerialClockPin,HIGH);
//   delay(1);
//   digitalWrite(ClockPin,LOW);
   digitalWrite(SerialClockPin,LOW);
}

void Reset()
{
  for(int i=0; i<16; ++i)
  {
    AddValue(false);
  }
  DoLetch();
  
  for(int i=0; i<16; ++i)
  {
    AddValue(true);
    DoLetch();
    delay(50);
  }
  for(int i=0; i<16; ++i)
  {
    AddValue(false);
  }
  DoLetch();
}
