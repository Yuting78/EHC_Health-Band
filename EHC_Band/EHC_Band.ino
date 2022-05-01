#include "rgb_lcd.h"

#include <math.h>

#include <Wire.h>
#include <math.h>

#define TEMP_SENSOR (A2)  // Grove - Temperature Sensor connect to A0
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.  
#define R_TEMP_SENSOR (A2)  // Grove - Temperature Sensor connect to A0
#define B_TEMP_SENSOR (A3)  // Grove - Temperature Sensor connect to A0


const int touchpin = 7; // the Arduino's input pin that connects to the sensor's SIGNAL pin 

int user_age_2;
int age;
int BTemp_State = 0;
int Smoke_State = 0;
int HR_State = 0;

int State = 0;
int user_age = 0;
int test_age;
int smokeA1 = A1; 
int myBPM;
int max_HR;
float R_temp_C; // Variable used to store Celsius temperature
float B_temp_C; // Variable used to store Fahrenheit temperature
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int greenLed = 4;          // The on-board Arduino LED, close to PIN 13.
const int redLed = 2; 
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
int buzzer = 5;
int smoke_sensorThres = 300;
const int buttonPin = 6;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status


int lastState = LOW;      // the previous state from the input pin
int currentState;         // the current reading from the input pin
                             
rgb_lcd lcd;

PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"


