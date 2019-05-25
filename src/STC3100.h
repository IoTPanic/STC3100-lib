#ifndef STC_H
#define STC_H
#include <Arduino.h>
#include <Wire.h>
/** 
 * The STC3100 is a battery management IC that monitors both voltage and current
 * This library was written by Samuel Archibald @IoTPanic 
 */

#define REG_MODE 0
#define REG_CTRL 1
#define REG_CHARGE_LOW 2
#define REG_CHARGE_HIGH 3
#define REG_COUNTER_LOW 4
#define REG_COUNTER_HIGH 5
#define REG_CURRENT_LOW 6
#define REG_CURRENT_HIGH 7
#define REG_VOLTAGE_LOW 8
#define REG_VOLTAGE_HIGH 9
#define REG_TEMP_LOW 10
#define REG_TEMP_HIGH 11

#define REG_ID0 24
#define REG_ID1 25
#define REG_ID2 26
#define REG_ID3 27
#define REG_ID4 28
#define REG_ID5 29
#define REG_ID6 30
#define REG_ID7 31

#define REG_RAM0 32 // RAM registers go up to 31 address 63 for 32 ram bytes

#define BUS_ADDRESS 0x70

class STC3100
{
    public: 

    typedef struct{
        float voltage;
        uint16_t current;
        uint16_t temp;
    } Reading;

    STC3100();
    void init();
    void start();
    Reading read();

    protected:
    float get_voltage();

    private:
    int get_value(uint8_t reg);
};

#endif