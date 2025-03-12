#include <iostream>
#include <ostream>
#include <string>



int main() {
    int ans[50] = {};
    std::string str;
    std::getline(std::cin, str);

    for (int i = 0; i < str.size(); i ++) {
        ++ ans[static_cast<int>(str[i]) - 97];
    }

    int pos = -1, max = -1;
    for (int j = 0; j < 26; j ++) {
        ans[j] > max ? (pos = j, max = ans[j]) : 0;
    }
    std::cout << (char)(pos + 97);
}
