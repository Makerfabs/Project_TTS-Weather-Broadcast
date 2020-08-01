#include "Arduino.h"
#include "MySpeech.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define I2S_DOUT 27
#define I2S_BCLK 26
#define I2S_LRC 25

//Audio audio;
Speech audio;

//WIFI
const char *ssid = "Makerfabs";
const char *password = "20160704";

const int Pin_mute = 35;

void setup()
{
    pinMode(Pin_mute, INPUT_PULLUP);

    Serial.begin(115200);
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // 0...21
    Serial.println("Ready to tts");

    audio.connecttospeech("Already.", "en");
}

uint button_time = 0;

void loop()
{
    audio.loop();
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
    if (millis() - button_time > 300)
    {
        if (digitalRead(Pin_mute) == 0)
        {
            Serial.println("Pin_mute");
            String text = weather_request();
            audio.connecttospeech(text, "zh-CN");
            audio.connecttospeech("Shenzhen, rain, 28 degrees Celsius, northwest wind, relative humidity 40 percent.", "en");
            button_time = millis();
        }
    }
}



String weather_request()
{
    HTTPClient http;
    String text = "";

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url

    http.begin("https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=enl");

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

            String cid = doc["HeWeather6"][0]["basic"]["cid"];

            Serial.println(cid);

            String location = doc["HeWeather6"][0]["basic"]["location"];

            Serial.println(location);

            text = cid + location;
            text = "深圳，雨，28摄氏度，西北风，相对湿度百分之四十。";
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return text;
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
