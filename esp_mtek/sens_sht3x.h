#ifndef SENS_SHT31_H
#define SENS_SHT31_H

#include "DFRobot_SHT3x.h"
#include "sensors.h"
#if I2C_SDA

class sens_sht3x :public sensor_th_t
{
public:
    sens_sht3x(int se_id, uint8_t addr);
    virtual ~sens_sht3x();

    void begin(int sda, int scl);
    void end();
    void loop();
    const th_t&   get_th()const{return _data;}
    const char* name()const{return "SHT3X";}

protected:
    DFRobot_SHT3x*   _sht = nullptr; // = new AHT10(addr);
};
#endif //#if I2C_SDA
#endif // SENS_SHT21_H
