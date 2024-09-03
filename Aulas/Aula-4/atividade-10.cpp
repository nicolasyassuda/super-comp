#include <iostream>
#include <vector>
#include <string>

struct Item {
    std::string name;
    int quantity;
};

void addItem(std::vector<Item>& inventory, const std::string& name, int quantity) {
    Item newItem;
    newItem.name = name;
    newItem.quantity = quantity;
    inventory.push_back(newItem);
}

void removeItem(std::vector<Item>& inventory, const std::string& name) {
    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if (it->name == name) {
            inventory.erase(it);
            break;
        }
    }
}

void displayInventory(const std::vector<Item>& inventory) {
    std::cout << "Inventory:\n";
    for (const auto& item : inventory) {
        std::cout << item.name << " - Quantity: " << item.quantity << std::endl;
    }
}

int main() {
    std::vector<Item> inventory;

    while (true) {
        std::cout << "1. Add item\n";
        std::cout << "2. Remove item\n";
        std::cout << "3. Display inventory\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string name;
                int quantity;
                std::cout << "Enter item name: ";
                std::cin >> name;
                std::cout << "Enter item quantity: ";
                std::cin >> quantity;
                addItem(inventory, name, quantity);
                break;
            }
            case 2: {
                std::string name;
                std::cout << "Enter item name: ";
                std::cin >> name;
                removeItem(inventory, name);
                break;
            }
            case 3:
                displayInventory(inventory);
                break;
            case 4:
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}