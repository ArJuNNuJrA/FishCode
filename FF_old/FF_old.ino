#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#define interruptPin 2 //Pin we are going to use to wake up the Arduino
#include <DS3232RTC.h>  //RTC Library https://github.com/JChristensen/DS3232RTC

const byte motorPlus = 7; //PIN for motor signal
const byte relay = 12; //PIN for LIGHT CONTROL
byte flag = 0;//Flag for Millis Loop
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);//Start Serial Comunication
  pinMode(relay, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP); //Set pin d2 to input using the buildin pullup resistor
  digitalWrite(relay, LOW);//turning LED on
  digitalWrite(8, LOW);
  digitalWrite(motorPlus, HIGH);


  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
  time_t t; //create a temporary time variable so we4 can set the time and read the time from the RTC
  t = RTC.get(); //Gets the current time of the RTC
  pinMode(motorPlus, OUTPUT);
//    setTime(13, 18, 10, 25, 5, 2021);
//    RTC.set(now());
//    RTC.writeRTC(5, 10);
//    RTC.writeRTC(4, 11);  // if reset was done
}

void loop() {
  byte flag1 = RTC.readRTC(5); //ALARM FLAG 1
  byte flag2 = RTC.readRTC(4); //ALARM FLAG 2
  time_t m;
  m = RTC.get();
  currentMillis = millis();
  //  Serial.println(flag1);
  //   Serial.println(flag2);
//  Serial.println("TIME: " + String(hour(m)) + ":" + String(minute(m)) + ":" + String(second(m)));


  if (hour(m) <= 6 || hour(m) >= 22) //LIGHTS OFF 10:00 PM - 7:00 AM
  {
    RTC.setAlarm(ALM2_MATCH_HOURS  , 0, 30, 7, 0); //7:30 AM ALARM
    RTC.alarm(ALARM_2);
    RTC.squareWave(SQWAVE_NONE);
    RTC.alarmInterrupt(ALARM_2, true);
    RTC.writeRTC(4, 11);
    lightOff();
    Going_To_Sleep();
  }
  if (hour(m) == 7 && minute(m) < 30)
  {
    RTC.setAlarm(ALM2_MATCH_HOURS  , 0, 30, 7, 0); //7:30 AM ALARM
    RTC.alarm(ALARM_2);
    RTC.squareWave(SQWAVE_NONE);
    RTC.alarmInterrupt(ALARM_2, true);
    RTC.writeRTC(4, 11);
    lightOff();
    Going_To_Sleep();
  }

  if ((hour(m) == 7 && minute(m) >= 30 && second(m) >= 0)  || (hour(m) == 8 && minute(m) <= 29 && second(m) <= 59)) //Light ON AT 7:30AM
  {
    RTC.setAlarm(ALM1_MATCH_HOURS  , 0, 30, 8, 0); //ALARM AT 8:30 AM FOR FOOD
    RTC.squareWave(SQWAVE_NONE);
    lightOn();
    RTC.writeRTC(4, 11);
    Going_To_Sleep1();
  }

  if (((hour(m) == 8 && minute(m) >= 30 && second(m) >= 0) || (hour(m) > 8 && (hour(m) < 20  || (hour(m) == 20 && minute(m) <= 29)))) && flag1 != 10 && flag2 != 10) //LIGHT ON WITH FOOD (8:30 AM - 8:29 PM)
  {
    lightOn();
    motorOn1();
    RTC.writeRTC(5, 10);
    RTC.writeRTC(4, 11);
    RTC.setAlarm(ALM2_MATCH_HOURS, 0, 30, 20, 0);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, true);
    Going_To_Sleep2();
  }
  if (((hour(m) == 8 && minute(m) >= 30 && second(m) > 0) || (hour(m) > 8 && (hour(m) < 20  || (hour(m) == 20 && minute(m) <= 29)))) && flag1 == 10 && flag2 != 10) //LIGHT ON WITHOUT FOOD (8:30 AM - 8:29 PM)
  {
 
    lightOn();
    RTC.setAlarm(ALM2_MATCH_HOURS, 0, 30, 20, 0);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, true);
    Going_To_Sleep2();

  }

  if ((hour(m) == 20 && minute(m) == 30 && second(m) >= 0) && flag2 != 10 ) //MOTOR ON FOR 8:30 PM FOOD
  { motorOn2();
    RTC.writeRTC(5, 11);
    RTC.writeRTC(4, 10);
    Going_To_Sleep3();
  }
  if (((hour(m) == 20 && minute(m) >= 30 && second(m) > 0) || (hour(m) == 21 && minute(m) <= 29 && second(m) <= 59)) && flag2 != 10) //LIGHT ON WITH FOOD (8:30 PM - 9:29 PM)
  {

    lightOn();
    motorOn2();
    RTC.writeRTC(5, 11);
    RTC.writeRTC(4, 10);
    RTC.setAlarm(ALM1_MATCH_HOURS, 0, 30, 21, 0);
    RTC.alarm(ALARM_1);
    RTC.alarmInterrupt(ALARM_1, true);
    RTC.alarmInterrupt(ALARM_2, false);
    Going_To_Sleep3();
  }

  if (((hour(m) == 20 && minute(m) >= 30 && second(m) > 0) || (hour(m) == 21 && minute(m) <= 29 && second(m) <= 59)) && flag2 == 10) //LIGHT ON WITHOUT FOOD (8:30 PM - 9:29 PM)
  {
    lightOn();
    RTC.writeRTC(5, 11);
    RTC.setAlarm(ALM1_MATCH_HOURS, 0, 30, 21, 0);
    RTC.alarm(ALARM_1);
    RTC.alarmInterrupt(ALARM_1, true);
    RTC.alarmInterrupt(ALARM_2, false);
    Going_To_Sleep3();
  }
  if ((hour(m) == 21 && minute(m) >= 30 && second(m) >= 0) || hour(m) >= 22) //LIGHT OFF AFTER 9:30 PM AND 10:00 PM
  {
    lightOff();
    RTC.writeRTC(4, 11);
    RTC.setAlarm(ALM2_MATCH_HOURS  , 0, 30, 7, 0);
    RTC.squareWave(SQWAVE_NONE);
    RTC.alarmInterrupt(ALARM_2, true);
    Going_To_Sleep();
  }


}

