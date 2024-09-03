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

int greedy_backpack(int weight, std::vector<Item>& items) {

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(items.begin(), items.end(), g);

    int total_value = 0;
    for (const auto& item : items) {
        if (item.weight <= weight) {
            total_value += item.value;
            weight -= item.weight;
        }
    }
    return total_value;
}

int main(int argc, char* argv[]) {

    std::vector<std::string> files = {"./entrada1.txt", "./entrada2.txt", "./entrada3.txt", "./entrada4.txt"};
    
    for (int i = 0; i < files.size(); i++) {
        std::ifstream file(files[i]);
        int weight, n;
        file >> n >> weight;

        printf("Weight: %d\n", weight);
        printf("Number of items: %d\n", n);

        std::vector<Item> items(n);
        for (int i = 0; i < n; i++) {
            file >> items[i].weight >> items[i].value;
        }

        // Medir o tempo da heurística gulosa
        auto start_greedy = std::chrono::high_resolution_clock::now();
        int result_greedy = greedy_backpack(weight, items);
        auto end_greedy = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_greedy = end_greedy - start_greedy;
        std::cout << "Heurística gulosa - Valor obtido: " << result_greedy 
                << ", Tempo de execução: " << duration_greedy.count() << " segundos" << std::endl;
    }



    return 0;
}
