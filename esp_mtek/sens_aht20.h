#ifndef SENS_AHT20_H
#define SENS_AHT20_H

#include "AHT10.h"
#include "sensors.h"
#if I2C_SDA

class sens_aht10 : public sensor_th_t
{
public:
    sens_aht10(int se_id, uint8_t);
    virtual ~sens_aht10();

    void begin(int sda, int scl);
    void end();
    void loop();
    const th_t&   get_th()const{return _data;}
    const char* name()const{return "AHT10";}
public:
    AHT10*  _aht = nullptr;

};
#endif // #if I2C_SDA
#endif // SENS_AHT20_H
