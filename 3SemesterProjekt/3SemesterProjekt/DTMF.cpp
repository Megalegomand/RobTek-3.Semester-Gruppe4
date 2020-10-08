#include "DTMF.h"
DTMF::DTMF() {
  
};
void DTMF::playDTMF(int tonevalg) {
    
    vector<sf::Int16> dtmf;
    const double incrementL = tonesL[tonevalg%4] / 44100;
    const double incrementH= tonesH[tonevalg / 4-1] / 44100;
    double x = 0;
    double y = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf.push_back(AMPLITUDE * sin(x * TWO_PI)+AMPLITUDE*sin(y*TWO_PI));
        x += incrementL;
        y += incrementH;
    };
      
       
sf::SoundBuffer buffer;

    buffer.loadFromSamples(&dtmf[0],dtmf.size(), 2, 44100);
    sf::Sound Sound;

    Sound.setBuffer(buffer);
    Sound.setLoop(true);
    Sound.play();
    sf::sleep(sf::milliseconds(1000));
};
DTMF::~DTMF() {};
