#include "Memory.hpp"
#include "Decoder.hpp"
#include "Register.hpp"
#include "RegisterFile.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);
    Memory mem;
    Instruction tmp;
    tmp.parse(0);
    Register<int> r;
    r.set_nxt(1);
    r.tick();
    RegisterFile rf;
    std::cout << r.get_val() << std::endl;
}