#include <iostream>

int main(){

    std::cout << "Digite o primeiro numero da operação:\n";
    int num1;
    std::cin >> num1;
    
    std::cout << "Digite o segundo numero da operação:\n";
    int num2;
    std::cin >> num2;

    std::cout << "Qual será a operação da conta?? (+,-,*,/)\n";

    char oper;

    std::cin >> oper;

    float result;

    switch (oper)
    {
    case '+':
        result = num1 + num2;
        break;
    case '-':
        result = num1 - num2;
        break;
    case '*':
        result = num1 * num2;
        break;
    case '/':
        result = (float)num1/(float)num2;
        break;
    default:
        result = 0;
        break;
    }

    std::cout<<"RESULTADO DA OPERAÇÂO:";

    std::cout<<result << "\n";
    return 0;
}