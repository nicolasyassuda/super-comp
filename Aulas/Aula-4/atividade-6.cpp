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

    result = vetor[0];
    for(int j=1; j<=4;j++){
        if(result<vetor[j]){
            result=vetor[j];
        }
    }

    std::cout << result << "\n";

    return 0;
}