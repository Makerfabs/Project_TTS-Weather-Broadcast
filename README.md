---
typora-root-url: md_pic
---

# MicroPython TTS Weather broadcast

```c++
/*
Version:		V1.0
Author:			Vincent
Create Date:	2020/7/1
Note:
*/
```

![1](https://github.com/Makerfabs/Project_TTS-Weather-Broadcast/blob/master/md_pic/1.JPG)

[toc]



# OVERVIEW

## Intruduce

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)

Through the MicroPython TTS text-to-speech module, the weather API on the network can be acquired through wifi, and the real-time weather can be broadcast by voice. Download the code and connect with the stereo, you will get a customized intelligent weather announcer!


## Equipment list

- [MakePython ESP32](https://www.makerfabs.com/wiki/index.php?title=MakePython_ESP32)
- [MakaPython Audio](https://www.makerfabs.com/wiki/index.php?title=MakaPython_Audio)
- Audio/headphones with 3.5mm audio connector
- Wi-fi routers

# STEPS

## 1 Download the code

- Connect MakePython ESP32 to your PC, open uPyCraft, and select connect to the serial port.
- If you have not burned Firmware before or for other reasons, you will be prompted to burn Firmware. Board Choose ESP32, BURN_addr Choose 0x1000,erase_flash Choose Yes,com Choose the corresponding port number.

![c0](https://github.com/Makerfabs/Project_TTS-Weather-Broadcast/blob/master/md_pic/c0.png)

- **Plug ESP32 and Audio expansion boards together after burning. Be sure to plug them together after burning or the firmware will not burn.** 
- Change webserver.py to change the sSID and passward to the password for your wireless router.


```pyth
SSID = "Makerfabs"      #Modify here with SSID
PASSWORD = "20160704"   #Modify here with PWD
```

- (optional) to replace the url with their own API interface, the source of the interface is the author apply for the trial of the API, [heweather](https://www.heweather.com/) is a free API, there may not be able to normal access. Makerfabs API trial is not available, please make your own application, application API example behind.

```python
#request weather api
url = "https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=en"
res = urequests.get(url)
```

- Save the changes and download all python programs ending in.py from the LoraGate folder to ESP32.

![c1](https://github.com/Makerfabs/Project_TTS-Weather-Broadcast/blob/master/md_pic/c1.png)

- Right click weather.py and set to Default Run to boot automatically.

![c2](https://github.com/Makerfabs/Project_TTS-Weather-Broadcast/blob/master/md_pic/c2.png)

- Due to the added function of WiFi connection, the boot will be slow, please pay attention to the information returned by the serial port, which contains the IP address ESP32 got when connected to WiFi.

```c++
[0;32mI (4066) network: CONNECTED[0m
[0;32mI (5296) event: sta ip: 192.168.1.137, mask: 255.255.255.0, gw: 192.168.1.1[0m
[0;32mI (5296) network: GOT_IP[0m


network config: ('192.168.1.137', '255.255.255.0', '192.168.1.1', '192.168.1.1')
```

- According to serial port information, the IP of this node in LAN is 192.168.1.137.

- After successful networking, the weather API will be automatically requested, and the interface of this call will return a string of weather JSON.

```c++
{
    "HeWeather6":[
        {
            "basic":{
                "cid":"CN101280601",
                "location":"Shenzhen",
                "parent_city":"Shenzhen",
                "admin_area":"Guangdong",
                "cnty":"China",
                "lat":"22.54700089",
                "lon":"114.08594513",
                "tz":"+8.00"
            },
            "update":{
                "loc":"2020-07-01 16:27",
                "utc":"2020-07-01 08:27"
            },
            "status":"ok",
            "now":{
                "cloud":"98",
                "cond_code":"104",
                "cond_txt":"Overcast",
                "fl":"34",
                "hum":"72",
                "pcpn":"0.0",
                "pres":"995",
                "tmp":"30",
                "vis":"8",
                "wind_deg":"171",
                "wind_dir":"S",
                "wind_sc":"2",
                "wind_spd":"6"
            }
        }
    ]
}

```

- The serial monitor displays the parsed string on the LCD screen.


![c3](https://github.com/Makerfabs/Project_TTS-Weather-Broadcast/blob/master/md_pic/c3.JPG)


```c++
Shenzhen weather is Overcast temperature is 30
```

- Plug the 3.5mm connector of the stereo or earphone into the audio outlet.


![c4](https://github.com/Makerfabs/Project_TTS-Weather-Broadcast/blob/master/md_pic/c4.JPG)

- Real-time weather will be played regularly.

## 2 Python Explain

### 2.1 ssd1306.py

- Ssd1306 led screen driver.

### 2.2 display_ssd1306_i2c.py

- Ssd1306 LED screen based on I2C basic display library.
- Text display function.
- Image display function.

### 2.3 webserver.py

- Wifi initialization and connection.

### 2.4 urequest.py

- Network request library for MicroPython
- Used to send a GET request to access the weather API.

### 2.5 weather.py

- main()
- Parse api responce.

### 2.6 audio_set.py

- TTS module initialization
- Break down long sentences into words and read them aloud.

# How to get a weather API?

Different regions may have different network restrictions. Since I can't run VPN on ESP32, and I don't have a wireless router with this function, I can't use API like Google, so I chose a local Weather API provider in China:

[heweather](https://github.com/heweather)

You can on Google looking for a fee or free API interface, most of time limits or individual users with a number of free API interface. The remaining is according to the use of API tutorial, generally is registered, choose the required interface, and then give you a key used to access the service chamber of commerce. After can replace your url and analytic function in this project.

I don't guarantee that the interface will be accessible by the time you see this, so it's best to simply replace the API you know you can use and modify the parse_HTML () function in weather.py to parse it properly.
