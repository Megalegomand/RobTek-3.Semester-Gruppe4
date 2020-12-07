#include "DTMF.h"
DTMF::DTMF() {
    for (int i = 0; i < 4; i++) {
        goertzelH[i] = new Goertzel(TONES_H[i], SAMPLE_RATE);
        goertzelL[i] = new Goertzel(TONES_L[i], SAMPLE_RATE);
    }

    prepareTones();

    this->setDevice(getDefaultDevice());
    cout << getDefaultDevice() << endl;

    goertzel = new Goertzel(SAMPLE_RATE);

    goertzelresH.resize(4);
    goertzelresL.resize(4);
}


void DTMF::prepareTones()
{
    for (int tonei = 0; tonei < 16; tonei++) {
        preparedTones[tonei] = new vector<sf::Int16>();

        const double incrementL = ((double)TONES_L[tonei / 4]) / SAMPLE_RATE;
        const double incrementH = ((double)TONES_H[tonei % 4]) / SAMPLE_RATE;

        double x = 0;
        double y = 0;
        
        int TONE_SAMPLES = ((SAMPLE_RATE * TONE_DURATION) / 1000);

        for (unsigned int i = 0; i < TONE_SAMPLES; i++) {
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

    buffer.loadFromSamples(&samples[0], samples.size(), 1, SAMPLE_RATE);
    
    sound.setBuffer(buffer);
    sound.play();
    this_thread::sleep_for(chrono::milliseconds(buffer.getDuration().asMilliseconds()));
}

vector<char> DTMF::listenSequence(int duration)
{
    this->start(SAMPLE_RATE); // Start recording
    Timer startTime = Timer();
    startTime.start();

    inputSamples = queue<Int16>();

    deque<Int16> currentTone = deque<Int16>();

    while (startTime.elapsedMillis() < duration) {
        while (inputSamples.size() < TONE_SAMPLES) {};
        inputSamples_mutex.lock();
        for (int i = 0; i < TONE_SAMPLES; i++) {
            currentTone.push_back(inputSamples.front());
            inputSamples.pop();
        }
        inputSamples_mutex.unlock();

        char tone = -1;
        while (tone == -1) {
            cout << "----------" << endl;
            Timer t = Timer();
            t.start();
            //cout << "DTone " << int(determineDTMF(currentTone, 0, TONE_SAMPLES / 2)) << " : " << int(determineDTMF(currentTone, TONE_SAMPLES / 2, TONE_SAMPLES)) << endl;
            tone = determineDTMF(currentTone, 0, TONE_SAMPLES);
            cout << "det" << t.elapsedMillis() << endl;
            cout << "Tone " <<int(tone) << endl;
            
            
            sampleMove_mutex.lock();
            int sampleMoveC = sampleMove;
            sampleMove_mutex.unlock();
            inputSamples_mutex.lock();
            if (sampleMoveC > inputSamples.size()) {
                sampleMoveC = inputSamples.size();
            }
            for (int i = 0; i < sampleMoveC; i++) { // Make sure there is still samples available
                currentTone.push_back(inputSamples.front());
                inputSamples.pop();
                currentTone.pop_front();
            }
            inputSamples_mutex.unlock();

            processCounter_mutex.lock();
            processCounter++;
            processCounter_mutex.unlock();
            cout << "Time" << t.elapsedMillis() << endl;
        }
        cout << "Tone: " << int(tone) << endl;
        
        //this->stop();
        //return vector<char>();
    }
    this_thread::sleep_for(chrono::milliseconds(10000));
    this->stop();
    return vector<char>();
}

char DTMF::listenTone(int duration)
{
    char t = receiveDTMF(duration);
    /*cout << "R " << int(t) << endl;*/
    return t;
    
}

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

char DTMF::determineDTMF(deque<Int16> samples, int start, int end)
{
    cout << "----------------------------------- Timer" << endl;
    Timer t = Timer();
    t.start();

    for (int i = 0; i < 4; i++) {
        goertzelresH[i] = goertzel->processSamples(samples, start, end, TONES_H[i]);
        goertzelresL[i] = goertzel->processSamples(samples, start, end, TONES_L[i]);
    }
    cout << "t" << t.elapsedMillis() << endl;

    int pos1 = 0, pos2 = 0;
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
    cout << "t" << t.elapsedMillis() << endl;

    float sum = 0.0;
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
    cout << "t" << t.elapsedMillis() << endl;

    float SNR = P_Signal / P_stoej;

    if (SNR > DBthreshhold)
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

void DTMF::prepareTones(int duration)
{
    for (int tonei = 0; tonei < 16; tonei++) {
        preparedTones[tonei] = new vector<sf::Int16>();

        const double incrementL = ((double)TONES_L[tonei / 4]) / SAMPLE_RATE;
        const double incrementH = ((double)TONES_H[tonei % 4]) / SAMPLE_RATE;

        double x = 0;
        double y = 0;

        int TONE_SAMPLES = ((SAMPLE_RATE * duration) / 1000);

        for (unsigned int i = 0; i < TONE_SAMPLES; i++) {
            preparedTones[tonei]->push_back(AMPLITUDE * sin(x * 2 * PI) + AMPLITUDE * sin(y * 2 * PI));
            x += incrementL;
            y += incrementH;
        };
    }
}




DTMF::~DTMF() {
    stop();
}


bool DTMF::onProcessSamples(const Int16* samples, std::size_t sampleCount)
{
    inputSamples_mutex.lock();
    for (int i = 0; i < sampleCount; i++) {
        inputSamples.push(samples[i]);
    }
    inputSamples_mutex.unlock();

    processCounter_mutex.lock();
    sampleMove_mutex.lock();
    sampleMove = inputSamples.size() / processCounter;
    processCounter = 1;
    processCounter_mutex.unlock();

    if (sampleMove > TONE_SAMPLES / 4) {
        cout << "SMBefore" << sampleMove << endl;
        cout << TONE_SAMPLES << ":" << TONE_SAMPLES / 4 << endl;
        sampleMove = TONE_SAMPLES / 4;
    }
    sampleMove_mutex.unlock();

    return true;
}
;
