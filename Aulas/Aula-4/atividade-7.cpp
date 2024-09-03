#include <iostream>
#include <vector>

int main()
{

    std::vector<int> vetor;
    int num = 0;

    for (int i = 0; i <= 20; i++)
    {
        std::cout << "Digite o numero:";
        std::cin >> num;
        vetor.push_back(num);
    }

    // Bubble Sort
    for (int i = 0; i < vetor.size() - 1; i++)
    {
        for (int j = 0; j < vetor.size() - i - 1; j++)
        {
            if (vetor[j] > vetor[j + 1])
            {
                // Swap elements
                int temp = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = temp;
            }
        }
    }

    // Print sorted vector
    for (int i = 0; i < vetor.size(); i++)
    {
        std::cout << vetor[i] << " ";
    }

    return 0;
}