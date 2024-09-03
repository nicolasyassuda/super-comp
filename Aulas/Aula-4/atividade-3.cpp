#include <iostream>

int main(){

    std::cout << "Digite o numero:\n";
    int num1;
    std::cin >> num1;

    if(num1%2==0){
        std::cout <<"Par\n";
    }else{
        std::cout <<"Impar\n";
    }
    return 0;
}