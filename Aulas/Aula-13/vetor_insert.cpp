#include <vector>
#include <iostream>
#include <unistd.h>
#include <omp.h>


double conta_complexa(int i) {
    return 2 * i;
}

int main() {
    int N = 10000; 
    std::vector<double> vec;
    double init_time, final_time;
    
    init_time = omp_get_wtime();
    vec.resize(N);
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        vec[i] = conta_complexa(i);
    }

    final_time = omp_get_wtime() - init_time;

    std::cout << "Tempo de execução parallel for e pre-alocação: " << final_time << " secs\n";
    return 0;
}