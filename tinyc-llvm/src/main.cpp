#include "tinyc.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc < 2) { std::cerr << "usage: tinyc <input.tiny> [-o output.ll]\n"; return 2; }
    std::string outPath;
    for (int i = 2; i + 1 < argc; ++i) if (std::string(argv[i]) == "-o") outPath = argv[i+1];
    try {
        std::ifstream in(argv[1]); if (!in) throw std::runtime_error("cannot open input file");
        std::stringstream ss; ss << in.rdbuf();
        tinyc::Lexer lexer(ss.str());
        tinyc::Parser parser(lexer.tokenize());
        tinyc::Function fn = parser.parse();
        std::string ir = tinyc::Codegen().emit(fn);
        if (outPath.empty()) std::cout << ir;
        else { std::ofstream out(outPath); if (!out) throw std::runtime_error("cannot open output file"); out << ir; }
    } catch (const std::exception& e) { std::cerr << "tinyc error: " << e.what() << '\n'; return 1; }
    return 0;
}
