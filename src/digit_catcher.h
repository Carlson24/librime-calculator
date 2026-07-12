#ifndef RIME_DIGIT_CATCHER_H_
#define RIME_DIGIT_CATCHER_H_

#include <rime/common.h>
#include <rime/component.h>
#include <rime/processor.h>

namespace rime {

class DigitCatcher : public Processor {
 public:
  explicit DigitCatcher(const Ticket& ticket);

  virtual ProcessResult ProcessKeyEvent(const KeyEvent& key_event);
};

const std::string& calc_prefix();
void calc_prefix_append(char ch);
void calc_prefix_clear();
bool calc_prefix_expired();

}  // namespace rime

#endif  // RIME_DIGIT_CATCHER_H_
