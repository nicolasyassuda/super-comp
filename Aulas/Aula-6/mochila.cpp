#include <iostream>
#include <fstream>

int backpack(int peso, int peso_atual[], int val_atual[], int n)
{
    if (n == 0 || peso == 0)
    {
        return 0;
    }
    if (peso_atual[n - 1] > peso)
    {
        return backpack(peso, peso_atual, val_atual, n - 1);
    }
    else
    {
        return std::max(val_atual[n - 1] + backpack(peso - peso_atual[n - 1], peso_atual, val_atual, n - 1), backpack(peso, peso_atual, val_atual, n - 1));
    }
}

int main(int argc, char *argv[])
{
    std::ifstream file("./entrada.txt");
    int peso, n;
    file >> peso >> n;
    
    printf("Peso: %d\n", peso);
    printf("Numero de itens: %d\n", n);

    int peso_atual[n], val_atual[n];
    for (int i = 0; i < n; i++)
    {
        file >> peso_atual[i] >> val_atual[i];
    }
    int result = backpack(peso, peso_atual, val_atual, n);
    std::cout << "Maximum value: " << result << std::endl;
    return 0;
}