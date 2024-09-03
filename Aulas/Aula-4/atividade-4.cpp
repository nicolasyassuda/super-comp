#include <iostream>

int main(){

    std::cout << "Digite uma palavra para ser contata:\n";

    std::string word;

    std::cin >> word;

    char carac = word[0];
    int counter = 1;

    while (carac != '\0')
    {
        carac = word[counter];
        counter++;
    }
    std::cout << counter-1 << "\n";

    return 0;
}