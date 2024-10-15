#include <omp.h>
#include <iostream>
#include <iomanip>

static long num_steps = 1024l * 1024 * 1024 * 2;
double sum = 0.0;

int main() {
    double step = 1.0 / (double)num_steps;
    double init_time, final_time;

    init_time = omp_get_wtime();

    // Usando parallel for para paralelizar o cálculo
    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for
        for (long i = 0; i < num_steps; i++) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }
        // Usando atomic para garantir atualização segura de sum
        #pragma omp atomic
        sum += local_sum;
    }

    double pi = step * sum;
    final_time = omp_get_wtime() - init_time;

    std::cout << "for " << num_steps << " steps pi = " << std::setprecision(15) << pi
              << " in " << final_time << " secs\n";

    return 0;
}
