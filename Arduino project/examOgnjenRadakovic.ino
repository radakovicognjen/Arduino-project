 //#define MotionSensorPin   12   errors with the motion sensors --> switched to button
#include <TimerOne.h>
#define LM35              A0
#define PHR               A1
#define RelayMotorCooler  11
#define RelayHeater       10
#define RelayDiode        7
#define RelayMaster       5
#define ButtonPin         2

int motionStateCurrent;   
int motionStatePrevious;  
int RelayDiodeStatus  = LOW;
int RelayHeaterStatus = LOW;
int RelayCoolerStatus = LOW;
int RelayMasterStatus = HIGH;
 
float CurrentTemperature;
int temperature;  
bool heating = false;
bool cooling = false;

void setup() {
  
  Serial.begin(9600);                // initialize serial (console writing)

  //illumination
  pinMode(RelayDiode, OUTPUT);
  digitalWrite(RelayDiode, RelayDiodeStatus);

  //motion/button
  pinMode(ButtonPin, INPUT);   // setting arduino pin to input mode

  //heater/cooling
  pinMode(RelayHeater, OUTPUT);  // to output mode
  digitalWrite(RelayHeater,  RelayHeaterStatus);
 
  pinMode(RelayMotorCooler, OUTPUT);
  digitalWrite(RelayMotorCooler,  RelayCoolerStatus);    
  
  temperature = (analogRead(LM35) * 500.0) / 1023; // formula for calculating temperature
  
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(autoTemp);
  
  // Master Relay
  pinMode(RelayMaster, OUTPUT);
  digitalWrite(RelayMaster,   RelayMasterStatus);



}

//here the functions will repeatedly be checked
void loop() {
  autoTemp();
  autoLight();
  homeSecure();  

 // int buttonState = digitalRead(ButtonPin);  // read the state of the button
 // if (buttonState == LOW) {                 // check if the button is pressed
 //   RelayDiodeStatus = !RelayDiodeStatus;   // toggle the state of the light diode
 //   digitalWrite(RelayDiode, RelayDiodeStatus);
 // }
  


  // turningi off and on parts directly through the serial monitor
  if(Serial.available() > 0){
    String SERIAL_INPUT = Serial.readString();

      if(SERIAL_INPUT == "Master is On"){
        RelayMasterStatus = HIGH;
        digitalWrite(RelayMaster, HIGH);
      //  Serial.println("Master Relay turned on!");
        
      } else if (SERIAL_INPUT == "Master is Off") {
        
        RelayMasterStatus = LOW;
        digitalWrite(RelayMaster, LOW);
      //  Serial.println("Master Relay turned off!");
        
      } else if (SERIAL_INPUT == "Lights are On"){
        if(RelayMasterStatus == HIGH){
          digitalWrite(RelayDiode, HIGH);
      //    Serial.println("Relay Diode turned on!");          
        }
      //  else Serial.println("Master Relay isn't on!");
        
      } else if (SERIAL_INPUT == "Lights are Off"){
        digitalWrite(RelayDiode, LOW);
     //   Serial.println("Relay Diode turned off!");
        
      } else if (SERIAL_INPUT == "Heater is On"){
     
        if(RelayMasterStatus == HIGH){
          digitalWrite(RelayHeater, HIGH);
       //   Serial.println("Relay Heater turned on!");
        }//else Serial.println("Master Relay isn't on!");
        
      } else if (SERIAL_INPUT == "Heater is off"){
     
        digitalWrite(RelayHeater, LOW);
        Serial.println("Relay Heater turned off!");
      
      } else if (SERIAL_INPUT == "Cooler is On"){
     
        if(RelayMasterStatus == HIGH){
          digitalWrite(RelayMotorCooler, HIGH);
       //   Serial.println("Relay Cooler/Motor turned on!");
        }// else Serial.println("Master Relay isn't on!");
       
      } else if (SERIAL_INPUT == "Cooler is Off"){
     
        digitalWrite(RelayMotorCooler, LOW);
     //   Serial.println("Relay Cooler/Motor turned off!");
       
      }
      
  }
}

//function for automatic temperature reading, if temperature is less than 17 degrees
//heater (in this case it is the DC motor) will be turnned on, if the temperature 
//is more than 23 cooler will be turned on

void autoTemp(){
  CurrentTemperature = (analogRead(LM35) * 500.0 / 1023);
  
  if(CurrentTemperature > 23)
  {
    cooling = false;
    //RelayCoolerStatus = LOW;
    digitalWrite(RelayMotorCooler, LOW);
    
    heating = true;
    //RelayHeaterStatus = HIGH;
    digitalWrite(RelayHeater, HIGH);
    
  }
  else{
    if(CurrentTemperature < 17)
    {
      cooling = true;
      //RelayCoolerStatus = HIGH;
      digitalWrite(RelayMotorCooler, HIGH);
      
      heating = false;
      //RelayHeaterStatus = LOW;
      digitalWrite(RelayHeater, LOW);

    }
  }
  Timer1.initialize(5000000);  // delay to avoid overloading, so it can read it properly
  Serial.println(CurrentTemperature);
}

// turning on lights via button
// when the button is pressed, it should turn the diode on for 10s
void homeSecure(){
   motionStateCurrent  = digitalRead(ButtonPin); // read new stated
   //Serial.println(motionStateCurrent);
   //delay(1000);
   if(motionStateCurrent == LOW){
     digitalWrite(RelayDiode, HIGH);  // turn on relay that is connected to the diode
     Timer1.initialize(10000000);                   // 10s delay
     digitalWrite(RelayDiode, LOW); // turn off relay that is connected to the diode
   }
   motionStatePrevious = motionStateCurrent;             // store old state 
   Timer1.initialize(5000000);
   Serial.println(motionStateCurrent);
  }

// if illumination is less than 30%, the diode will be turned on, and it will stay like that
// until the illumination goes above 30%
void autoLight(){
  
  //illimination/
  int i = map(analogRead(PHR), 0, 1023, 0, 100);
  if(i < 30) {
    digitalWrite(RelayDiode, HIGH);
  } else {
    digitalWrite(RelayDiode, LOW);
  }
  Timer1.initialize(5000000);
  Serial.println(i);
}
