//to change!
//add current_sink to the ni output
// current sink input command in %!!



/*
#### Due WindShield Firmware ####
version alpha 0.1:


*/


/*
######## Programming Hint: ########
For Arduino Due, datatype int is the same size as long: -2.147.483.648 to 2.147.483.647
######## Programming Hint  ########
*/


#include "speed_enc.h"
#include "constString.h"
#include "MCP4726.h"
#include <Wire.h>




//defines
#define F_CPU 84000000L
//#define USE_VREF_REF_INPUT //Setup the V_Ref as Reference Voltage Input


// header definitions
boolean DEBUG=false;
//uint32_t LED_PIN = 13;
String version ="Due 0.0.1alpha";
const long buad = 250000;
int32_t Number;
uint16_t Load;
uint16_t WindSpeed=0;
int PAngle;
uint8_t PDir=0;

// ########### Pin definitions #########
// ## Pitch ##
const uint8_t P_DIR		= 22;
const uint8_t P_STP		= 24;
const uint8_t P_CFG2	= 32;
const uint8_t P_CFG1	= 34;
const uint8_t P_EN		= 36;
// ## Motor ##
const uint8_t M_BRK = 50;
const uint8_t M_DIR = 46;
const uint8_t M_PWM_out = 3;
const uint8_t M_VPROPI = A8;		// current sense pin 500mV/1A
const uint8_t  M_SWITCH = 8;		// Set Pin HIGH for Motormode; LOW for Generatormode
const uint8_t M_SLEEP= 48;		// sets controller to sleep mode


//########### switches ###########
uint8_t sendData = 0;
uint8_t LoadControl = 0;
uint8_t setWindSpeed = 0;
uint8_t setPitch =0;
uint8_t setMotorSpeed=0;
uint8_t setMotorMode=0;
uint8_t setMotorBrake=0;
uint8_t newInterface =1;

//######## variables for time readings ########
volatile unsigned long currTime = 0;

//####### Variables for ADC/Data readings ######
long readA0;
long readA1;
long RPM;
int ADCbits = 12;
long ADCresolution= pow(2,ADCbits);
long V_Ref_ext = 3; // From MAX6216 external Voltage reference in [V]
long V_bit_ref = 3*ADCresolution; // for explanation see function readVcc!
long Power;





//Variables for DAC
uint8_t DAC_Address = 0x61; // Address for MCP4726A1T-E/CH with A1 = �1100001� I2C Address (Datasheet)

//Variables for Serial Communication
//String msg = "HH";
String msg = "";

//Variables for Pitch controll
int Pitch_Set_Angle=0;					//Holds the desired Angle
int Pitch_Current_Step_Position=0;
int32_t Pitch_Target_Position=0;

uint8_t Pitch_Set=0;					//boolean switch
uint8_t Pitch_Maintenace_Forward=0;		//boolean switch
uint8_t Pitch_Maintenance_Backward=0;	//boolean switch
uint8_t MStep=2;						// initial Microstep setting
uint16_t Pitch_delayTimeHigh = 200;
uint16_t Pitch_delayTimeLow =  100;

// variables for motor control
long Motor_lastSpeed = 0;

//External classes binding
speed_enc speed(1024*4, 100);  //Encoder Pulses per Revolution (QuadEncoder), Samples per Seconds
MCP4726 dac=MCP4726(DAC_Address); // object for the external DAC

//Functions

//############### ADC readings ###################
int readVcc(){
	/*
	* preparation for later!
	* read Voltage Reference at Analog input (3.000V)
	* to compensate possible Input Voltage fluctuations
	*
	* at the moment it just returns the input Voltage
	* V=V_Ref*bit/2^n (n is the bit resolution of the ADC, bit is what the ADC gi8ves back)
	* In this case V_ref is the supply Voltage of the Arduino and is ca 3,3V.
	* This voltage is not supposed to be pretty stable and temperature depended
	* and hence not really good for accurate ADC measurements.
	* This is to improve the this fact.
	* We know the Reference Voltage from A2
	* (comes from the MAX6216 External Voltage Reference Chip),
	* which is 3,000V with a maximum error of +-0,02%
	* => V_Ref = (V* 2^n)/ bit
	* V*2^n = V_bit_ref
	*/
	long result;
	result = analogRead(A2);
	result = V_bit_ref/result;
	
	//result = 3299; //Vcc in mV
	return result;
}

