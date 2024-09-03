#include <iostream>
#include <vector>

int main(){

    std::vector<int> vetor;

    int result = 0;
    int num = 0;

    for(int i=0; i<=4; i++){
        std::cout << "Digite o numero:";
        std::cin >> num;
        vetor.push_back(num);
    }


    for(int j=0; j<=4;j++){
        result += vetor[j];
    }

    std::cout << result;

    return 0;
}