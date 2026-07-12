#include <cmath>
#include <regex>
#include <sstream>
#include <stack>
#include <stdexcept>

#include <rime/common.h>

#include "calculator_functions.h"
#include "calculator_parser.h"

namespace rime {

static double factorial_double(double n) {
  if (n < 0 || n != std::floor(n)) {
    throw std::runtime_error("阶乘参数必须为非负整数");
  }
  if (n == 0 || n == 1)
    return 1;
  double r = 1;
  for (int i = 2; i <= (int)n; ++i)
    r *= i;
  return r;
}

static double permutation_double(double n, double r) {
  if (n < 0 || r < 0 || n != std::floor(n) || r != std::floor(r))
    throw std::runtime_error("排列数参数必须为非负整数");
  if (r > n)
    throw std::runtime_error("r不能大于n");
  if (r == 0)
    return 1;
  double v = 1;
  for (int i = (int)(n - r + 1); i <= (int)n; ++i)
    v *= i;
  return v;
}

static double combination_double(double n, double r) {
  if (n < 0 || r < 0 || n != std::floor(n) || r != std::floor(r))
    throw std::runtime_error("组合数参数必须为非负整数");
  if (r > n)
    throw std::runtime_error("r不能大于n");
  r = std::min(r, n - r);
  if (r == 0)
    return 1;
  double v = 1;
  for (int i = 1; i <= (int)r; ++i) {
    v = v * (n - r + i) / i;
  }
  return v;
}

static double gcd_double(double a, double b) {
  if (a != std::floor(a) || b != std::floor(b))
    throw std::runtime_error("GCD参数必须为整数");
  a = std::abs(a);
  b = std::abs(b);
  while (b != 0) {
    double t = b;
    b = std::fmod(a, b);
    a = t;
  }
  return a;
}

static double lcm_double(double a, double b) {
  return a * b / gcd_double(a, b);
}

static double CeilD(double x) {
  return std::ceil(x);
}

static double FloorD(double x) {
  return std::floor(x);
}

static double SinD(double x) {
  return std::sin(x);
}

static double CosD(double x) {
  return std::cos(x);
}

static double TanD(double x) {
  return std::tan(x);
}

static double AsinD(double x) {
  return std::asin(x);
}

static double AcosD(double x) {
  return std::acos(x);
}

static double AtanD(double x) {
  return std::atan(x);
}

static double Atan2D(const std::vector<double>& args) {
  return std::atan2(args[0], args[1]);
}

static double SinhD(double x) {
  return std::sinh(x);
}

static double CoshD(double x) {
  return std::cosh(x);
}

static double TanhD(double x) {
  return std::tanh(x);
}

static double DegD(double x) {
  return x * 180.0 / M_PI;
}

static double RadD(double x) {
  return x * M_PI / 180.0;
}

static double ExpD(double x) {
  return std::exp(x);
}

static double LdexpD(const std::vector<double>& args) {
  return args[0] * std::pow(2.0, args[1]);
}

static double LogD(const std::vector<double>& args) {
  if (args[0] <= 0 || args[1] <= 0)
    throw std::runtime_error("对数参数必须为正数");
  return std::log(args[1]) / std::log(args[0]);
}

static double LogeD(double x) {
  if (x <= 0)
    throw std::runtime_error("对数参数必须为正数");
  return std::log(x);
}

static double LogtD(double x) {
  if (x <= 0)
    throw std::runtime_error("对数参数必须为正数");
  return std::log10(x);
}

static double SqrtD(double x) {
  if (x < 0)
    throw std::runtime_error("算术平方根参数必须为非负数");
  return std::sqrt(x);
}

static double NrootD(const std::vector<double>& args) {
  double x = args[0], n = args[1];
  if ((int)n % 2 == 0 && x < 0)
    throw std::runtime_error("偶次方根参数必须为非负数");
  if (x < 0)
    return -std::pow(-x, 1.0 / n);
  return std::pow(x, 1.0 / n);
}

static double PowerD(double a, double b) {
  return std::pow(a, b);
}

static double FmodD(double a, double b) {
  double r = std::fmod(a, b);
  if (r < 0)
    r += b;
  return r;
}

static double AvgD(const std::vector<double>& args) {
  double s = 0;
  for (auto v : args)
    s += v;
  return s / args.size();
}

CalculatorParser::CalculatorParser() {
  constants_ = kConstants;

  funcs_["sin"] = [](const auto& a) { return SinD(a[0]); };
  funcs_["cos"] = [](const auto& a) { return CosD(a[0]); };
  funcs_["tan"] = [](const auto& a) { return TanD(a[0]); };
  funcs_["asin"] = [](const auto& a) { return AsinD(a[0]); };
  funcs_["acos"] = [](const auto& a) { return AcosD(a[0]); };
  funcs_["atan"] = [](const auto& a) { return AtanD(a[0]); };
  funcs_["atan2"] = [](const auto& a) { return Atan2D(a); };
  funcs_["sinh"] = [](const auto& a) { return SinhD(a[0]); };
  funcs_["cosh"] = [](const auto& a) { return CoshD(a[0]); };
  funcs_["tanh"] = [](const auto& a) { return TanhD(a[0]); };
  funcs_["deg"] = [](const auto& a) { return DegD(a[0]); };
  funcs_["rad"] = [](const auto& a) { return RadD(a[0]); };
  funcs_["exp"] = [](const auto& a) { return ExpD(a[0]); };
  funcs_["ldexp"] = [](const auto& a) { return LdexpD(a); };
  funcs_["log"] = [](const auto& a) { return LogD(a); };
  funcs_["loge"] = [](const auto& a) { return LogeD(a[0]); };
  funcs_["logt"] = [](const auto& a) { return LogtD(a[0]); };
  funcs_["sqrt"] = [](const auto& a) { return SqrtD(a[0]); };
  funcs_["nroot"] = [](const auto& a) { return NrootD(a); };
  funcs_["ceil"] = [](const auto& a) { return CeilD(a[0]); };
  funcs_["floor"] = [](const auto& a) { return FloorD(a[0]); };
  funcs_["fact"] = [](const auto& a) { return factorial_double(a[0]); };
  funcs_["perm"] = [](const auto& a) { return permutation_double(a[0], a[1]); };
  funcs_["comb"] = [](const auto& a) { return combination_double(a[0], a[1]); };
  funcs_["avg"] = [](const auto& a) { return AvgD(a); };
  funcs_["mod"] = [](const auto& a) { return FmodD(a[0], a[1]); };
  funcs_["abs"] = [](const auto& a) { return std::abs(a[0]); };
  funcs_["sqsum"] = [](const auto& a) {
    double n = a[0];
    return n * (n + 1) * (2 * n + 1) / 6;
  };
  funcs_["cbsum"] = [](const auto& a) {
    double n = a[0];
    return std::pow(n * (n + 1), 2) / 4;
  };
  funcs_["qpsum"] = [](const auto& a) {
    double n = a[0];
    return n * (n + 1) * (2 * n + 1) * (3 * n * n + 3 * n - 1) / 30;
  };
  funcs_["osqsum"] = [](const auto& a) {
    double n = a[0];
    return n * (4 * n * n - 1) / 3;
  };
  funcs_["esqsum"] = [](const auto& a) {
    double n = a[0];
    return 2 * n * (n + 1) * (2 * n + 1) / 3;
  };
  funcs_["ocbsum"] = [](const auto& a) {
    double n = a[0];
    return n * n * (2 * n * n - 1);
  };
  funcs_["ecbsum"] = [](const auto& a) {
    double n = a[0];
    return 2 * std::pow(n * (n + 1), 2);
  };
  funcs_["oqpsum"] = [](const auto& a) {
    double n = a[0];
    return (48 * std::pow(n, 5) - 40 * std::pow(n, 3) + 7 * n) / 15;
  };
  funcs_["eqpsum"] = [](const auto& a) {
    double n = a[0];
    return 8 * n * (n + 1) * (2 * n + 1) * (3 * n * n + 3 * n - 1) / 15;
  };
}

std::string CalculatorParser::FormatNumber(double x) {
  if (std::isinf(x) || std::isnan(x))
    return "错误";
  std::string s = std::to_string(x);
  auto dot = s.find('.');
  if (dot == std::string::npos)
    return s;
  while (s.size() > 1 && s.back() == '0')
    s.pop_back();
  if (s.back() == '.')
    s.pop_back();
  return s;
}

static double fn(double x) {
  if (std::isinf(x) || std::isnan(x))
    return x;
  if (x == std::floor(x))
    return x;
  double t = std::round(x * 1e12) / 1e12;
  return t;
}

std::string CalculatorParser::Preprocess(const std::string& expr) {
  static std::regex fact_re(R"((\d+)!)");
  std::string r = std::regex_replace(expr, fact_re, "fact($1)");
  return r;
}

std::vector<CalculatorParser::Token> CalculatorParser::Tokenize(
    const std::string& expr) {
  std::vector<Token> tokens;
  size_t i = 0;
  while (i < expr.size()) {
    char c = expr[i];
    if (std::isspace(c)) {
      ++i;
      continue;
    }
    if (std::isdigit(c) || c == '.') {
      std::string num;
      while (i < expr.size() && (std::isdigit(expr[i]) || expr[i] == '.')) {
        num += expr[i++];
      }
      tokens.push_back({NUMBER, num});
      continue;
    }
    if (std::isalpha(c)) {
      std::string name;
      while (i < expr.size() && std::isalnum(expr[i])) {
        name += expr[i++];
      }
      if (i < expr.size() && expr[i] == '(') {
        tokens.push_back({FUNCTION, name});
      } else {
        tokens.push_back({CONSTANT, name});
      }
      continue;
    }
    if (c == '(') {
      tokens.push_back({LPAREN, "("});
      ++i;
      continue;
    }
    if (c == ')') {
      tokens.push_back({RPAREN, ")"});
      ++i;
      continue;
    }
    if ((unsigned char)c == 0xC3 && i + 1 < expr.size() &&
        (unsigned char)expr[i + 1] == 0xB7) {
      tokens.push_back({OPERATOR, "÷"});
      i += 2;
      continue;
    }
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^') {
      std::string op(1, c);
      tokens.push_back({OPERATOR, op});
      ++i;
      continue;
    }
    ++i;
  }
  tokens.push_back({END, ""});
  return tokens;
}

