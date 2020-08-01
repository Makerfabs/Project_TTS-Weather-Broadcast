#include "MySpeech.h"
#include "mp3_decoder/mp3_decoder.h"
#include "aac_decoder/aac_decoder.h"

bool Speech::connecttospeech(String speech, String lang){

    Serial.println("MYYYYYYYYY SPEECHHHHHHHHHHH");

    Audio::reset();
    //String host="translate.google.com";
    String host="translate.google.cn";
    String path="/translate_tts";
    uint16_t free=0;

    String resp=   String("GET / HTTP/1.0\r\n") +
                   String("Host: ") + host + String("\r\n") +
                   String("User-Agent: GoogleTTS for ESP32/1.0.0\r\n") +
                   String("Accept-Encoding: identity\r\n") +
                   String("Accept: text/html\r\n\r\n");

    Serial.println(resp);

    if (!clientsecure.connect(host.c_str(), 443)) {
        Serial.println("Connection failed");
        return false;
    }
    clientsecure.print(resp);
    sprintf(chbuf, "SSL has been established, free Heap: %u bytes", ESP.getFreeHeap());
    if(audio_info) audio_info(chbuf);
    while (clientsecure.connected()) {  // read the header
        String line = clientsecure.readStringUntil('\n');
        line+="\n";
        if (line == "\r\n") break;
    }

    String tkkFunc="";
    char ch;

    do {  // search for TKK
        tkkFunc = "";
        clientsecure.readBytes(&ch, 1);
        if (ch != 't') continue;
        tkkFunc += String(ch);
        clientsecure.readBytes(&ch, 1);
        if (ch != 'k') continue;
        tkkFunc += String(ch);
        clientsecure.readBytes(&ch, 1);
        if (ch != 'k') continue;
        tkkFunc += String(ch);
        clientsecure.readBytes(&ch, 1);
        if (ch != ':') continue;
        tkkFunc += String(ch);
    } while(tkkFunc.length() < 4);

    tkkFunc +=  clientsecure.readStringUntil(',');  // "TKK='xxxxxxxxx.yyyyyyyyy'"
    tkkFunc = tkkFunc.substring(5 /* length of "TKK='" */, tkkFunc.lastIndexOf('\''));

//    log_i("tkkFunc_%s", tkkFunc.c_str());

    // create token
    int16_t periodPos = tkkFunc.indexOf('.');
    String key1 = tkkFunc.substring(0,periodPos);
    String key2 = tkkFunc.substring(periodPos + 1);
    int64_t a, b;
    a = b = strtoll(key1.c_str(), NULL, 10);

    int f;
    int len = strlen(speech.c_str());
    for (f = 0; f < len; f++) {
      a += speech[f];
      a = XL(a, "+-a^+6");
    }
    a = XL(a, "+-3^+b+-f");
    a = a ^ (strtoll(key2.c_str(), NULL, 10));
    if (0 > a) {
      a = (a & 2147483647) + 2147483648;
    }
    a = a % 1000000;
    String token=String(lltoa(a, 10)) + '.' + lltoa(a ^ b, 10);

    int i,j;
    const char* t = speech.c_str();
    for(i=0,j=0;i<strlen(t);i++) {
      if (t[i] < 0x80 || t[i] > 0xbf) {
        j++;
      }
    }
//    log_i("Token=%s", token.c_str());
    String tts= String("https://") + host + path +
                        "?ie=UTF-8&q=" + urlencode(speech) +
                        "&tl=" + lang +
                        "&sl=" + lang +
                        "&textlen=" + String(j) +
                        "&tk=" + token +
                        "&total=1&idx=0&client=t&prev=input&ttsspeed=1";

    Serial.println(tts);

    clientsecure.stop();  clientsecure.flush();

    resp=   String("GET ") + tts + String("HTTP/1.1\r\n") +
            String("Host: ") + host + String("\r\n") +
            String("Connection: close\r\n\r\n");

    if (!clientsecure.connect(host.c_str(), 443)) {
        Serial.println("Connection failed");
        return false;
    }
    clientsecure.print(resp);

    Serial.println(resp);

    while (clientsecure.connected()) {
        String line = clientsecure.readStringUntil('\n');
        line+="\n";
//      if(audio_info) audio_info(line.c_str());
        if (line == "\r\n") break;
    }
    m_inBuffwindex=0;
    m_codec = CODEC_MP3;
    AACDecoder_FreeBuffers();
    MP3Decoder_AllocateBuffers();
    sprintf(chbuf, "MP3Decoder has been initialized, free Heap: %u bytes", ESP.getFreeHeap());
    if(audio_info) audio_info(chbuf);
    uint16_t res;
    uint16_t x=0;
    while(!playI2Sremains()){;}
    while(clientsecure.available()==0){;}
    while(clientsecure.available() > 0) {
            free=m_inBuffsize-m_inBuffwindex;// free space
            res=clientsecure.read(m_inBuff + m_inBuffwindex, free);
            if(res>0){
                m_inBuffwindex+=res;
                free-=res;
            }
            while(m_inBuffwindex>=1600){
                x=sendBytes(m_inBuff, m_inBuffsize);
                m_inBuffwindex-=x;
                memmove(m_inBuff, m_inBuff + x , m_inBuffsize-x);
           }
    }
    sendBytes(m_inBuff, m_inBuffsize -free);
    memset(m_outBuff, 0, sizeof(m_outBuff));
    for(int i=0; i<4; i++){
        m_validSamples = 2048;
        while(m_validSamples) {
            playChunk();
        }
    }
    while(!playI2Sremains()){;}
    MP3Decoder_FreeBuffers();
    stopSong();
    clientsecure.stop();  clientsecure.flush();
    m_codec = CODEC_NONE;
    if(audio_eof_speech) audio_eof_speech(speech.c_str());
    return true;
}