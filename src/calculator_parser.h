#ifndef RIME_CALCULATOR_PARSER_H_
#define RIME_CALCULATOR_PARSER_H_

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace rime {

class CalculatorParser {
 public:
  using NumericFunc = std::function<double(const std::vector<double>&)>;

  CalculatorParser();

  std::string Evaluate(const std::string& expression);
  static std::string FormatNumber(double x);

 private:
  enum TokenType { NUMBER, OPERATOR, LPAREN, RPAREN, FUNCTION, CONSTANT, END };

  struct Token {
    TokenType type;
    std::string value;
  };

  std::vector<Token> Tokenize(const std::string& expr);
  bool ToRPN(const std::vector<Token>& infix, std::vector<Token>& rpn);
  double EvalRPN(const std::vector<Token>& rpn);
  std::string Preprocess(const std::string& expr);

  static int Precedence(const std::string& op);
  static bool IsLeftAssoc(const std::string& op);

  std::map<std::string, double> constants_;
  std::map<std::string, NumericFunc> funcs_;
};

}  // namespace rime

#endif  // RIME_CALCULATOR_PARSER_H_
