#ifndef RIME_CALCULATOR_FUNCTIONS_H_
#define RIME_CALCULATOR_FUNCTIONS_H_

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace rime {

using CalcFunc = std::function<std::string(const std::vector<std::string>&)>;

struct FuncInfo {
  CalcFunc impl;
  std::string desc;
  int min_args;
  int max_args;     // -1 = variadic
  bool need_quote;  // auto-quote string params (base, unit)
};

extern const std::map<std::string, double> kConstants;
extern const std::map<std::string, FuncInfo> kCalcFunctions;

}  // namespace rime

#endif  // RIME_CALCULATOR_FUNCTIONS_H_
