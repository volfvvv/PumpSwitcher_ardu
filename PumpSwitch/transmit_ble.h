/*
 * transmit_ble.h
 */

#define Transmit_interleave	1
extern BTLE btle;

const char Ble_name[] = "PT>";

int8_t Transmit_interleave_cnt = Transmit_interleave;

//uint8_t ble_buffer[32 * 3];
//uint8_t ble_pls;

void transmit_ble_internal(int16_t T)
{
		if (!btle.begin(Ble_name)){
			Serial.println(F("\nBLE error"));
			return;
		} 
#if DEBUG_CONS == 1
		Serial.println(F("\nBLE Ok"));
		Serial.print(F("Send on chan: "));
		Serial.println(btle.current);
#endif
		nrf_service_data buf;
		//buf.service_uuid = NRF_BATTERY_SERVICE_UUID;
		buf.service_uuid = NRF_TEMPERATURE_SERVICE_UUID;
		buf.value = T;
		uint8_t result = 3;
		for (uint8_t i = 0; i < 3; ++i){
			if (!btle.advertise(0x16, &buf, sizeof(buf))){
				--result;
			}
			btle.hopChannel();
		}
		////result = result && btle.advertise(0x16, &buf, sizeof(buf));
		////result = result && btle.advertise(0x16, &buf, sizeof(buf));
		if(!result) {
#if DEBUG_CONS == 1
			Serial.print(F("BTLE advertisement failure "));
			Serial.print(result);
			Serial.println(F(" times"));
#endif
		}
	
}

void transmit_ble(int16_t T)
{
	--Transmit_interleave_cnt;
	if (Transmit_interleave_cnt == 0){
		Transmit_interleave_cnt = Transmit_interleave;
		transmit_ble_internal(T);
	}
	//else	if (Transmit_interleave_cnt < 0){
		//Serial.println("Send");
		//Transmit_interleave_cnt = Transmit_interleave;
		////for (uint8_t i = 0; i < 3; ++i){
			////memcpy(&btle.buffer, &ble_buffer[i*32], 32);
			////btle.advertise(ble_pls);
			////btle.hopChannel();
		////}
		
	//}
}
