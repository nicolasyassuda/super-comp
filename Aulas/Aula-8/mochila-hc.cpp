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
    std::shuffle(items.begin(), items.end(), g);  // Embaralha os itens para garantir aleatoriedade na seleção

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
    std::vector<std::string> files = {"./Entrada_1.txt", "./Entrada_2.txt", "./Entrada_3.txt"};
    
    for (int i = 0; i < files.size(); i++) {
        std::ifstream file(files[i]);
        int weight, n;
        file >> n >> weight;

        std::vector<Item> items(n);
        for (int j = 0; j < n; j++) {
            file >> items[j].weight >> items[j].value;
        }
        file.close();  // Fechando o arquivo após a leitura

        int best_value = 0;
        double best_time = 0;
        std::cout << "Resultados para " << files[i] << ":\n";

        for (int test = 0; test < 10; test++) {
            auto start_greedy = std::chrono::high_resolution_clock::now();
            int result_greedy = greedy_backpack(weight, items);
            auto end_greedy = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration_greedy = end_greedy - start_greedy;

            if (result_greedy > best_value) {
                best_value = result_greedy;
                best_time = duration_greedy.count();
            }

            std::cout << "Teste " << (test + 1) << ": Valor obtido = " << result_greedy 
                      << ", Tempo = " << duration_greedy.count() << " segundos\n";
        }

        // Exibir o melhor resultado após 10 testes
        std::cout << "\nMelhor resultado: Valor = " << best_value 
                  << ", Tempo = " << best_time << " segundos\n" << std::endl;
    }

    return 0;
}
