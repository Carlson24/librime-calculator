#include <rime/component.h>
#include <rime/registry.h>
#include <rime_api.h>

#include "calculator_translator.h"
#include "digit_catcher.h"

using namespace rime;

static void rime_calculator_initialize() {
  Registry& r = Registry::instance();
  r.Register("calculator_translator", new Component<CalculatorTranslator>);
  r.Register("digit_catcher", new Component<DigitCatcher>);
}

static void rime_calculator_finalize() {}

RIME_REGISTER_MODULE(calculator)
