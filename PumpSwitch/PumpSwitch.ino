

#include <BTLE.h>

//char No[] PROGMEM  =  "No ";
//#include <EEPROM.h>

#include <MsTimer2.h>

#include <DS18B20.h>

#define DEBUG_CONS    1
#define MENU_ENABLED  0

#define N_SENSORS  1

const uint8_t addresses[N_SENSORS][8] PROGMEM = {{ 0x28, 0xFF, 0x2D, 0x60, 0x2D, 0x04, 0x00, 0x73,} , };
int8_t temperatures[N_SENSORS];
uint8_t ds_address[8];
// #include "/home/volk/Arduino/My/pumpswitch/PumpSwitchTerminal.h"
#include "PumpSwitch.h"
#include "transmit_ble.h"
#include "transmit_nrf.h"
#if MENU_ENABLED == 1
//#include "/home/volk/Arduino/My/pumpswitch/menu.h"
#endif

// { 0x28, 0xFF, 0x2D, 0x60, 0x2D, 0x04, 0x00, 0x73}


RF24 radio(3, 10);

BTLE btle(&radio);

DS18B20 ds(2);



void setup() {
#if DEBUG_CONS == 1
  Serial.begin(115200);
  Serial.print("Devices: ");
  Serial.println(ds.getNumberOfDevices());
  Serial.println();
#endif
  ///btle.begin(Ble_name);
  switchPump();

  blinkIndicatorPin(200, 1);
  ledRunForward(200);
#if MENU_ENABLED == 1
  initMenu();
#endif
  read_T1T2();
}

void loop() {

  //  delay(1000);
  //  return;
#if MENU_ENABLED == 1
  enterMenu();
#endif
  //ds.selectNext();
  memcpy_P(ds_address, &addresses[0,0], 8);
  GlobalError = 0;
  if (!ds.selectNext()) {
  //if (!ds.select(ds_address)) {
    GlobalError = 1;
    Serial.print(F("No "));
    print_OWaddress(ds_address);
    switchPump();
    delay(100);
    return;
  }
  delay(1000);
  //ds.selectNext();
  
  ds.setResolution(9);
  ds.getAddress(ds_address);
#if DEBUG_CONS == 1
  print_OWaddress(ds_address);
#endif
#if DEBUG_CONS == 1
  Serial.print(F("Temperature: "));
#endif
  int16_t t = ds.getTempC();

  if (t > -10000) {
#if DEBUG_CONS == 1
    Serial.print(t);
    //      Serial.print(" C");
#endif
    
    blinkIndicatorPin(200, 1);
    if (t < 0){
      delay(1000);
    }
    int8_t  tt;
    if (t < -128){
      tt = -128;
    } else if (t > 127){
      tt = 127;
    } else {
      tt = t;
    }
    compareT(tt);
    switchPump();
    transmit_ble(t);
    temperatures[0] = tt;
    transmit_NRF();
    //switchLeds(tt);
#if DEBUG_CONS == 1
    Serial.println();
#endif
  } else {
    blinkIndicatorPin(1500, 2);
#if DEBUG_CONS == 1
    Serial.println(F("No sensor"));
#endif
  }
  // ds.selectNext();


}
