#include "canvas.h"
#include "eeprom.h"
#include "application.h"

#if WITH_GRAPH

canvas::canvas(int w,int h):_w(w),_h(h)
{

    ::memset(_graph,0,sizeof(_graph));
    ::memset(_relay,0,sizeof(_relay));
}

void    canvas::draw(String& page)
{
    page +="<hr>";
    page +=F("\n<canvas width='1140' height='400' id='ka'></canvas>\n");
    page +=F("\n<script>\n");
    //////////////////////////////////////////////////////////////////////////////////////
    page +=F("var c=document.getElementById(\"ka\");\n");
    page +=F("c.addEventListener('click',onclick,false);\n");
    page +=F("var Isens=0;\n");
    page +=F("c.width=c.parentElement.clientWidth;\n");
    page +=F("var cx=c.getContext(\"2d\");\n");
    page +=F("var gw="); page+=String(SAMPLES_PER_DAY); page+= F(".0;\n");
    page +=F("var hack=0;\n");
    page +=F("var Scnt="); page += String(MAX_SENS); page+=F(";\n");
    if(CFG(faren))
        page +=F("var gh=200.0;hack=30.0;\n"); // max 200 FH
    else
        page +=F("var gh=100.0;\n");
    page +=F("var w=c.width;\n");
    page +=F("var h=c.height;\n");

    //////////////////////////////////////////////////////////////////////////////////////
    page +=F("function sy(y){return  ((y-hack)*h)/gh;}\n");
    page +=F("function sx(x){return  (x*w)/gw;}\n");
    page +=F("function syh(y){return (y*h)/100;}\n");
    page +=F("var Trgt=")+String(_trigger.t) + F(";\n");
    page +=F("var Thgh=")+String(_trigger.h) + F(";\n");
    ///////////////////////////////////////////////////////////////////////////////////////
    page +=F("var Now=");
    page +=String(_minutes);
    page +=F(";\n");
    page +=F("var Tgr=[");
    for(int i=0;i<SAMPLES_PER_DAY;i++)
    {
        page += String(int(_graph[CFG(sid)][i].t)) + F(",");
    }
    page +=F("0,0,0];\n\n");
    page +=F("var Hgr=[");
    for(int i=0;i<SAMPLES_PER_DAY;i++)
    {
        page += String(int(_graph[CFG(sid)][i].h)) + F(",");
    }
    page +=F("0,0,0];\n\n");
    page +=F("var Rl=[");
    for(int e=0;e < SAMPLES_PER_DAY;e++)
    {
        page +=_relay[e];
        page +=F(",");
    }
    page +=F("0,0,0];\n\n");
    page +=F("start_timer();\n");

    /////////////////////////////////////////////////////////////////////////////////////
    if(CFG(hrg)==101)
        page +=F("function start_timer() {\n"
                 "    setInterval(update,1000);\n"
                 "}\n\n");
    else
        page +=F("function start_timer() {\n"
                 "    setInterval(update,10000);\n"
                 "}\n\n");

    page +=F("var Pm=-1;\n"
             "function update() {\n"
             "let options={\n"
             "  method: 'GET',\n"
             "  headers: {}\n"
             "};\n"
             //  time,Rl,Temp,Hum,
             "fetch('/fetch',options)\n"
             ".then(response=> response.text())\n"
             ".then(response=> {\n"
             "const svals=response.split(\":\");\n" //T R t  h  t  h
             "if(svals.length>=2+(2*Scnt)){"                 //2:0:20:31:22:38
             "Now=parseInt(svals[0]);\n");
    page+=F( "const rstate=parseInt(svals[1]);\n"
             "Rl[Now]=rstate;\n"
             "Tgr[Now]=parseInt(svals[2+(2*Isens)]);\n"
             "Hgr[Now]=parseInt(svals[3+(2*Isens)]);\n"
             "draw();\n"
             "if(rstate){\n"
             "document.getElementById('r_on').style.color='green';\n"
             "}else{\n"
             "document.getElementById('r_on').style.color='grey';\n"
             "}\n"
             "};\n"
             "});\n"
             "}\n");
    /////////////////////////////////////////////////////////////////////////////////////

    page +=F("draw();\n"
            "update();\n"
            "function onclick(event)\n"
            "{\n"
            "if(1)\n"
            "{\n"
            "Isens++; \n"
            "if(Isens>=Scnt)\n"
            "Isens=0;\n"
            "fetch('/graph?sens='+Isens)\n"
            ".then(response=> response.text())\n"
            ".then(\n"
            "response=> {\n"
            "const svals=response.split(',');var j=0;\n"
            "for(var s=0;s<svals.length-1;)\n"
            "{\n"
            "Tgr[j]=parseInt(svals[s++]);\n"
            "Hgr[j]=parseInt(svals[s++]);\n"
            "j++;if(j>=Tgr.length)break;}\n"
            "draw();\n"
            "});\n"
            "}}\n");

    ////////////////////////////////////////////////////////////////////////////////////
    page +=F("\nfunction draw()\n{\n");
    page +=F("cx.beginPath();\n");
    page +=F("cx.rect(0,0,w,h);\n");
    page +=F("cx.fillStyle=\"#000\";\n");
    page +=F("cx.fill();\n");

    page +=F("cx.resetTransform();\n");
    page +=F("cx.scale(1,-1);\n");
    page +=F("cx.translate(0,-h);\n");

    // hours and units
    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#446\";\n");
    page +=F("for(var x=0;x < w;x +=w/24)\n");
    page +=F("{\n");
    page +=F("cx.moveTo(x,0);\n");
    page +=F("cx.lineTo(x,h);\n");
    page +=F("}\n");

    // temps / units
    page +=F("for(var yy=0;yy<h;yy+=h/10)\n");
    page +=F("{\n");
    page +=F("cx.moveTo(0,yy);\n");
    page +=F("cx.lineTo(w,yy);\n");
    page +=F("}\n");
    page +=F("cx.stroke();\n");


    //  CURTIME LINE
    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#6F6\";\n");
    page +=F("cx.moveTo(sx(Now)+1,0);\n");
    page +=F("cx.lineTo(sx(Now)+1,h);\n");
    page +=F("cx.stroke();\n");

    //trigger lines
    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#D77\";\n");
    page +=F("cx.moveTo(0,sy(Trgt));\n");
    page +=F("cx.lineTo(w,sy(Trgt));\n");
    page +=F("cx.stroke();\n");

    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#77D\";\n");
    page +=F("cx.moveTo(0,syh(Thgh));\n");
    page +=F("cx.lineTo(w,syh(Thgh));\n");
    page +=F("cx.stroke();\n");

    // the sensor data points
    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#F99\";\n");
    page +=F("var xo=0;\n");
    page +=F("var yo=(Tgr[0]);\n");
    page +=F("for(var x=1;x<Tgr.length-1;x++)\n");
    page +=F("{\n");
    page +=F("if(Tgr[x]||Tgr[x+1]){\n");
    page +=F("cx.moveTo(sx(xo),sy(yo));\n");
    page +=F("cx.lineTo(sx(x),sy(Tgr[x]));\n");
    page +=F("}xo=x;\n");
    page +=F("yo=Tgr[x];\n");
    page +=F("}\n");
    page +=F("cx.stroke();\n");

    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#99F\";\n");
    page +=F("var xo=0;\n");
    page +=F("var yo=(Hgr[0]);\n");
    page +=F("for(var x=1;x < Hgr.length-1;x++)\n");
    page +=F("{\n");
    page +=F("if(Hgr[x]||Hgr[x+1]){\n");
    page +=F("cx.moveTo(sx(xo),syh(yo));\n");
    page +=F("cx.lineTo(sx(x),syh(Hgr[x]));\n");
    page +=F("}xo=x;\n");
    page +=F("yo=Hgr[x];\n");
    page +=F("}\n");
    page +=F("cx.stroke();\n");

    //relay
    page +=F("cx.beginPath();\n");
    page +=F("cx.strokeStyle=\"#FF8\";\n");
    page +=F("var xo=0;\n");
    page +=F("var yo=sy(Rl[0]);\n");
    page +=F("var yi=sy(Rl[1]);\n");
    page +=F("for(var x=1;x < Rl.length-1;x++)\n");
    page +=F("{\n");
    page +=F("if(Rl[x-1]!=Rl[x]){\n");
    page +=F("cx.moveTo(sx(x),  h );\n");
    page +=F("cx.lineTo(sx(x),  h-h/10 );\n");
    page +=F("}if(Rl[x]){\n");
    page +=F("cx.moveTo(sx(x),h-h/10 );\n");
    page +=F("cx.lineTo(sx(x+1),h-h/10 );\n");
    page +=F("}\n");
    page +=F("}\n");
    page +=F("cx.stroke();\n");
    page +=F("cx.resetTransform();\n");

    // hours text
    page +=F("var hour=0;\n");
    page +=F("cx.font=\"12px Arial\";\n");
    page +=F("cx.fillStyle=\"#888\";\n");
    page +=F("for(var x=0;x < w;x +=w/24)\n");
    page +=F("{\n");
    page +=F("const ss=hour.toString();\n");
    page +=F("cx.fillText(ss,x+2,14);\n");
    page +=F("hour++;\n");
    page +=F("}\n");

    // hums text coords
    page +=F("hour=0;\n");
    page +=F("cx.fillStyle=\"#88C\";\n");
    page +=F("for(var y=0;y < h;y +=(h/10))\n");
    page +=F("{\n");
    page +=F("const ss=hour.toString();\n");
    page +=F("cx.fillText(ss+'*C',w-20,h-y-7);\n");
    page +=F("hour+=10;\n");
    page +=F("}\n");
    // temp text coords
    if(CFG(faren))
    {
        page +=F("yp=0;\n");
        page +=F("cx.fillStyle=\"#C88\";\n");
        page +=F("for(var y=0;y<h;y+=h/10)\n");
        page +=F("{\n");
        page +=F("const ssf=(yp*(9.0/5.0))+32.0;\n");
        page +=F("const ss=parseInt(ssf.toString());\n");
        page +=F("cx.fillText(ss+'*F',1,h-y-7);\n");
        page +=F("yp+=10;\n");
        page +=F("}\n");
    }

    page +=F("cx.fillStyle=\"#FBB\";\n");
    page +=F("var tstr;\n");
    page +=F("var hstr;\n");
    page +=F("var ypt,yph;\n");

    if(CFG(faren)){
        page +=F("tstr=Tgr[Now].toString()+'*F';\n");
        page +=F("ypt=h-sy(Tgr[Now]);\n");
        page +=F("cx.fillText(tstr,sx(Now)+9,ypt);\n");
        page +=F("cx.fillStyle=\"#BBF\";\n");
        page +=F("hstr=Hgr[Now].toString()+'%';\n");
        page +=F("yph=h-syh(Hgr[Now]);\n");
        //        page +=F("if(Math.abs(ypt,yph)<14){if(yph<ypt)yph-=10; else yph+=10;}\n");
        page +=F("cx.fillText(hstr,sx(Now)+9,yph);\n");
    }
    else{
        page +=F("tstr=Tgr[Now].toString()+'*C';\n");
        page +=F("cx.fillText(tstr,sx(Now)+9,h-sy(Tgr[Now]) );\n");
        page +=F("cx.fillStyle=\"#BBF\";\n");
        page +=F("hstr=Hgr[Now].toString()+'%';\n");
        page +=F("cx.fillText(hstr,sx(Now)+9,(h-sy(Hgr[Now])) );\n");
    }
    // left top box
    page +=F("cx.fillStyle=\"#006\";\n");
    page +=F("cx.fillRect(22,12,48,54);\n");

    page +=F("cx.fillStyle=\"#FFF\";\n");
    if(CFG(faren))
        page +=F("cx.fillText('Fahrenh\',26 ,25 );\n");
    else
        page +=F("cx.fillText('Celsius\',26 ,25 );\n");
    page +=F("cx.fillStyle=\"#AAF\";\n");
    page +=F("cx.fillText('H: '+hstr,23 ,38 );\n");
    page +=F("cx.fillStyle=\"#FAA\";\n");
    page +=F("cx.fillText('T: '+tstr,23 ,50);\n");
    page +=F("cx.fillText('S: '+Isens,23 ,62);\n");
    page +=F("}\n");
    page +=F("</script>\n\n");
}

