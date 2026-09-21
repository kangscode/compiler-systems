#include "tinyc.hpp"
#include <stdexcept>

namespace tinyc {
const Token& Parser::peek() const { return tokens_.at(index_); }
Token Parser::take() { return tokens_.at(index_++); }
bool Parser::match(TokenKind kind) { if (peek().kind != kind) return false; ++index_; return true; }
void Parser::expect(TokenKind kind, std::string_view what) { if (!match(kind)) throw std::runtime_error("expected " + std::string(what) + " near token '" + peek().lexeme + "'"); }

Function Parser::parse() {
    expect(TokenKind::Fn, "fn");
    if (take().lexeme != "main") throw std::runtime_error("only main is supported");
    expect(TokenKind::LParen, "("); expect(TokenKind::RParen, ")"); expect(TokenKind::LBrace, "{");
    Function f;
    while (!match(TokenKind::RBrace)) f.body.push_back(parseStmt());
    expect(TokenKind::End, "end of file");
    return f;
}

std::unique_ptr<Stmt> Parser::parseStmt() {
    if (match(TokenKind::Let)) {
        Token name = take();
        if (name.kind != TokenKind::Ident) throw std::runtime_error("expected identifier after let");
        expect(TokenKind::Equal, "=");
        auto e = parseExpr(); expect(TokenKind::Semicolon, ";");
        return std::make_unique<LetStmt>(name.lexeme, std::move(e));
    }
    if (match(TokenKind::Return)) {
        auto e = parseExpr(); expect(TokenKind::Semicolon, ";");
        return std::make_unique<ReturnStmt>(std::move(e));
    }
    throw std::runtime_error("expected statement near token '" + peek().lexeme + "'");
}
std::unique_ptr<Expr> Parser::parseExpr() { return parseAddSub(); }
std::unique_ptr<Expr> Parser::parseAddSub() {
    auto lhs = parseMulDiv();
    while (peek().kind == TokenKind::Plus || peek().kind == TokenKind::Minus) {
        char op = take().lexeme[0]; auto rhs = parseMulDiv(); lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}
std::unique_ptr<Expr> Parser::parseMulDiv() {
    auto lhs = parsePrimary();
    while (peek().kind == TokenKind::Star || peek().kind == TokenKind::Slash) {
        char op = take().lexeme[0]; auto rhs = parsePrimary(); lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}
std::unique_ptr<Expr> Parser::parsePrimary() {
    Token t = take();
    if (t.kind == TokenKind::Integer) return std::make_unique<IntExpr>(t.value);
    if (t.kind == TokenKind::Ident) return std::make_unique<VarExpr>(t.lexeme);
    if (t.kind == TokenKind::LParen) { auto e = parseExpr(); expect(TokenKind::RParen, ")"); return e; }
    throw std::runtime_error("expected expression near token '" + t.lexeme + "'");
}
}
