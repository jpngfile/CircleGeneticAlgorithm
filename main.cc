#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <bitset>
#include <iostream>
#include <math.h>

#include <chrono>
#include <thread>

/*
 * Seeds are 29 bits long
 */

// Constants
const double MUTATION_RATE = 0.001;
const double CROSSOVER_RATE = 0.7;

//const double MUTATION_RATE = 1;
//const double CROSSOVER_RATE = 1;
//
double dist(int x1, int y1, int x2, int y2){
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

std::bitset<29> generateSeed(){
    int xcoord = rand() % 400;
    int ycoord = rand() % 400;
    int radius = rand() % 100;
    int seed = (xcoord << 19) + (ycoord << 9) + radius;
    auto set = std::bitset<29>(seed);
    //std::cout << "(" << xcoord << ", " << ycoord << ") " << radius << std::endl;
    //std::cout << set.to_string() << std::endl;
    //std::cout << set.to_ulong() << std::endl;
    return set;
}

sf::CircleShape getCircleFromSeed(std::bitset<29> seed){
    int circleNum = (int) seed.to_ulong();
    //std::cout << circleNum << std::endl;
    int radius = (circleNum << 23) >> 23;
    int ycoord = (circleNum << 13) >> 22;
    int xcoord = circleNum >> 19;
    sf::CircleShape circle(radius);
    circle.setPosition(xcoord, ycoord);
    //std::cout << "(" << xcoord << ", " << ycoord << ") " << radius << std::endl;
    return circle;
}

void mutateSeed(std::bitset<29>& seed){
    //std::cout << "start mutation" << std::endl;
    for (int i = 0; i < seed.size(); i++){
        if (rand() < RAND_MAX * MUTATION_RATE){
            seed.flip(i);
        }
    }
    //std::cout << "end mutation" << std::endl;
}

std::vector<int> getFitness(std::vector<sf::CircleShape> seeds, std::vector<sf::CircleShape> circles){
    std::vector<int> fitnesses;
    for (auto seed : seeds){
        auto seedPos = seed.getPosition();
        for (auto circle : circles){
            auto circlePos = circle.getPosition();
            if (dist(seedPos.x, seedPos.y, circlePos.x, circlePos.y) < seed.getRadius() + circle.getRadius()){
                fitnesses.push_back(1);
                goto nextSeed;
            }
        }
        fitnesses.push_back(seed.getRadius());
nextSeed:;
    }
    return fitnesses;
}

void crossoverSeeds(std::bitset<29>& seedA, std::bitset<29>& seedB){
    if (rand() < RAND_MAX * CROSSOVER_RATE){
        int index = rand() % 29;
        std::cout << index << std::endl;
        std::bitset<29> seedACopy = seedA;
        //std::cout << "copy: " << seedACopy << std::endl;
        for (int i = index; i < 29; i++){
            seedA[i] = seedB[i];
            seedB[i] = seedACopy[i];
        }
        //std::cout << "copy: " << seedACopy << std::endl;
    }
}

std::ostream & operator<<(std::ostream & out, std::bitset<29> set){
    return out << set.to_string();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
    srand(time(NULL));

    std::vector<sf::CircleShape> circles;
    for (int i = 0; i < 10; i++){
        int radius = (rand() & 50) + 5;
        sf::CircleShape shape(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(rand() % 400, rand() % 400);
        //shape.setPosition(0, 0);
        shape.setOrigin(radius, radius);
        circles.push_back(shape);
    }

    std::vector<std::bitset<29>> seeds;
    for (int i = 0; i < 10; i++){
        auto seed = generateSeed();
        seeds.push_back(seed);
        //auto circle = getCircleFromSeed(seed);
        //circle.setFillColor(sf::Color::Blue);
        //circles.push_back(circle);
    }
    
    std::vector<sf::CircleShape> seedCircles;
    std::transform(seeds.begin(), seeds.end(), std::back_inserter(seedCircles), getCircleFromSeed);

    for (auto &circle : seedCircles){
        circle.setFillColor(sf::Color::Blue);
        circle.setOrigin(circle.getRadius(), circle.getRadius());
    }

    auto fitnessVector = getFitness(seedCircles, circles);
    for (int fit : fitnessVector){
        std::cout << fit << " ";
    }
    std::cout << std::endl;

    //std::cout << "Seeds: " << std::endl;
    //std::vector<std::bitset<29>> seeds;
    //for (int i = 0; i < 2; i++){
    //    auto seed = generateSeed();
    //    std::cout << seed << std::endl;
    //    seeds.push_back(seed);
    //}
    //std::cout << "Mutations: " << std::endl;
    //for (auto & seed : seeds){
    //    mutateSeed(seed);
    //    std::cout << seed << std::endl;
    //}
    //std::cout << "Crossover: " << std::endl;
    //crossoverSeeds(seeds[0], seeds[1]);
    //for (auto seed : seeds){
    //    std::cout << seed << std::endl;
    //}

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        for (auto shape : seedCircles){
            window.draw(shape);
        }
        for (sf::CircleShape shape : circles){
            window.draw(shape);
        }
        window.display();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}
