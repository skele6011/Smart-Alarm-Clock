#include <iostream> 
#include <vector>

int main() {
    int num;
    std::vector<int> numbers;

    std::cout << "Enter integers. Once you want to be done, enter -1. \n";
    while (true) {
        std::cin >> num;
        if (num != -1) {
            numbers.push_back(num);
        } else {
            break;
        }
    }

    int sum = numbers[0];
    std::cout << "Numbers: " << sum << ", ";
    for (int i = 1; i < numbers.size(); i++) {
        std::cout << numbers[i] << ", ";
        sum = numbers[i] + sum;
    }
    std::cout << "\nSum: " << sum << "\n";
    std::cout << "Average: " << sum/numbers.size();

    for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end();) {
        if (*it % 2 == 0) { 
            it = numbers.erase(it);
        } else {
            ++it;
        }
    }

    std::cout << "\nNumbers without evens: ";
    for (auto& num : numbers) { 
        std::cout << num << ", ";
    }

    return 0; 
}