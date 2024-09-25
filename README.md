# Temperature and Humidity relay control

Once flashed connect to mckteck and setup the wifi and AP new password. Default passord is 12345678.

![image](https://github.com/user-attachments/assets/8ee904b7-9e29-4bd2-ab56-342177fe6a1c)


Uses this board by default in: _myconfig.h


![image](https://github.com/user-attachments/assets/af5ba058-c2a2-4036-8d7e-1f8a97062230)


Uses VCC GND MOSI MISO to T-H sensors, see soldering

![image](https://github.com/user-attachments/assets/3ad7a4c2-9352-4a9a-a393-ed2ff4d6583b)

Can hook up AHT10  SHT21  or SHT3X, or any combination, Maxim 2 sensors on I2C BUS.
Clicking on graph will switch data rendering between the sensors.


HVAC humidifier intake:
![image](https://github.com/user-attachments/assets/7932a69a-5a0d-4881-8268-d70a50dcdea9)

Water tank out pipe monitor:
![image](https://github.com/user-attachments/assets/b554a810-297c-4176-832a-c8a16b24203a)






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




