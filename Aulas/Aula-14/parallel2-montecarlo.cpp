#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <omp.h>

void random_double(int min, int max, unsigned int seed, double* x, double* y) {
    std::default_random_engine generator(seed);  // Inicializa o gerador com a semente
    std::uniform_real_distribution<double> distribution(min, max);  // Distribuição uniforme

    *x = distribution(generator);  // Gera o número aleatório para x
    *y = distribution(generator);  // Gera o número aleatório para y
}

int main() {
    long in_circle = 0;
    long num_steps = 1000000;
    double x = 0.0;
    double y = 0.0;

    auto start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for reduction(+:in_circle) private(x, y)
    for (int i = 0; i < num_steps; i++) {
        unsigned int seed = omp_get_thread_num() + i;
        random_double(0, 1, seed, &x, &y);
        if (x * x + y * y <= 1) {
            in_circle += 1;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    double pi = 4.0 * in_circle / num_steps;
    std::cout << "For " << num_steps << " steps, pi = " << std::setprecision(15) << pi << "\n";
    std::cout << "Elapsed time: " << elapsed.count() << "s\n";

    return 0;
}
