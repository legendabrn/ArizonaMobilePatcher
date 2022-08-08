#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

const struct stInfo {
    std::vector<unsigned char> pattern;
    std::size_t length;
};

const stInfo convert(std::string_view ip) {
    std::vector<unsigned char> vec(ip.begin(), ip.end());
    vec.push_back(0x00);
    return { vec, ip.length() };
}

const std::vector<stInfo> arizona_ip{
    convert("185.169.134.3"),   convert("185.169.134.4"),   convert("185.169.134.43"),
    convert("185.169.134.44"),  convert("185.169.134.45"),  convert("185.169.134.5"),
    convert("185.169.134.59"),  convert("185.169.134.61"),  convert("185.169.134.107"),
    convert("185.169.134.109"), convert("185.169.134.166"), convert("185.169.134.171"),
    convert("185.169.134.172"), convert("185.169.134.173"), convert("185.169.134.174"),
    convert("80.66.82.191"),    convert("80.66.82.190"),    convert("80.66.82.188"),
    convert("80.66.82.168"),    convert("80.66.82.159"),    convert("80.66.82.200"),
    convert("80.66.82.144")
};

int main() {
    std::cout << "[patcher] created by legendabrn[08.08.2022]" << std::endl << std::endl;
    std::cout << "[patcher] please input ip:port = ";

    std::string cin_input;
    std::cin >> cin_input;
    /*
    * regex ip:port
    */
    const std::regex r(R"((.+):(.+))");
    std::smatch m;
    if (!std::regex_match(cin_input, m, r)) {
        std::cout << "[patcher] input IP:PORT" << std::endl;
        system("pause");
        return 0;
    }

    const std::string get_ip(m[1].str());
    const int get_port(std::stoi(m[2].str()));
    /*
    * read libsamp
    */
    std::ifstream input("libsamp.so", std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    input.close();
    if (buffer.size() == NULL) {
        std::cout << "[patcher] buffer libsamp is NULL" << std::endl;
        system("pause");
        return 0;
    }
    
    for (const auto& [pattern, length] : arizona_ip) {
        static int count = 0;
        count++;
        if (get_ip.length() == length) {
            auto result = std::search(buffer.begin(), buffer.end(), pattern.begin(), pattern.end());
            if (result == std::end(buffer)) {
                std::cout << "[patcher] error find pattern, continue" << std::endl;
                continue;
            }
            else {
                std::cout << "[patcher] pattern find [" << pattern.data() << "]" << std::endl << std::endl;
                /*
                * patch ip
                */
                std::cout << "[patcher] start patching IP" << std::endl;
                for (unsigned char s : get_ip) {
                    static int pos = 0;
                    buffer[result - buffer.begin() + pos] = s;
                    pos++;
                }
                std::cout << "[patcher] end patching IP" << std::endl << std::endl;
                /*
                * patch port
                */
                std::cout << "[patcher] start patching PORT" << std::endl;
                uintptr_t address_port = result - buffer.begin();
                auto bytes = reinterpret_cast<std::uint8_t*>(&address_port);
                std::vector<unsigned char> pattern_port { bytes[0], bytes[1], bytes[2], bytes[3] };
                auto result_port = std::search(buffer.begin(), buffer.end(), pattern_port.begin(), pattern_port.end());
                *reinterpret_cast<int*>(&buffer[result_port - buffer.begin() + 0x8]) = get_port;
                std::cout << "[patcher] end patching PORT" << std::endl << std::endl;
                /*
                * write to libsamp.so
                */
                std::cout << "[patcher] start write" << std::endl;
                std::ofstream output("libsamp.so", std::ios::binary);
                std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<unsigned char>(output));
                output.close();
                std::cout << "[patcher] end write" << std::endl;
                std::cout << "========================" << std::endl;
                std::cout << "SERVERID: " << count << std::endl;
                std::cout << "========================" << std::endl;
            }
            break;
        }
    }
    system("pause");
}