long readVoltage(long Vcc){
	//int Vcc = readVcc();
	long V_G = Vcc*analogRead(A0)*5/ADCresolution;
	return V_G; //in mV
}

long readCurrent(long Vcc){
	//int Vcc = readVcc();
	long I_G = Vcc*analogRead(A1)/ADCresolution;
	
	return I_G; //in mA
}

void setLoad(uint16_t value){
	// DAC Voltage_out = V_ref * bit# / 2^n
	// n = 12 (bit resolution of the DAC)
	// V_Ref = 3,000V from external Voltage Reference
	// rewrite formula to bit#
	// bit# = Voltage_out * 2^n/V_Ref
	// bit# = Voltage_out * 4096/3,000
	// The Output Voltage comes in mV  (in this function) and is later divided by 2 in the hardware.
	// To overcome this the conversion formula is as follows:
	// bit# = Voltage_out * 2* 1365/1000
	// (1 V  equals 1 A)
	value = constrain(round(value*2*1365/1000),0,4095);
	dac.setVoltage(value);
}



//################# Pitch command functions #####################
void P_resetPitchPosition(void){
	Pitch_Current_Step_Position=0;
	Pitch_Target_Position=0;
}

int32_t P_Distance_to_go(){
	return Pitch_Target_Position-Pitch_Current_Step_Position;
}


void P_setPitchAngle(int32_t _Pitch_Set_Angle){
	MStep=4;
	P_setMSteps(MStep);
	int32_t _Pitch_Set_Step_Position;
	_Pitch_Set_Step_Position=P_Angle2Step(_Pitch_Set_Angle);
	
	if(_Pitch_Set_Step_Position>Pitch_Current_Step_Position){
		
		P_setStep(_Pitch_Set_Step_Position-Pitch_Current_Step_Position,MStep,1);
	}
	else if (_Pitch_Set_Step_Position<Pitch_Current_Step_Position){
		P_setStep(Pitch_Current_Step_Position-_Pitch_Set_Step_Position,MStep,0);
	}
	//Pitch_Current_Step_Position=_Pitch_Set_Step_Position;
}

void P_Enable(uint8_t activate){
	if (activate)
	{
		delayMicroseconds(1);	//according to datasheet of TMC2100 (silentStepStick Board) chapter 11.1 Timing
		digitalWrite(P_EN,LOW); //pin is active Low
		delayMicroseconds(1);	//according to datasheet of TMC2100 (silentStepStick Board) chapter 11.1 Timing
	}
	else
	{
		delayMicroseconds(1);
		digitalWrite(P_EN,HIGH);
		delayMicroseconds(1);
	}
}

void P_setStep(unsigned long _Steps, uint8_t _msteps, uint8_t _dir){
	P_Enable(1);
	if(_dir){
		digitalWrite(P_DIR,HIGH);
	}
	else{
		digitalWrite(P_DIR,LOW);
	}
	unsigned long _Totalsteps;
	_Totalsteps= _Steps*_msteps+1;
	
	do{
		digitalWrite(P_STP,HIGH);
		delayMicroseconds(Pitch_delayTimeHigh);
		digitalWrite(P_STP,LOW);
		delayMicroseconds(Pitch_delayTimeLow);
		//Serial.println(Steps);
		if(_dir){
			Pitch_Current_Step_Position++;
		}
		else{
			Pitch_Current_Step_Position--;
		}
		_Totalsteps--;
	}while(_Totalsteps);
	P_Enable(0);
}