void Going_To_Sleep() {
  sleep_enable();//Enabling sleep mode
  attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
  delay(1000); //wait a second to allow the led to be turned off before going to sleep
  sleep_cpu();//activating sleep mode;
  RTC.alarm(ALARM_2);
  RTC.alarm(ALARM_1);
}
void Going_To_Sleep1() {
  RTC.alarm(ALARM_2);
  RTC.alarm(ALARM_1);
  sleep_enable();//Enabling sleep mode
  attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.alarmInterrupt(ALARM_1, true);

  delay(1000); //wait a second to allow the led to be turned off before going to sleep
  sleep_cpu();//activating sleep
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, 30, 20, 0);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, true);
  RTC.alarm(ALARM_2);
  RTC.alarm(ALARM_1);
}
void Going_To_Sleep2() {
  sleep_enable();//Enabling sleep mode
  attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, 30, 21, 0);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  delay(1000); //wait a second to allow the led to be turned off before going to sleep
  sleep_cpu();//activating sleep mode

  RTC.alarmInterrupt(ALARM_1, true);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
}
void Going_To_Sleep3() {
  sleep_enable();//Enabling sleep mode
  attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, 30, 7, 0);
  RTC.alarmInterrupt(ALARM_1, true);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  delay(1000); //wait a second to allow the led to be turned off before going to sleep
  sleep_cpu();//activating sleep mode

  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, true);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
}

void wakeUp() {
  sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;

}
//void motorOn1()
//{ digitalWrite(8, HIGH);
//  delay(60000);
//  digitalWrite(motorPlus, LOW);
//  delay(400); //Food motor time
//  RTC.writeRTC(5, 10);
//  digitalWrite(motorPlus, HIGH);
//  delay(600000);
//  digitalWrite(8, LOW);
//}
//void motorOn2()
//{ digitalWrite(8, HIGH);
//  delay(60000);
//  digitalWrite(motorPlus, LOW);
//  delay(400); //Food moto time
//  RTC.writeRTC(4, 10);
//  digitalWrite(motorPlus, HIGH);
//  delay(600000);
//  digitalWrite(8, LOW);
//}
void motorOn1()
{
  digitalWrite(8, HIGH);
  previousMillis = millis();
  while (flag == 0) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 60000) {
      flag = 1;
    }
  }

  digitalWrite(motorPlus, LOW);
  flag = 0;
  previousMillis = millis();
  while (flag == 0) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 400) {
      flag = 1;
    }
  }
  RTC.writeRTC(5, 10);
  digitalWrite(motorPlus, HIGH);

  flag = 0;
  previousMillis = millis();
  while (flag == 0) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 600000) {
      flag = 1;
    }
  }
  digitalWrite(8, LOW);
}
void motorOn2()
{
  digitalWrite(8, HIGH);
  flag = 0;
  previousMillis = millis();
  while (flag == 0) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 60000) {
      previousMillis = currentMillis;
      flag = 1;
    }
  }

  digitalWrite(motorPlus, LOW);
  flag = 0;
  previousMillis = millis();
  while (flag == 0) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 400) {
      flag = 1;
    }
  }
  RTC.writeRTC(4, 10);
  digitalWrite(motorPlus, HIGH);

  flag = 0;
  previousMillis = millis();
  while (flag == 0) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 600000) {
      flag = 1;
    }
  }
  digitalWrite(8, LOW);
}

void lightOn()
{ digitalWrite(relay, HIGH);
}
void lightOff()
{ digitalWrite(relay, LOW);
}
