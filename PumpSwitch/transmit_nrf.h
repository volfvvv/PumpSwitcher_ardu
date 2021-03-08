/*
 * transmit_nrf.h
 */

#include "transmit_types.h"

#define Transmit_interleave_NRF	1

#define ADDRESS_WIDTH 3
extern RF24 radio;
extern void printHex(uint8_t b);

//const uint8_t Server_address[] = {1, 0, 0, 0, 0};
//const uint8_t Server_address[] = "SPT";
const uint8_t Server_address[] = SERVER_ADDRESS;

//const uint8_t Server_address[ADDRESS_WIDTH] = {1, 0, 0};
const uint8_t Self_address = 2;

int8_t Transmit_interleave_NRF_cnt = Transmit_interleave_NRF;


Header4TransmitTypedefV1 transmitHeader = {
	.dst_addr = 1,
	.src_addr = Self_address,
	.version = 1,
	
};


//uint8_t ble_buffer[32 * 3];
//uint8_t ble_pls;


bool radio_NRF_begin()
{
	//if (!radio.begin()){
		//return  false;
	//}
	radio.setAddressWidth(ADDRESS_WIDTH);
	radio.setAutoAck(false);
	radio.enableDynamicPayloads();
	radio.setDataRate(RF24_1MBPS);
	radio.setCRCLength(RF24_CRC_16);
	radio.setChannel( 103 );
    //radio.setRetries(4, 5);   
	radio.setPALevel(RF24_PA_MAX);

	// set advertisement address: 0x8E89BED6 (bit-reversed -> 0x6B7D9171)
	//radio.setAddressWidth(ADDRESS_WIDTH);
	radio.openReadingPipe(0, (uint8_t *)Server_address);
	//radio.openReadingPipe(1, (uint8_t *)Self_address);
	radio.openWritingPipe(  (uint8_t *)Server_address);
	return true;
}

uint8_t * setValue2pbuf(uint8_t * pbuf, void * value, uint8_t len)
{
	memcpy(pbuf,value, len);
	return (pbuf + len);
}

void transmit_NRF()
{
	--Transmit_interleave_NRF_cnt;
	if (Transmit_interleave_NRF_cnt == 0){
		Transmit_interleave_NRF_cnt = Transmit_interleave_NRF;
		if (!radio_NRF_begin()){
			return;
		}
#if DEBUG_CONS == 1
		Serial.println("\nNRF Ok!");
#endif
		uint8_t buf[32];
		uint8_t *pbuf = buf;
		uint8_t len;
		transmitHeader.mills = millis();
		transmitHeader.command = TemperatureInfo;
		pbuf = setValue2pbuf(pbuf, &transmitHeader, sizeof(transmitHeader));
		int8_t i_sensor = N_SENSORS;
		pbuf = setValue2pbuf(pbuf, &i_sensor, sizeof(i_sensor));
		for (i_sensor = 0; i_sensor < N_SENSORS; ++i_sensor){
			pbuf = setValue2pbuf(pbuf, &temperatures[i_sensor], sizeof(temperatures[0]));
		}
		pbuf = setValue2pbuf(pbuf, &CurrentProgramm, sizeof(CurrentProgramm));
		pbuf = setValue2pbuf(pbuf, &CurrentState, sizeof(CurrentState));
		
		pbuf = setValue2pbuf(pbuf, &Limits, sizeof(Limits));
		pbuf = setValue2pbuf(pbuf, &ds_address, sizeof(ds_address));
		//pbuf = setValue2pbuf(pbuf, Limits, sizeof(Limits));
		len = pbuf - buf;
		//uint8_t crc = OneWire::crc8(buf, len);
		//pbuf = setValue2pbuf(pbuf, &crc, sizeof(crc));
		//len = pbuf - buf;
		printBuffer(buf, len);
		Serial.println();
		bool result = radio.write(buf, len);
#if DEBUG_CONS == 1
		if(!result) {
			Serial.println("NRF sending error");
		} else{
			Serial.print("NRF sended ");
			Serial.print(len);
			Serial.println(" bytes");
		}
#endif
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
