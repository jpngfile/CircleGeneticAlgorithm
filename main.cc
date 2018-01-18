#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <bitset>
#include <iostream>
#include <math.h>

std::bitset<29> generateSeed(){
    int xcoord = rand() % 500;
    int ycoord = rand() % 500;
    int radius = rand() % 250;
    int seed = (xcoord << 19) + (ycoord << 9) + radius;
    auto set = std::bitset<29>(seed);
    std::cout << "(" << xcoord << ", " << ycoord << ") " << radius << std::endl;
    std::cout << set.to_string() << std::endl;
    std::cout << set.to_ulong() << std::endl;
    return set;
}

sf::CircleShape getCircleFromSeed(std::bitset<29> seed){
    int circleNum = (int) seed.to_ulong();
    std::cout << circleNum << std::endl;
    int radius = (circleNum << 23) >> 23;
    int ycoord = (circleNum << 13) >> 22;
    int xcoord = circleNum >> 19;
    sf::CircleShape circle(radius);
    circle.setPosition(xcoord, ycoord);
    std::cout << "(" << xcoord << ", " << ycoord << ") " << radius << std::endl;
    return circle;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
    srand(time(NULL));

    auto seed = generateSeed();
    auto circ = getCircleFromSeed(seed);

    std::vector<sf::CircleShape> circles;
    for (int i = 0; i < 15; i++){
        sf::CircleShape shape((rand() & 50) + 5);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(rand() % 500, rand() % 500);
        circles.push_back(shape);
    }

/*
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        for (sf::CircleShape shape : circles){
            window.draw(shape);
        }
        window.display();
    }
*/
    return 0;
}
