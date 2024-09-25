
#include "sensors.h"
#include "wirewire.h"
#include "sens_sht21.h"
#include "sens_sht3x.h"
#include "sens_aht20.h"
#include "hot_ram.h"
#if I2C_SDA

void sensors_t::begin(int a, int c)//sda,scl
{
    uint8_t   ads[MAX_SENS]; // = {0,0};

    if(Ramm.sensfail){LOG("SENS=0");return;}
    Ramm.sensfail=true;
    hot_store();
    ::i2c_begin(a, c);
    memset(_sensors,0,sizeof(_sensors));

    memset(ads,0,sizeof(ads));
    ::i2c_scan(nullptr, ads);
    _count=0;
    int j=0;
    for(int i=0; i<MAX_SENS;i++)
    {
        j=i; //i;
        if(ads[i]==AHT10_ADDRESS_0X38)
            _sensors[j] = new sens_aht10(i, ads[i]);
        else if(ads[i]==0x40)
            _sensors[j] = new sens_sht21(i, ads[i]);
        else if(ads[i]==0x44)
            _sensors[j] = new sens_sht3x(i, ads[i]);

        if(_sensors[j])
        {
            LOG("%d S=[%s]",_count,_sensors[j]->name());
            _count++;
        }

    }
    for(int i=0; i<MAX_SENS;i++)
    {
        if(_sensors[i]){
            _sensors[i]->begin(_i2c_sda,_i2c_scl);
        }
    }
    Ramm.sensfail=false;
    hot_store();
}

int sensors_t::loop()
{
    int looped = 0;
    if(Ramm.sensfail){
        LOG("SENS=0");
        return 0;
    }
    Ramm.sensfail=true;
    hot_store();

    for(int i=0; i<MAX_SENS; i++)
    {
        if(_sensors[i])
        {
            //LOG("loops1 %d %s",i,_sensors[i]->name());
            _sensors[i]->loop();
            delay(2);
        }
        ++looped;
    }
    Ramm.sensfail=false;
    hot_store();
    return looped;
}

void sensors_t::end()
{
    for(int i=0; MAX_SENS;i++)
    {
        delete _sensors[i];
    }
}

void sensors_t::detect(String& p)
{
    ::i2c_scan(&p,nullptr);
}


#endif //#if I2C_SDA
