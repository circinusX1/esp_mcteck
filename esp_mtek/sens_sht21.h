#ifndef SENS_SHT21_H
#define SENS_SHT21_H

#include "SHT2x.h"
#include "sensors.h"
#if I2C_SDA

class sens_sht21 :public sensor_th_t
{
public:
    sens_sht21(int se_id, uint8_t addr);
    virtual ~sens_sht21();

    void begin(int sda, int scl);
    void end();
    void loop();
    const th_t&   get_th()const{return _data;}
    const char* name()const{return "SHT21";}
    SHT2x*  get(){return _sht ;}
protected:
    SHT2x*   _sht = nullptr; // = new AHT10(addr);
};
#endif //#if I2C_SDA
#endif // SENS_SHT21_H
