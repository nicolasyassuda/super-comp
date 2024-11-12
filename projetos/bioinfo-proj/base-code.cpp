#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para armazenar os contadores
typedef struct {
    long countA;
    long countT;
    long countC;
    long countG;
} Counters;

// EX1
// Função para contar A, T, C, G em uma porção do arquivo
Counters contar_nucleotideos(const char* nome_arquivo, long inicio, long fim) {
    Counters contadores = {0, 0, 0, 0};
    FILE *file = fopen(nome_arquivo, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Mover para o ponto de início
    if (fseek(file, inicio, SEEK_SET) != 0) {
        fprintf(stderr, "Erro ao mover para a posição %ld no arquivo %s\n", inicio, nome_arquivo);
        fclose(file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    long tamanho = fim - inicio;
    char *buffer = (char*) malloc(sizeof(char) * tamanho);
    if (buffer == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        fclose(file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    size_t lido = fread(buffer, sizeof(char), tamanho, file);
    if (lido != tamanho) {
        // Não é necessariamente um erro se for o último processo
        // printf("Aviso: Esperado ler %ld bytes, mas leu %zu bytes no arquivo %s\n", tamanho, lido, nome_arquivo);
    }

    fclose(file);

    // Contagem paralela com OpenMP
    #pragma omp parallel
    {
        long localA = 0, localT = 0, localC = 0, localG = 0;
        #pragma omp for nowait
        for (long i = 0; i < lido; i++) {
            char c = buffer[i];
            switch (c) {
                case 'A':
                case 'a':
                    localA++;
                    break;
                case 'T':
                case 't':
                    localT++;
                    break;
                case 'C':
                case 'c':
                    localC++;
                    break;
                case 'G':
                case 'g':
                    localG++;
                    break;
                default:
                    break;
            }
        }

        // Atualizar os contadores globais com redução
        #pragma omp atomic
        contadores.countA += localA;
        #pragma omp atomic
        contadores.countT += localT;
        #pragma omp atomic
        contadores.countC += localC;
        #pragma omp atomic
        contadores.countG += localG;
    }

    free(buffer);
    return contadores;
}


//EX2
// Função de transcrição de DNA para RNA
void transcritionDNAtoRNA(const char* nome_arquivo, long inicio, long fim, int rank) {
    FILE *file = fopen(nome_arquivo, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Mover para o ponto de início
    if (fseek(file, inicio, SEEK_SET) != 0) {
        fprintf(stderr, "Erro ao mover para a posição %ld no arquivo %s\n", inicio, nome_arquivo);
        fclose(file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    long tamanho = fim - inicio;
    char *buffer = (char*) malloc(sizeof(char) * tamanho);
    if (buffer == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        fclose(file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    size_t lido = fread(buffer, sizeof(char), tamanho, file);
    if (lido != tamanho) {
        // Não é necessariamente um erro se for o último processo
        // printf("Aviso: Esperado ler %ld bytes, mas leu %zu bytes no arquivo %s\n", tamanho, lido, nome_arquivo);
    }

    fclose(file);

    // Alocar buffer para o RNA transcrito
    char *rna_buffer = (char*) malloc(sizeof(char) * lido);
    if (rna_buffer == NULL) {
        fprintf(stderr, "Erro ao alocar memória para RNA\n");
        free(buffer);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Transcrição paralela com OpenMP
    #pragma omp parallel for
    for (long i = 0; i < lido; i++) {
        char c = buffer[i];
        switch (c) {
            case 'A':
            case 'a':
                rna_buffer[i] = 'U';
                break;
            case 'C':
            case 'c':
                rna_buffer[i] = 'G';
                break;
            case 'T':
            case 't':
                rna_buffer[i] = 'A';
                break;
            case 'G':
            case 'g':
                rna_buffer[i] = 'C'; // Se desejar, pode ser ajustado conforme a regra de transcrição
                break;
            default:
                rna_buffer[i] = c; // Mantém caracteres não relacionados
                break;
        }
    }

    // Construir o nome do arquivo de saída
    char nome_saida[256];
    snprintf(nome_saida, sizeof(nome_saida), "%s-RNA-%d.txt", nome_arquivo, rank);

    // Escrever o RNA transcrito no arquivo de saída
    FILE *saida = fopen(nome_saida, "w");
    if (saida == NULL) {
        fprintf(stderr, "Erro ao criar o arquivo de saída %s\n", nome_saida);
        free(buffer);
        free(rna_buffer);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    size_t escrito = fwrite(rna_buffer, sizeof(char), lido, saida);
    if (escrito != lido) {
        fprintf(stderr, "Erro ao escrever no arquivo %s. Esperado: %zu, Escrito: %zu\n", nome_saida, lido, escrito);
    }

    fclose(saida);
    free(buffer);
    free(rna_buffer);
}


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0)
            fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    const char* nome_arquivo = argv[1];
    long tamanho_arquivo;
    long header_size = 0;

    if (rank == 0) {
        FILE *file = fopen(nome_arquivo, "r");
        if (file == NULL) {
            fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Ler e ignorar a primeira linha
        char *linha = NULL;
        size_t len = 0;
        ssize_t read;

        read = getline(&linha, &len, file);
        if (read == -1) {
            fprintf(stderr, "Erro ao ler a primeira linha do arquivo %s\n", nome_arquivo);
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        header_size = ftell(file); // Posição após a primeira linha

        // Determinar o tamanho total do arquivo
        fseek(file, 0, SEEK_END);
        tamanho_arquivo = ftell(file);
        fclose(file);

        if (linha)
            free(linha);
    }

    // Broadcast do header_size e tamanho_arquivo para todos os processos
    MPI_Bcast(&header_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tamanho_arquivo, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Calcular o tamanho real a ser processado (excluindo a primeira linha)
    long data_size = tamanho_arquivo - header_size;
    if (data_size < 0) data_size = 0; // Proteção contra arquivos menores que a primeira linha

    // Dividir o arquivo entre os processos
    long porcao = data_size / size;
    long inicio = header_size + rank * porcao;
    long fim = (rank == size -1) ? tamanho_arquivo : inicio + porcao;

    // Garantir que nenhum processo ultrapasse o tamanho do arquivo
    if (inicio > tamanho_arquivo) inicio = tamanho_arquivo;
    if (fim > tamanho_arquivo) fim = tamanho_arquivo;

    // Chamar a função para contar nucleotídeos na porção
    // Counters local_contadores = contar_nucleotideos(nome_arquivo, inicio, fim);


    // Chamar a função para transcrever DNA para RNA
   // Transcrição local de DNA para RNA e escrita no arquivo de saída
    transcritionDNAtoRNA(nome_arquivo, inicio, fim, rank);
    // Agregar os resultados de todos os processos
    Counters total_contadores = {0, 0, 0, 0};
    // MPI_Reduce com tipo MPI_LONG para cada contador
    // MPI_Reduce(&local_contadores.countA, &total_contadores.countA, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    // MPI_Reduce(&local_contadores.countT, &total_contadores.countT, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    // MPI_Reduce(&local_contadores.countC, &total_contadores.countC, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    // MPI_Reduce(&local_contadores.countG, &total_contadores.countG, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Processo raiz imprime o resultado
    // if (rank == 0) {
    //     printf("Total A: %ld\n", total_contadores.countA);
    //     printf("Total T: %ld\n", total_contadores.countT);
    //     printf("Total C: %ld\n", total_contadores.countC);
    //     printf("Total G: %ld\n", total_contadores.countG);
    // }

    MPI_Finalize();
    return 0;
}
