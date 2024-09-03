#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <ctime>

struct Item {
    int weight;
    int value;
};

// Função para selecionar itens com base em probabilidade
std::pair<int, int> selectItems(const std::vector<Item>& items, double threshold, int backpackCapacity) {
    std::vector<Item> backpack;
    int currentCapacity = 0;
    int totalWeight = 0;
    int totalValue = 0;

    // Inicializar o gerador de números aleatórios
    srand(time(nullptr));

    for (const auto& item : items) {
        if (currentCapacity + item.weight <= backpackCapacity) {
            double probability = rand() / (RAND_MAX + 1.0); // Gera um número aleatório entre 0 e 1
            if (probability > threshold) { // Verifica se a probabilidade é maior que o limiar
                backpack.push_back(item);
                currentCapacity += item.weight;
                totalWeight += item.weight;
                totalValue += item.value;
            }
        }
    }

    return {totalWeight, totalValue};
}

int main() {

    double threshold = 0.5;
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

        auto start_greedy = std::chrono::high_resolution_clock::now();
        auto [totalWeight, totalValue] =  selectItems(items, threshold, weight);
        auto end_greedy = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_greedy = end_greedy - start_greedy;
        std::cout << "Heurística probabilistica - Valor obtido: " << totalValue << "Peso total: " << totalWeight
                << ", Tempo de execução: " << duration_greedy.count() << " segundos" << std::endl;
    }

    return 0;
}