void P_Step(){
	//does one step in specified direction
	P_Enable(1);
	if(P_Distance_to_go()>0){
		digitalWrite(P_DIR,HIGH);
		Pitch_Current_Step_Position++;
		
	}
	else if(P_Distance_to_go()<0){
		digitalWrite(P_DIR,LOW);
		Pitch_Current_Step_Position--;
	}
	else{
		return;
	}
	digitalWrite(P_STP,HIGH);
	delayMicroseconds(Pitch_delayTimeHigh);
	digitalWrite(P_STP,LOW);
	delayMicroseconds(Pitch_delayTimeLow);
	//Serial.println(Steps);

	P_Enable(0);
	
}


unsigned long P_Angle2Step(int32_t _Angle){
	//converts an angle to respective steps; 1 step = 0,1� (for the moment, in lack of correct knowing)
	int32_t step;
	
	if (_Angle<-100){
		step=-100;
	}
	else if (_Angle>500){
		_Angle=500;
	}
	step = 10*_Angle+100;
	return (unsigned long)step;
}
int32_t P_Step2Angle(unsigned long _Step){
	// converts the current step position back into an angle (for data output)
	//Serial.println(_Step);
	return ((int32_t)_Step-100)/10;
}

void P_setMSteps(uint8_t _msteps){
	if (_msteps==2){
		// half step mode
		pinMode(P_CFG2, OUTPUT);
		digitalWrite(P_CFG2,LOW);
		digitalWrite(P_CFG1,LOW);	// to be sure the internal pull-up resistor is disabled
		pinMode(P_CFG1,INPUT);		// set the digital pin in quasi tri-state (high impedance state)
		// SpreadCycle Mode \w 256 Steps Interpolation
	}
	if (_msteps==4){
		// quarter step mode
		//digitalWrite(P_CFG2,LOW);	// to be sure the internal pull-up resistor is disabled
		pinMode(P_CFG2,INPUT);
		pinMode(P_CFG1,OUTPUT);
		digitalWrite(P_CFG1,HIGH);	//stelthChop Mode \w 256 Steps Interpolation//digitalWrite(P_CFG2,HIGH);
	}
	else{
		MStep=2;					//set half Steps as default
		pinMode(P_CFG2, OUTPUT);
		digitalWrite(P_CFG2,LOW);
		pinMode(P_CFG1,INPUT);		//SpreadCycle Mode \w 256 Steps Interpolation
	}
}

/*
void IRQ_SetStep(uint32_t Steps){
uint32_t counter++;
if (counter== Steps){

}
}
*/

//############# Windspeed commands ##############
void Fan(int Speed){
	if(!DEBUG){
		if(Speed&&(Speed<=100)){
			Serial1.print("F");
			Serial1.print(Speed);
			Serial1.println("%");
		}
		else{
			Serial1.println("X");
		}
	}
	else{
		if(Speed&&(Speed<=100)){
			Serial.print("F");
			Serial.print(Speed);
			Serial.println("%");
		}
		else{
			Serial.println("X");
		}
	}
}


//############ Motormode commands #############
void M_Brake_active(){
	digitalWrite(M_BRK,HIGH);
}

void M_Brake_deactive(){
	digitalWrite(M_BRK,LOW);
}

void M_forward(){
	M_Brake_active();	//Set Brake for better Performance according to DRV8801 Truth Table on page: https://www.pololu.com/product/2136
	digitalWrite(M_DIR,HIGH);
	
}

void M_backward(){
	M_Brake_active();	//Set Brake for better Performance according to DRV8801 Truth Table on page: https://www.pololu.com/product/2136
	digitalWrite(M_DIR,LOW);
}

void M_Brake(){
	digitalWrite(M_PWM_out,LOW);
	M_Brake_active();
}

void M_Brake_release(){
	digitalWrite(M_PWM_out,LOW);
	M_Brake_deactive();
}

