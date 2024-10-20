#include <iostream>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

// Constants
const int P = 100;
const int G = 14;
const double R[] = { 5, 4, 3, 2 };
const double C[] = { 500, 400, 300, 200 };
const double XP = 0.9;
const double MP = 0.3;
const int MAX_GEN = 30;
const double MAX_R = 5;
const double MAX_C = 500;
const int fairness = 4;
const int ELITE_COUNT = 2; // Number of elite individuals to preserve

// Variables
int c[P][G];
double fitness[P];
int parents[2][G];
int child[2][G];
int newc[P][G];
double new_fitness[P];
int countc;
ofstream af, bf, bc;
float avgf = 0, bestf = -1;
int bestc[G];

void initializationPopulation() {
    srand(time(NULL));
    for (int p = 0; p < P; p++) {
        for (int g = 0; g < G; g++) {
            c[p][g] = (rand() % 4) + 1;
        }
    }
}

void printChromosome() {
    for (int p = 0; p < P; p++) {
        cout << "C" << p << ": ";
        for (int g = 0; g < G; g++) {
            cout << c[p][g] << " ";
        }
        cout << endl;
    }
}

void evaluateChromosome() {
    int totalR, totalC;
    double avgR, avgC;
    int prev, count;
    int nurse1 = 0, nurse2 = 0, nurse3 = 0, nurse4 = 0;
    for (int p = 0; p < P; p++) {
        totalR = 0;
        totalC = 0;
        avgR = 0.00;
        avgC = 0.00;
        prev = 0;
        count = 0;
        nurse1 = 0;
        nurse2 = 0;
        nurse3 = 0;
        nurse4 = 0;
        for (int g = 0; g < G; g++) {
            if (c[p][g] == prev) {
                count++;
            }
            if (c[p][g] == 1) {
                nurse1++;
            }
            else if (c[p][g] == 2) {
                nurse2++;
            }
            else if (c[p][g] == 3) {
                nurse3++;
            }
            else {
                nurse4++;
            }
            int index = c[p][g] - 1;
            totalR += R[index];
            totalC += C[index];
            prev = c[p][g];
        }
        avgR = totalR / (double)G;
        avgC = totalC / (double)G;

        if (count == 0 && nurse1 < fairness && nurse2 < fairness && nurse3 < fairness && nurse4 < fairness) {
            fitness[p] = (avgR / MAX_R) + (1 - (avgC / MAX_C));
        }
        else if (count == 0 && (nurse1 > fairness || nurse2 > fairness || nurse3 > fairness || nurse4 > fairness)) {
            fitness[p] = ((avgR / MAX_R) + (1 - (avgC / MAX_C))) * 0.7;
        }
        else if (count > 0 && nurse1 < fairness && nurse2 < fairness && nurse3 < fairness && nurse4 < fairness) {
            fitness[p] = ((avgR / MAX_R) + (1 - (avgC / MAX_C))) * 0.5;
        }
        else {
            fitness[p] = (((avgR / MAX_R) + (1 - (avgC / MAX_C))) * 0.7) * 0.5;
        }

        cout << "C" << p << ": ";
        cout << "Consecutive Shift: " << count << " ";
        cout << "Average Rating: " << avgR << "\t";
        cout << "Average Cost: " << avgC << " ";
        cout << "Fitness: " << fitness[p] << endl;
    }
}

