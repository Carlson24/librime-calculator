#ifndef RIME_CALCULATOR_TRANSLATOR_H_
#define RIME_CALCULATOR_TRANSLATOR_H_

#include <rime/translator.h>

namespace rime {

class CalculatorTranslator : public Translator {
 public:
  CalculatorTranslator(const Ticket& ticket);

  virtual an<Translation> Query(const string& input, const Segment& segment);

 protected:
  void Initialize();

  string tag_;
  bool initialized_ = false;
  string prefix_;
  string tips_;
};

}  // namespace rime

#endif  // RIME_CALCULATOR_TRANSLATOR_H_