void M_SleepMode(int _Mode){
	
	if(_Mode){
		digitalWrite(M_SLEEP,LOW);
	}
	else{
		digitalWrite(M_SLEEP,HIGH);
		delay(2);
	}
}

void M_SwitchMotorModeOff(){
	M_backward();
	M_Brake_release();
	M_SleepMode(1);
	digitalWrite(M_SWITCH,LOW);
	setMotorMode=0;
}

void M_SwitchMotorModeOn(){
	digitalWrite(M_SWITCH,HIGH);  // activates the relay and connects the Turbine generator to the motor controller
	setMotorMode=1;
}


void M_SetMotorSpeed(long _speed){
	M_SleepMode(0);
	M_SwitchMotorModeOn();
	long _Motorspeed= constrain(_speed,0,4095);
	if(_Motorspeed!=0){
		M_Brake_active();
		M_forward();
		analogWrite(M_PWM_out,_speed);
		Motor_lastSpeed = _Motorspeed;
	}
	else{
		M_Brake();
	}
}

long M_Percent2bit(int _percent){
	return (long)(4095*_percent/100);
}

//############## serial communication ##########
void printHelp(){
	//Help Massage
	Serial.println();
	Serial.println("WindShield");
	Serial.print("Firmware Version: ");
	Serial.println(version);
	Serial.println("##########################");
	Serial.println("###### Commands ##########");
	Serial.println("###### general ###########");
	Serial.println("HH : prints this massage");
	Serial.println("DD : toggle debug mode on and off");
	Serial.println("ST : Stops the Fan and turns off the Load ");
	Serial.println("##### Data stream ########");
	Serial.println("AA : print Volt, Current, RPM, Power and a Timestamp (since last restart) in actual Units ([mV],[mA],[1/min],[mW],[us])");
	Serial.println("     Output is in JSON Style!!!");
	Serial.println("##### Load control #######");
	Serial.println("IIx: Set the Load to value x in [mA] (0 - 1500mA)");
	Serial.println("##### Fan control ########");
	Serial.println("WSx: Set The fan to value x in [cm/s] (0 - 140 cm/s)");
	Serial.println("##### Pitch control ######");
	Serial.println("PAx: The Pitch slide goes to angle in [�/10] (-100 - +500 �/10)");
	Serial.println("PFx: The Pitch slide goes x Steps forward (to rotor plane) (for firmware backward compatibility)");
	Serial.println("PBx: The Pitch slide goes x Steps backward (away from rotor plane) (for firmware backward compatibility)");
	Serial.println("PP : The Pitch slide goes 1000 Steps forward (Maintenance Mode)");
	Serial.println("PM : The Pitch slide goes 1000 Steps backward (Maintenance Mode)");
	Serial.println("P0 : Resets the current Position variable to zero");
	Serial.println("##### Motor mode #########");
	Serial.println("MM : Switch to Motor mode");
	Serial.println("MSx: Set  the Motor speed in [%] (0-100%)");
	Serial.println("MB : Brakes the Motor until stand still (0rpm)");
	Serial.println("MR : Release the Brake");
	Serial.println("MG : Switch to Generator mode");
	Serial.println("##### End of Help ########");
	Serial.println("##########################");
	Serial.println("NI : Switch the Interface from old to new one");
}

