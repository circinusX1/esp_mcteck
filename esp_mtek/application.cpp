
#include "application.h"
#include "hot_ram.h"
#include "sens_aht20.h"  //debug granph only
#include "css.h"

app_t* app_t::TheApp;
#define CHECK_NTP           100000
#define ACTIVE_LOOP_DELAY   10000
#define EPROM_SIG_APP       MAGIC

//////////////////////////////////////////////////////////////////////////////////////////
bool	GlobalError = false;
bool    GTest = false;
bool    Running = false;

//////////////////////////////////////////////////////////////////////////////////////////
app_t::app_t()
{
    eeprom_t eprom(-1);                                     // load
    hot_restore();
    Serial.begin(SERIAL_BPS);
    if(Ramm.loops && Ramm.loops<OKAY_MAX_LOOPS)             // prev run
    {
        LOG("! RUN ERR");
        GlobalError=true;
    }
    LOG("LOOP=%d",Ramm.loops);
}

//////////////////////////////////////////////////////////////////////////////////////////
app_t::~app_t(){
    _wifi->end();
    delete _wifi;
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::begin()
{
    app_t::TheApp = this;

#if I2C_SDA
    _sensors.begin(I2C_SDA, I2C_SCL);
#endif

    if(CFG(hrg)==101){                                  // time test 10x faster
        LOG("T=1");
        _timer.attach_ms(1, app_t::tick_10);
    }
    else{
        LOG("T=100");
        _timer.attach_ms(100, app_t::tick_10);
    }

    _wifi = new espxxsrv_t();
    _wifi->begin();
    LED ? pinMode(LED, OUTPUT) : __nill();
    RELAY ? pinMode(RELAY, OUTPUT) : __nill();
}

/////////////////////////////////////////////////////////////////////////////////////////
void app_t::loop()
{
    _wifi->loop();
    if(GlobalError){ digitalWrite(LED,LED_ON); return;}                      // no init

    if(!_init)
    {
        _init = true;
        on_init();
    }

    if(_wifi && _wifi->is_otaing())
    {
        return;
    }

    if(_tenth)
    {
        _tenth=false;
        if(Sclk.decisec() % _wifi->mode() == 0)
        {
            _led_ctrl(_led_state=!_led_state);
        }
        if(_led_state == LED_ON){
            _led_ctrl(LED_ON);
            _led_state=LED_ON;
        }
    }

    if(Sclk.diff_time(millis(), _last_active_loop) > ACTIVE_LOOP_DELAY)
    {
        _last_active_loop = millis();
        if(_wifi)
        {
            if(_wifi->mode()==1)
            {
                LOG("AP: 10.5.5.1");
                LOG(CFG(mcteck));
            }
        }
#if I2C_SDA
        int looped = _sensors.loop();
#if WITH_GRAPH
        if(!looped) //weid stuff
        {
            sens_aht10 tt(0,0);
            tt._data.h = 30+rand()%10;
            tt._data.t = 60+rand()%10;
            _canvas.add_th(Sclk.minutes(), &tt, _relay_state);
            _temp++;
            if(_temp==MINS_PER_DAY)
            {
                _temp = 0;
            }
        }
#endif

#endif
        if(_wifi->sta_connected())
        {
            Sclk.update_ntp();
        }
        if(_day_changed)
        {
            _day_changed=false;
            on_day_changed(false);
        }
    }

    Ramm.loops++;
    hot_store();
}

/////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_init()
{
    LED ? pinMode(LED, OUTPUT) : __nill();
    RELAY ? pinMode(RELAY, OUTPUT) : __nill();

#if I2C_SDA
    _sensors.loop();
#endif
#if  WITH_GRAPH
    th_t triger = {CFG(trg),CFG(hrg)};
    _canvas.set_trigger(triger);
    load();
#endif
    Sclk.init_ntp();
    LOG("APP OKAY");
    Running = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
void app_t::led_ctrl(bool rm)
{
    app_t::TheApp->_led_ctrl(rm);
}

/////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_wifi_state(bool state)
{
    LOG("WIFI=%d",state);
    if(state)
    {
        Sclk.update_ntp(true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_web_event(const sensdata_t* val)
{
    LOG(__FUNCTION__);
    _inevent = true;

    const sensdata_t* p = val;
    while(p)
    {
        switch(p->type)
        {
        case eTIME:   Sclk.set_seconds(p->u.z); break;
        case eRELAY:  _relay_ctrl(p->u.uc); break;
        }
        p=p->next;
    }
    sensdata_t::release(val);
    _inevent = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_relay_event(const sensdata_t* val)
{
    LOG(__FUNCTION__);
    if(!_inevent)
    {
        _wifi->on_relay_event(val);
    }
    sensdata_t::release(val);
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_button_event(const sensdata_t* val)
{
    LOG(__FUNCTION__);
    if(BUTTON)
    {
        if(val->u.uc==BUTTON_DOWN){
            _buttonstate[0]=BUTTON_DOWN;
            _buttonstate[1]=millis();
        }
        if(_buttonstate[0]=BUTTON_DOWN && val->u.uc==BUTTON_UP)
        {
            _buttonstate[0]=BUTTON_UP;
            if(millis() - _buttonstate[1]>64)
            {
                _led_state=~_led_state;
                _relay_ctrl(_led_state);
            }
        }
        sensdata_t::release(val);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::_relay_ctrl(bool on)
{
    if(on)
        digitalWrite(RELAY, RELAY_ON);
    else
        digitalWrite(RELAY, RELAY_OFF);

    sensdata_t* p = sensdata_t::get(eRELAY, on);
    if(_relay_state != on)              // changed
    {
        on_relay_event(p);
        _relay_state = on;

        if(on)                          // count relay on time. reset this at month switch
        {
            _ae._uptime[TODAYY].s = Sclk.ntp_epoch_secs();
        }
        else if(_ae._uptime[TODAYY].s>=0)
        {
            _ae._uptime[TODAYY].tot += Sclk.ntp_epoch_secs() - _ae._uptime[TODAYY].s;
            _ae._uptime[TODAYY].s = 0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::tick_10()
{
    Sclk.tick();
    TheApp->_tick10();
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::_tick10()
{
    _tenth = true;
}

////////////////////////////////////////////////////////////////////////////////////////
void app_t::_led_ctrl(bool rm)
{
    digitalWrite(LED, rm ? LED_ON : LED_OFF);
}

////////////////////////////////////////////////////////////////////////////////////////
void app_t::_draw_canvas(String& page)
{
#if WITH_GRAPH
    _canvas.draw(page);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_sensor_event(const sensor_th_t* ps)
{
#if I2C_SDA
    if(CFG(trigger_rule)!=0)
    {
        th_t th = ps->get_th();
        int id = ps->id();
        on_relay_control(id, th.t, th.h);
    }

    if(_wifi){
        _wifi->on_sensor_event(ps);
    }
#if WITH_GRAPH
    _canvas.add_th(Sclk.minutes(), ps, _relay_state);
#endif
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_relay_control(int sid, float temp, float hum)
{
    if(sid == CFG(sid))
    {
        if(Sclk.diff_time(millis(),_lastrelay_ctrl) > 30000)
        {
            _lastrelay_ctrl = millis();
            if(CFG(trg) && CFG(hrg))
            {
                float   tt = CFG(trg);
                float   ht = CFG(hrg);
                bool    relay = _relay_state;

                switch(CFG(trigger_rule))
                {
                default:
                case 0:
                    break;
                case 1: relay = temp > tt && hum < ht; break; // Temp above and Humidity belo
                case 2: relay = temp > tt && hum > ht; break; // Temp above and Humidity abov
                case 3: relay = temp < tt && hum > ht; break; // Temp below and Humidity abov
                case 4: relay = temp < tt && hum < ht; break; // Temp below and Humidity belo
                case 6: relay = temp > tt || hum < ht; break; // Temp above OR Humidity below
                case 5: relay = temp > tt || hum > ht; break; // Temp above OR Humidity above
                case 7: relay = temp < tt || hum > ht; break;
                case 8: relay = temp < tt || hum < ht; break;
                }
                _relay_ctrl(relay);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::reboot_board(int err, const String* replacement)
{
    String page;
    if(replacement)
        page=*replacement;
    else
    {
        page  = get_redirect();
        page += F("<p hidden id='ip'>");
        if(_wifi->sta_connected())
        {
#if ASYNC_WEB
            page += espxxsrv_t::toStrIp(WiFi.localIP());
#else
            page += espxxsrv_t::toStrIp(REQ_OBJECT.client().localIP());
#endif
        }
        else
        {
            page += espxxsrv_t::toStrIp(WiFi.softAPIP());
        }
        page += F("</p>");
        if(err==0)
        {
            page += "<h1>Please wait, Board rebooting <p id='cd'></p><br></h1>";
        }
        else
        {
            page += "<h1><font color='red'>ERROR !</font>. Please wait <div style='display:inline' id='cd'></div></h1>";
        }
    }
    page+= "<hr><br /><div class='red'>Do not refresh !</div></br><hr>";
    _wifi->send(page.c_str());
    _wifi->flush();
    delay(128);
    cli();
    do{ static void (*_JMPZERO)(void)=nullptr; (_JMPZERO)(); } while(0);
    for(;;); //wd reset
}



//////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_day_changed(bool fromINT)
{
    if(fromINT)
    {
        _day_changed=true;
        return;
    }

    _ae._uptime[MONTH].tot     += _ae._uptime[TODAYY].tot;
    _ae._uptime[YEAR].tot      += _ae._uptime[TODAYY].tot;
    _ae._uptime[YESTERDAY].tot  = _ae._uptime[TODAYY].tot;
    _ae._uptime[TODAYY].tot     = 0;
    _ae._uptime[TODAYY].s       = -1;
    Sclk.reroute_event();
    save();
}


//////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_month_changed()
{
    _ae._uptime[LASTMONTH].tot = _ae._uptime[MONTH].tot;
    _ae._uptime[MONTH].tot = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::on_year_changed()
{
    _ae._uptime[LASTYEAR].tot = _ae._uptime[YEAR].tot;
    _ae._uptime[YEAR].tot = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::_sav_data()
{
    _ae._reboots++;
    _ae._eprom_writes++;
    eeprom_t::eprom_writes(reinterpret_cast<const uint8_t*>(&_ae), sizeof(_ae));
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::_load_data()
{
    eeprom_t::eprom_reads(reinterpret_cast<uint8_t*>(&_ae), sizeof(_ae));
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::load()
{
    LOG("LOADING APP");
    eeprom_t::eprom_start();
    uint8_t sig = eeprom_t::eprom_read();
    if(sig==EPROM_SIG_APP)
    {
        LOG("APP<-LOAD");
        _load_data();
#if  WITH_GRAPH
        _canvas.load();
#endif
    }
    else
    {
        LOG("SAVE<-APP");
        _sav_data();
#if  WITH_GRAPH
        _canvas.save();
#endif
    }
    eeprom_t::eprom_end();
    _ae._reboots++;
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::save()
{
    _ae._eprom_writes++;

    eeprom_t::eprom_start();
    eeprom_t::eprom_write(EPROM_SIG_APP);
    _sav_data();


#if  WITH_GRAPH
    _canvas.save();
#endif
    eeprom_t::eprom_end();
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::factory()
{
    memset(_ae._uptime,0,sizeof(_ae._uptime));
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::convert_graph(bool from, bool to)
{
#if  WITH_GRAPH
    _canvas.convert(from,to);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void app_t::canvas_it()
{
#if  WITH_GRAPH
    _canvas.canvasit();
#endif
}

void app_t::detect()
{
    String page = espxxsrv_t::start_htm();
    ::memset(&Ramm,0,sizeof(Ramm));
    hot_store();
    sensors_t::detect(page);
    if(espxxsrv_t::ping(CFG(gw)))
        page+="<li>Can reach gateway";
    if(espxxsrv_t::ping(CFG(ntp_srv))){
        page+="<li>Can reach ntp";
        Sclk.update_ntp();
    }
    espxxsrv_t::end_htm(page);

}

bool app_t::get_relay(int daymin)const
{
#if  WITH_GRAPH
    bool wason = _canvas.get_relay(daymin);
#endif
    return _relay_state|wason;

}
