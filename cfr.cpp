#include <random>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#define CARDS 4
#define ITERATIONS 10000

void __init__(std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>>& strategies,
    const int cards, const bool random)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    for (int i = 0; i < cards; i++) {
        for (int j = i + 1; j < cards; j++) {
            strategies[{i, j}] = std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>(
                std::pair<double, double>(random ? dis(gen) : 0.5, 0.0), std::map<int, std::pair<double, double>>()
            );
            for (int k = 0; k < cards; k++) {
                if (k != i && k != j)
                    strategies[{i, j}].second[k] = std::pair<double, double>(random ? dis(gen) : 1.0, 0.0);
            }
        }
    }
}

int terminal_node(const int a0, const int a1, const int b0, const int b1) {
    return (((a0 < b0) ? 1 : -1) + ((a1 < b1) ? 1 : -1)) >> 1;
}

double node1(const int hand0Card0, const int hand0Card1, const int hand1Card0, const int hand1Card1, std::pair<double, double>& strategy,
    const double probability)
{
    strategy.second += (
        strategy.first * terminal_node(hand0Card0, hand0Card1, hand1Card0, hand1Card1) -
        (1.0 - strategy.first) * terminal_node(hand0Card0, hand0Card1, hand1Card1, hand1Card0)
    ) * probability;
    return strategy.second;
}

void node0(std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>>::iterator& hand0,
    std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>>::iterator& hand1)
{
    hand0->second.first.second +=
        node1(hand0->first.first, hand0->first.second, hand1->first.first, hand1->first.second, hand1->second.second[hand0->first.first], hand0->second.first.first) -
        node1(hand0->first.second, hand0->first.first, hand1->first.first, hand1->first.second, hand1->second.second[hand0->first.second], 1.0 - hand0->second.first.first);
}

void cfr(std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>>& strategies,
    const double probability)
{
    for (auto i = strategies.begin(); i != strategies.end(); i++) {
        for (auto j = strategies.begin(); j != strategies.end(); j++) {
            if (i->first.first != j->first.first && i->first.first != j->first.second && i->first.second != j->first.first && i->first.second != j->first.second) {
                node0(i, j);
            }
        }
    }
    for (auto i = strategies.begin(); i != strategies.end(); i++) {
        i->second.first.first = std::min(1.0, std::max(0.0, i->second.first.first + i->second.first.second * probability));
        i->second.first.second = 0.0;
        for (auto j = i->second.second.begin(); j != i->second.second.end(); j++) {
            j->second.first = std::min(1.0, std::max(0.0, j->second.first - j->second.second * probability));
            j->second.second = 0.0;
        }
    }
}

void expectedValue(std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>>& strategies,
    const double probability, double& expVal)
{
    for (auto i = strategies.begin(); i != strategies.end(); i++) {
        for (auto j = strategies.begin(); j != strategies.end(); j++) {
            if (i->first.first != j->first.first && i->first.first != j->first.second && i->first.second != j->first.first && i->first.second != j->first.second) {
                expVal +=
                    i->second.first.first * (
                        j->second.second[i->first.first].first * terminal_node(i->first.first, i->first.second, j->first.first, j->first.second) +
                        (1.0 - j->second.second[i->first.first].first) * terminal_node(i->first.first, i->first.second, j->first.second, j->first.first)
                    ) +
                    (1.0 - i->second.first.first) * (
                        j->second.second[i->first.second].first * terminal_node(i->first.second, i->first.first, j->first.first, j->first.second) +
                        (1.0 - j->second.second[i->first.second].first) * terminal_node(i->first.second, i->first.first, j->first.second, j->first.first)
                    );
            }
        }
    }
    expVal *= probability;
}

void exportData(std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>> strategies,
    const int cards, const int iterations, const double probability)
{
    double expVal = 0;
    expectedValue(strategies, probability, expVal);
    std::ofstream out("Strategy.csv");
    out << "Cards:," << cards << std::endl;
    out << "Iterations:," << iterations << std::endl;
    out.precision(3);
    out << "Expected value:," << expVal << std::endl;
    out << std::endl;
    out << ",,,Revealed Card" << std::endl;
    out << "Card 0,Card 1,,None,";
    for (int i = 0; i < cards; i++) {
        out << "," << i;
    }
    out << std::endl;
    out << std::fixed;
    printf("Expected value: %.3f\n", expVal);
    out.precision(1);
    for (auto i = strategies.begin(); i != strategies.end(); i++) {
        printf("[%d, %d]: [%.1f%%, %.1f%%]\n", i->first.first, i->first.second, 100 * i->second.first.first, 100 * (1.0 - i->second.first.first));
        out << i->first.first << "," << i->first.second << ",," << 100 * i->second.first.first << "%,";
        int it = 0;
        for (auto j = i->second.second.begin(); j != i->second.second.end(); j++) {
            printf("[%d, %d], %d: [%.1f%%, %.1f%%]\n", i->first.first, i->first.second, j->first, 100 * j->second.first, 100 * (1.0 - j->second.first));
            while (it != j->first) {
                out << ",";
                it++;
            }
            out << "," << 100 * j->second.first << "%";
            it++;
        }
        out << std::endl;
    }
}

int main(int argc, char* argv[]) {
    int cards = (argc > 1) ? std::atoi(argv[1]) : CARDS;
    int iterations = (argc > 2) ? std::atoi(argv[2]) : ITERATIONS;
    bool random = argc > 3 && std::string(argv[3]) == "RANDOM";
    double probability = 4.0 / (double)(cards * (cards - 1) * (cards - 2) * (cards - 3));

    // 'strategies' object consists of keys (card0, card1) and values ((player0 strategy, player 0 ev), (player0 revealed card: player 1 strategy, player 1 ev))
    std::map<std::pair<int, int>, std::pair<std::pair<double, double>, std::map<int, std::pair<double, double>>>> strategies;
    __init__(strategies, cards, random);

    for (int i = 0; i < iterations; i++) {
        double expVal = 0;
        if (i % (iterations / 10) == 0) {
            expectedValue(strategies, probability, expVal);
            printf("Expected value (iteration %d): %.3f\n", i, expVal);
        }
        cfr(strategies, probability);
    }

    exportData(strategies, cards, iterations, probability);
}