#include "tinyc.hpp"
#include <stdexcept>

namespace tinyc {
std::string Codegen::newTemp() { return "%t" + std::to_string(nextTemp_++); }

std::string Codegen::emitExpr(const Expr* expr) {
    if (auto* i = dynamic_cast<const IntExpr*>(expr)) return std::to_string(i->value);
    if (auto* v = dynamic_cast<const VarExpr*>(expr)) {
        auto it = values_.find(v->name); if (it == values_.end()) throw std::runtime_error("undefined variable: " + v->name);
        return it->second;
    }
    auto* b = dynamic_cast<const BinaryExpr*>(expr);
    if (!b) throw std::runtime_error("unknown expression node");
    const std::string lhs = emitExpr(b->lhs.get()); const std::string rhs = emitExpr(b->rhs.get());
    const std::string temp = newTemp();
    const char* opname = nullptr;
    switch (b->op) { case '+': opname = "add"; break; case '-': opname = "sub"; break; case '*': opname = "mul"; break; case '/': opname = "sdiv"; break; default: throw std::runtime_error("unknown binary operator"); }
    out_ += "  " + temp + " = " + opname + " i32 " + lhs + ", " + rhs + "\n";
    return temp;
}

std::string Codegen::emit(const Function& fn) {
    values_.clear(); nextTemp_ = 0; out_.clear();
    out_ = "define i32 @main() {\nentry:\n";
    bool hasReturn = false;
    for (const auto& stmt : fn.body) {
        if (auto* let = dynamic_cast<const LetStmt*>(stmt.get())) values_[let->name] = emitExpr(let->expr.get());
        else if (auto* ret = dynamic_cast<const ReturnStmt*>(stmt.get())) { out_ += "  ret i32 " + emitExpr(ret->expr.get()) + "\n"; hasReturn = true; }
    }
    if (!hasReturn) throw std::runtime_error("main must return a value");
    out_ += "}\n";
    return out_;
}
}
