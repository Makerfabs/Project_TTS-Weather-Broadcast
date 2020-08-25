#include "Arduino.h"
#include "MySpeech.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "weather_icon.h"

#define SR505 15

#define I2S_DOUT 27
#define I2S_BCLK 26
#define I2S_LRC 25

//SSD1306
#define MAKEPYTHON_ESP32_SDA 4
#define MAKEPYTHON_ESP32_SCL 5
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Audio audio;
Speech audio;

//WIFI
const char *ssid = "Makerfabs"*;
const char *password = "20160704";

const int Pin_mute = 35;

//Weather
String wind_txt[] = {"north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"};

void setup()
{
    pinMode(Pin_mute, INPUT_PULLUP);
#ifdef SR505
    pinMode(SR505, INPUT_PULLDOWN);
#endif

    Serial.begin(115200);

    //LCD init
    Wire.begin(MAKEPYTHON_ESP32_SDA, MAKEPYTHON_ESP32_SCL);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();
    logoshow();

    /*
    //Screen dirction
    display.setRotation(2);

    //Weather icon
    draw_weather(2);
    */

    //Wifi init
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");
    lcd_text("WIFI OK");

    //Audio init
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // 0...21
    Serial.println("Ready to tts");
    lcd_text("Ready to tts");

    //audio.connecttospeech("Already.", "en");
}

uint button_time = 0;

void loop()
{
    audio.loop();
    /*
    if (Serial.available())
    { // put streamURL in serial monitor
        audio.stopSong();
        Serial.println("Ready to tts");
        String r = Serial.readString();
        r.trim();
        if (r.length() > 5)
        {
            audio.connecttospeech(r, "zh-CN");
        }
        log_i("free heap=%i", ESP.getFreeHeap());
    }
    */
    if (millis() - button_time > 300)
    {
        if (digitalRead(Pin_mute) == 0)
        {
          /*  Serial.println("Pin_mute");
            String text = weather_request();
            audio.connecttospeech(text, "en");
            //audio.connecttospeech(text, "zh-CN");
            //audio.connecttospeech("Shenzhen, rain, 28 degrees Celsius, northwest wind, relative humidity 40 percent.", "en");
            button_time = millis();  */
        }
    }
#ifdef SR505
    //infrared detection
    if (millis() - button_time > 15000)
    {
        if (digitalRead(SR505) == 1)
        {
            Serial.println("SR505");
            String text = weather_request();
            audio.connecttospeech(text, "en");
            button_time = millis();
        }
    }
#endif
}

String weather_request()
{
    HTTPClient http;
    String text = "";

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url

    //persional api,please change to yourself
    http.begin("https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=en");

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            Serial.println(payload);

            //JSON
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            JsonObject obj = doc.as<JsonObject>();

            String cond_txt = doc["HeWeather6"][0]["now"]["cond_txt"];
            String tmp = doc["HeWeather6"][0]["now"]["tmp"];
            String hum = doc["HeWeather6"][0]["now"]["hum"];
            int wind_deg = doc["HeWeather6"][0]["now"]["wind_deg"];
            lcd_weather(cond_txt, tmp, hum, wind_txt[wind_deg / 45]);
            text = "Shenzhen, " + cond_txt + ", " + tmp + " centigrade, " + wind_txt[wind_deg / 45] + " wind,relative humidity " + hum + " percent.";
            //text = "深圳，雨，28摄氏度，西北风，相对湿度百分之四十。";
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return text;
}

void lcd_weather(String cond_txt, String tmp, String hum, String wind_dir)
{
    display.clearDisplay();
    
   if(cond_txt == "Sunny")
      {
        draw_weather(0);
        display.setTextSize(4);              // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.setCursor(64, 16);
        display.println(tmp);
        display.setTextSize(2);
        display.setCursor(112, 32);
        display.println("C");
      }
    else if (cond_txt == "Rain")
      {
        draw_weather(1);
        display.setTextSize(4);              // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.setCursor(64, 16);
        display.println(tmp);
        display.setTextSize(2);
        display.setCursor(112, 32);
        display.println("C");
      }
    else if (cond_txt == "Overcast")
      {
        draw_weather(2);
        display.setTextSize(4);              // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.setCursor(64, 16);
        display.println(tmp);
        display.setTextSize(2);
        display.setCursor(112, 32);
        display.println("C");
      }
    else 
    {
    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(cond_txt);
    display.setCursor(86, 48); // Start at top-left corner
    display.println("C"); 
    display.setTextSize(4);
    display.setCursor(32, 32); // Start at top-left corner
    display.println(tmp);
    }
   
    display.display();
  
}

void logoshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Weather"));
    display.setCursor(0, 20); // Start at top-left corner
    display.println(F("Broadcast"));
    display.setCursor(0, 40); // Start at top-left corner
    display.println(F("TTS V2"));
    display.display();
    delay(2000);
}

void lcd_text(String text)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(text);
    display.display();
    delay(500);
}

void draw_weather(int a)
{
    const unsigned char *icon;
    switch (a)
    {
    case 0:
        icon = sun;
        break;
    case 1:
        icon = rain;
        break;
    case 2:
        icon = cloud;
        break;
    default:
        icon = sun;
    }
    //display.clearDisplay();

    display.drawBitmap(0, 0, icon, ICON_W, ICON_H, 1);
//    display.display();
//    delay(1000);
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ //id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ //end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
    Serial.print("streaminfo  ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ //duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ //homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ //stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
void audio_eof_speech(const char *info)
{
    Serial.print("eof_speech  ");
    Serial.println(info);
}
