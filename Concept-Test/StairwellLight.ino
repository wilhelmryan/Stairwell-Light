/*
PIR->Base of NPN transistor, Emitter->pin2
http://snag.gy/sGc7A.jpg
*/

#include <Wire.h>
#include <RTClib.h>

#define STARTHOUR 19
#define ENDHOUR 5

unsigned long rampperiod=200;
unsigned long rampdownperiod=1000;
unsigned long awaketime=5000;
volatile unsigned long trigger=0;

//interrupt pin
const int motionPin=2;
//PWM pin
const int driverPin=9;
//max pwm value
int maxPWM=255;
//notification PIN
const int notificationPin=13;

//fade information
volatile bool rampup=false;
volatile bool rampdown=false;
bool constant=false;
volatile bool run=false;

//clock information
RTC_DS1307 rtc;

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
	
	pinMode(driverPin,OUTPUT);
	pinMode(notificationPin,OUTPUT);
	
	
	//start wire service
	Wire.begin();
	//start the RTC
	rtc.begin();
	
	DateTime now=rtc.now();
	
	Serial.print("The current time is :");
	Serial.print(now.hour());
	Serial.print(":");
	Serial.print(now.minute());
	Serial.print(":");
	Serial.println(now.second());
}



void loop()
{
	do {
		DateTime now=rtc.now();
		if(now.hour()<=ENDHOUR||now.hour()>=STARTHOUR)
		{
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
				if(current-trigger>=rampperiod+awaketime+rampdownperiod)
				{
					rampdown=false;
					run=false;
				}
			
				else
				{
					float val=current-(trigger+rampperiod+awaketime);
					val=maxPWM-((val/rampdownperiod)*maxPWM);
					analogWrite(driverPin,(int)val);
				}

			}
		
			else analogWrite(driverPin,0);
		}
		
		else
		{
			for(int i=0;i<3;i++)
			{
				digitalWrite(notificationPin,HIGH);
				delay(500);
				digitalWrite(notificationPin,LOW);
				delay(500);
			}
		}
	}while(run);
	
	
}