void parentSelection() {
    // Declare necessary variables
    int indexparents[2], player1, player2;
    // For both parents
    srand(time(NULL));
    for (int i = 0; i < 2; i++) {
        // Pick a random number to be the index for player 1
        player1 = rand() % P;
        // Pick another random number to be the index for player 2
        player2 = rand() % P;

        while (player1 == player2) {
            player2 = rand() % P;
        }

        cout << "Player 1 is " << player1 << "\tFitness Player 1 is " << fitness[player1] << endl;
        cout << "Player 2 is " << player2 << "\tFitness Player 2 is " << fitness[player2] << endl;

        // Compare fitness between player 1 and 2, pick the best one to be parent
        if (fitness[player1] > fitness[player2]) {
            indexparents[i] = player1;
        }
        else {
            indexparents[i] = player2;
        }

        cout << "Winner is " << indexparents[i] << endl;
    }

    // Copy selected parents to array parents
    for (int p = 0; p < 2; p++) {
        for (int g = 0; g < G; g++) {
            parents[p][g] = c[indexparents[p]][g];
        }
    }

    // Print parent 1 and 2
    for (int p = 0; p < 2; p++) {
        cout << "Parent " << p + 1 << ": ";
        for (int g = 0; g < G; g++) {
            cout << parents[p][g] << " ";
        }
        cout << endl;
    }
}

void crossover() {
    double randnum;
    int xpoint;
    srand(time(NULL));
    // Copy both parent’s chromosome to children chromosomes
    for (int p = 0; p < 2; p++) {
        for (int g = 0; g < G; g++) {
            child[p][g] = parents[p][g];
        }
    }
    // Generate a random number from 0-1
    randnum = (rand() % 11) / 10.0;
    // If randnum less than crossover probability
    if (randnum < XP) {
        xpoint = rand() % G;
        cout << "Crossover happened at index " << xpoint << endl;

        for (int i = xpoint; i < G; i++) {
            child[0][i] = parents[1][i];
            child[1][i] = parents[0][i];
        }
    }
    else {
        cout << "Crossover did not happen" << endl;
    }

    for (int p = 0; p < 2; p++) {
        cout << "Child " << p + 1 << ": ";
        for (int g = 0; g < G; g++) {
            cout << child[p][g] << " ";
        }
        cout << endl;
    }
}

void mutation() {
    double randnum;
    srand(time(NULL));
    for (int p = 0; p < 2; p++) {
        randnum = (rand() % 11) / 10.0;
        if (randnum < MP) {
            int start = rand() % G;
            int end = start + rand() % (G - start);
            cout << "Scramble Mutation for Child " << p + 1 << " from index " << start << " to " << end << endl;
            random_shuffle(child[p] + start, child[p] + end);
        }
        else {
            cout << "Mutation did not happen for Child " << p + 1 << endl;
        }
    }

    for (int p = 0; p < 2; p++) {
        cout << "Child " << p + 1 << ": ";
        for (int g = 0; g < G; g++) {
            cout << child[p][g] << " ";
        }
        cout << endl;
    }
}

