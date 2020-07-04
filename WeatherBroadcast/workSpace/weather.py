

import urequests
import webserver
import time
import micropython
import ujson
import audio_set
import display_ssd1306_i2c
import gc

def main():
  
    wind_txt = ["north","northeast","east","southeast","south","southwest","west","northwest"]
  
    print('LOG::This message is from main function')
    micropython.mem_info()
    
    lcd = display_ssd1306_i2c.Display()
    
    ip = webserver.connect()
    lcd.show_text_wrap(ip)
   
    print('LOG::WIFI connect over.')
    micropython.mem_info()
    
    while True:
      
      """
      try:
      """
      #request weather api
      url = "https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=en"
      print("Prepare to get API")
      res = urequests.get(url)
      html=res.text
      print(html)
      print("request over")
      

      
      #parse json from weather api
      str_weather = ""
      weather_json = ujson.loads(html)
        
      up_time = weather_json["HeWeather6"][0]["update"]["loc"]
      location = weather_json["HeWeather6"][0]["basic"]["location"]
      cond_code = weather_json["HeWeather6"][0]["now"]["cond_code"]
      cond_txt = weather_json["HeWeather6"][0]["now"]["cond_txt"]
      tmp = weather_json["HeWeather6"][0]["now"]["tmp"]
      wind_deg = weather_json["HeWeather6"][0]["now"]["wind_deg"]
      wind_spd = weather_json["HeWeather6"][0]["now"]["wind_spd"]
      wind_dir = wind_txt[int(int(wind_deg)/45)]
      
      print("Prepare to Show")
      lcd.clear()
      lcd.show_text(up_time,0,0,False,True)
      lcd.show_text(location,0,10,False,True)
      lcd.show_text(tmp +" centigrade",0,20,False,True)
      lcd.show_text(cond_txt,0,30,False,True)
      lcd.show_text(wind_dir + " wind",0,40,False,True)
      lcd.show_text(wind_spd + " km/h",0,50,False,True)
      
      #init i2s to output audio
      speaker = audio_set.audio()
      
      print("Prepare to Speak")
      str_weather = location + " is " + tmp + " cent grade"
      speaker.say_sentance(str_weather)
      time.sleep(2)   #wait speak over
      
      str_weather = "weather is " + cond_txt
      speaker.say_sentance(str_weather)
      time.sleep(2)   #wait speak over
  
      str_weather = "wind direction is " + wind_dir
      speaker.say_sentance(str_weather)
      time.sleep(2)   #wait speak over
      
      str_weather = "wind speed is " + wind_spd + " kilometers per hour "
      speaker.say_sentance(str_weather)
      time.sleep(2)   #wait speak over
      
      speaker.close()
      """
      except :
        print("ERROR")
        pass
        """
      
      gc.collect()
      
      time.sleep(5)


if __name__ == '__main__':
    main()









