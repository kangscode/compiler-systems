#include "tinyc.hpp"
#include <cctype>
#include <stdexcept>

namespace tinyc {
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> out;
    auto push = [&](TokenKind k, std::string lex, std::int64_t v = 0, std::size_t p = 0) { out.push_back({k, std::move(lex), v, p}); };
    while (pos_ < source_.size()) {
        char c = source_[pos_];
        if (std::isspace(static_cast<unsigned char>(c))) { ++pos_; continue; }
        std::size_t p = pos_;
        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::int64_t v = 0;
            while (pos_ < source_.size() && std::isdigit(static_cast<unsigned char>(source_[pos_]))) {
                v = v * 10 + (source_[pos_] - '0'); ++pos_;
            }
            push(TokenKind::Integer, source_.substr(p, pos_ - p), v, p); continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            while (pos_ < source_.size() && (std::isalnum(static_cast<unsigned char>(source_[pos_])) || source_[pos_] == '_')) ++pos_;
            std::string s = source_.substr(p, pos_ - p);
            if (s == "fn") push(TokenKind::Fn, s, 0, p);
            else if (s == "let") push(TokenKind::Let, s, 0, p);
            else if (s == "return") push(TokenKind::Return, s, 0, p);
            else push(TokenKind::Ident, s, 0, p);
            continue;
        }
        ++pos_;
        switch (c) {
            case '(': push(TokenKind::LParen, "(", 0, p); break;
            case ')': push(TokenKind::RParen, ")", 0, p); break;
            case '{': push(TokenKind::LBrace, "{", 0, p); break;
            case '}': push(TokenKind::RBrace, "}", 0, p); break;
            case ';': push(TokenKind::Semicolon, ";", 0, p); break;
            case '=': push(TokenKind::Equal, "=", 0, p); break;
            case '+': push(TokenKind::Plus, "+", 0, p); break;
            case '-': push(TokenKind::Minus, "-", 0, p); break;
            case '*': push(TokenKind::Star, "*", 0, p); break;
            case '/': push(TokenKind::Slash, "/", 0, p); break;
            default: throw std::runtime_error("unexpected character at position " + std::to_string(p));
        }
    }
    push(TokenKind::End, "", 0, pos_);
    return out;
}
}
