#include "calculator.hpp"

#include <algorithm>
#include <cmath>
#include <vector>
#include <variant>
#include <QString>

namespace {

using ExpressionNode = std::variant<double, char>;
using ExpressionVec = std::vector<ExpressionNode>;
using NumberStack = std::vector<double>;
using OpStack = std::vector<char>;

bool IsValidOp(char c) {
    return c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/';
}

bool IsZero(double n) {
    return std::fabs(n) <= std::numeric_limits<double>::epsilon();
}

// Parse @number to double, then push it back into @expr
void ParseAndAddNumber(ExpressionVec& expr, std::string& number) {
    if (number.length() > 0) {
        // Since std::atof and std::stod depends on locale and Qt environment changes it,
        // use QString::toDouble method instead.
        auto converted = QString::fromUtf8(number.c_str()).toDouble();
        expr.emplace_back(converted);
        number = "";
    }
}

// Parse expression @expr as string into ExpressionVec
// @expr must be valid math expression that contains float numbers, +-*/ and brackets
ExpressionVec CreateExpressionVec(const std::string& expr) {
    ExpressionVec vec;
    vec.reserve(expr.length());
    std::string number;

    for (auto c : expr) {
        if (std::isspace(c)) {
            ParseAndAddNumber(vec, number);
        } else if (std::isdigit(c)) {
            number += c;
        } else if (c == ',' || c == '.') {
            number += '.';
        } else if (IsValidOp(c)) {
            ParseAndAddNumber(vec, number);
            vec.emplace_back(c);
        } else {
            throw std::runtime_error("Invalid character in expression");
        }
    }
    // In most cases, number ends an expression
    ParseAndAddNumber(vec, number);

    vec.shrink_to_fit();
    return vec;
}

// Resolve negations in front of numbers, eg. cases: -5, --5, +5, +--+-+5 etc.
void ResolveLastNumberNegation(NumberStack& numbers, OpStack& ops) {
    while (ops.size() >= numbers.size()) {
        auto last = ops.back();
        if (last != '+' && last != '-') {
            throw std::runtime_error("Invalid expression.");
        }
        ops.pop_back();
        if (last == '-') {
            numbers.back() *= -1.0;
        }
    }
}

// Resolve single operation. Take last two numbers from @numbers and perform last operation from @ops.
void ResolveOperation(NumberStack& numbers, OpStack& ops) {
    auto lastNumber = numbers.back();
    auto lastOp = ops.back();
    numbers.pop_back();
    ops.pop_back();
    if (lastOp == '*') {
        numbers.back() *= lastNumber;
    } else if (lastOp == '/') {
        if (IsZero(lastNumber)) {
            throw std::runtime_error("Zero division detected");
        }
        numbers.back() /= lastNumber;
    } else if (lastOp == '-') {
        numbers.back() -= lastNumber;
    } else {
        numbers.back() += lastNumber;
    }
}

// This function is called when new number is added to @numbers stack.
void PerformCalculationStep(NumberStack& numbers, OpStack& ops, bool finished = false) {
    if (numbers.empty()) {
        throw std::runtime_error("Numbers are empty");
    }
    ResolveLastNumberNegation(numbers, ops);
    if (ops.size() != numbers.size() - 1) {
        throw std::runtime_error("Invalid expresion. Numbers are not paired with operators");
    }
    // Leave single number alone
    if (ops.empty()) {
        return;
    }
    if (ops.back() == '*' || ops.back() == '/' || finished) {
        ResolveOperation(numbers, ops);
        return;
    }
    // More than one additions or subtractions present in expression.
    if (ops.size() > 1) {
        auto lastOp = ops.back();
        auto lastNumber = numbers.back();
        ops.pop_back();
        numbers.pop_back();
        ResolveOperation(numbers, ops);
        ops.emplace_back(lastOp);
        numbers.emplace_back(lastNumber);
    }
}

double SolveSimpleExpression(const ExpressionVec& vec) {
    NumberStack numbers;
    OpStack ops;
    bool numberProcessed = false;

    for (const auto& e : vec) {
        // not a number
        if (std::holds_alternative<char>(e)) {
            ops.emplace_back(std::get<char>(e));
            numberProcessed = false;
            continue;
        }
        numbers.emplace_back(std::get<double>(e));
        PerformCalculationStep(numbers, ops);
        numberProcessed = true;
    }
    if (!numberProcessed) {
        throw std::runtime_error("Invalid expression");
    }
    PerformCalculationStep(numbers, ops, true);
    if (numbers.size() != 1) {
        throw std::runtime_error("Calculation error");
    }
    return numbers.front();
}

}

double CalculateExpression(const std::string& stringExpr) {
    if (stringExpr.empty()) {
        throw std::runtime_error("Given expression is empty");
    }
    auto vec = CreateExpressionVec(stringExpr);
    std::vector<size_t> leftBrackets;
    leftBrackets.reserve(vec.size());

    // Divide expression into smaller expressions according to brackets
    // For each bracket solve the expression separately, then replace it with it's result
    for (size_t i = 0u; i < vec.size(); i++) {
        const auto& expr = vec[i];
        if (!std::holds_alternative<char>(expr)) {
            continue;
        }
        char c = std::get<char>(expr);
        if (c == '(') {
            leftBrackets.emplace_back(i);
        } else if (c == ')') {
            if (leftBrackets.size() == 0) {
                throw std::runtime_error("Invalid expression - more right brackets than left");
            }
            // Solve expression inside brackets
            auto begin = leftBrackets.back() + 1;
            leftBrackets.pop_back();
            vec[begin - 1] = SolveSimpleExpression(ExpressionVec(vec.begin() + begin, vec.begin() + i));
            // Replace the solved expression with it's result
            vec.erase(vec.begin() + begin, vec.begin() + i + 1);
            i -= i - begin + 1;
        }
    }
    if (leftBrackets.size() > 0) {
        throw std::runtime_error("Invalid expression - more left brackets than right");
    }

    // Remaining expression is without brackets
    return SolveSimpleExpression(vec);
}
