#include "constString.h"




long readA0          = 1500;   // in mV
long readA1          = 1234;   // in mA
long RPM           = 500000;    // in rpm
// Power  = readA0*readA1/1000;  //in milliWatt
uint16_t windVelocity = 100;      // send back the set windspeed for the moment
int16_t pitchAngle    =  -100;
int16_t temperature   = 23*10;        // temperature in �C*10
unsigned long pressure_amb  = 101300000;      // in ambient pressure in mPa
uint16_t humidity   = 1000;         // humidity in %*10
int32_t g_x       = 0;          // Acceleration against windflow direction in mg
int32_t g_y       = 0;          // Acceleration perpendicular to windflow in horizontal direction in mg
int32_t g_z       = -1000;         // Acceleration in up direction  in mg      
uint16_t current_sink     = 1234; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  delay(10000);
}

void loop() {
        //
  // put your main code here, to run repeatedly:
        unsigned long currTime        =  micros();
        Serial.println("{");
        Serial.print("\t\"voltage\": ");          //voltage
        Serial.print(constStringLength(readA0,5));
        Serial.println(",");
        Serial.print("\t\"current\": ");          // current
        Serial.print(constStringLength(readA1,4));
        Serial.println(",");
     //  Serial.print("\t\"power\": ");          //rpm
      //  Serial.print(constStringLength(Power,4));
      //  Serial.println(",");
        Serial.print("\t\"rpm\": ");          //rpm
        Serial.print(constStringLength(RPM,8));
        Serial.println(",");
        Serial.print("\t\"windSpeed\": ");          //windspeed
        Serial.print(constStringLength(windVelocity,4));
        Serial.println(",");
        Serial.print("\t\"pitchAngle\": ");         //pitch angle
        Serial.print(constStringLength(pitchAngle,4));
        Serial.println(",");
        Serial.print("\t\"temperature\": ");          //temperature
        Serial.print(constStringLength(temperature,4));
        Serial.println(",");
        Serial.print("\t\"ambientPressure\": ");          //ambient pressure
        Serial.print(constStringLength(pressure_amb,9));
        Serial.println(",");
        Serial.print("\t\"humidity\": ");         //humidity
        Serial.print(constStringLength(humidity,4));
        Serial.println(",");
        Serial.print("\t\"accelerationX\": ");          //x accelerometer
        Serial.print(constStringLength(g_x,5));
        Serial.println(",");
        Serial.print("\t\"accelerationY\": ");          //y accelerometer
        Serial.print(constStringLength(g_y,5));
        Serial.println(",");
        Serial.print("\t\"accelerationZ\": ");          //z accelerometer
        Serial.print(constStringLength(g_z,5));
        //Serial.println(",");
        //Serial.print("\t\"pressure1\": ");          //dummy 1
        //Serial.print(constStringLength(pressure_1,9));
        //Serial.println(",");
        //Serial.print("\t\"pressure2\": ");          //dummy 2
        //Serial.print(constStringLength(pressure_2,9));
        //Serial.println(",");
        //Serial.print("\t\"pressure3\": ");          //dummy 3
        //Serial.print(constStringLength(pressure_3,9));
        Serial.println(",");
        Serial.print("\t\"currentSink\": ");          //dummy 3
        Serial.print(constStringLength(current_sink,4));
        //Serial.println(",");
        //Serial.print("\t\"dummy_d\": ");          //dummy 4
        //Serial.print(constStringLength(pressure_4,9));
        //Serial.println(",");
        //Serial.print("\t\"dummy_e\": ");          //dummy 5
        //Serial.print(constStringLength(pressure_5,9));
        Serial.println(",");
        Serial.print("\t\"timestamp\": ");          //timestamp
        Serial.println(constStringLength(currTime,10));
      Serial.println("}");
      Serial.println("EOL");
}