int CalculatorParser::Precedence(const std::string& op) {
  if (op == "+" || op == "-")
    return 1;
  if (op == "*" || op == "/" || op == "%" || op == "÷")
    return 2;
  if (op == "^")
    return 3;
  if (op == "u")
    return 4;  // unary minus
  return 0;
}

bool CalculatorParser::IsLeftAssoc(const std::string& op) {
  return op != "^";
}

bool CalculatorParser::ToRPN(const std::vector<Token>& infix,
                             std::vector<Token>& rpn) {
  std::stack<Token> ops;
  bool expect_operand = true;

  for (size_t i = 0; i < infix.size(); ++i) {
    const Token& t = infix[i];

    if (t.type == NUMBER || t.type == CONSTANT) {
      rpn.push_back(t);
      expect_operand = false;
      continue;
    }

    if (t.type == FUNCTION) {
      ops.push(t);
      expect_operand = true;
      continue;
    }

    if (t.type == OPERATOR) {
      if (expect_operand && t.value == "-") {
        Token ut = {OPERATOR, "u"};
        ops.push(ut);
        continue;
      }
      if (expect_operand && t.value == "+") {
        continue;
      }

      while (!ops.empty() && ops.top().type != LPAREN) {
        const auto& top = ops.top();
        if (top.type == FUNCTION)
          break;
        int tp = Precedence(top.value);
        int cp = Precedence(t.value);
        if ((IsLeftAssoc(t.value) && cp <= tp) || cp < tp) {
          rpn.push_back(top);
          ops.pop();
        } else {
          break;
        }
      }
      ops.push(t);
      expect_operand = true;
      continue;
    }

    if (t.type == LPAREN) {
      ops.push(t);
      expect_operand = true;
      continue;
    }

    if (t.type == RPAREN) {
      while (!ops.empty() && ops.top().type != LPAREN) {
        rpn.push_back(ops.top());
        ops.pop();
      }
      if (ops.empty())
        return false;
      ops.pop();  // pop LPAREN
      if (!ops.empty() && ops.top().type == FUNCTION) {
        rpn.push_back(ops.top());
        ops.pop();
      }
      expect_operand = false;
      continue;
    }

    if (t.type == END) {
      while (!ops.empty()) {
        if (ops.top().type == LPAREN)
          return false;
        rpn.push_back(ops.top());
        ops.pop();
      }
      continue;
    }
  }
  return true;
}

