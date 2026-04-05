# Temperature and Humidity relay control

### Hardware Search ESP8266 RELAY, SHT21 and AHT10 on amazopn.ca
##### Board: ESP8266 ESP-12F WI FI Module 1 Channel Relay Shield Over Current Protection Wi-Fi Network Relay Module 10A DC 7-30V 
##### I2C: Temperature Humidity Sensor GY-213V-HTU21D I2C Replace SHT21 SI7021 HDC1080 Module

###### Soldering I2C module to the board on the dedicated I2C BUS.
###### You can solder 2 seonsors MAX. SHT21 or AHT10 on same I2C lines

![image](https://github.com/user-attachments/assets/af5ba058-c2a2-4036-8d7e-1f8a97062230)


Once flashed connect to AP: **mckteck** and setup the wifi to your **AP**. 
Default passord for **mkteck** is **12345678**. See source code.

![image](https://github.com/user-attachments/assets/8ee904b7-9e29-4bd2-ab56-342177fe6a1c)



![image](https://github.com/user-attachments/assets/3ad7a4c2-9352-4a9a-a393-ed2ff4d6583b)

###### Can hook up AHT10  SHT21  or SHT3X, or any combination, Maxim 2 sensors on I2C BUS.
Clicking on graph will switch data rendering between the sensors.


HVAC humidifier intake:
![image](https://github.com/user-attachments/assets/7932a69a-5a0d-4881-8268-d70a50dcdea9)


Sensors are detected by the I2C address as seen in the code below.

```
  if(ads[i]==AHT10_ADDRESS_0X38)
            _sensors[j] = new sens_aht10(i, ads[i]);
        else if(ads[i]==0x40)
            _sensors[j] = new sens_sht21(i, ads[i]);
        else if(ads[i]==0x44)
            _sensors[j] = new sens_sht3x(i, ads[i]);

```

![image](https://github.com/user-attachments/assets/e9d456c2-6265-451c-949f-604793905860)


In config page you can set the RELAY rules for either sensor.


![image](https://github.com/user-attachments/assets/2f7ca060-8aef-41a9-a45b-e19335ba5689)

As mine is in the hum intake will turn the humidifier by the rule above.

Enjoy.

#### Humidifier: Valve on water supply, in series with relay and proper voltage transformer


<img width="364" height="529" alt="image" src="https://github.com/user-attachments/assets/dd662d28-8566-4265-92e5-3dca97a98a9d" />

<img width="335" height="340" alt="image" src="https://github.com/user-attachments/assets/a7ab2f54-e4b7-446a-9b86-b13e54e31e23" />

#### Sensor in the AIR intake
<img width="349" height="465" alt="image" src="https://github.com/user-attachments/assets/898e8446-7a64-43da-b548-72741df74ef7" />


##### Water tank out pipe monitor (optional second I2C sensor ):
![image](https://github.com/user-attachments/assets/b554a810-297c-4176-832a-c8a16b24203a)


####  ESP module controlelr powered at 9VDC

<img width="609" height="361" alt="image" src="https://github.com/user-attachments/assets/d0125e4a-e09a-4795-abc9-f3affc270add" />









    



