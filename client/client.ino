/**
 * Authorization.ino
 *
 *  Created on: 09.12.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

void setup()
{

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for (uint8_t t = 4; t > 0; t--)
    {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("Makerfabs", "20160704");
}

void loop()
{
    // wait for WiFi connection

    if ((wifiMulti.run() == WL_CONNECTED))
    {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url

        http.begin("https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=enl");

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK)
            {
                String payload = http.getString();
                USE_SERIAL.println(payload);

                //JSON
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, payload);
                JsonObject obj = doc.as<JsonObject>();

                String cid = doc["HeWeather6"][0]["basic"]["cid"];

                Serial.println(cid);

                String location = doc["HeWeather6"][0]["basic"]["location"];

                Serial.println(location);
            }
        }
        else
        {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(10000);
}

String weather_request()
{
    HTTPClient http;
    String text = "";

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url

    http.begin("https://free-api.heweather.net/s6/weather/now?location=shenzhen&key=2d63e6d9a95c4e8f8d3f65d0b5bcdf7f&lang=enl");

    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            USE_SERIAL.println(payload);

            //JSON
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            JsonObject obj = doc.as<JsonObject>();

            String cid = doc["HeWeather6"][0]["basic"]["cid"];

            Serial.println(cid);

            String location = doc["HeWeather6"][0]["basic"]["location"];

            Serial.println(location);

            text = cid + location;
        }
    }
    else
    {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return text;
}