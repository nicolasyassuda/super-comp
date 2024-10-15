#include <omp.h>
#include <iostream>
#include <iomanip>

static long num_steps = 1024l * 1024 * 1024 * 2;

#define MIN_BLK 1024 * 1024 * 256

double sum = 0.0;

void pi_r(long Nstart, long Nfinish, double step) {
    long i, iblk;
    if (Nfinish - Nstart < MIN_BLK) {
        double local_sum = 0.0;
        for (i = Nstart; i < Nfinish; i++) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }
        #pragma omp atomic
        sum += local_sum;
    } else {
        iblk = Nfinish - Nstart;
        #pragma omp task  // Criar uma tarefa para paralelizar a execução
        pi_r(Nstart, Nfinish - iblk / 2, step);
        
        #pragma omp task  // Outra tarefa para o segundo bloco
        pi_r(Nfinish - iblk / 2, Nfinish, step);

        #pragma omp taskwait  // Esperar ambas as tarefas finalizarem
    }
}

int main() {
    double step = 1.0 / (double)num_steps;
    double init_time, final_time;

    init_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
            pi_r(0, num_steps, step);
        }
    }

    double pi = step * sum;
    final_time = omp_get_wtime() - init_time;

    std::cout << "for " << num_steps << " steps pi = " << std::setprecision(15) << pi
              << " in " << final_time << " secs\n";

    return 0;
}
