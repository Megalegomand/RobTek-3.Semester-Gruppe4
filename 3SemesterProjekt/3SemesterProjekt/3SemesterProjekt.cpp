#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
#include "Goertzel.h"
//#include"DTMF.h"

using namespace std;
int main()
{
    Goertzel g = Goertzel(1633, 8000);
    cout << g.processSamples(NULL, 205) << endl;
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
    //DTMF s;
    //s.receiveDTMF();
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