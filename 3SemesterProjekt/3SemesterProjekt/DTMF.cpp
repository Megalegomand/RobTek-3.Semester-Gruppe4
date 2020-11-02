#include "DTMF.h"
DTMF::DTMF() {
    for (int i = 0; i < 4; i++) {
        goertzelH[i] = new Goertzel(tonesH[i], SAMPLING_RATE);
        goertzelL[i] = new Goertzel(tonesL[i], SAMPLING_RATE);
    }
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
    
    //vector<int> even;
    //vector<int> odd;
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();
    std::string inputDevice = availableDevices[0];
    sf::SoundBufferRecorder recorder;
    recorder.start();
    sleep_for(milliseconds(100));
    sleep_until(system_clock::now() + milliseconds(100));
    recorder.stop();
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    buffer.saveToFile("my_record.ogg");
    /*sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sin1633.wav")) {
        cout << "Load failed!" << endl;
        return;
    }*/
    cout << recorder.getSampleRate() << endl;
    const sf::Int16* samples = buffer.getSamples();
    std::size_t count = buffer.getSampleCount();

    for (Goertzel* g : goertzelL) {
        cout << g->processSamples(samples, count) << " ";
    }
    for (Goertzel* g : goertzelH) {
        cout << g->processSamples(samples, count) << " ";
    }
    cout << endl;

    /*
    //Ovenstående optager lyd og gemmer det 
    float coeff;
    float Q1;
    float Q2;
    float sine;
    float cosine;

    float floatN;
    float omega;
    
    /*doSomething(samples, count);*/
    /*

    floatN = (float) N;
    k = (int)(0.5 + ((TARGET_FREQ) / SAMPLING_RATE));
    omega = (2.0 * PI * k) / floatN;
    sine = sin(omega);
    cosine = cos(omega);
    coeff = 2.0 * cosine;

    printf("For SAMPLING_RATE = %f", SAMPLING_RATE);
    printf(" N = %d", N);
    printf(" and FREQUENCY = %f,\n", samples);
    printf("k = %d and coeff = %f\n\n", k, coeff);

    //ResetGoertzel();

    
    */
   /* for (int i = 0; i < count; i+=2)
    {
        even.push_back(samples[i]);
    }
    for (int i = 1; i < count; i += 2)
    {
        odd.push_back(samples[i]);
    }

   for (int k = 0; k < count / 2; k++)*//*
    {
        //Complex t = polar(1.0, -2 * PI * k / count) * odd[k];
        //samples[k] = even[k] + t;
        //samples[k + count / 2] = even[k] - t;
    }
    */
}

DTMF::~DTMF() {};
