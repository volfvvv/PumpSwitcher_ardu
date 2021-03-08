/*
 * PumpSwitch.h
 */



#define PM PROGMEM


#define RelePin	 5
#define IndicatorPin	 4
#define CommonLed 7

#define SwitchCounterLimit1  4
#define SwitchCounterLimit2  7

extern DS18B20 ds;

#define RADIO_DEBUG 1

//#define MINIMUM_T_LED	20
//#define DISCRET_T_LED	1
//int16_t T1 = 26;
//int16_t T2 = 26;

typedef enum{
	SWITCHED_OFF,
	SWITCHED_ON
} PumpStateTypedef;

typedef enum{
	LEDS_COLOR_YELLOW,
	LEDS_COLOR_GREEN,
} LedsGreenTypedef;

typedef enum {
	Programm1 = 0,
	Programm2 = 1,
	Programm3 = 2,
	Programm_Count = Programm3 + 1,
} ProgrammNumberTypedef;

// ProgrammNumberTypedef CurrentProgramm = Programm1;

typedef struct{
	int8_t T1;
	int8_t T2;
} LimitsTypedef;

uint8_t GlobalError = 0;

LimitsTypedef Limits[Programm_Count] = {{60,60},{63,59},{66,59}};
//ProgrammNumberTypedef CurrentProgramm = 0;

const uint8_t LedsArray[] = {14, 15, 16, 17, 18, 19, 8, 9};

uint8_t CurrentProgramm = Programm1;

#define MINIMUM_T_LED	40
#define DISCRET_T_LED	5
#define SIZE_T_LED_ARRAY	sizeof(LedsArray)

#define T1 (Limits[CurrentProgramm].T1)
#define T2 (Limits[CurrentProgramm].T2)

//int16_t T1 = 60;
//int16_t T2 = 60;

uint8_t CurrentState = SWITCHED_OFF;
uint8_t SwitchCounter = 0;

uint8_t LedsColor = LEDS_COLOR_YELLOW;// LEDS_COLOR_GREEN;
uint8_t CommonLedLevel;
uint8_t LedLevel;
int8_t CurrentSwitchedLed = -1;
uint8_t BlinkLedArray = 0;

// запись параметров в EEPROM
void write_T1T2()
{
#if DEBUG_CONS == 1
	Serial.println(F("Writing EEPROM"));
#endif
	uint16_t eeprom_address;
	uint8_t * ram_address;
	//for(;;){
		eeprom_address = 0;
		ram_address = (uint8_t *)&Limits;
		for(uint8_t i = 0; i <= (Programm3*2); ++i){
//			Serial.println("Temperatures writing ");
			eeprom_write_byte(eeprom_address++, *ram_address++);
			//eeprom_address += 1;
			
		}
		//bool found = false;
		//eeprom_address = 0;
		//ram_address = (uint16_t *)&Limits;
		//for(uint8_t i = 0; i <= Programm3; ++i){
			//uint16_t v = eeprom_read_word(eeprom_address++);
			//if (v != *ram_address++){
				//found = true;
				//break;
			//}
		//}
		//if (!found)break;
	//}
}

// чтение параметров из EEPROM
void read_T1T2()
{
	//write_T1T2();
	uint16_t eeprom_address = 0;
	uint8_t * ram_address = (uint8_t *)&Limits;
	for(uint8_t i = 0; i <= (Programm3*2); ++i){
		*ram_address++ = eeprom_read_byte(eeprom_address++);
	}
	{
		Serial.print(F("Temperatures: "));
		uint8_t * ram_address = (uint8_t *)&Limits;
		for(uint8_t i = 0; i < 6; ++i){
				Serial.print(*ram_address++);
				Serial.print(F(", "));
		}
		Serial.println();
	}
}

