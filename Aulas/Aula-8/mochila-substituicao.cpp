#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

struct Item {
    int weight;
    int value;
    bool used = false;
};

int greedy_backpack(int max_weight, std::vector<Item>& items) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(items.begin(), items.end(), g);

    int total_value = 0;
    int current_weight = 0;
    std::vector<Item*> used_items;

    for (auto& item : items) {
        if (current_weight + item.weight <= max_weight) {
            item.used = true;
            used_items.push_back(&item);
            total_value += item.value;
            current_weight += item.weight;
        }
    }

    bool improved;
    do {
        improved = false;
        for (size_t i = 0; i < used_items.size(); i++) {
            for (auto& item : items) {
                if (!item.used && item.weight <= used_items[i]->weight) {
                    int potential_new_value = total_value - used_items[i]->value + item.value;
                    if (potential_new_value > total_value) {
                        total_value = potential_new_value;
                        used_items[i]->used = false;
                        item.used = true;
                        used_items[i] = &item;
                        improved = true;
                        break;
                    }
                }
            }
            if (improved) break;
        }
    } while (improved);

    return total_value;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> files =  {"./Entrada_1.txt", "./Entrada_2.txt", "./Entrada_3.txt"};

    for (int fileIndex = 0; fileIndex < files.size(); fileIndex++) {
        std::ifstream file(files[fileIndex]);
        int weight, n;
        file >> n >> weight;

        std::vector<Item> items(n);
        for (int i = 0; i < n; i++) {
            file >> items[i].weight >> items[i].value;
        }

        int best_value = 0;
        double best_time = 0;
        int best_execution = 0;

        for (int test = 1; test <= 10; test++) {
            auto start_greedy = std::chrono::high_resolution_clock::now();
            int result_greedy = greedy_backpack(weight, items);
            auto end_greedy = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration_greedy = end_greedy - start_greedy;

            if (result_greedy > best_value) {
                best_value = result_greedy;
                best_time = duration_greedy.count();
                best_execution = test;
            }

            std::cout << "Execução " << test << " para arquivo " << files[fileIndex] << ": "
                      << "Valor obtido: " << result_greedy 
                      << ", Tempo de execução: " << duration_greedy.count() << " segundos" << std::endl;
        }

        std::cout << "\nMelhor Execução (" << best_execution << "): Valor obtido: " << best_value 
                  << ", Tempo de execução: " << best_time << " segundos" << std::endl << std::endl;
    }

    return 0;
}
