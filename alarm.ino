#include <DS3231.h>
#include <Wire.h>
#include <EEPROM.h> 
#include <LiquidCrystal.h> 
#include <IRremote.h> 
//pin variables
int buzzerpin = 5; 
int RECV_PIN = 6; 
//objects 
IRrecv irrecv(RECV_PIN); 
LiquidCrystal lcd(7,8,9,10,11,12); 
DS3231 clock1;
decode_results results; 
//alarm variables 
int alarm_hour = 22; 
int alarm_minute = 27; 
int alarm_seconds = 0; 
//eeprom addresses 
const int alarm_hour_address = 0; 
const int alarm_minute_address = 1; 
//rtc variables 
bool century = false;
bool h12Flag;
bool pmFlag;
int hour = 0; 
int minute = 0; 
int second = 0; 
int temp_minutes = 0; 
int temp_hour = 0; 
//function declars
void display_date(String date);
void get_date(int &hour, int &minute, int &second);
//alarm variables 
bool isAlarmTime = false; 
bool mainDisplay = true; 
bool setHourDisplay = false; 
bool setMinuteDisplay = false; 
bool menuDisplay = false; 
void setup() {
  Serial.begin(9600); 
  // add a code to set time if its not set 
  // add code to check for and create alarm if it does not exit
   irrecv.enableIRIn(); 
  irrecv.blink13(true); 
  pinMode(buzzerpin, OUTPUT); 
  lcd.begin(16,2); 
  Wire.begin(); 
}
void loop() {
  alarm_minute = EEPROM.read(alarm_minute_address); 
  alarm_hour =   EEPROM.read(alarm_hour_address); 
get_date(hour,minute,second); 
if (hour == alarm_hour && alarm_minute == minute && second < 3){
  isAlarmTime = true; 
}
if (isAlarmTime){
     digitalWrite(buzzerpin, HIGH);
     delay(1000); 
     digitalWrite(buzzerpin, LOW); 
     delay(1000); 
  }
  if ( irrecv.decode(&results)){
      Serial.print(" "); 
      Serial.print ("Code: "); 
      Serial.println(results.value, HEX); 
      Serial.println(" "); 
     irrecv.resume(); 
    }
    if (results.value == 0xFFA25D && isAlarmTime){
         isAlarmTime = false;  
        Serial.println("Alarm off"); 
        delay(500); 
        results.value = 0; 
        
      
      }
    if (results.value == 0xFFE21D){
         mainDisplay= false; 
         menuDisplay = true;  
         results.value = 0; 
        delay(500); 
        lcd.clear(); 
      
      }
    
if (mainDisplay){
   display_date(hour, minute, false); 
   display_date(alarm_hour, alarm_minute, true); 
  }
else if (menuDisplay){
    lcd.setCursor(0,0); 
    lcd.print("0 (Time), 1 (H)"); 
    lcd.setCursor(0,1); 
    lcd.print(" 2(M) "); 
    delay(1000); 
    if (results.value == 0xFF6897){
      menuDisplay = false; 
      mainDisplay = true; 
      results.value = 0; 
      delay(500); 
      lcd.clear(); 
      }
      else if (results.value == 0xFF30CF){
        menuDisplay = false; 
        mainDisplay = false; 
        setHourDisplay = true; 
        results.value = 0; 
        delay(500); 
        lcd.clear(); 
      }
      else if (results.value == 0xFF18E7){
           menuDisplay = false; 
           mainDisplay = false; 
           setMinuteDisplay = true; 
           results.value = 0; 
           delay(500); 
           lcd.clear(); 
        }
  }
else if (setHourDisplay){
  
   if (temp_hour == -1)
      { 
         temp_hour = 23;
       }
      if (temp_hour == 24)
      {
          temp_hour = 0; 
       }
    lcd.setCursor(0,0); 
    lcd.print("Hour: "); 
    if (temp_hour > 0 && temp_hour <= 12 ){
      lcd.print(String(temp_hour));  
      lcd.print("AM"); 
      }
     else if (temp_hour == 0){
      lcd.print("12AM"); 
     }
      else {
        lcd.print(String(temp_hour - 12));  
        lcd.print("PM"); 
        }
     
    if (results.value == 0xFF906F){
        temp_hour++; 
        results.value = 0; 
        delay(500); 
        lcd.clear(); 
    }
    else if (results.value == 0xFFE01F){
      temp_hour--;
      results.value = 0; 
      delay(500); 
      lcd.clear(); 
     }
   else if (results.value == 0xFF02FD){
      setHourDisplay = false; 
      menuDisplay = true; 
      results.value = 0; 
      EEPROM.write(alarm_hour_address, temp_hour); 
      temp_hour = 0; 
      delay(500); 
      lcd.clear(); 
    }
}
else if (setMinuteDisplay){
       
   if (temp_minutes == -1){
       temp_minutes = 60; 
   }
   
    if (temp_minutes == 61){
        temp_minutes = 0; 
    }
    lcd.setCursor(0,0); 
    lcd.print("Minutes: ");
    lcd.print(String(temp_minutes)); 
    }
     if (results.value == 0xFF906F){
        temp_minutes++; 
        results.value = 0; 
        delay(500); 
        lcd.clear(); 
    }
    else if (results.value == 0xFFE01F){
      temp_minutes--;
      results.value = 0; 
      delay(500); 
      lcd.clear(); 
     }
   else if (results.value == 0xFF02FD){
      setMinuteDisplay = false; 
      menuDisplay = true;
      //alarm_minute = temp_minutes; 
      EEPROM.write(alarm_minute_address, temp_minutes); 
      temp_minutes = 0;  
      results.value = 0; 
      delay(500); 
      lcd.clear(); 
    }


}



//function implementations 
void display_date(int hour, int minute, bool isAlarm){
  String date_string = ""; 
  //lcd.clear();
 if (isAlarm){
  lcd.setCursor(0,1); 
    date_string += "Alarm: "; 
  }
  else {
    lcd.setCursor(0,0); 
   date_string += "Time: "; 
  }
  if (hour > 0 &&hour <= 12){
     date_string += String(hour) + ":"; 
   }
   else if (hour == 0 ){
    date_string += "12" ;
    date_string += ":"; 
    }
   else {
       date_string += String(hour-12) + ":"; 
    }
    if (minute <=9){
      date_string += "0"+ String(minute); 
      }
      else {
          date_string += String(minute); 
        }
   
   if (hour <= 12){
     date_string += "AM";
    }
   else{
     date_string += "PM"; 
     }
  
  lcd.print(date_string); 
  delay(150); 
  }
  
//get current date 
void get_date(int &hour, int &minute, int &second){
 hour = clock1.getHour(h12Flag, pmFlag);
 minute = clock1.getMinute(); 
 second = clock1.getSecond(); 
}
//set alarm 
void set_alarm(int hour, int minute){
       
  }
bool check_alarm(){
  
  }
