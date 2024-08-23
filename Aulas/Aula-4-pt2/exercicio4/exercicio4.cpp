#include <iostream>
#include <vector>

class Vector {
public:
    Vector(int tamanho);
    ~Vector();
    void inicializa(int valor);
    int get(int index) const;
    void set(int index, int valor);
    void inserir(int index, int valor);
    void remover(int index);
    void imprime() const;

private:
    int* dados;
    int tam;
    int capacidade;
    void redimensiona(int novaCapacidade);
};

// Implementação das funções...

Vector::Vector(int tamanho) {
    dados = new int[tamanho];
    tam = 0;
    capacidade = tamanho;
}

Vector::~Vector() {
    delete[] dados;
}

void Vector::inicializa(int valor) {
    for (int i = 0; i < capacidade; i++) {
        dados[i] = valor;
    }
}

int Vector::get(int index) const {
    return dados[index];
}

void Vector::set(int index, int valor) {
    dados[index] = valor;
}

void Vector::inserir(int index, int valor) {
    if (tam == capacidade) {
        redimensiona(2 * capacidade);
    }
    for (int i = tam; i > index; i--) {
        dados[i] = dados[i - 1];
    }
    dados[index] = valor;
    tam++;
}

void Vector::remover(int index) {
    for (int i = index; i < tam - 1; i++) {
        dados[i] = dados[i + 1];
    }
    tam--;
    if (tam > 0 && tam == capacidade / 4) {
        redimensiona(capacidade / 2);
    }
}

void Vector::imprime() const {
    for (int i = 0; i < tam; i++) {
        std::cout << dados[i] << " ";
    }
    std::cout << std::endl;
}

void Vector::redimensiona(int novaCapacidade) {
    int* novo = new int[novaCapacidade];
    for (int i = 0; i < tam; i++) {
        novo[i] = dados[i];
    }
    delete[] dados;
    dados = novo;
    capacidade = novaCapacidade;
}

int main() {
    Vector vec(5);
    vec.inicializa(0);
    vec.inserir(0,1);
    vec.inserir(1,2);
    vec.inserir(2,3);
    vec.inserir(3,4);
    vec.inserir(4,5);

    vec.remover(4);
    vec.remover(0);

    vec.get(0);
    vec.imprime();

    return 0;
}