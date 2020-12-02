#include "DTMF.h"
DTMF::DTMF() {
    for (int i = 0; i < 4; i++) {
        goertzelH[i] = new Goertzel(tonesH[i], SAMPLING_RATE);
        goertzelL[i] = new Goertzel(tonesL[i], SAMPLING_RATE);
    }
}

DTMF::DTMF(int duration) : DTMF()
{
    prepareTones(duration);
}

void DTMF::prepareTones(int duration)
{
    for (int tonei = 0; tonei < 16; tonei++) {
        preparedTones[tonei] = new vector<sf::Int16>();

        const double incrementL = ((double)tonesL[tonei / 4]) / SAMPLING_RATE;
        const double incrementH = ((double)tonesH[tonei % 4]) / SAMPLING_RATE;

        double x = 0;
        double y = 0;
        
        int antalSamples = ((SAMPLING_RATE * duration) / 1000);

        for (unsigned int i = 0; i < antalSamples; i++) {
            preparedTones[tonei]->push_back(AMPLITUDE * sin(x * 2 * PI) + AMPLITUDE * sin(y * 2 * PI));
            x += incrementL;
            y += incrementH;
        };
    }
}

void DTMF::sendSequence(vector<char>& sequence)
{
    vector<sf::Int16> samples = vector<sf::Int16>();
    for (char tone : sequence) {
        samples.insert(samples.end(), preparedTones[tone]->begin(), preparedTones[tone]->end());
    }

    sf::SoundBuffer buffer;

    buffer.loadFromSamples(&samples[0], samples.size(), 1, SAMPLING_RATE);
    
    sound.setBuffer(buffer);
    sound.play();
    this_thread::sleep_for(chrono::milliseconds(buffer.getDuration().asMilliseconds()));
}

char DTMF::listenTone(int duration)
{
    char t = receiveDTMF(duration);
    /*cout << "R " << int(t) << endl;*/
    return t;
    
}

//void DTMF::sendTone(char tonevalg, int duration) {
//    
//    vector<sf::Int16> dtmf;
//    const double incrementL = ((double) tonesL[tonevalg / 4]) / SAMPLING_RATE;
//    const double incrementH= ((double) tonesH[tonevalg % 4]) / SAMPLING_RATE;
//    double x = 0;
//    double y = 0;
//    int antalSamples = ((SAMPLING_RATE * duration) / 1000);
//    for (unsigned int i = 0; i < antalSamples; i++) {
//        dtmf.push_back(AMPLITUDE * sin(x * 2 * PI) + AMPLITUDE * sin(y * 2 * PI));
//        x += incrementL;
//        y += incrementH;
//    };
//   
//    buffer.loadFromSamples(&dtmf[0], dtmf.size(), 1, SAMPLING_RATE);
//    cout << "Time" << buffer.getDuration().asMilliseconds() << endl;
//    sound.setBuffer(buffer);
//   /* Sound.setLoop(true);*/
//    sound.play();
//    cout << sound.getStatus() << endl;
//    this_thread::sleep_for(chrono::milliseconds(duration));
//    cout << sound.getStatus() << endl;
//}

char DTMF::receiveDTMF(int duration)
{
    
    vector<float> goertzelresL;
    vector<float> goertzelresH;
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    sf::SoundBufferRecorder recorder;
    recorder.start();
    this_thread::sleep_for(chrono::milliseconds(duration));
    recorder.stop();
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    //cout << "Kage" << buffer.getSampleCount() << endl;
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
     int res = 0;
    //finding Largest second largset
    for (int i = 0; i < goertzelresH.size(); i++)
    {
        if (goertzelresL[i] > largest)
        {
            largest = goertzelresL[i];
            pos1 = i;
        }
        if (goertzelresH[i] > second_largest)
        {
            second_largest = goertzelresH[i];
            pos2 = i;

         }
    }
    float P_Signal = (largest + second_largest) / 2;

    float sum=0.0;
    for (int i = 0; i < goertzelresH.size(); i++)
    {
        if (i != pos1) {
            sum += goertzelresL[i];
        }
        if (i != pos2) {
            sum += goertzelresH[i];
        }
    }
    float P_stoej = sum / 6.0;

    float SNR = P_Signal/P_stoej;

    if (SNR>DBthreshhold)
    {
        cout << SNR << endl;
        return  pos1 * 4 + pos2;
    }
    else
    {
        cout << SNR << endl;
        return -1;
    }
}




DTMF::~DTMF() {};
