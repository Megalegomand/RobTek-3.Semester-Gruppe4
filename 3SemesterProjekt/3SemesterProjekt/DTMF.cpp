#include "DTMF.h"
DTMF::DTMF() {
  
};
void DTMF::playDTMF(int tonevalg) {
    
    vector<sf::Int16> dtmf;
    const double incrementL = tonesL[tonevalg % 4] / 44100;
    const double incrementH= tonesH[tonevalg / 4 - 1] / 44100;
    double x = 0;
    double y = 0;
    for (unsigned i = 0; i < 50000; i++) {
        dtmf.push_back(AMPLITUDE * sin(x * PI) + AMPLITUDE * sin(y * PI));
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
}
void DTMF::receiveDTMF()
{
    vector<sf::Int16> even;
    vector<sf::Int16> odd;
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();
    std::string inputDevice = availableDevices[0];
    sf::SoundBufferRecorder recorder;
    recorder.start();
    sleep_for(seconds(1));
    sleep_until(system_clock::now() + seconds(1));
    recorder.stop();
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    buffer.saveToFile("my_record.ogg");
    const sf::Int16* samples = buffer.getSamples();
    std::size_t count = buffer.getSampleCount();
    /*doSomething(samples, count);*/

    for (int i = 0; i < count; i+=2)
    {
        even.push_back(samples[i]);
    }
    for (int i = 1; i < count; i += 2)
    {
        odd.push_back(samples[i]);
    }

   for (int k = 0; k < count / 2; k++)
    {
        //Complex t = polar(1.0, -2 * PI * k / count) * odd[k];
        //samples[k] = even[k] + t;
        //samples[k + count / 2] = even[k] - t;
    }
}
;
DTMF::~DTMF() {};
