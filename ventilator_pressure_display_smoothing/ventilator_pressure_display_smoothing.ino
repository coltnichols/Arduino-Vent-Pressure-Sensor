
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int buzzer = 11;             // Piezo buzzer with + on pin 11, - on GND
const int led = 12;                // LED with + on pin12, - on GND

const int Vcc = 7;               //Pressure sensor pins
//const int Out2 = A2;
const int Out1 = A1;

unsigned long time_old = 0;            // Variables to record time
unsigned long time_new = 0;
unsigned long time_lb = 0;


int limit = 10;                    // ms limit to for how fast to run program
bool alarm = false;                // Turns alarm on or off  

int t_buzz = 400;                  // ms length of each alarm buzz
int t_buzz_brk = 100;              // ms length between buzz
int f_buzz = 500;                  // buzzer frequency
int p_init;

const int numReadings = 32;
int srate = 500;                // sampling rate, ms between
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;
int average = 0;                // the average
unsigned long tlast = limit;

int concern = 0;
int peak_ind;
int ind = 0;
void setup()
{  
  Serial.begin(9600);              //This is just for the prototyper to open the serial port and see the stream of data

  pinMode(buzzer, OUTPUT);         // Set buzzer - pin 11 as an output
  pinMode(led, OUTPUT);            // Set LED - pin 13 as an output

  pinMode(Out1, INPUT);
  pinMode(A3, OUTPUT);
  analogWrite(A3, 400); 

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  void lcdClear();

  p_init = analogRead(Out1);     //initial pressure as a standard for disconnects

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
  readings[thisReading] = 0;
  
}
}
void loop()
{
  time_new = millis();              // Update time

//Read Pressure
 int P1 = analogRead(Out1);
 
//Alarm Conditions
  if(P1<p_init){alarm = true;}
  else if(concern >= 5000/limit){alarm = true;}
  else {alarm = false;}
  
//Alarm buzzer
if(alarm==true){
    if( millis()< time_lb + t_buzz){
      tone(buzzer, f_buzz);
    }
    else {
      noTone(buzzer);
      if (millis()>= time_lb + t_buzz + t_buzz_brk){
        time_lb = millis();
      } 
    }
  }
if(alarm==false){
    noTone(buzzer);
  }

  Serial.print("\t");
  Serial.print(P1);
  Serial.print("\t");         
  Serial.print(100*alarm+550 );  
  Serial.print("\t"); 
  Serial.print(p_init); 
  Serial.print("\t"); 
  Serial.print(concern); 
  Serial.print("\t");
  Serial.println(average); 


//LCD screen
  lcd.setCursor(0,0);
  lcd.print("Pres:");
  lcd.print(P1);

//Data Averaging and Recording
if(millis()>=tlast+srate){
  tlast=millis();
  total = total - readings[readIndex];          // subtract the saved data location, to prep for new

  readings[readIndex] = P1;   // read from the sensor:
  
  total = total + readings[readIndex];          // add the reading to the total:
  
  readIndex = readIndex + 1;                    // advance to the next position in the array:
  
  if (readIndex >= numReadings) {               // if we're at the end of the array...
    readIndex = 0;                                 // ...wrap around to the beginning:
  }
  average = total / numReadings;                // calculate the average:
}
//How long has pressure not peaked
if(concern > 0 ){
  if(P1<ind){
  concern += 1; }
  else{ concern = 0;}
} 
  
if(P1<average && concern == 0 ){
  concern += 1;
  ind = average;
}

//Program Max Speed Limit
  while(millis()<time_new+limit)        
  {} //do nothing

}

void lcdClear()
{
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}
