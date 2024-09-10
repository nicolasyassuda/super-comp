#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct Item {
    int peso;
    int valor;
};

bool comparaMaisCaro(const Item& a, const Item& b) {
    return a.valor > b.valor;
}

bool comparaMaisLeve(const Item& a, const Item& b) {
    return a.peso < b.peso;
}

void lerEntrada(const std::string& nome_arquivo, std::vector<Item>& itens, int& capacidade_maxima) {
    std::ifstream arquivo(nome_arquivo);
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo " << nome_arquivo << "\n";
        return;
    }

    int numero_itens;
    arquivo >> numero_itens >> capacidade_maxima;

    for (int i = 0; i < numero_itens; ++i) {
        int valor, peso;
        arquivo >> valor >> peso;
        itens.push_back({peso, valor});
    }
    
    arquivo.close();
}

void mochilaMaisCaroPrimeiro(std::vector<Item>& itens, int capacidade_maxima) {
    std::sort(itens.begin(), itens.end(), comparaMaisCaro);

    int peso_atual = 0;
    int valor_total = 0;

    for (const auto& item : itens) {
        if (peso_atual + item.peso <= capacidade_maxima) {
            peso_atual += item.peso;
            valor_total += item.valor;
            std::cout << "Adicionando item com peso " << item.peso << " e valor " << item.valor << "\n";
        }
    }

    std::cout << "Valor total na mochila (Mais cara): " << valor_total << "\n";
}

void mochilaMaisLevePrimeiro(std::vector<Item>& itens, int capacidade_maxima) {
    std::sort(itens.begin(), itens.end(), comparaMaisLeve);

    int peso_atual = 0;
    int valor_total = 0;

    for (const auto& item : itens) {
        if (peso_atual + item.peso <= capacidade_maxima) {
            peso_atual += item.peso;
            valor_total += item.valor;
            std::cout << "Adicionando item com peso " << item.peso << " e valor " << item.valor << "\n";
        }
    }

    std::cout << "Valor total na mochila (Mais leve): " << valor_total << "\n";
}

int main() {
    std::vector<Item> itens;
    int capacidade_maxima;
    
    lerEntrada("entrada.txt", itens, capacidade_maxima);

    std::cout << "Abordagem: Mais cara\n";
    mochilaMaisCaroPrimeiro(itens, capacidade_maxima);

    std::cout << "\nAbordagem: Mais leve\n";
    mochilaMaisLevePrimeiro(itens, capacidade_maxima);

    return 0;
}
