#include <iostream>
#include <fstream>
#include <vector>

struct Item {
    int weight;
    int value;
};

int backpack(int weight, const std::vector<Item>& items, int n) {
    if (n == 0 || weight == 0) {
        return 0;
    }
    if (items[n - 1].weight > weight) {
        return backpack(weight, items, n - 1);
    } else {
        return std::max(items[n - 1].value + backpack(weight - items[n - 1].weight, items, n - 1),
                        backpack(weight, items, n - 1));
    }
}

int main(int argc, char* argv[]) {
    std::ifstream file("./entrada4.txt");
    int weight, n;
    file >> n >> weight;

    printf("Weight: %d\n", weight);
    printf("Number of items: %d\n", n);

    std::vector<Item> items(n);
    for (int i = 0; i < n; i++) {
        file >> items[i].weight >> items[i].value;
    }
    int result = backpack(weight, items, n);
    std::cout << "Maximum value: " << result << std::endl;
    return 0;
}