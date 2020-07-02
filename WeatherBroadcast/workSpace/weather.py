
import urequests
import webserver
import time
import micropython
import ujson
import audio_set
import display_ssd1306_i2c
import gc

def parse_html(html):
  temp_str = ""
  weather_json = ujson.loads(html)
  city = weather_json["HeWeather6"][0]["basic"]["location"]
  status = weather_json["HeWeather6"][0]["now"]["cond_txt"]
  temperature = weather_json["HeWeather6"][0]["now"]["tmp"]

  temp_str += city + " weather is " + status + " temperature is " + temperature
  return temp_str

def main():
    print('LOG::This message is from main function')
    micropython.mem_info()
    
    lcd = display_ssd1306_i2c.Display()
    
    ip = webserver.connect()
    lcd.show_text_wrap(ip)
   
    print('LOG::WIFI connect over.')
    micropython.mem_info()
    
    while True:
      
      try:
        #request weather api
        url = "https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=en"
        res = urequests.get(url)
        html=res.text
        print(html)
        print("request over")
        
        #parse json from weather api
        str_weather = parse_html(html)
        print(str_weather)
        lcd.show_text_wrap(str_weather)
        micropython.mem_info()
        
        #init i2s to output audio
        speaker = audio_set.audio()
        speaker.say_sentance(str_weather)
        time.sleep(2)   #wait speak over
        speaker.close()
      except :
        pass
      
      gc.collect()
      
      time.sleep(5)


if __name__ == '__main__':
    main()







