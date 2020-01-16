#include "STC3100.h"

/**
 * @brief Construct a new STC3100::STC3100 object to reference IC
 * 
 * @param resistor_value The value of the current sensing resistor in milliOhms
 */
STC3100::STC3100(uint resistor_value){
    current_resistor_value = resistor_value;
}

void STC3100::init(){
    Wire.begin();
}

/**
 * @brief start() will check if it can read and confirm the serial number. 
 * If the serial number can be read and confirmed, this function will set the
 * chip to start taking readings.
 * 
 * @return true When serial is read and confirmed and chip is started
 * @return false Serial number could not be correctly read
 */

bool STC3100::start(){
    if(!get_serial(serial_number)){
        return false;
    }
    _running = true;
    Serial.println(get_value(REG_CTRL), BIN);
    write_byte(REG_CTRL, 0x02);
    // Start gauge
    write_byte(REG_MODE, 0x10);
    return true;
}

STC3100::Reading STC3100::read(){
    Reading r;
    if(!_running){
        return r;
    }
    r.valid = true;
    r.voltage = get_value(REG_VOLTAGE_LOW);
    r.temp = get_value(REG_TEMP_LOW);
    r.current = get_value(REG_CURRENT_LOW);
    return r;
}

/**
 * @brief Gets the current battery voltage
 * 
 * @return float Battery voltage in milliVolts
 */
uint STC3100::voltage(){
    uint v = ((uint) get_value(REG_VOLTAGE_LOW) * 2.44);
    return v;
}

/**
 * @brief Gets the current temperature as a float in celsius
 * 
 * @return float Temperature in celsius
 */
uint STC3100::temp(){
    uint t = ((uint)get_value(REG_TEMP_LOW));
    return t;
}

long STC3100::current(){
    //uint16_t v = get_value(REG_CURRENT_LOW);
    short current =  get_value(REG_CURRENT_LOW);
    unsigned short cov = 48210 / 33;
    current &= 0x3fff;
    if (current>=0x2000) current -= 0x4000;
    long v = ((long)current * cov)>>12;
    return v;
}

bool STC3100::running(){
    return _running;
}

/**
 * @brief Reads serial number from battery IC
 * 
 * @param serial Pointer to 8 byte array to store serial number in
 * @return true Serial number is valid
 * @return false Serial number failed CRC
 */

bool STC3100::get_serial(uint8_t *serial){
    Wire.beginTransmission(BUS_ADDRESS);
    Wire.write(REG_ID0);
    Wire.endTransmission();
    Wire.requestFrom(BUS_ADDRESS, 8);
    for(uint i =0; i<8; i++){
        serial[i] = Wire.read();
    }
    return crc(serial, 7)==serial[7];
}

/**
 * @brief CRC preforms a checksum on data
 * 
 * @param data Pointer to data array we want to checksum
 * @param size Size of data we want to checksum
 * @return uint8_t CRC8 value generated from data
 */

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

/**
 * @brief Get value from the STC3100 chip by taking the high
 * and low byte of a sensor reading and returning a single 16 bit value
 * 
 * @param reg Register to read two bytes from
 * @return uint16_t Value read
 */

uint16_t STC3100::get_value(uint8_t reg){
    Wire.beginTransmission(BUS_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(BUS_ADDRESS, 2);
    uint8_t low =  Wire.read();         // print the character
    uint8_t high = Wire.read();
    uint16_t high_byte  = high;
    uint16_t value = low;
    value |= high<<8;
    return value;
}

/**
 * @brief Writes byte to STC3100 IC register
 * 
 * @param location Location in i2c IC to write value
 * @param value Value to write to IC
 */
void STC3100::write_byte(uint8_t location, uint8_t value){
    Wire.beginTransmission(BUS_ADDRESS);
    Wire.write(REG_MODE);
    Wire.write(value);
    Wire.endTransmission();
}