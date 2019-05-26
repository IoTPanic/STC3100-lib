#include "STC3100.h"

STC3100::STC3100(){

}

void STC3100::init(){
    Wire.begin();
}

bool STC3100::start(){
    if(!get_serial(serial_number)){
        return false;
    }
    Wire.beginTransmission(0x70);
    Wire.write(REG_MODE);
    Wire.write(0x8);
    Wire.endTransmission();
    running = true;
    return true;
}

STC3100::Reading STC3100::read(){
    Reading r;
    if(!running){
        return r;
    }
    r.valid = true;
    r.voltage = get_voltage();
    r.temp = get_temp();
    r.current = get_current();
    return r;
}

float STC3100::get_voltage(){
    float v = ((float) get_value(REG_VOLTAGE_LOW) * 2.44)/1000;
    return v;
}

float STC3100::get_temp(){
    float t = ((float)get_value(REG_TEMP_LOW))/10;
    return t;
}

float STC3100::get_current(){
    float c = ((float)get_value(REG_CURRENT_LOW) * 11.77);
    Serial.println(get_value(REG_CURRENT_LOW));
    return c;
}

bool STC3100::get_serial(uint8_t *serial){
    Wire.beginTransmission(BUS_ADDRESS);
    Wire.write(REG_ID0);
    Wire.endTransmission();
    Wire.requestFrom(BUS_ADDRESS, 8);
    for(uint i =0; i<8; i++){
        serial[i] = Wire.read();
    }
    Serial.println(crc(serial, 7));
    return crc(serial, 7)==serial[7];
}

uint8_t STC3100::crc(const void * data, size_t size){
    uint8_t val = 0;
    uint8_t * pos = (uint8_t *) data;
    uint8_t * end = pos + size;
    while (pos < end) {
        val = CRC_TABLE[val ^ *pos];
        pos++;
    }
    return val;
}

int STC3100::get_value(uint8_t reg){
    Wire.beginTransmission(BUS_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(BUS_ADDRESS, 2);
    uint8_t low =  Wire.read();         // print the character
    uint8_t high = Wire.read();
    unsigned int high_byte  = (unsigned int)high;
    int value = 0;
    high_byte <<=8;
    value = (high_byte&0xFF00) | low;
    return value;
}