double CalculatorParser::EvalRPN(const std::vector<Token>& rpn) {
  std::stack<double> st;

  for (const auto& t : rpn) {
    if (t.type == NUMBER) {
      st.push(std::stod(t.value));
    } else if (t.type == CONSTANT) {
      auto it = constants_.find(t.value);
      if (it != constants_.end()) {
        st.push(it->second);
      } else {
        throw std::runtime_error("未知常量: " + t.value);
      }
    } else if (t.type == FUNCTION) {
      std::vector<double> args;
      // peek how many args needed by checking args before LPAREN in RPN
      auto fit = funcs_.find(t.value);
      if (fit == funcs_.end()) {
        throw std::runtime_error("未知函数: " + t.value);
      }
      // For simplicity, we push all stack values as args,
      // but we need to know how many args the function expects.
      // Strategy: pop 1 or 2 args based on known arity patterns.
      // For variadic, we use a sentinel on the stack.
      // Here we use a simpler approach: pop the number of comma-separated
      // values. Actually with shunting-yard, the function token consumes all
      // preceding expressions until a LPAREN-marked separation. We'll handle
      // this by having the function and its args stored differently.

      // For now, handle common arities:
      static const std::map<std::string, int> kArities = {
          {"sin", 1},    {"cos", 1},    {"tan", 1},    {"asin", 1},
          {"acos", 1},   {"atan", 1},   {"atan2", 2},  {"sinh", 1},
          {"cosh", 1},   {"tanh", 1},   {"deg", 1},    {"rad", 1},
          {"exp", 1},    {"ldexp", 2},  {"log", 2},    {"loge", 1},
          {"logt", 1},   {"sqrt", 1},   {"nroot", 2},  {"ceil", 1},
          {"floor", 1},  {"fact", 1},   {"perm", 2},   {"comb", 2},
          {"mod", 2},    {"abs", 1},    {"sqsum", 1},  {"cbsum", 1},
          {"qpsum", 1},  {"osqsum", 1}, {"esqsum", 1}, {"ocbsum", 1},
          {"ecbsum", 1}, {"oqpsum", 1}, {"eqpsum", 1},
      };

      auto ait = kArities.find(t.value);
      int arity = (ait != kArities.end()) ? ait->second : 1;

      for (int j = arity - 1; j >= 0; --j) {
        if (st.empty())
          throw std::runtime_error("参数不足: " + t.value);
        args.insert(args.begin(), st.top());
        st.pop();
      }
      st.push(fit->second(args));
    } else if (t.type == OPERATOR) {
      if (t.value == "u") {
        if (st.empty())
          throw std::runtime_error("语法错误");
        double a = st.top();
        st.pop();
        st.push(-a);
      } else {
        if (st.size() < 2)
          throw std::runtime_error("操作数不足");
        double b = st.top();
        st.pop();
        double a = st.top();
        st.pop();
        if (t.value == "+")
          st.push(a + b);
        else if (t.value == "-")
          st.push(a - b);
        else if (t.value == "*")
          st.push(a * b);
        else if (t.value == "/" || t.value == "÷") {
          if (b == 0)
            throw std::runtime_error("除零错误");
          st.push(a / b);
        } else if (t.value == "%") {
          if (b == 0)
            throw std::runtime_error("除零错误");
          st.push(std::fmod(a, b));
        } else if (t.value == "^")
          st.push(std::pow(a, b));
      }
    }
  }
  if (st.empty())
    throw std::runtime_error("表达式错误");
  return st.top();
}

std::string CalculatorParser::Evaluate(const std::string& expression) {
  try {
    std::string pre = Preprocess(expression);
    auto tokens = Tokenize(pre);
    std::vector<Token> rpn;
    if (!ToRPN(tokens, rpn)) {
      return "解析失败";
    }
    double result = EvalRPN(rpn);
    return FormatNumber(fn(result));
  } catch (const std::exception& e) {
    return "执行错误";
  }
}

}  // namespace rime
