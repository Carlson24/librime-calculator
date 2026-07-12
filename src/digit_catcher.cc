#include <chrono>

#include <rime/common.h>
#include <rime/composition.h>
#include <rime/context.h>
#include <rime/engine.h>
#include <rime/key_event.h>

#include "digit_catcher.h"

namespace rime {

static std::string g_calc_prefix;
static std::chrono::steady_clock::time_point g_last_digit_time;
static constexpr auto kTimeout = std::chrono::milliseconds(2000);

const std::string& calc_prefix() {
  return g_calc_prefix;
}

void calc_prefix_append(char ch) {
  g_calc_prefix += ch;
}

void calc_prefix_clear() {
  g_calc_prefix.clear();
}

bool calc_prefix_expired() {
  auto elapsed = std::chrono::steady_clock::now() - g_last_digit_time;
  return elapsed > kTimeout;
}

DigitCatcher::DigitCatcher(const Ticket& ticket) : Processor(ticket) {}

ProcessResult DigitCatcher::ProcessKeyEvent(const KeyEvent& key_event) {
  if (key_event.alt() || key_event.ctrl() || key_event.super() ||
      key_event.release())
    return kNoop;

  int ch = key_event.keycode();
  if (ch < '0' || ch > '9')
    return kNoop;

  Context* ctx = engine_->context();

  if (ctx->get_option("ascii_mode"))
    return kNoop;

  if (ctx->HasMenu())
    return kNoop;

  if (ctx->input().empty()) {
    auto elapsed = std::chrono::steady_clock::now() - g_last_digit_time;
    if (elapsed > kTimeout) {
      calc_prefix_clear();
    }
  }

  calc_prefix_append((char)ch);

  engine_->CommitText(string(1, (char)ch));
  g_last_digit_time = std::chrono::steady_clock::now();
  return kAccepted;
}

}  // namespace rime