int SetCommand(String _msg){
	//! Definition of the Serial Commands and the send back Data
	if (_msg.equals("")) {return 0;}
	if (_msg.equals("DD")){
		DEBUG=!DEBUG;
		//msg= "AA";
		if (DEBUG){
			Serial.println("Debug: On");
		}
		else{
			Serial.println("Debug: Off");
		}
		sendData=0;
	}
	else if (_msg.equals("AA")){
		sendData = 1;
	}
	else if (msg.equals("NI")){			//toggles between new and old data output
		if (newInterface==1){
			newInterface=0;
		}
		else if (newInterface==0){
			newInterface=1;
		}
		
	}
	/* for later
	else if(_msg.equals("I0")){
	LoadControl=0;
	//msg="";
	
	}
	else if(_msg.equals("I1")){
	LoadControl=1;
	Load = Number;
	}
	*/
	else if (msg.equals("II")){				//Set Load value
		LoadControl=1;
		Load=(uint16_t)Number;
	}
	else if(_msg.equals("WS")){
		setWindSpeed=1;
		WindSpeed=Number;
	}
	else if (_msg.equals("HH")){
		printHelp();
	}

	else if(_msg.equals("ST")){
		TurnOffWindLab();

	}
	// ########## pitch ##############
	else if(_msg.equals("PF")){				//Set the forward Pitch (old behavior, awaits steps not angle)
		PDir=0;
		MStep=4;
		PAngle=MStep*Number+Pitch_Current_Step_Position;
	}
	else if(_msg.equals("PB")){				//Set the backward Pitch (old behavior, awaits steps not angle)
		PDir=1;
		MStep=4;
		PAngle=MStep*Number+Pitch_Current_Step_Position;
	}
	else if(_msg.equals("PP")){				//Set the forward Pitch in maintenance mode (1000 steps forward)(old behavior, awaits steps not angle)
		setPitch=1;
		PAngle=1000;
		PDir=0;
		MStep=2;
	}
	else if(_msg.equals("PM")){				//Set the backward Pitch in maintenance mode (1000 steps backward) (old behavior, awaits steps not angle)
		setPitch=1;
		PAngle=1000;
		PDir=1;
		MStep=2;
	}
	else if (msg.equals("P0")){
		P_resetPitchPosition();
	}
	else if(_msg.equals("PA")){
		MStep=4;
		Pitch_Target_Position=P_Angle2Step(Number);//MStep*
		//Serial.println(Pitch_Target_Position);
		//Serial.println(P_Step2Angle(Pitch_Target_Position));
		
	}
	//########## Motor mode ######################
	else if(msg.equals("MM")){
		M_SwitchMotorModeOn();
	}
	else if(msg.equals("MS")){
		setMotorSpeed=1;
	}
	else if(msg.equals("MB")){
		setMotorBrake=1;
	}
	else if(msg.equals("MR")){
		M_Brake_release();
	}
	else if(msg.equals("MG")){
		M_SwitchMotorModeOff();
	}
	else{
		msg="";
		Number=0;
		
	}
	msg="";
	Number=0;
}


// ####### programm internal communication ###########
void TurnOffWindLab(void){
	sendData=0;
	LoadControl=0;
	//SetWindSpeed=0;
	
	//Turn off controll
	Load=0;
	WindSpeed=0;
	
}



//######### not used functions #############

/* //further investigations has to be done!!!
uint32_t nanos( void )
{
uint32_t ticks ;
uint32_t count ;

SysTick->CTRL;
do {
ticks = SysTick->VAL;
} while (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk);

// GetTickCount() is millis()
uint32_t load= (SysTick->LOAD + 1 - ticks);
uint32_t milliS = GetTickCount() * 1000000;
uint32_t microS =  load/ (SystemCoreClock/1000000) * 1000;
uint32_t nanoS = load/ (SystemCoreClock/10000000); // these are actually 100s of nanoseconds; 84MHz  = 11.9ns

//Serial.print(milliS); Serial.print(" "); Serial.print(microS); Serial.print(" "); Serial.println(nanoS);

return milliS + microS + nanoS;

}
*/

