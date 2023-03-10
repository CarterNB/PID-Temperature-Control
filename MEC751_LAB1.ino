#include <PID_v1.h>
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance 
#define TEMPERATURENOMINAL 25  
// The beta coefficient of the thermistor
#define BCOEFFICIENT 3500
// the value of the 'other' resistor
#define SERIESRESISTOR 10000
   
double motorSpeed, desiredTemp, steinhart;
int motorPin = 9;

PID myPID(&steinhart,&motorSpeed,&desiredTemp, 6, 10, 3,REVERSE); 

#define NUMSAMPLES 5
int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(9600);
  desiredTemp=34;
  pinMode(motorPin, OUTPUT);
  myPID.SetMode(AUTOMATIC);
}
 
void loop(void) {
  uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;

  Serial.print("Average analog reading "); 
  Serial.println(average);
  
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
  
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C
  
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *C");
  myPID.SetSampleTime(1000);
  myPID.Compute();
  Serial.print("Motor speed:");
  Serial.println(motorSpeed);
  analogWrite(motorPin,motorSpeed);
  delay(3000);
}
