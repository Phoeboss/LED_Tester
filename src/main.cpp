#include <Arduino.h>
#include <HT1621.h>
#include <MyDisplay.h>
#include <ADS1115_WE.h>
#include <Wire.h>
#include <math.h>

#define BUTTON_POWER A3
#define POWER_HOLD A0

#define HV_ENABLE 1
#define HV_BOOST 0
#define I_OFF 0 //Not a Pin
#define I_01MA 7 
#define I_1MA 6
#define I_10MA 8
#define STRING_ENABLE 10

#define LED_CENTER 9
#define BACKLIGHT 5
#define BUTTON_ANALOG A2

#define MODE_DIODE_LOW 1
#define MODE_DIODE_HIGH 2
#define MODE_HOLD 1
#define MODE_CONTINOUS 0

#define I2C_ADDRESS 0x48

MyDisplay displ(3, 4, 2);
HT1621 lcd(3, 4, 2);
ADS1115_WE adc(I2C_ADDRESS);

uint8_t mode = MODE_DIODE_LOW;
uint8_t current = I_1MA;
uint8_t hold = MODE_HOLD;

void beepPowerOn(){
  displ.beep(100, HT1621::TONE2K);
  displ.beep(100, HT1621::TONE4K);
}

void beepPowerOff(){
  displ.beep(500, HT1621::TONE4K);
  displ.beep(200, HT1621::TONE2K);
}

void beepAccept(){
  displ.beep(100, HT1621::TONE4K);
}

void beepDetected(){
  displ.beep(50, HT1621::TONE2K);
}

void currentSet(uint8_t setting){
  digitalWrite(I_01MA, LOW); digitalWrite(I_1MA, LOW); digitalWrite(I_10MA, LOW);
  if(setting != I_OFF)
    digitalWrite(setting, HIGH);
}

uint8_t buttonsCheck(){
  if(analogRead(BUTTON_POWER) < 50)
    return 4;
  uint16_t reading = analogRead(BUTTON_ANALOG);
  if(reading > 600)
    return 0;
  if(reading > 420)
    return 1;
  if(reading > 200)
    return 2;
  if(reading > 40)
    return 3;
  
  return 0;
}

float readChannel(ADS1115_MUX channel) {
  float TmpVoltage = 0.0;
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
  while(adc.isBusy()){}
  TmpVoltage = adc.getResult_V(); // alternative: getResult_mV for Millivolt
  return TmpVoltage;
}


void setup()
{
  //IO Config
  pinMode(POWER_HOLD, OUTPUT);
  digitalWrite(POWER_HOLD, HIGH);
  pinMode(BUTTON_POWER, INPUT);
  currentSet(I_OFF);
  pinMode(STRING_ENABLE, INPUT_PULLUP);
  //digitalWrite(STRING_ENABLE, LOW);
  //LED Check
  digitalWrite(BACKLIGHT, HIGH);
  delay(1000);
  digitalWrite(BACKLIGHT, LOW);
  //HV Check/Init
  pinMode(HV_ENABLE, OUTPUT);
  pinMode(HV_BOOST, OUTPUT);
  digitalWrite(HV_BOOST, LOW);
  digitalWrite(HV_ENABLE, HIGH);
  //Display
  displ.begin();
  displ.clear();
  displ.symbol(S_HOLD);
  //ADS1115  
  Wire.begin();
  if(!adc.init())
    displ.print("ERR1");
  adc.setVoltageRange_mV(ADS1115_RANGE_4096);

  beepPowerOn();
  delay(2000);
}
float voltage = 1.0;
float voltage_min = 60.0;
float voltage_hold = 60.0;
uint8_t button = 0;
boolean detected = false;
uint16_t timeout = 1000;

void loop()
{
  if(timeout == 0){
    currentSet(I_OFF);
    beepPowerOff();
    digitalWrite(POWER_HOLD, LOW);
  }
  timeout--;

  button = 0;
  if(buttonsCheck() != 0)
  {
    uint8_t nextMode=mode;
    timeout = 1000;
    //Average Button
    for(uint8_t i=0;i<32;i++)
      button += buttonsCheck();
    button /= 32;

    beepAccept();
    displ.clear();
    if(button == 2){
      uint8_t nextHold=hold;
      if(hold == MODE_HOLD)
        nextHold=MODE_CONTINOUS;
      if(hold == MODE_CONTINOUS)
        nextHold=MODE_HOLD;

      hold=nextHold;
    }
    //------------------
    if(button == 1){
      if(mode == MODE_DIODE_LOW){
        displ.print("]60["); displ.symbol(S_VOLT);
        nextMode=MODE_DIODE_HIGH;
      }
      else {
        displ.print("]40["); displ.symbol(S_VOLT);
        nextMode=MODE_DIODE_LOW;
      }
      mode=nextMode;
      delay(1000); displ.clear();
    } 
    //------------------
    if(button == 3){
      uint8_t nextCurrent=current;
      if((mode == MODE_DIODE_LOW) || (mode=MODE_DIODE_HIGH)){
        if(current == I_1MA){
          displ.print((uint16_t)10); displ.symbol(S_MILLI); displ.symbol(S_AMPERE);
          nextCurrent=I_10MA;
        }
        if(current == I_10MA){
          displ.print((uint16_t)100); displ.symbol(S_MICRO); displ.symbol(S_AMPERE);
          nextCurrent=I_01MA;
        }
        if(current == I_01MA){
          displ.print((uint16_t)1); displ.symbol(S_MILLI); displ.symbol(S_AMPERE);
          nextCurrent=I_1MA;
        }
        current=nextCurrent;
      }
    delay(1500); displ.clear();
    } 

    if(button == 4){
      //beepPowerOff();
      //digitalWrite(POWER_HOLD, LOW);
    } 
  }

  //------------------------------------------------------------------
  //
  if(mode == MODE_DIODE_LOW){
    currentSet(current); delay(600);
    displ.symbol(S_DIODE);
    voltage = readChannel(ADS1115_COMP_2_3)*11.0;
    if(voltage < 3.4)
      adc.setVoltageRange_mV(ADS1115_RANGE_1024);
      voltage = readChannel(ADS1115_COMP_2_GND)*11.0;
      adc.setVoltageRange_mV(ADS1115_RANGE_4096);
    if((voltage < 36) && (detected == false)){
      beepDetected();
      detected=true;
    }
    if(detected == true){
      if(voltage < voltage_min)
        voltage_min = voltage;
      if(voltage > (voltage_min+1.0)){
        delay(500);
        detected=false;
        voltage_min=60.0;
      } 
    }
    if(hold == MODE_CONTINOUS)
      displ.print(voltage); displ.symbol(S_VOLT);
    if((hold == MODE_HOLD) && (detected==true))
      displ.print(voltage_min); displ.symbol(S_VOLT);    
  }

  if(mode == MODE_DIODE_HIGH){
    displ.symbol(S_DIODE);
    voltage = readChannel(ADS1115_COMP_2_3)*11.0;
    displ.print("nope"); displ.symbol(S_VOLT); displ.symbol(S_TRIANGLE);
  }


  if(hold==MODE_HOLD)
    displ.symbol(S_HOLD);

  
  
  
  //digitalWrite(I_1MA, LOW);
  
}