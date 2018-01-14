#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
    srand(time(NULL));

    std::vector<sf::CircleShape> circles;
    for (int i = 0; i < 15; i++){
        sf::CircleShape shape(10.f);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(rand() % 500, rand() % 500);
        circles.push_back(shape);
    }
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

    return 0;
}