// мигание светодиодом POWER
void blinkIndicatorPin(uint16_t period, uint8_t cnt)
{
	pinMode(IndicatorPin, OUTPUT);
	for (uint8_t i = 0; i < cnt; ++i){
		digitalWrite(IndicatorPin, LOW);    // turn the LED off by making the voltage LOW
		delay(period);                       // wait for a second
		digitalWrite(IndicatorPin, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(period);                       // wait for a second
	}
}

// переключение питания насоса
void switchPump()
{
	pinMode(RelePin, OUTPUT);
	uint8_t state = LOW;
	if (GlobalError != 0){
		CurrentState = SWITCHED_OFF;
	}
	if (SWITCHED_OFF == CurrentState){
		state = HIGH;
	}
//#ifdef RADIO_DEBUG
	//if ()
	//Serial.println
//#else
	digitalWrite(RelePin, state);
//#endif
}

// сравнение текущей температуры с пределами установленными в Limits
// и преключение текущей программы, если нужно
void compareT(int8_t T)
{
	uint8_t oldCurrentState = CurrentState;
	if ( T > T1 ) {
		CurrentState = SWITCHED_ON;
		if (oldCurrentState != CurrentState ) {
#if RADIO_DEBUG == 1 && DEBUG_CONS == 1
			if (CurrentState == SWITCHED_ON){
				Serial.println(F("SWITCHED_ON"));
			} else{
				Serial.println(F("SWITCHED_OFF"));
			}
#endif
			if (CurrentProgramm < Programm3){
				++SwitchCounter;
				if (SwitchCounterLimit1 == SwitchCounter){
					CurrentProgramm = Programm2;
				} else if (SwitchCounterLimit2 == SwitchCounter){
					CurrentProgramm = Programm3;
				}
			}
		}
	} else if (T < T2) {
		CurrentState = SWITCHED_OFF;
	}
}


// инициализация питания множественных светодиодов
void initLeds2Run()
{
	CommonLedLevel = LOW;
	LedLevel = HIGH;
	if (LEDS_COLOR_GREEN == LedsColor){
		CommonLedLevel = HIGH;
		LedLevel = LOW;
	}
	pinMode(CommonLed, OUTPUT);
	digitalWrite(CommonLed, CommonLedLevel);
}

// бегушие огни от центра
void ledRunFromCenter(uint8_t p)
{
	#define SIZE_T_LED_ARRAY_2 (SIZE_T_LED_ARRAY/2)
	initLeds2Run();
	//return;
	for(int8_t i = 0; i < SIZE_T_LED_ARRAY_2+1; ++i){
		if (i < SIZE_T_LED_ARRAY_2){
			digitalWrite(LedsArray[SIZE_T_LED_ARRAY_2-1-i], LedLevel);
			pinMode(LedsArray[SIZE_T_LED_ARRAY_2-1-i], OUTPUT);
			
			digitalWrite(LedsArray[SIZE_T_LED_ARRAY_2+i], LedLevel);
			pinMode(LedsArray[SIZE_T_LED_ARRAY_2+i], OUTPUT);
		}
		if (i != 0){
			uint8_t j = i - 1;
			pinMode(LedsArray[SIZE_T_LED_ARRAY_2-1-j], INPUT);
			pinMode(LedsArray[SIZE_T_LED_ARRAY_2+j], INPUT);
		}
		delay(p);
	}
}

// бегушие огни вперед
void ledRunForward(uint8_t p)
{
	initLeds2Run();
	//return;
	for(int8_t i = 0; i < SIZE_T_LED_ARRAY+1; ++i){
		if (i < SIZE_T_LED_ARRAY){
			uint8_t led = LedsArray[i];
			digitalWrite(led, LedLevel);
			pinMode(led, OUTPUT);
		}
		if (i != 0){
			pinMode(LedsArray[i-1], INPUT);
		}
		delay(p);
	}
	//delay(p);
	//pinMode(LedsArray[SIZE_T_LED_ARRAY-1], INPUT);
}

//// бегушие огни вперед
//void ledRunForward(uint8_t p)
//{
	//initLeds2Run();
	////return;
	//for(int8_t i = 0; i < SIZE_T_LED_ARRAY; ++i){
		//digitalWrite(LedsArray[i], LedLevel);
		//pinMode(LedsArray[i], OUTPUT);
		//int8_t j = i-1;
		//if (i == 0){
			//j = sizeof(LedsArray)-1;
		//}
		//pinMode(LedsArray[j], INPUT);
		//delay(p);
	//}
	//delay(p);
	//pinMode(LedsArray[SIZE_T_LED_ARRAY-1], INPUT);
//}


// бегушие огни назад
void ledRunBackward(uint8_t p)
{
	initLeds2Run();
	//return;
	for(int8_t i = SIZE_T_LED_ARRAY-1; i >= -1 ; --i){
		if (i >= 0){
			digitalWrite(LedsArray[i], LedLevel);
			pinMode(LedsArray[i], OUTPUT);
		}
		if (i < (SIZE_T_LED_ARRAY-1)){
			pinMode(LedsArray[i+1], INPUT);
			
		}
		delay(p);
	}
}

//// бегушие огни назад
//void ledRunBackward(uint8_t p)
//{
	//initLeds2Run();
	////return;
	//for(int8_t i = SIZE_T_LED_ARRAY; i >= 0 ; --i){
		//digitalWrite(LedsArray[i], LedLevel);
		//pinMode(LedsArray[i], OUTPUT);
		//int8_t j = i+1;
		//if (i == SIZE_T_LED_ARRAY){
			//j = 0;
		//}
		//pinMode(LedsArray[j], INPUT);
		//delay(p);
	//}
	//delay(p);
	//pinMode(LedsArray[0], INPUT);
//}

// переключение единственного светодиода из массива согласно index
void switchLed(int8_t index, uint8_t on_Off)
{
	initLeds2Run();
	if (index>=0 && index<SIZE_T_LED_ARRAY){
		uint8_t led = LedsArray[index];
		if (on_Off){
			digitalWrite(led, LedLevel);
			pinMode(led, OUTPUT);
		} else{
			pinMode(led, INPUT);
		}
	}
}

// частое мигание соответствующего CurrentSwitchedLed светодиода
void InterruptBlickCurrentLed()
{
	int8_t led = CurrentSwitchedLed;
	if (led < -1){
		MsTimer2::stop();
	}
	if (led == -1) led = 0;
	led = LedsArray[led];
	pinMode(led, OUTPUT);
	uint8_t state = digitalRead(led);
	digitalWrite(led, state == HIGH ? LOW : HIGH);
}

// переключение режима работы светодиода
void switchLeds(int8_t T)
{
	
	MsTimer2::stop();
	if (CurrentSwitchedLed>=0){
		switchLed(CurrentSwitchedLed, 0);
		CurrentSwitchedLed = -2;
	}
	if (CurrentSwitchedLed == -1){
		switchLed(0, 0);
		CurrentSwitchedLed = -2;
	}
	if (T < 0) return;
	int8_t t = T;
	t -= MINIMUM_T_LED;
	if (t < 0){
		CurrentSwitchedLed = -1;
		LedsColor = LEDS_COLOR_YELLOW;
		switchLed(0, 1);
		return;
	}
	
	t = T / DISCRET_T_LED;
	uint8_t oldLedsColor = LedsColor;
	if (t > 7) {
		LedsColor = LEDS_COLOR_YELLOW;
		t = 7;
	} else{
		LedsColor = LEDS_COLOR_GREEN;
		if ((T % (DISCRET_T_LED / 2 )) > 2){
			LedsColor = LEDS_COLOR_YELLOW;
		}
	}
	CurrentSwitchedLed = t;
	if (SWITCHED_ON == CurrentState){
		//LedsColor = LEDS_COLOR_YELLOW;
		initLeds2Run();
		MsTimer2::set(50, InterruptBlickCurrentLed);
		MsTimer2::start();
		return;
	}
	switchLed(CurrentSwitchedLed, 1);
}

  //while (ds.selectNext()) {
    //switch (ds.getFamilyCode()) {
      //case MODEL_DS18S20:
        //Serial.println("Model: DS18S20/DS1820");
        //break;
      //case MODEL_DS1822:
        //Serial.println("Model: DS1822");
        //break;
      //case MODEL_DS18B20:
        //Serial.println("Model: DS18B20");
        //break;
      //default:
        //Serial.println("Unrecognized Device");
        //break;
    //}
