#include <iostream>
#include <vector>

int main()
{
    std::vector<std::vector<int>> matriz(3, std::vector<int>(3, 0));
    int somaDiagonalPrincipal = 0;

    // Preenche a matriz com os valores fornecidos pelo usuário
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            std::cout << "Digite o valor para a posicao [" << i << "][" << j << "]: ";
            std::cin >> matriz[i][j];
        }
    }

    // Calcula a soma dos elementos na diagonal principal
    for (int i = 0; i < 3; i++)
    {
        somaDiagonalPrincipal += matriz[i][i];
    }

    // Exibe a soma dos elementos na diagonal principal
    std::cout << "A soma dos elementos na diagonal principal e: " << somaDiagonalPrincipal << std::endl;

    return 0;
}
