// transmit_types.h

typedef struct{
	uint8_t ds_address[8];
} DS18B20AddressTypedef;
//#pragma pack
typedef enum
{
	TemperatureInfo = 1,
} RadioCommandTypedef; // __attribute__((packed));

typedef struct{
	uint8_t dst_addr;
	uint8_t src_addr;
	uint8_t version;
	uint32_t mills;
	uint8_t command;
} Header4TransmitTypedefV1;

typedef struct {
	DS18B20AddressTypedef address;
	int16_t T;
	uint8_t CurrentState;
	
} TempBufferTypedefV1;

typedef struct {
	Header4TransmitTypedefV1 header;
	union{
		TempBufferTypedefV1 tempStruct;
	};
		
} Buffer4TransmitTypedef;

#if DEBUG_CONS == 1
void printHex(uint8_t b)
{
  Serial.print(" 0x");
  if ((b & 0xf0) == 0) {
    Serial.print("0");
  }
  Serial.print(b, HEX);
}
void printBuffer(uint8_t * buffer, uint8_t len)
{
	printHex(buffer[0]);
	for(uint8_t i = 1; i < len; ++i){
		Serial.print(',');
		printHex(buffer[i]);
	}
}

void print_OWaddress(uint8_t * buffer)
{
	Serial.print("address:{");
	printBuffer(buffer, 8);
	Serial.println("}");
}
#else
#define printHex(b)
#define printBuffer(b,l)
#endif

#define SERVER_ADDRESS  {'1', 'S','P'}

