#define private public
#include "Audio.h"

class Speech : public Audio
{
    public:
    bool connecttospeech(String speech, String lang);
};