/////////////////////////////////////////////////////////////////////////////////////////
void    canvas::save()
{
    for(int i=0; i<  MAX_SENS;i++)
    {
        for(int s=0;s<SAMPLES_PER_DAY;s++)
        {
            byte t=byte(_graph[i][s].t);
            byte h=byte(_graph[i][s].h);
            eeprom_t::eprom_write(t);
            eeprom_t::eprom_write(h);
        }
    }
    LOG("SAVE<-H5");
}

//////////////////////////////////////////////////////////////////////////////////////////
void    canvas::load()
{
    for(int i=0; i<  MAX_SENS;i++)
    {
        for(int s=0;s<SAMPLES_PER_DAY;s++)
        {
            _graph[i][s].t=(float)(eeprom_t::eprom_read());
            _graph[i][s].h=(float)(eeprom_t::eprom_read());
        }
    }
    float   tt = CFG(trg);
    float   ht = CFG(hrg);
    for(int s=0;s<SAMPLES_PER_DAY;s++)
    {
        float temp = _graph[CFG(sid)][s].t;
        float hum = _graph[CFG(sid)][s].h;
        bool relay = false;
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
        _relay[s]=relay;
    }

    LOG("H5<-LOAD");
}

void canvas::convert(bool from, bool to)
{
    LOG(__FUNCTION__);
    for(int i=0; i< MAX_SENS;i++)
    {
        for(int s=0;s<SAMPLES_PER_DAY;s++)
        {
            if(from==true)  //F to C
            {
                float v =(_graph[i][s].t-32)*(5.0/9.0);
                if(v>100)v=0;
                _graph[i][s].t=uint8_t(v);
            }
            else            // C to F
            {
                float v =(_graph[i][s].t*(9.0/5.0))+32;
                if(v>200)v=0;
                _graph[i][s].t=uint8_t(v);
            }
        }
    }
}

void canvas::canvasit()
{
    float temp = 0;

    for(int i=0; i<  MAX_SENS;i++)
    {
        for(int s=0;s<SAMPLES_PER_DAY;s++)
        {
            if((s+1) % 20==0)
            {
                temp += 10;
            }
            if(CFG(faren))
            {
                _graph[i][s].t = (temp*(9.0/5.0))+32;
            }
            else
            {
                _graph[i][s].t = temp;
            }
            if(temp>100)temp=0;
        }
    }
}

String canvas::samples(int s)
{
    String ret;
    LOG("SG=%d",s);
    if(s>=MAX_SENS){ s = MAX_SENS-1;}
    LOG("SG=%d",s);
    for(int i=0;i<SAMPLES_PER_DAY;i++)
    {
        ret+= String(int(_graph[s][i].t));
        ret+= ",";
        ret+= String(int(_graph[s][i].h));
        ret+= ",";
    }
    return ret;
}


#endif
