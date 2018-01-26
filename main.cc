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
const double MUTATION_RATE = 0.003;
const double CROSSOVER_RATE = 0.7;
const int GENERATION_SIZE = 500;
const int WINDOW_SIZE = 500;
const int MAX_RADIUS = 250;

double dist(int x1, int y1, int x2, int y2){
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

std::bitset<29> generateSeed(){
    int xcoord = rand() % WINDOW_SIZE;
    int ycoord = rand() % WINDOW_SIZE;
    int radius = rand() % MAX_RADIUS;
    int seed = (xcoord << 19) + (ycoord << 9) + radius;
    auto set = std::bitset<29>(seed);
    return set;
}

sf::CircleShape getCircleFromSeed(std::bitset<29> seed){
    int circleNum = (int) seed.to_ulong();
    int radius = abs((circleNum << 23) >> 23) % MAX_RADIUS;
    int ycoord = abs((circleNum << 13) >> 22) % WINDOW_SIZE;
    int xcoord = abs(circleNum >> 19) % WINDOW_SIZE;
    sf::CircleShape circle(radius);
    circle.setPosition(xcoord, ycoord);
    circle.setFillColor(sf::Color::Blue);
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    return circle;
}

void mutateSeed(std::bitset<29>& seed){
    for (int i = 0; i < seed.size(); i++){
        double mutationRate = i < 9 ? MUTATION_RATE*3 : MUTATION_RATE;
        if (rand() < RAND_MAX * mutationRate){
            seed.flip(i);
        }
    }
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
        std::bitset<29> seedACopy = seedA;
        for (int i = index; i < 29; i++){
            seedA[i] = seedB[i];
            seedB[i] = seedACopy[i];
        }
    }
}

// Assumed that sum is the sum of the list
int getRouletteSelection(std::vector<int> list, int sum){
    int index = rand() % sum;
    for (int i = 0; i < list.size(); i++){
        index -= list[i];
        if (index <= 0){
            return i;
        }
    }
    return 0;
}

std::vector<std::bitset<29>> createNewGeneration(std::vector<std::bitset<29>> generation, std::vector<int> fitnesses){
    int sumFitness = accumulate(fitnesses.begin(), fitnesses.end(), 0);
    std::vector<std::bitset<29>> newGeneration;
    for (int i = 0; i < generation.size(); i+= 2){
        int indexA = getRouletteSelection(fitnesses, sumFitness);
        int indexB = getRouletteSelection(fitnesses, sumFitness);
        auto seedA = generation[indexA];
        auto seedB = generation[indexB];
        crossoverSeeds(seedA, seedB);
        mutateSeed(seedA);
        mutateSeed(seedB);
        newGeneration.push_back(seedA);
        newGeneration.push_back(seedB);
    }
    return newGeneration;
}

std::ostream & operator<<(std::ostream & out, std::bitset<29> set){
    return out << set.to_string();
}

int main() {
    
    // Initialize window and random func
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "SFML works!");
    srand(time(NULL));

    // Create obstacle circles
    std::vector<sf::CircleShape> circles;
    for (int i = 0; i < 15; i++){
        int radius = (rand() & 50) + 5;
        sf::CircleShape shape(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(rand() % WINDOW_SIZE, rand() % WINDOW_SIZE);
        shape.setOrigin(radius, radius);
        circles.push_back(shape);
    }

    // Generate first generation
    std::vector<std::bitset<29>> seeds;
    for (int i = 0; i < GENERATION_SIZE; i++){
        auto seed = generateSeed();
        seeds.push_back(seed);
    }
    
    // Create circles from bitsets
    std::vector<sf::CircleShape> seedCircles;
    std::transform(seeds.begin(), seeds.end(), std::back_inserter(seedCircles), getCircleFromSeed);

    auto fitnessVector = getFitness(seedCircles, circles);
 
    int generationCounter = 0;
    sf::CircleShape currentMax(0);
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Draw shapes in window
        window.clear();
        for (int i = 0; i < seedCircles.size(); i++){
            if (fitnessVector[i] > 1){
                sf::CircleShape shape = seedCircles[i];
                window.draw(shape);
            }
        }
        for (sf::CircleShape shape : circles){
            window.draw(shape);
        }
        window.draw(currentMax);
        window.display();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Create next generation and find max circle
        seeds = createNewGeneration(seeds, fitnessVector);
        seedCircles.clear();
        std::transform(seeds.begin(), seeds.end(), std::back_inserter(seedCircles), getCircleFromSeed);

        fitnessVector = getFitness(seedCircles, circles);
        auto max = std::max_element(fitnessVector.begin(), fitnessVector.end());
        if (currentMax.getRadius() < *max){
            int maxIndex = distance(fitnessVector.begin(), max);
            currentMax = seedCircles[maxIndex];
            currentMax.setFillColor(sf::Color::Red);
        }
        //std::cout << generationCounter++ << " " << *max << " " << currentMax.getRadius() << std::endl;
        std::cout << generationCounter++ << " (" << currentMax.getPosition().x << ", " << currentMax.getPosition().y << ") " << currentMax.getRadius() << std::endl;
    }
    return 0;
}
