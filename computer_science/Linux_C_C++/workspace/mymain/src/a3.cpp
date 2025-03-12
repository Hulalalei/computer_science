#include <iostream>
#include <ostream>
#include <string>
#include <vector>



int main() {
    std::vector<size_t> poss;
    std::string str, ans;
    std::getline(std::cin, str);

    for (int i = 0; i < str.size(); i ++) {
        if (str[i] == 'y' && str[i + 1] == 'o' && str[i + 2] == 'u') {
            ans += "we";
            i += 2;
        } else {
            ans += str[i];
        }
    }

    std::cout << ans << std::endl;
}
