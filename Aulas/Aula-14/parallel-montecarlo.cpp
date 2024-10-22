#include <iostream>
#include <iomanip>
#include <chrono>
#include <omp.h>

double random_double(int min, int max){
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

int main(){
    
    long in_circle = 0;
    long num_steps = 100000;
    double x = 0;
    double y = 0;
    
    auto start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for reduction(+:in_circle) private(x, y)
    for (int i = 0; i < num_steps; i++){
        #pragma omp critical
        {
            x = random_double(0, 1);
            y = random_double(0, 1);
        }
        if (x*x + y*y <= 1){
            in_circle += 1;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    double pi = 4.0 * in_circle / num_steps;
    std::cout << "for " << num_steps << " steps pi = " << std::setprecision(15) << pi << "\n" << "Elapsed time: " << elapsed.count() << "s\n";
    return 0;
}