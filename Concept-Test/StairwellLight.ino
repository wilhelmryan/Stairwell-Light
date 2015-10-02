/*
PIR->Base of NPN transistor, Emitter->pin2
*/

unsigned long rampperiod=200;
unsigned long awaketime=5000;
volatile unsigned long trigger=0;

//interrupt pin
const int motionPin=2;
//PWM pin
const int driverPin=9;

int maxPWM=150;

volatile bool rampup=false;
volatile bool rampdown=false;
bool constant=false;
volatile bool run=false;

void interruptservice()
{
  run=true;
  rampdown=false;
  if(!constant&&!rampup)
  {
    rampup=true;
  }
  
  trigger=millis();
}

void setup()
{
 
  Serial.begin(9600);
  Serial.println("LED stair light V1.0");
  attachInterrupt(0,interruptservice,RISING);
} 



void loop()
{
  
  
  do {
    unsigned long current=millis();
    if(rampup)
    {
      if(current-trigger>=rampperiod)
      {
        rampup=false;
        constant=true;
      }
      else
      {
        float val=(current-trigger);
        val=(val/rampperiod)*maxPWM;
        analogWrite(driverPin,val);
      }
    }
    
    else if(constant)
    {
      if(current-trigger>=rampperiod+awaketime)
      {
        constant=false;
        rampdown=true;
      }
      
      else analogWrite(driverPin,maxPWM);
    }
    
    else if(rampdown)
    {
      if(current-trigger>=rampperiod+awaketime+rampperiod)
      {
        rampdown=false;
        run=false;
      }
      
      else 
      {
        float val=current-(trigger+rampperiod+awaketime);
        val=maxPWM-((val/rampperiod)*maxPWM);
        analogWrite(driverPin,(int)val);
      }

    }
    
    else analogWrite(driverPin,0);
  }while(run);
  
  
}
