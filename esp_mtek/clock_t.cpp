
#include "clock_t.h"
#include "application.h"

simple_clock_t  Sclk;

#define NTP_INTERVAL_MS     7000000
void simple_clock_t::fire_day_event()
{
    app_t::TheApp->on_day_changed(true);
}

void simple_clock_t::reroute_event() // from main loop not from INTR
{
    _day = _ntp->getDay();

    int month = _ntp->getMonth();
    if(_month!=month)
    {
        app_t::TheApp->on_month_changed();
        _month = month;
    }

    int year = _ntp->getYear();
    if(year != _year)
    {
        app_t::TheApp->on_year_changed();
        _year = year;
    }
}

bool simple_clock_t::update_ntp(bool force)
{
#if WITH_NTP
    if(false == force)
    {
        if(CFG(hrg)==101)
        {
            LOG("NTP=TEST");
            return true;
        }
    }
    if(_ntp == nullptr){
        LOG("NTP=0");
        return false;
    }
    if(diff_time(millis(),_ntplast)>NTP_INTERVAL_MS || force)
    {
        bool pntp = espxxsrv_t::ping(CFG(ntp_srv));
        if(pntp)
        {
            LOG("NTP=%d",_ntp->getSeconds());
            Ramm.ntpfail = true;
            _ntp->update();
            Ramm.ntpfail = false;
            size_t s = _ntp->getHours()*60*60;
            s += _ntp->getMinutes()*60;
            s += _ntp->getSeconds();
            set_seconds(s);

            _day = _ntp->getDay();
            _month = _ntp->getMonth();
            _year = _ntp->getYear();
            _epochlocal = _ntp->getEpochTime();
            if(_resettitme==0){
                _resettitme = _epochlocal;
            }
            _ntplast = millis();
            return true;
        }
        else{
            LOG("NTP-PING=0");
        }
    }
#endif
    return false;
}
