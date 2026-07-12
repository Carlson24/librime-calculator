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
static constexpr auto kTimeout = std::chrono::milliseconds(5000);

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

void calc_prefix_pop_back() {
  if (!g_calc_prefix.empty())
    g_calc_prefix.pop_back();
}

void calc_prefix_touch() {
  g_last_digit_time = std::chrono::steady_clock::now();
}

void calc_prefix_set(const std::string& s) {
  g_calc_prefix = s;
}

DigitCatcher::DigitCatcher(const Ticket& ticket) : Processor(ticket) {}

ProcessResult DigitCatcher::ProcessKeyEvent(const KeyEvent& key_event) {
  if (key_event.alt() || key_event.ctrl() || key_event.super() ||
      key_event.release())
    return kNoop;

  int ch = key_event.keycode();

  if (ch == XK_BackSpace) {
    Context* ctx = engine_->context();
    if (ctx->input().empty() && !ctx->HasMenu() && !g_calc_prefix.empty()) {
      g_calc_prefix.pop_back();
      g_last_digit_time = std::chrono::steady_clock::now();
    }
    return kNoop;
  }

  if (ch == XK_Return) {
    if (!g_calc_prefix.empty())
      calc_prefix_clear();
    return kNoop;
  }

  if (ch == '.') {
    Context* ctx = engine_->context();
    if (ctx->get_option("ascii_mode") || ctx->HasMenu())
      return kNoop;
    if (g_calc_prefix.empty())
      return kNoop;
    if (g_calc_prefix.back() == '.') {
      g_calc_prefix.pop_back();
      engine_->CommitText(string(1, '\b'));
      calc_prefix_clear();
      g_last_digit_time = std::chrono::steady_clock::now();
      return kNoop;
    }
    calc_prefix_append('.');
    engine_->CommitText(".");
    g_last_digit_time = std::chrono::steady_clock::now();
    return kAccepted;
  }

  if (ch < '0' || ch > '9') {
    if (ch != XK_plus && ch != XK_minus && ch != XK_asterisk &&
        ch != XK_slash && ch != XK_percent && ch != XK_asciicircum &&
        ch != XK_KP_Add && ch != XK_KP_Subtract &&
        ch != XK_KP_Multiply && ch != XK_KP_Divide &&
        ch != XK_division) {
      if (!g_calc_prefix.empty())
        calc_prefix_clear();
    }
    return kNoop;
  }

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
