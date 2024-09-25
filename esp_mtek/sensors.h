#ifndef SENSORS_H
#define SENSORS_H


#include "_my_config.h"
#include "wirewire.h"
#if I2C_SDA

/////////////////////////////////////////////////////////////////////////////////////////
#define MTEMP      0x1
#define MHUM       0x2
#define MPRES      0x4
#define MLUM       0x8
#define MIR        0x10

/////////////////////////////////////////////////////////////////////////////////////////
struct th_t{
    float t;
    float h;
    bool operator!=(const th_t& r){
        return (::fabs(t-r.t)>0.2 && ::fabs(h-r.h)>0.2);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////
class sensor_th_t
{
public:

    sensor_th_t(int se_id, uint8_t addr):_who(se_id),_addr(addr){
        memset(&_data,0,sizeof(_data));
    }
    virtual ~sensor_th_t(){
    }
    virtual void begin(int sda, int scl)=0;
    virtual void end()=0;
    virtual void loop()=0;
    virtual const th_t& get_th()const=0;
    virtual const char* name()const=0;
    uint8_t id()const{return _who;}
    String t_h()const{
        String r=F(" (");
        r+=String(int(_data.t));
        r+=F("/");
        r+=String(int(_data.h));
        r+=F(")");
        return r;
    }
public:
    uint32_t type()const {return _type;}

protected:
    uint8_t   _addr    = 0;
    uint8_t   _who     = 0;
    bool      _enabled = true;
    uint32_t  _type;
public:
    th_t      _data;
};

//////////////////////////////////////////////////////////////////////////////////////////
class sensors_t
{
public:
    sensors_t(){};
    ~sensors_t(){};
    void begin(int a, int c);
    void end();
    int loop();
    const sensor_th_t* sensor(int i)const{ return i < MAX_SENS ? _sensors[i] : nullptr;}
    sensor_th_t* sensor(const char* n){
        for(int i=0;i<MAX_SENS;i++)
        {
            if(_sensors[i])
            {
                if(!strcmp(_sensors[i]->name(),n)){
                    return _sensors[i];
                }
            }
        }
        return nullptr;
    }
    static void detect(String& p);
    int count()const{return _count;}
protected:
    sensor_th_t* _sensors[MAX_SENS] = {0,0};
    int          _count = 0;
};

#endif // #if I2C_SDA
#endif // SENSORS_H
