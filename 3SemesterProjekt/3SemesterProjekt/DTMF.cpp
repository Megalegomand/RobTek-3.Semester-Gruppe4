#include "DTMF.h"
DTMF::DTMF() {
    for (int i = 0; i < 4; i++) {
        goertzelH[i] = new Goertzel(tonesH[i], SAMPLING_RATE);
        goertzelL[i] = new Goertzel(tonesL[i], SAMPLING_RATE);
    }
}
void DTMF::sendSequence(vector<char>& sequence, int duration)
{
    for (char tone : sequence) {
       sendTone(tone, duration);
    }
}
char DTMF::listenTone(int duration)
{
    char t = receiveDTMF(duration);
    cout << "R " << int(t) << endl;
    return t;
    
}
void DTMF::sendTone(char tonevalg, int duration) {
    
    vector<sf::Int16> dtmf;
    const double incrementL = ((double) tonesL[tonevalg % 4]) / SAMPLING_RATE;
    const double incrementH= ((double) tonesH[tonevalg / 4]) / SAMPLING_RATE;
    double x = 0;
    double y = 0;
    int antalSamples = ((SAMPLING_RATE * duration) / 1000);
    for (unsigned int i = 0; i < antalSamples; i++) {
        dtmf.push_back(AMPLITUDE * sin(x * 2 * PI) + AMPLITUDE * sin(y * 2 * PI));
        x += incrementL;
        y += incrementH;
    };
       
    sf::SoundBuffer buffer;

    buffer.loadFromSamples(&dtmf[0],dtmf.size(), 1, SAMPLING_RATE);
    sf::Sound Sound;

    Sound.setBuffer(buffer);
    //Sound.setLoop(true);
    Sound.play();
    this_thread::sleep_for(chrono::milliseconds(duration));
}
char DTMF::receiveDTMF(int duration)
{
    
    vector<float> goertzelresL;
    vector<float> goertzelresH;
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();
    std::string inputDevice = availableDevices[0];
    sf::SoundBufferRecorder recorder;
    recorder.start();
    this_thread::sleep_for(chrono::milliseconds(duration));
    recorder.stop();
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    //buffer.saveToFile("my_record.ogg");
    /*sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sin1633.wav")) {
        cout << "Load failed!" << endl;
        return;
    }*/
    const sf::Int16* samples = buffer.getSamples();
    std::size_t count = buffer.getSampleCount();

    for (Goertzel* g : goertzelL) {
        /*cout << g->processSamples(samples, count) << " ";*/
        goertzelresL.push_back(g->processSamples(samples, count));
    }
    for (Goertzel* g : goertzelH) {
       /* cout << g->processSamples(samples, count) << " ";*/
        goertzelresH.push_back(g->processSamples(samples, count));
    }
    //cout << endl;

    return determineDTMF(goertzelresL, goertzelresH);
    
}
 
char DTMF::determineDTMF(vector<float> goertzelresL, vector<float> goertzelresH)
{
     int pos1=0, pos2=0;
     float  largest = 0, second_largest = 0;
     float stoej[6] = {};
     int stoeji=0;
     int res = 0;
    //finding Largest second largset
    for (int i = 0; i < goertzelresH.size(); i++)
    {
        if (goertzelresL[i] > largest)
        {
            largest = goertzelresL[i];
            pos1 = i;
        }
        else
        {
            stoej[stoeji] = goertzelresL[i];
            stoeji++;
        }
        if (goertzelresH[i] > second_largest)
        {
            second_largest = goertzelresH[i];
            pos2 = i;

         }
        {
            stoej[stoeji];
            stoeji++;
        }
    }
    float P_Signal = (largest + second_largest) / 2;
    float sum=0.0;
    for (int i = 0; i < 6; i++)
    {
        sum += stoej[i];
    }
    float P_stoej = sum / 6.0;

    float SNR = 10 * log10(P_Signal/P_stoej);

    if (SNR>DBthreshhold)
    {
        
        return pos1*4 + pos2;
    }
    else
    {
        return -1;
    }
    /*cout << "nn Largest Number :" << largest << " at position " << (pos1 + 1);
    cout << "nn Second Largest Number :" << second_largest << " at position " << (pos2 + 4) << endl;*/
    
}




DTMF::~DTMF() {};
