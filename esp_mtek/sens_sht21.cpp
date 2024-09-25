
#include "sens_sht21.h"
#include "application.h"
#if I2C_SDA
sens_sht21::sens_sht21(int se_id, uint8_t addr):sensor_th_t(se_id, addr)
{
    _type=MTEMP|MHUM;
}

sens_sht21::~sens_sht21()
{
    end();
}

void sens_sht21::begin(int sda, int scl)
{
    _sht = new SHT2x();
    _sht->begin();
}

void sens_sht21::end()
{
    delete _sht;
    _sht=nullptr;
}

void sens_sht21::loop()
{
    if(_sht && _enabled)
    {
        _sht->read();
        _data.t = _sht->getTemperature();
        if(CFG(faren))
        {
            _data.t = (_data.t * (9.0f/5.0f))+32.f;
        }

        _data.h = _sht->getHumidity();
        app_t::TheApp->on_sensor_event(this);
    }
}

#endif //#if I2C_SDA