void survivalSelection() {
    // Create a temporary array to hold both the current population and new children
    double temp_fitness[P + 2];
    int temp_population[P + 2][G];

    // Copy current population to temp array
    for (int p = 0; p < P; p++) {
        temp_fitness[p] = fitness[p];
        for (int g = 0; g < G; g++) {
            temp_population[p][g] = c[p][g];
        }
    }

    // Add the new children to temp array
    for (int p = 0; p < 2; p++) {
        temp_fitness[P + p] = 0; // Initial fitness for new children
        for (int g = 0; g < G; g++) {
            temp_population[P + p][g] = child[p][g];
        }
    }

    // Evaluate the fitness of new children
    for (int p = P; p < P + 2; p++) {
        int totalR = 0, totalC = 0;
        double avgR = 0.0, avgC = 0.0;
        int prev = 0, count = 0;
        int nurse1 = 0, nurse2 = 0, nurse3 = 0, nurse4 = 0;
        for (int g = 0; g < G; g++) {
            if (temp_population[p][g] == prev) {
                count++;
            }
            if (temp_population[p][g] == 1) {
                nurse1++;
            }
            else if (temp_population[p][g] == 2) {
                nurse2++;
            }
            else if (temp_population[p][g] == 3) {
                nurse3++;
            }
            else {
                nurse4++;
            }
            int index = temp_population[p][g] - 1;
            totalR += R[index];
            totalC += C[index];
            prev = temp_population[p][g];
        }
        avgR = totalR / (double)G;
        avgC = totalC / (double)G;

        if (count == 0 && nurse1 < fairness && nurse2 < fairness && nurse3 < fairness && nurse4 < fairness) {
            temp_fitness[p] = (avgR / MAX_R) + (1 - (avgC / MAX_C));
        }
        else if (count == 0 && (nurse1 > fairness || nurse2 > fairness || nurse3 > fairness || nurse4 > fairness)) {
            temp_fitness[p] = ((avgR / MAX_R) + (1 - (avgC / MAX_C))) * 0.7;
        }
        else if (count > 0 && nurse1 < fairness && nurse2 < fairness && nurse3 < fairness && nurse4 < fairness) {
            temp_fitness[p] = ((avgR / MAX_R) + (1 - (avgC / MAX_C))) * 0.5;
        }
        else {
            temp_fitness[p] = (((avgR / MAX_R) + (1 - (avgC / MAX_C))) * 0.7) * 0.5;
        }
    }

    // Select the top P individuals for the next generation
    vector<pair<double, int>> fitness_index;
    for (int p = 0; p < P + 2; p++) {
        fitness_index.push_back(make_pair(temp_fitness[p], p));
    }
    sort(fitness_index.rbegin(), fitness_index.rend()); // Sort in descending order

    for (int p = 0; p < P; p++) {
        int idx = fitness_index[p].second;
        new_fitness[p] = temp_fitness[idx];
        for (int g = 0; g < G; g++) {
            newc[p][g] = temp_population[idx][g];
        }
    }

    for (int p = 0; p < P; p++) {
        cout << "New C" << p << ": ";
        for (int g = 0; g < G; g++) {
            cout << newc[p][g] << " ";
        }
        cout << endl;
    }
}

void copyChromosome() {
    for (int p = 0; p < P; p++) {
        for (int g = 0; g < G; g++) {
            c[p][g] = newc[p][g];
        }
        fitness[p] = new_fitness[p];
    }
}

void calculateAverageFitness() {
    float totalfitness = 0;
    for (int p = 0; p < P; p++) {
        totalfitness += fitness[p];
    }
    avgf = totalfitness / P;
    cout << "Average Fitness: " << avgf << endl;
    af << avgf << endl;
}

void recordBestFitness() {
    for (int p = 0; p < P; p++) {
        if (fitness[p] > bestf) {
            bestf = fitness[p];
            for (int g = 0; g < G; g++) {
                bestc[g] = c[p][g];
            }
        }
    }
    cout << "BSF = " << bestf << endl;
    bf << bestf << endl;
    cout << "Best Chromosome: ";
    for (int g = 0; g < G; g++) {
        cout << bestc[g];
        bc << bestc[g];
    }
    cout << endl;
    bc << endl;
}

int main() {
    af.open("Average_Fitness.txt");
    bf.open("Best_Fitness.txt");
    bc.open("Best_Chromosome.txt");
    cout << "Initialization:" << endl;
    initializationPopulation();
    for (int g = 0; g < MAX_GEN; g++) {
        cout << "Generation " << g + 1 << ": " << endl;
        printChromosome();
        cout << "Evaluation " << g + 1 << ":" << endl;
        evaluateChromosome();
        calculateAverageFitness();
        recordBestFitness();
        countc = 0;
        for (int p = 0; p < P / 2; p++) {
            cout << "Parent Selection Gen " << g + 1 << " | " << p + 1 << ":" << endl;
            parentSelection();
            cout << "Crossover Gen " << g + 1 << " | " << p + 1 << ":" << endl;
            crossover();
            cout << "Mutation Gen " << g + 1 << " | " << p + 1 << ":" << endl;
            mutation();
            cout << "Selection Gen " << g + 1 << " | " << p + 1 << ":" << endl;
            survivalSelection();
        }
        copyChromosome();
    }
    af.close();
    bf.close();
    bc.close();
}
