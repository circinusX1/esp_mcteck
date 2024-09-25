#ifndef CANVAS_H
#define CANVAS_H
#include "_my_config.h"
#include "eeprom.h"

#if WITH_GRAPH
#include "_esp_mpus.h"
#include "sensors.h"



class sensor_th_t;
class canvas
{
public:
    canvas(int w=700, int h=400);
    ~canvas(){};
    void  set_trigger(const th_t& trigger)
    {
        _trigger = trigger;
    }
    void  add_th(int now, const sensor_th_t* ps, uint8_t relay_state)
    {
        _minutes = now/TIME_STEP;
        if(_minutes<SAMPLES_PER_DAY)
        {
            const th_t& th = ps->get_th();
            int idx = ps->id();
            _graph[idx][_minutes].t = th.t;
            _graph[idx][_minutes].h = th.h;
            if(_relay[_minutes]==0){    //hold on on on relay in this 6 min interval
                _relay[_minutes] = relay_state;
            }
            _relay[_minutes+1] = 0;
        }
    }
    void    draw(String& out);
    void    save();
    void    load();
    void    reset(){
        ::memset(&_graph[CFG(sid)],0,sizeof(_graph[CFG(sid)]));
        ::memset(&_relay,0,sizeof(_relay));
    }
    void convert(bool from, bool to);
    void canvasit();
    String samples(int);
    bool get_relay(int mymin)const{return mymin < SAMPLES_PER_DAY && mymin>=0 ? _relay[mymin] : false;};
private:

    inline int bindex(int b) { return b / 8; }
    inline int boffset(int b) { return b % 8; }

private:
    int     _w,_h;
    int     _minutes;
    int     _has;
    th_t    _trigger = {0,0};
    th_t    _graph[MAX_SENS][SAMPLES_PER_DAY+2];
    uint8_t _relay[SAMPLES_PER_DAY+2];

};

#else

#endif

#endif // CANVAS_H
