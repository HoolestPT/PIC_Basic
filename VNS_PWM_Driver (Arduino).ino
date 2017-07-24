//Vagus Nerve Stimulator PWM Driver
//(C) 2017 Hoolest PT

void setup() {
  //Pin setup
  pinMode(8, OUTPUT);         //GND for Increment Pin
  pinMode(9, INPUT_PULLUP);   //Increment Pin
  pinMode(2, OUTPUT);         //GND for Decrement Pin
  pinMode(3, INPUT_PULLUP);   //Decrement Pin
  pinMode(4, OUTPUT);         //Positive Output Pin
  pinMode(5, OUTPUT);         //Negative Output Pin

  Serial.begin(9600);
}


void printPWM(int currentDuty)
{
  Serial.print(2*100*currentDuty/255);
  Serial.print("\n");
}

void loop() {
  //Variable Declaration
                              
  int buttonDelay = 250;      //The button debouncng delay (ms)

  //Timer Variables:
  unsigned long INClastMillis = 0;
  unsigned long INCelapsed = 0;
  unsigned long DEClastMillis = 0;
  unsigned long DECelapsed = 0;
  unsigned long PWMlastMillis = 0;
  unsigned long PWMelapsed = 0;
 

  int timerComp = 64;        //Timer compensation due to custom PWM frequency
  
  int currentDuty = 0;       //Current PWM duty cycle

  int frequency = 350;       //Output frequency in Hz
  double OPdutyCycle = 0.10; //Output (ON) duty cycle

  //Calculated delay variables:
  unsigned long ONdelay = (unsigned long)(1000000*OPdutyCycle)/(frequency*2);
  unsigned long OFFdelay = (unsigned long)(1000000*(1-OPdutyCycle))/(frequency*2);
  
  //Adjust PWM frequency for Pin 6 (Timer0) to 62.5 kHz
  TCCR0B = TCCR0B & 0b11111000 | 0x01;


  //Engage GND pins
  digitalWrite(8, LOW);
  digitalWrite(2, LOW);

  //Set default states for output (+) and output (-)
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  
  //Operating Loop:
  while(1)
  {
   
    //------------------------------Check the state of the increment/decrement buttons------------------------------------------
    if(!digitalRead(9))
    {
      
      INCelapsed = millis()/timerComp - INClastMillis;    //Debouncing of input
      if(INCelapsed >= buttonDelay)
      {
      
        //Varied increment sensitivity (step size increases progressively)
        
        if(currentDuty < 4)
        {
          currentDuty += 1;
        }
        if((currentDuty >= 4) && (currentDuty < 12))
        {
          currentDuty += 2;
        }
        if(currentDuty >= 12)
        {
          currentDuty += 4;
        }
        
        if(currentDuty >= 128)
        {
          currentDuty = 128;       //Limit currentDuty to 128 (50% Duty Cycle)
        }

         printPWM(currentDuty);
        
        INClastMillis = millis()/timerComp;
      }
    }
    if(!digitalRead(3))
    {
      
      DECelapsed = millis()/timerComp - DEClastMillis;      //Debouncing of input
      if(DECelapsed >= buttonDelay)
      {
                  //Varied increment sensitivity (step size increases progressively)
        
        if(currentDuty < 4)
        {
          currentDuty -= 1;
        }
        if((currentDuty >= 4) && (currentDuty < 12))
        {
          currentDuty -= 2;
        }
        if(currentDuty >= 12)
        {
          currentDuty -= 4;
        }
          
          if(currentDuty <= 0)
          {
             currentDuty = 0;         //Assure that currentDuty cannot be less than 0
          }

        printPWM(currentDuty);

        DEClastMillis = millis()/timerComp;
      }
    }


    //---------------------------------Set the boost converter PWM duty cycle (update every 100 ms)--------------------------------------------
    PWMelapsed = millis()/timerComp - PWMlastMillis;
    if(PWMelapsed >= 100)
      {
        analogWrite(6, currentDuty);
        PWMlastMillis = millis()/timerComp;
      }

    //-----------------------------------Output sequence timer----------------------------------
    //Switch positive on:
    digitalWrite(4, HIGH);
    delayMicroseconds(ONdelay);
    //Switch positive off:
    digitalWrite(4, LOW);
    delayMicroseconds(OFFdelay);
    //Switch negative on:
    digitalWrite(5, LOW);
    delayMicroseconds(ONdelay); 
    //Switch negative off:
    digitalWrite(5, HIGH);
    delayMicroseconds(OFFdelay);
    //Wait and reset counter

    
  }
}