void setup() {
  
  pinMode(R_TEMP_SENSOR, INPUT); // Configure pin A2 as an INPUT
  pinMode(B_TEMP_SENSOR, INPUT); // Configure pin A2 as an INPUT 
  pinMode(buzzer, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(smokeA1, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(touchpin, INPUT);
  
  
  pulseSensor.analogInput(PulseWire);   
  //pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);  
  Serial.begin(9600);
  noTone(buzzer);
  
  pulseSensor.begin();



  lcd.begin(16,2);
  lcd.clear();
  Serial.println("Please Input Your Age"); 
  lcd.setCursor(0,0);
  lcd.print("Please Input");
  lcd.setCursor(0,1);
  lcd.print("Your Age");  

  
}

void loop() {
  if (Serial.available() > 0) {
    lcd.clear();
    String input_age = Serial.readStringUntil('\n');
    int age = input_age.toInt();
  
    if (isValidNumber(input_age) == 1 and 1 <= age and age < 110){
      user_age = age;
      Serial.print("Your age is: "); 
      Serial.print(age); 
      
      lcd.setCursor(4,0);
      lcd.print("Welcome!"); 
      lcd.setCursor(5,1);
      lcd.print("Age:");
      lcd.print(age);
      delay(5000);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Please Wear");
      lcd.setCursor(0,1);
      lcd.print("Device");
      delay(20000);

      lcd.clear();
      load();
      //delay(2000);
      delay(80000);
      
      lcd.clear();
      
      lcd.setCursor(0,0);
      lcd.print("Ready!");
      delay(2000);
      lcd.clear();
      State = 1;
      
                       
    }
    else{
      Serial.println("Please Input Valid Age");
      lcd.setCursor(0,0);
      lcd.print("Please Input");
      lcd.setCursor(0,1);
      lcd.print("Valid Age");     
    }  
   
  }
  const int user_age_2 = user_age;


  
  if (State == 1){  
    
    int test_age = user_age_2;
    R_temp_C = read_Temperature(A2);
    B_temp_C = read_Temperature(A3);
    int smoke = analogRead(smokeA1);
    int myBPM = pulseSensor.getBeatsPerMinute();
    buttonState = digitalRead(buttonPin);

    
    
    Serial.println("♥  Values detected! "); // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                        // Print phrase "BPM: " 
    Serial.println(myBPM);
    Serial.print("Safe Heart Rate Vale: ");
    Serial.println(220-test_age);
    Serial.print("Safe Temperature Value: ");
    Serial.println(get_max_Temp(B_temp_C, R_temp_C));

    
    
    Serial.print("Body Temperature State:");
    Serial.println(check_abnormal_BTemp(B_temp_C, B_temp_C ));
    Serial.print("Smoke State:");
    Serial.println(check_abnormal_smoke(smoke, smoke_sensorThres));
    Serial.print("Heart Rate State: ");
    Serial.println(check_abnormal_HR(user_age, myBPM));

    BTemp_State = check_abnormal_BTemp(B_temp_C, B_temp_C );
    Smoke_State = check_abnormal_smoke(smoke, smoke_sensorThres);
    HR_State = check_abnormal_HR(user_age, myBPM);
    

    if(check_abnormal_BTemp(B_temp_C, B_temp_C )){
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      tone(buzzer, 1000, 200);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Abnormal");
      lcd.setCursor(0,1);
      lcd.print("Body Temp!");
    }
    else if(check_abnormal_smoke(smoke, smoke_sensorThres)){
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      tone(buzzer, 1000, 200);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Abnormal");
      lcd.setCursor(0,1);
      lcd.print("Environment!");
    }
    
    else if(check_abnormal_HR(user_age, myBPM)){
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      tone(buzzer, 1000, 200);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Abnormal");
      lcd.setCursor(0,1);
      lcd.print("Heart Rate!");
    }
    
     
    else if(buttonState == HIGH){
 
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      tone(buzzer, 1000, 200); 
      
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Emergency!");
    }
    
    else{ 
       
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      noTone(buzzer);

      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("B:");
      lcd.print(B_temp_C,1);
      lcd.print("C ");
      lcd.print("R:");
      lcd.print(R_temp_C,1);
      lcd.print("C");  
           
      lcd.setCursor(0,1);
      lcd.print("S:");
      lcd.print(smoke); 
      lcd.print("PPM");
      
      lcd.setCursor(8,1);
      lcd.print("BPM:"); 
      lcd.print(myBPM);

    }

  }
  
delay(1000);
}

boolean isValidNumber(String str){
  for(byte i=0;i<str.length();i++)
  {
   if(isDigit(str.charAt(i))){
    return true;
   }
  }
  return false;
}

void load(){
  
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setCursor(1, 0);
  lcd.write(byte(1));
  lcd.setCursor(2, 0);
  lcd.write(byte(2));
  lcd.setCursor(3, 0);
  lcd.write(byte(3));
  lcd.setCursor(4, 0);
  lcd.write(byte(4));
  lcd.setCursor(1, 1);
  lcd.write(byte(5));
  lcd.setCursor(2, 1);
  lcd.write(byte(6));
  lcd.setCursor(3, 1);
  lcd.write(byte(7));
  lcd.setCursor(4, 1);
  lcd.write(byte(8));
  lcd.setCursor(6, 1); 
  lcd.print("Warming Up");

}

// forearm 32.6±0.9
//https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8266026/
boolean check_abnormal_BTemp(float B_Temp, float R_Temp){
  float cutoff_value = 36.2;
  if(R_Temp < 15){
    float cutoff_value_cold = cutoff_value - 0.96; 
    if(cutoff_value_cold < B_Temp){
      return true;
    }
  }
  if(15<=R_Temp and R_Temp<= 47){
    if(cutoff_value < B_Temp){
      return true;
    }
  }
  if(R_Temp> 47){
    float cutoff_value_hot = cutoff_value + 0.96;
    if(cutoff_value_hot < B_Temp){
      return true;
    }
  }
  return false;    
}

boolean check_abnormal_smoke(int smoke, int smoke_sensorThres){
  if(smoke>smoke_sensorThres){
    return true;
  }
  return false;
}

boolean check_abnormal_HR(int age, int heart_rate){
  int  max_HR = 220 - age;
  if(heart_rate>max_HR){
    return true;
  }
  return false;
}


/*
 * @brief:  Reads an Analog input. 
 *          Converts the analog voltage value into a Temperature value in degrees Celsius. 
 * @param:  pin - Analog Input pin number
 * @ret:    temperature - Temperature value in degrees Celsius (float).
 */
float read_Temperature(int pin){
  const int B = 4275;         // B value of the thermistor
  const int R0 = 100000;      // R0 = 100k
  int a = analogRead(pin);    // Integer: 0-1023
  float R = 1023.0/a-1.0;
  R = R0*R;
  float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;  // convert to temperature via datasheet
  return temperature;
}


float get_max_Temp(float B_Temp, float R_Temp){
  float cutoff_value = 36.2;
  float max_Temp;
  if(R_Temp < 15){
    float cutoff_value_cold = cutoff_value - 0.96;
    max_Temp =  cutoff_value_cold;
  }
  if(15<=R_Temp and R_Temp<= 47){
    max_Temp = cutoff_value;
  }
  if(R_Temp> 47){
    float cutoff_value_hot = cutoff_value + 0.96;
    max_Temp =  cutoff_value_hot;

  }
  return max_Temp;   
}


//heart rate range: https://my.clevelandclinic.org/health/diagnostics/17402-pulse--heart-rate
//220 - your age = predicted maximum heart rate

//https://www.ncbi.nlm.nih.gov/pmc/articles/PMC7992731/
//Regional Variation of Human Skin Surface Temperature

//https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8266026/
//Validity of the Use of Wrist and Forehead Temperatures in Screening the General Population for COVID-19: A Prospective Real-World Study
