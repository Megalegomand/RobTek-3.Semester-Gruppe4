#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
#include "Goertzel.h"
#include"DTMF.h"
#include"FrameHandler.h"
#include<thread>
#include<fstream>
#include <functional>
#include "TicTacToe.h"

void data(vector<char> data) {
    cout << "Data: ";
    for (char c : data) {
        cout << int(c) << ":";
    }
    cout << endl;
}

void tokenpass() {
    cout << "TOKEN" << endl;
}

void closed1() {
    cout << "CLOSED1" << endl;
}

void closed2() {
    cout << "CLOSED2" << endl;
}

using namespace std;
using namespace std::placeholders;
int main()
{
    FrameHandler* f = new FrameHandler(std::bind(data, _1), std::bind(tokenpass), std::bind(closed1));
    if (f->bind(10)) {
        vector<char> data = vector<char>();
        for (char i = 0; i < 15; i++) {
            data.push_back(i);
        }
        
        cout << f->sendData(data) << endl;
    }
    this_thread::sleep_for(chrono::milliseconds(100000));

    //TicTacToe ttt = TicTacToe();
    //ttt.game();

    //FrameHandler* f1 = new FrameHandler(std::bind(data, _1), std::bind(tokenpass), std::bind(closed1));
    //FrameHandler* f2 = new FrameHandler(std::bind(data, _1), std::bind(tokenpass), std::bind(closed2));

    //vector<char> data = vector<char>();
    //for (char i = 0; i < 15; i++) {
    //    data.push_back(i);
    //}

    //thread t1(&FrameHandler::bind, f1, 10);
    //this_thread::sleep_for(chrono::milliseconds(1500));
    //thread t2(&FrameHandler::bind, f2, 10);

    //cout << "B" << f1->bind(10) << endl;
    //this_thread::sleep_for(chrono::milliseconds(100000));

    //cout << "d" << int(f1->bind(10)) << endl;

    //t1.join();
    //t2.join();

    return 0;

    //cout << "Passing " << dl2->passToken() << endl;
    //this_thread::sleep_for(chrono::milliseconds(1000));
    
    //cout << "Passing " << dl1->passToken() << endl;

    //this_thread::sleep_for(chrono::milliseconds(100000));

    //dl1Thread.join();

    //vector<char> data;
    //data.push_back(0);
    //data.push_back(4);
    //std::thread dl3Thread(&DataLink::waitData, &dl1, 1000000);
    //dl2.sendData(data);
    

    //mediumReaderThread.join();
    //dl3Thread.join();



    //Goertzel g = Goertzel(1633, 8000);
    //cout << g.processSamples(NULL, 205) << endl;
    /*sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();

        std::vector<sf::Int16> samples;
        const unsigned AMPLITUDE = 30000;
        const double TWO_PI = 6.28318;
        const double increment = 440. / 44100;
        double x = 0;
        for (unsigned i = 0; i < 50000; i++) {
            samples.push_back(AMPLITUDE * sin(x * TWO_PI));
            x += increment;
        }

        sf::SoundBuffer buffer;
        
        buffer.loadFromSamples(&samples[0], samples.size(), 2, 44100);
        sf::Sound Sound;

        Sound.setBuffer(buffer);
        Sound.setLoop(true);
        Sound.play();
        while (1) {
            sf::sleep(sf::milliseconds(100));
        }
    

    }*/
    
    DTMF s;
    
    for (int i = 0; i < 100; i++)
    {
        //s.listenTone(100);
     }
          
    /////*s.sendSequence(data);*/
    /*while (true)
    {
        cout << int(s.listenTone(100)) << endl;
    }*/
    
    
        

    
    
    /*for (int i = 0; i < 16; i++) {
        s.playDTMF(i);
    }*/
    // Kode til list af input devices !!!!!!!!//
    //std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();

    //for (unsigned int i = 0; i < availableDevices.size(); i++)
    //{
    //    cout << availableDevices[i] << endl;
    //}

    return 0;
}