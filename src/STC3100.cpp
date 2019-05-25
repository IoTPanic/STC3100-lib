#include "STC3100.h"

STC3100::STC3100(){

}

void STC3100::init(){
    Wire.begin();
}

void STC3100::start(){
    Wire.beginTransmission(0x70);
    Wire.write(REG_MODE);
    Wire.write(16);
    Wire.endTransmission();
}

STC3100::Reading STC3100::read(){
    Reading r;
    r.voltage = get_voltage();
    return r;
}

float STC3100::get_voltage(){
    float v = ((float) get_value(REG_VOLTAGE_LOW) * 2.44)/1000;
    return v;
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