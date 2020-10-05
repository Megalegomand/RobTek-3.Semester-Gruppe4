#include "DTMF.h"
DTMF::DTMF() {
   //Oprettelse af lave toner//
    vector<sf::Int16> dtmf697;
    const double increment0 = 697. / 44100;
    double x0 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf697.push_back(AMPLITUDE * sin(x0 * TWO_PI));
        x0 += increment0;
    };
    tonesL.push_back(dtmf697);
      
    
    vector<sf::Int16> dtmf770;
    const double increment1 = 770. / 44100;
    double x1 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf770.push_back(AMPLITUDE * sin(x1 * TWO_PI));
        x1 += increment1;
    };
    tonesL.push_back(dtmf697);


    vector<sf::Int16> dtmf852;
    const double increment2 = 852. / 44100;
    double x2 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf852.push_back(AMPLITUDE * sin(x2 * TWO_PI));
        x2 += increment2;
    };
    tonesL.push_back(dtmf852);

    vector<sf::Int16> dtmf941;
    const double increment3 = 941. / 44100;
    double x3 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf941.push_back(AMPLITUDE * sin(x3 * TWO_PI));
        x3 += increment3;
    };
    tonesL.push_back(dtmf941);

    //Oprettelse af høje toner//
    vector<sf::Int16> dtmf1209;
    const double increment4 = 1209. / 44100;
    double x4 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf941.push_back(AMPLITUDE * sin(x4 * TWO_PI));
        x4 += increment4;
    };
    tonesH.push_back(dtmf1209);


    vector<sf::Int16> dtmf1336;
    const double increment5 = 1336. / 44100;
    double x5 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf941.push_back(AMPLITUDE * sin(x5 * TWO_PI));
        x5 += increment5;
    };
    tonesH.push_back(dtmf1336);


    vector<sf::Int16> dtmf1477;
    const double increment6 = 1477. / 44100;
    double x6 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf941.push_back(AMPLITUDE * sin(x6 * TWO_PI));
        x6 += increment6;
    };
    tonesH.push_back(dtmf1477);


    vector<sf::Int16> dtmf1633;
    const double increment7 = 941. / 44100;
    double x7 = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf941.push_back(AMPLITUDE * sin(x7 * TWO_PI));
        x7 += increment7;
    };
    tonesH.push_back(dtmf1633);


};
void DTMF::playDTMF(int tonevalg) {
    sf::SoundBuffer buffer;

    buffer.loadFromSamples(&tonesH[tonevalg][0], tonesH[0].size(), 2, 44100);
    sf::Sound Sound;

    Sound.setBuffer(buffer);
    Sound.setLoop(true);
    Sound.play();

};
DTMF::~DTMF() {};
