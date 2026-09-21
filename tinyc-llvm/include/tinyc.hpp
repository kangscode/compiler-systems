#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace tinyc {

enum class TokenKind { End, Fn, Let, Return, Ident, Integer, LParen, RParen, LBrace, RBrace, Semicolon, Equal, Plus, Minus, Star, Slash };
struct Token { TokenKind kind; std::string lexeme; std::int64_t value{}; std::size_t pos{}; };

class Lexer {
public:
    explicit Lexer(std::string source) : source_(std::move(source)) {}
    std::vector<Token> tokenize();
private:
    std::string source_;
    std::size_t pos_ = 0;
};

struct Expr { virtual ~Expr() = default; };
struct IntExpr final : Expr { std::int64_t value; explicit IntExpr(std::int64_t v): value(v) {} };
struct VarExpr final : Expr { std::string name; explicit VarExpr(std::string n): name(std::move(n)) {} };
struct BinaryExpr final : Expr { char op; std::unique_ptr<Expr> lhs, rhs; BinaryExpr(char o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r): op(o), lhs(std::move(l)), rhs(std::move(r)) {} };
struct Stmt { virtual ~Stmt() = default; };
struct LetStmt final : Stmt { std::string name; std::unique_ptr<Expr> expr; LetStmt(std::string n, std::unique_ptr<Expr> e): name(std::move(n)), expr(std::move(e)) {} };
struct ReturnStmt final : Stmt { std::unique_ptr<Expr> expr; explicit ReturnStmt(std::unique_ptr<Expr> e): expr(std::move(e)) {} };
struct Function { std::vector<std::unique_ptr<Stmt>> body; };

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}
    Function parse();
private:
    const Token& peek() const;
    Token take();
    bool match(TokenKind kind);
    void expect(TokenKind kind, std::string_view what);
    std::unique_ptr<Stmt> parseStmt();
    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseAddSub();
    std::unique_ptr<Expr> parseMulDiv();
    std::unique_ptr<Expr> parsePrimary();
    std::vector<Token> tokens_;
    std::size_t index_ = 0;
};

class Codegen {
public:
    std::string emit(const Function& fn);
private:
    std::string emitExpr(const Expr* expr);
    std::string newTemp();
    std::unordered_map<std::string, std::string> values_;
    unsigned nextTemp_ = 0;
    std::string out_;
};

} // namespace tinyc