//######### minor setup  ###########
void PinSetup(){
	//Pitch
	pinMode(P_CFG1,OUTPUT);
	pinMode(P_CFG2,OUTPUT);
	pinMode(P_DIR,OUTPUT);
	pinMode(P_EN,OUTPUT);
	pinMode(P_STP,OUTPUT);
	digitalWrite(P_CFG1,LOW);
	digitalWrite(P_CFG2,LOW);
	digitalWrite(P_DIR,LOW);
	digitalWrite(P_EN,HIGH);
	digitalWrite(P_STP,LOW);
	
	// motor controller
	pinMode(M_BRK,OUTPUT);
	pinMode(M_DIR,OUTPUT);
	pinMode(M_PWM_out,OUTPUT);
	pinMode(M_SWITCH,OUTPUT);
	pinMode(M_SLEEP,OUTPUT);
	digitalWrite(M_BRK,LOW);
	digitalWrite(M_DIR,LOW);
	digitalWrite(M_PWM_out,LOW);
	digitalWrite(M_SWITCH,LOW);
	digitalWrite(M_SLEEP,LOW);
	
	////debug LED
	//pinMode(LED_PIN,OUTPUT);
	//digitalWrite(LED_PIN,LOW);
	//
}



//######### main setup ##########
void setup(){
	//pinMode(13,OUTPUT);
	//digitalWrite(13,LOW);
	//Serial.begin(buad);
	Serial.begin(buad);
	Serial1.begin(9600);
	//digitalWrite(13,HIGH);
	//Serial.println("Setup WindShield...");
	//
	////digitalWrite(13,LOW);
	//delay(1000);
	analogReadResolution(ADCbits);
	analogWriteResolution(ADCbits);
	//adc_init(ADC, SystemCoreClock, 21000000L, 3); // (xxx,xxx, ADC_CLock, ADC_Startup_time in us) refer to Datasheet!
	PinSetup();
	P_Enable(0);
	//start DAC and set to 0A (Open circuit)
	dac.begin();
	setLoad(0);
	
	
	//delay(1000);
	//digitalWrite(13,HIGH);s
	
	//Serial.println("Ready");
	//attachInterrupt(25,IRQ_SetStep,HIGH);
	
	//digitalWrite(13,LOW);
}


