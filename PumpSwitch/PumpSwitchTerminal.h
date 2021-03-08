/*
 * PumpSwitchTerminal.h
 */

#define PM PROGMEM

#define ToAttr(Attr) ((AttributeStructTypedef*)Attr)
#define ValueAddress(Attr) (&((&ToAttr(Attr)->len)[sizeof(ToAttr(Attr)->len)]))
#define ToStr(Attr)	((char*)ValueAddress(Attr))
#define ToInt(Attr)	((char*)ValueAddress(Attr))

class VAttribute
{
	public:
	VAttribute(const char * name):name(name){}
	const char * getValue(){
		return &(((const char *)&name)[sizeof(name)]);
	}
	void setValue(const char * value){
		strcpy( &((char *)&name)[sizeof(name)], value);
	}
	const char * name;
};

typedef struct{
	const char * name;
	uint8_t len;
	//void (*getValue)(void* Attr, void * where);
	//void (*setValue)(void* Attr, void * value);
} AttributeStructTypedef;

void InitAttributeStruct(void * Attr, const char * name)
{
	ToAttr(Attr)->name = name;
}

void getValueDefault(void* Attr, void * where)
{
	memcpy(where, ValueAddress(Attr), ToAttr(Attr)->len );
}

void setValueDefault(void* Attr, void * where)
{
	memcpy(ValueAddress(Attr), where, ToAttr(Attr)->len );
}

AttributeStructTypedef * findByName(void * array, uint8_t elementNumber, const char *name)
{
	AttributeStructTypedef *p = array;
	for(uint8_t i = 0; i< elementNumber; ++i){
		if (!strcmp(p->name, name)){
			return p;
		}
		uint8_t *pp = (uint8_t *)p;
		pp += sizeof(p->name) + p->len;
	}
	return 0;
}
