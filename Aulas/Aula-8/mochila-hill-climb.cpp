#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

struct Item {
    int weight;
    int value;
};

int calculateValue(const std::vector<bool>& solution, const std::vector<Item>& items, int capacity) {
    int total_value = 0, total_weight = 0;

    for (size_t i = 0; i < solution.size(); i++) {
        if (solution[i]) {
            total_value += items[i].value;
            total_weight += items[i].weight;
        }
    }

    return (total_weight <= capacity) ? total_value : -1;
}

std::vector<std::vector<bool>> generateNeighbors(const std::vector<bool>& solution) {
    std::vector<std::vector<bool>> neighbors;
    
    for (size_t i = 0; i < solution.size(); i++) {
        std::vector<bool> neighbor = solution;
        neighbor[i] = !solution[i];
        neighbors.push_back(neighbor);
    }
    
    return neighbors;
}

std::vector<bool> hillClimbingKnapsack(int capacity, const std::vector<Item>& items) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<Item> shuffled_items = items;
    std::shuffle(shuffled_items.begin(), shuffled_items.end(), g);

    std::vector<bool> current_solution(items.size(), false);
    int remaining_capacity = capacity;

    for (size_t i = 0; i < items.size(); i++) {
        if (shuffled_items[i].weight <= remaining_capacity) {
            current_solution[i] = true;
            remaining_capacity -= shuffled_items[i].weight;
        }
    }

    int current_value = calculateValue(current_solution, items, capacity);

    bool improved;
    do {
        improved = false;
        auto neighbors = generateNeighbors(current_solution);

        for (const auto& neighbor : neighbors) {
            int neighbor_value = calculateValue(neighbor, items, capacity);

            if (neighbor_value > current_value) {
                current_solution = neighbor;
                current_value = neighbor_value;
                improved = true;
                break;
            }
        }
    } while (improved);

    return current_solution;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> files = {"./Entrada_1.txt", "./Entrada_2.txt", "./Entrada_3.txt"};
    
    for (const auto& filename : files) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir o arquivo: " << filename << std::endl;
            continue;
        }

        int capacity, n;
        file >> n >> capacity;

        std::cout << "Capacidade da mochila: " << capacity << std::endl;
        std::cout << "Número de itens: " << n << std::endl;

        std::vector<Item> items(n);
        for (int i = 0; i < n; i++) {
            file >> items[i].weight >> items[i].value;
        }

        // Executa o Hill Climbing 10 vezes para cada entrada
        double total_duration = 0.0;
        int best_value = 0;

        for (int j = 0; j < 10; j++) {
            auto start_hill_climbing = std::chrono::high_resolution_clock::now();
            std::vector<bool> best_solution = hillClimbingKnapsack(capacity, items);
            auto end_hill_climbing = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration_hill_climbing = end_hill_climbing - start_hill_climbing;
            
            int current_value = calculateValue(best_solution, items, capacity);
            best_value = std::max(best_value, current_value);
            total_duration += duration_hill_climbing.count();

            std::cout << "Execução " << j+1 << " - Valor obtido: " << current_value 
                      << ", Tempo de execução: " << duration_hill_climbing.count() << " segundos" << std::endl;
        }

        double average_duration = total_duration / 10.0;
        std::cout << "Melhor valor obtido após 10 execuções: " << best_value 
                  << ", Tempo médio de execução: " << average_duration << " segundos" << std::endl;
    }

    return 0;
}
