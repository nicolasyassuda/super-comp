#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <omp.h>

double averageOfVector(const std::vector<double>& vector) {
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < vector.size(); ++i) {
        sum += vector[i];
    }
    return sum / vector.size();
}

double maxOfVector(const std::vector<double>& vector) {
    double max_value = vector[0]; // Assume o primeiro valor como o maior para diminuir a complexidade
    #pragma omp parallel for reduction(max:max_value)
    for (size_t i = 0; i < vector.size(); ++i) {
        if (vector[i] > max_value) {
            max_value = vector[i];
        }
    }
    return max_value;
}

double productOfVector(const std::vector<double>& vector) {
    double product = 1.0; // Inicializa com 1 para não zerar o produto
    #pragma omp parallel for reduction(*:product)
    for (size_t i = 0; i < vector.size(); ++i) {
        product *= vector[i];
    }
    return product;
}

int main() {
    std::vector<double> vector = {1.5,2,3.5,4,5.5,6};

    double average = 0.0;
    double max_value = 0.0;
    double product = 0.0;
    
    average = averageOfVector(vector);
    max_value = maxOfVector(vector);
    product = productOfVector(vector);

    std::cout << "Media: " << average << std::endl;
    std::cout << "Valor Maximo: " << max_value << std::endl;
    std::cout << "Produto (entre os valores do vetor): " << product << std::endl;

    return 0;
}