//########## main loop ###########
void loop(){
	//! Read the Serial Data in for the commands
	// receive command
	if(Serial.available()>0){
		msg="";
		while(Serial.available()>0){
			msg+=char(Serial.read());
			delay(10);
		}
		Number= msg.substring(2).toInt();
		msg = msg.substring(0,2);
		msg.toUpperCase();
		//Serial.print("msg: ");
		//Serial.println(msg);
		//Serial.print("Number: ");
		//Serial.println(Number);
		//rated command
		SetCommand(msg);
		
	}
	
	
	
	
	
	// performs command
	if (setWindSpeed){
		Fan(WindSpeed);
		setWindSpeed=0;
	}
	
	//Loadcontrol
	if(LoadControl){
		//Serial.print("II: ");
		//Serial.println(Load);
		setLoad(Load);
		Number=0;
		msg="";
		LoadControl=0;
	}
	
	// Pitchcontrol
	if(!setPitch){
		P_Step();
	}
	else if(setPitch){
		P_setStep(PAngle,MStep,PDir);
		setPitch=0;
	}
	
	//motor mode control
	if (setMotorSpeed&&setMotorMode){
		M_SetMotorSpeed(M_Percent2bit(Number));
		setMotorMode=0;
	}
	
	//Data stream Control
	
	//if(sendData){
		//
		//digitalWrite(LED_PIN,HIGH);
	//}
	//else{
		//digitalWrite(LED_PIN,LOW);
	//}	
	if (sendData){
		
		
		
		if(!DEBUG&&!newInterface){
			//JSON Output
			long VCC = readVcc();
			currTime = micros();
			readA0	 = readVoltage(VCC);	//in mV
			readA1	 = readCurrent(VCC);	//in mA
			RPM		 = speed.speed();		//in rpm
			Power	 = readA0*readA1/1000;	//in milliWatt
			
			Serial.println("{");
				Serial.print("\t\"voltage\": ");
				Serial.print(constStringLength(readA0,5));
				Serial.println(",");
				Serial.print("\t\"current\": ");
				Serial.print(constStringLength(readA1,4));
				Serial.println(",");
				Serial.print("\t\"rpm\": ");
				Serial.print(constStringLength(RPM,8));
				Serial.println(",");
				Serial.print("\t\"power\": ");
				Serial.print(constStringLength(Power,4));
				Serial.println(",");
				Serial.print("\t\"timestamp\": ");
				Serial.println(constStringLength(currTime,10));
			Serial.println("}");
			Serial.println("EOL");
		}
		else if(!DEBUG&&newInterface)
		{
			//t timestamp-
			//u voltage-
			//i current-
			//v wind speed-
			//g pitch angle-
			//n rpm-
			//k temperature
			//p pressure
			//h humidity
			//x accelerometer
			//y accelerometer
			//z accelerometer
			//a dummy1
			//b dummy2
			//c dummy3
			//d dummy4
			//e dummy5
			long VCC				= readVcc();
			currTime				= micros();
			readA0					= readVoltage(VCC);		// in mV
			readA1					= readCurrent(VCC);		// in mA
			RPM						= speed.speed();		// in rpm
    // Power  = readA0*readA1/1000;  //in milliWatt
			uint16_t windVelocity	= WindSpeed;			// send back the set windspeed for the moment
			int16_t pitchAngle		= P_Step2Angle(Pitch_Target_Position);
			int16_t	temperature		= 23*10;				// temperature in �C*10
			unsigned long pressure_amb	= 101300000;			// in ambient pressure in mPa
			uint16_t humidity		= 1000;					// humidity in %*10
			int32_t g_x				= 0;					// Acceleration against windflow direction in mg
			int32_t g_y				= 0;					// Acceleration perpendicular to windflow in horizontal direction in mg
			int32_t g_z				= -1;					// Acceleration in up direction  in mg			
			uint16_t current_sink			= Load;					//
			//unsigned long pressure_1		= 101300000;			// dummy 1
			//unsigned long pressure_2		= 101300000;			// dummy 2
			//unsigned long pressure_3		= 101300000;			// dummy 3
			//unsigned long pressure_4		= 101300000;			// dummy 4
			//unsigned long pressure_5		= 101300000;			// dummy 5
			
			//Serial.println("{");
			//Serial.print("\t\"u\": ");					//voltage
			//Serial.print(constStringLength(readA0,5));
			//Serial.println(",");
			//Serial.print("\t\"i\": ");					// current
			//Serial.print(constStringLength(readA1,4));
			//Serial.println(",");
			//Serial.print("\t\"n\": ");					//rpm
			//Serial.print(constStringLength(RPM,8));
			//Serial.println(",");
			//Serial.print("\t\"v\": ");					//windspeed
			//Serial.print(constStringLength(windVelocity,4));
			//Serial.println(",");
			//Serial.print("\t\"g\": ");					//pitch angle
			//Serial.print(constStringLength(pitchAngle,4));
			//Serial.println(",");
			//Serial.print("\t\"k\": ");					//temperature
			//Serial.print(constStringLength(temperature,4));
			//Serial.println(",");
			//Serial.print("\t\"p\": ");					//ambient pressure
			//Serial.print(constStringLength(pressure_amb,9));
			//Serial.println(",");
			//Serial.print("\t\"h\": ");					//humidity
			//Serial.print(constStringLength(humidity,4));
			//Serial.println(",");
			//Serial.print("\t\"x\": ");					//x accelerometer
			//Serial.print(constStringLength(g_x,5));
			//Serial.println(",");
			//Serial.print("\t\"y\": ");					//y accelerometer
			//Serial.print(constStringLength(g_y,5));
			//Serial.println(",");
			//Serial.print("\t\"z\": ");					//z accelerometer
			//Serial.print(constStringLength(g_z,5));
			//Serial.println(",");
			//Serial.print("\t\"a\": ");					//dummy 1
			//Serial.print(constStringLength(pressure_1,9));
			//Serial.println(",");
			//Serial.print("\t\"b\": ");					//dummy 2
			//Serial.print(constStringLength(pressure_2,9));
			//Serial.println(",");
			//Serial.print("\t\"c\": ");					//dummy 3
			//Serial.print(constStringLength(pressure_3,9));
			//Serial.println(",");
			//Serial.print("\t\"d\": ");					//dummy 4
			//Serial.print(constStringLength(pressure_4,9));
			//Serial.println(",");
			//Serial.print("\t\"e\": ");					//dummy 5
			//Serial.print(constStringLength(pressure_5,9));
			//Serial.println(",");
			//Serial.print("\t\"t\": ");					//timestamp
			//Serial.println(constStringLength(currTime,10));
			//Serial.println("}");
			//Serial.println("EOL");
			Serial.println("{");
				Serial.print("\t\"voltage\": ");					//voltage
				Serial.print(constStringLength(readA0,5));
				Serial.println(",");
				Serial.print("\t\"current\": ");					// current
				Serial.print(constStringLength(readA1,4));
				Serial.println(",");
     //  Serial.print("\t\"power\": ");          //rpm
      //  Serial.print(constStringLength(Power,4));
      //  Serial.println(",");
				Serial.print("\t\"rpm\": ");					//rpm
				Serial.print(constStringLength(RPM,8));
				Serial.println(",");
				Serial.print("\t\"windSpeed\": ");					//windspeed
				Serial.print(constStringLength(windVelocity,4));
				Serial.println(",");
				Serial.print("\t\"pitchAngle\": ");					//pitch angle
				Serial.print(constStringLength(pitchAngle,4));
				Serial.println(",");
				Serial.print("\t\"temperature\": ");					//temperature
				Serial.print(constStringLength(temperature,4));
				Serial.println(",");
				Serial.print("\t\"ambientPressure\": ");					//ambient pressure
				Serial.print(constStringLength(pressure_amb,9));
				Serial.println(",");
				Serial.print("\t\"humidity\": ");					//humidity
				Serial.print(constStringLength(humidity,4));
				Serial.println(",");
				Serial.print("\t\"accelerationX\": ");					//x accelerometer
				Serial.print(constStringLength(g_x,5));
				Serial.println(",");
				Serial.print("\t\"accelerationY\": ");					//y accelerometer
				Serial.print(constStringLength(g_y,5));
				Serial.println(",");
				Serial.print("\t\"accelerationZ\": ");					//z accelerometer
				Serial.print(constStringLength(g_z,5));
				//Serial.println(",");
				//Serial.print("\t\"pressure1\": ");					//dummy 1
				//Serial.print(constStringLength(pressure_1,9));
				//Serial.println(",");
				//Serial.print("\t\"pressure2\": ");					//dummy 2
				//Serial.print(constStringLength(pressure_2,9));
				//Serial.println(",");
				//Serial.print("\t\"pressure3\": ");					//dummy 3
				//Serial.print(constStringLength(pressure_3,9));
				Serial.println(",");
				Serial.print("\t\"currentSink\": ");					//dummy 3
				Serial.print(constStringLength(current_sink,4));
				//Serial.println(",");
				//Serial.print("\t\"dummy_d\": ");					//dummy 4
				//Serial.print(constStringLength(pressure_4,9));
				//Serial.println(",");
				//Serial.print("\t\"dummy_e\": ");					//dummy 5
				//Serial.print(constStringLength(pressure_5,9));
				Serial.println(",");
				Serial.print("\t\"timestamp\": ");					//timestamp
				Serial.println(constStringLength(currTime,10));
			Serial.println("}");
			Serial.println("EOL");
			
		}
		else{
			//Raw data output
			Serial.print("V: ");
			Serial.print(readA0);
			Serial.print("; I: ");
			Serial.print(readA1);
			Serial.print("; N: ");
			Serial.print(RPM);
			Serial.print("; t: ");
			Serial.println(currTime);
		}
	}

}
