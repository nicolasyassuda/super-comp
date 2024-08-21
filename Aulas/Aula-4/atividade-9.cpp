#include <iostream>
#include <string>

bool isPalindrome(const std::string& str)
{
    int left = 0;
    int right = str.length() - 1;

    while (left < right)
    {
        if (str[left] != str[right])
        {
            return false;
        }
        left++;
        right--;
    }

    return true;
}

int main()
{
    std::string str;

    std::cout << "Digite uma string: ";
    std::cin >> str;

    if (isPalindrome(str))
    {
        std::cout << "A string é um palíndromo." << std::endl;
    }
    else
    {
        std::cout << "A string não é um palíndromo." << std::endl;
    }

    return 0;
}