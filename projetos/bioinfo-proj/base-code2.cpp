#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <climits>
#include <algorithm>
#include <cctype> // Para std::toupper

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

// Função para contar o número de códons de início 'AUG' em um segmento de RNA
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

// Função para traduzir RNA para aminoácidos
std::vector<char> translate_RNA_to_aminoacids(const std::vector<char>& rna_sequence) {
    size_t length = rna_sequence.size();
    size_t num_codons = length / 3;
    std::vector<char> aminoacids_sequence(num_codons);

    #pragma omp parallel for
    for (size_t idx = 0; idx < num_codons; idx++) {
        size_t i = idx * 3;
        std::string codon;
        codon += std::toupper(rna_sequence[i]);
        codon += std::toupper(rna_sequence[i + 1]);
        codon += std::toupper(rna_sequence[i + 2]);

        char amino_acid;
        if (codon == "CCA" || codon == "CCG" || codon == "CCU" || codon == "CCC") {
            amino_acid = 'P';
        } else if (codon == "UCU" || codon == "UCA" || codon == "UCG" || codon == "UCC") {
            amino_acid = 'S';
        } else if (codon == "CAG" || codon == "CAA") {
            amino_acid = 'Q';
        } else if (codon == "ACA" || codon == "ACC" || codon == "ACU" || codon == "ACG") {
            amino_acid = 'T';
        } else if (codon == "AUG") {
            amino_acid = 'M';
        } else if (codon == "UGA") {
            amino_acid = '*';
        } else if (codon == "UGC" || codon == "UGU") {
            amino_acid = 'C';
        } else if (codon == "GUG" || codon == "GUA" || codon == "GUC" || codon == "GUU") {
            amino_acid = 'V';
        } else {
            amino_acid = 'X';
        }
        aminoacids_sequence[idx] = amino_acid;
    }
    return aminoacids_sequence;
}

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// Assume these functions are defined elsewhere
std::vector<char> transcritionDNAtoRNA(const std::string& nome_arquivo, long inicio, long fim);
long count_start_codons(const std::vector<char>& sequence);
std::vector<char> translate_RNA_to_aminoacids(const std::vector<char>& rna_sequence);
int size_t_to_int(size_t value);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define a list of filenames to process
    std::vector<std::string> filenames = {
        "chr1.subst.fa",
        "chr2.subst.fa",
        "chr3.subst.fa",
        // "chr4.subst.fa",
        // "chr5.subst.fa",
        // "chr6.subst.fa",
        // "chr7.subst.fa",
        // "chr8.subst.fa",
        // "chr9.subst.fa",
        // "chr10.subst.fa"
        // "chr11.subst.fa",
        // "chr12.subst.fa",
        // "chr13.subst.fa",
        // "chr14.subst.fa",
        // "chr15.subst.fa",
        // "chr16.subst.fa",
        // "chr17.subst.fa",
        // "chr18.subst.fa",
        // "chr19.subst.fa",
        // "chr20.subst.fa",
        // "chr21.subst.fa",
        // "chr22.subst.fa"
    };

    // Loop over each filename
    for (const auto& nome_arquivo : filenames) {
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

        // Ajustar inicio para alinhar com múltiplo de 3
        long data_inicial = inicio - header_size;
        long remainder = data_inicial % 3;
        if (remainder != 0) {
            inicio -= remainder;
            if (inicio < header_size) {
                inicio = header_size;
            }
        }

        // Ajustar fim para alinhar com múltiplo de 3
        long data_final = fim - header_size;
        remainder = data_final % 3;
        if (remainder != 0) {
            fim += (3 - remainder);
            if (fim > tamanho_arquivo) {
                fim = tamanho_arquivo;
            }
        }
        // Função para contabilizar o tempo demorado em determinado arquivo
        double inicio_tempo = MPI_Wtime();
        // Chamar a função para transcrever DNA para RNA
        std::vector<char> local_rna_sequence = transcritionDNAtoRNA(nome_arquivo, inicio, fim);
        double fim_tempo = MPI_Wtime();

        // Calcular o tempo gasto na transcrição
        double tempo_transcricao = fim_tempo - inicio_tempo;

        std::cout << "Tempo de transcrição para " << nome_arquivo << " no processo " << rank << ": " << tempo_transcricao << " segundos" << std::endl;

        size_t local_rna_length = local_rna_sequence.size();

        // Verificar se local_rna_length cabe em int
        int local_rna_length_int = size_t_to_int(local_rna_length);

        // Calcular o tempo gasto na Contagem de códons de início

        inicio_tempo = MPI_Wtime();
        // Contar o número de códon de início 'AUG' no segmento local
        long local_start_codon_count = count_start_codons(local_rna_sequence);

        fim_tempo = MPI_Wtime();

        double tempo_contagem = fim_tempo - inicio_tempo;

        std::cout << "Tempo de contagem de códons de início para " << nome_arquivo << " no processo " << rank << ": " << tempo_contagem << " segundos" << std::endl;

        // Reduzir as contagens locais para obter a contagem total
        long total_start_codon_count = 0;
        MPI_Reduce(&local_start_codon_count, &total_start_codon_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

        // Calcular o tempo gasto na tradução de RNA para aminoácidos
        inicio_tempo = MPI_Wtime();
        // Traduzir RNA para aminoácidos
        std::vector<char> local_amino_sequence = translate_RNA_to_aminoacids(local_rna_sequence);
        fim_tempo = MPI_Wtime();

        std::cout << "Tempo de tradução de RNA para aminoácidos para " << nome_arquivo << " no processo " << rank << ": " << fim_tempo - inicio_tempo << " segundos" << std::endl;

        size_t local_amino_length = local_amino_sequence.size();
        int local_amino_length_int = size_t_to_int(local_amino_length);

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

        // Coletar as sequências de aminoácidos de todos os processos
        std::vector<int> amino_recvcounts(size);
        std::vector<int> amino_displs(size);

        MPI_Gather(&local_amino_length_int, 1, MPI_INT, amino_recvcounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

        std::vector<char> total_amino_sequence;
        int total_amino_length = 0;
        if (rank == 0) {
            amino_displs[0] = 0;
            for (int i = 0; i < size; i++) {
                if (amino_recvcounts[i] < 0) {
                    std::cerr << "Erro: amino_recvcounts[" << i << "] é negativo." << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                if (i > 0) amino_displs[i] = amino_displs[i - 1] + amino_recvcounts[i - 1];
                total_amino_length += amino_recvcounts[i];
                if (total_amino_length < 0) {
                    std::cerr << "Erro: total_amino_length excede o limite do tipo int." << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
            total_amino_sequence.resize(total_amino_length);
        }

        MPI_Gatherv(local_amino_sequence.data(), local_amino_length_int, MPI_CHAR,
                    rank == 0 ? total_amino_sequence.data() : nullptr,
                    rank == 0 ? amino_recvcounts.data() : nullptr,
                    rank == 0 ? amino_displs.data() : nullptr,
                    MPI_CHAR,
                    0, MPI_COMM_WORLD);

        // Processo raiz escreve as sequências totais em arquivos
        if (rank == 0) {
            // Escrever sequência de RNA
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
                // Remover quebras de linha do header
                linha.erase(std::remove(linha.begin(), linha.end(), '\n'), linha.end());
                linha.erase(std::remove(linha.begin(), linha.end(), '\r'), linha.end());
                saida << linha; // Não adicionar std::endl
            }

            file.close();

            // Remover quebras de linha da sequência de RNA
            total_rna_sequence.erase(std::remove(total_rna_sequence.begin(), total_rna_sequence.end(), '\n'), total_rna_sequence.end());
            total_rna_sequence.erase(std::remove(total_rna_sequence.begin(), total_rna_sequence.end(), '\r'), total_rna_sequence.end());

            // Escrever a sequência de RNA
            saida.write(total_rna_sequence.data(), total_rna_sequence.size());
            if (!saida) {
                std::cerr << "Erro ao escrever no arquivo " << nome_saida << std::endl;
            }

            saida.close();

            // Escrever sequência de aminoácidos
            std::string nome_amino_saida = nome_arquivo + "-Amino.txt";

            std::ofstream amino_saida(nome_amino_saida, std::ios::binary);
            if (!amino_saida.is_open()) {
                std::cerr << "Erro ao criar o arquivo de saída " << nome_amino_saida << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            // Opcionalmente, escrever um header
            amino_saida << ">AminoAcidSequence"; // Não adicionar nova linha

            // Escrever a sequência de aminoácidos
            amino_saida.write(total_amino_sequence.data(), total_amino_sequence.size());
            if (!amino_saida) {
                std::cerr << "Erro ao escrever no arquivo " << nome_amino_saida << std::endl;
            }

            amino_saida.close();

            // Imprimir o total de proteínas inicializadas
            std::cout << "Arquivo: " << nome_arquivo << std::endl;
            std::cout << "Total de proteínas inicializadas (AUG encontrados): " << total_start_codon_count << std::endl;
        }
    }

    // Finalizar o MPI
    MPI_Finalize();
    return 0;
}
