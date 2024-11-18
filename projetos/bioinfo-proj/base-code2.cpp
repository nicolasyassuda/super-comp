#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <climits>

// Função para converter size_t para int com verificação de overflow
int size_t_to_int(size_t size) {
    if (size > INT_MAX) {
        std::cerr << "Erro: O tamanho " << size << " excede o limite do tipo int." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    return static_cast<int>(size);
}

// Função de transcrição de DNA para RNA que retorna a sequência de RNA
std::vector<char> transcritionDNAtoRNA(const std::string& nome_arquivo, long inicio, long fim) {
    std::ifstream file(nome_arquivo, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << nome_arquivo << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Mover para o ponto de início
    file.seekg(inicio);
    if (!file) {
        std::cerr << "Erro ao mover para a posição " << inicio << " no arquivo " << nome_arquivo << std::endl;
        file.close();
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    long tamanho = fim - inicio;
    std::vector<char> buffer(tamanho);

    file.read(buffer.data(), tamanho);
    std::streamsize lido = file.gcount();
    if (lido != tamanho) {
        // Não é necessariamente um erro se for o último processo
        // std::cerr << "Aviso: Esperado ler " << tamanho << " bytes, mas leu " << lido << " bytes no arquivo " << nome_arquivo << std::endl;
    }

    file.close();

    // Transcrição paralela com OpenMP
    std::vector<char> rna_buffer(lido);

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
                rna_buffer[i] = 'C';
                break;
            default:
                rna_buffer[i] = c; // Mantém caracteres não relacionados
                break;
        }
    }

    return rna_buffer;
}

// Função para contar o número de códon de início 'AUG' em um segmento de RNA
long count_start_codons(const std::vector<char>& rna_sequence) {
    long count = 0;
    size_t length = rna_sequence.size();

    if (length >= 3) {
        #pragma omp parallel for reduction(+:count)
        for (size_t i = 0; i <= length - 3; i++) {
            if (rna_sequence[i] == 'A' && rna_sequence[i + 1] == 'U' && rna_sequence[i + 2] == 'G') {
                count++;
            }
        }
    }

    return count;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0)
            std::cerr << "Uso: " << argv[0] << " <arquivo>" << std::endl;
        MPI_Finalize();
        exit(1);
    }

    std::string nome_arquivo = argv[1];
    long tamanho_arquivo;
    long header_size = 0;

    if (rank == 0) {
        std::ifstream file(nome_arquivo, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir o arquivo " << nome_arquivo << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Ler e ignorar a primeira linha
        std::string linha;
        if (!std::getline(file, linha)) {
            std::cerr << "Erro ao ler a primeira linha do arquivo " << nome_arquivo << std::endl;
            file.close();
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        header_size = file.tellg(); // Posição após a primeira linha

        // Determinar o tamanho total do arquivo
        file.seekg(0, std::ios::end);
        tamanho_arquivo = file.tellg();
        file.close();
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
    long fim = (rank == size - 1) ? tamanho_arquivo : inicio + porcao;

    // Garantir que nenhum processo ultrapasse o tamanho do arquivo
    if (inicio > tamanho_arquivo) inicio = tamanho_arquivo;
    if (fim > tamanho_arquivo) fim = tamanho_arquivo;

    // Adicionar sobreposição de 2 caracteres (exceto para o último processo)
    if (rank != size - 1) {
        fim += 2;
    }

    // Chamar a função para transcrever DNA para RNA
    std::vector<char> local_rna_sequence = transcritionDNAtoRNA(nome_arquivo, inicio, fim);
    size_t local_rna_length = local_rna_sequence.size();

    // Verificar se local_rna_length cabe em int
    int local_rna_length_int = size_t_to_int(local_rna_length);

    // Contar o número de códon de início 'AUG' no segmento local
    long local_start_codon_count = count_start_codons(local_rna_sequence);

    // Reduzir as contagens locais para obter a contagem total
    long total_start_codon_count = 0;
    MPI_Reduce(&local_start_codon_count, &total_start_codon_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Coletar as sequências de RNA de todos os processos para formar a sequência completa
    // Primeiro, coletar os tamanhos das sequências de RNA
    std::vector<int> recvcounts(size);
    std::vector<int> displs(size);

    MPI_Gather(&local_rna_length_int, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Processo raiz calcula deslocamentos e total de bytes
    std::vector<char> total_rna_sequence;
    int total_rna_length = 0;
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 0; i < size; i++) {
            if (recvcounts[i] < 0) {
                std::cerr << "Erro: recvcounts[" << i << "] é negativo." << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            if (i > 0) displs[i] = displs[i - 1] + recvcounts[i - 1];
            total_rna_length += recvcounts[i];
            if (total_rna_length < 0) {
                std::cerr << "Erro: total_rna_length excede o limite do tipo int." << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        total_rna_sequence.resize(total_rna_length);
    }

    // Coletar as sequências de RNA de todos os processos
    MPI_Gatherv(local_rna_sequence.data(), local_rna_length_int, MPI_CHAR,
                rank == 0 ? total_rna_sequence.data() : nullptr,
                rank == 0 ? recvcounts.data() : nullptr,
                rank == 0 ? displs.data() : nullptr,
                MPI_CHAR,
                0, MPI_COMM_WORLD);

    // Processo raiz escreve a sequência total em um único arquivo
    if (rank == 0) {
        std::string nome_saida = nome_arquivo + "-RNA.txt";

        std::ofstream saida(nome_saida, std::ios::binary);
        if (!saida.is_open()) {
            std::cerr << "Erro ao criar o arquivo de saída " << nome_saida << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Escrever a primeira linha (header)
        std::ifstream file(nome_arquivo);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir o arquivo " << nome_arquivo << " para ler o header" << std::endl;
            saida.close();
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        std::string linha;
        if (std::getline(file, linha)) {
            saida << linha << '\n';
        }
        file.close();

        // Escrever a sequência de RNA
        saida.write(total_rna_sequence.data(), total_rna_length);
        if (!saida) {
            std::cerr << "Erro ao escrever no arquivo " << nome_saida << std::endl;
        }

        saida.close();

        // Imprimir o total de proteínas inicializadas
        std::cout << "Total de proteínas inicializadas (AUG encontrados): " << total_start_codon_count << std::endl;
    }

    // Finalizar o MPI
    MPI_Finalize();
    return 0;
}