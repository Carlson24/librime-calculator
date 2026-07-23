#include <rime/candidate.h>
#include <rime/engine.h>
#include <rime/schema.h>
#include <rime/segmentation.h>
#include <rime/context.h>
#include <rime/ticket.h>
#include <rime/translation.h>

#include "calculator_functions.h"
#include "calculator_parser.h"
#include "calculator_translator.h"
#include "digit_catcher.h"

namespace rime {

static std::string g_chain_result;
static std::string g_last_op_input;

void calc_translator_clear_chain() {
  g_chain_result.clear();
  g_last_op_input.clear();
}

CalculatorTranslator::CalculatorTranslator(const Ticket& ticket)
    : Translator(ticket), tag_("calculator"), prefix_("V") {
  if (!ticket.schema)
    return;
  Config* config = ticket.schema->config();
  config->GetString(name_space_ + "/tag", &tag_);
  std::string cfg_prefix;
  if (config->GetString(name_space_ + "/prefix", &cfg_prefix)) {
    prefix_ = cfg_prefix;
  } else {
    std::string calc_pat;
    if (config->GetString("recognizer/patterns/calculator", &calc_pat) &&
        !calc_pat.empty()) {
      auto pos = calc_pat.find_first_not_of("^");
      if (pos != std::string::npos) {
        size_t end = pos;
        while (end < calc_pat.size() && std::isalpha(calc_pat[end]))
          ++end;
        if (end > pos) {
          std::string extracted = calc_pat.substr(pos, end - pos);
          auto eq = calc_pat.find('=', pos);
          if (eq != std::string::npos && eq < end)
            extracted = calc_pat.substr(pos, eq - pos);
          if (!extracted.empty())
            prefix_ = extracted;
        }
      }
    }
  }
}

void CalculatorTranslator::Initialize() {
  initialized_ = true;
  if (!engine_)
    return;
  Config* config = engine_->schema()->config();
  if (!config)
    return;
  config->GetString(name_space_ + "/tips", &tips_);
  if (tips_.empty())
    config->GetString("calculator/tips", &tips_);
}

static std::string strip_prefix(const std::string& input,
                                const std::string& prefix) {
  if (!prefix.empty() && input.size() >= prefix.size() &&
      input.compare(0, prefix.size(), prefix) == 0) {
    return input.substr(prefix.size());
  }
  return input;
}

static bool is_identifier(const std::string& s) {
  if (s.empty())
    return false;
  if (!std::isalpha(s[0]))
    return false;
  for (char c : s) {
    if (!std::isalnum(c) && c != '_')
      return false;
  }
  return true;
}

static std::string make_sig(const std::string& name, const FuncInfo& info) {
  std::string s = name + "(";
  if (info.max_args == 0) {
  } else if (info.max_args > 0) {
    for (int i = 0; i < info.max_args; ++i) {
      if (i > 0)
        s += ",";
      s += (char)('a' + i);
    }
  } else {
    for (int i = 0; i < info.min_args; ++i) {
      if (i > 0)
        s += ",";
      s += (char)('a' + i);
    }
    if (info.min_args > 0)
      s += ",";
    s += "...";
  }
  s += "): " + info.desc;
  return s;
}

static void add_candidates(an<FifoTranslation>& translation,
                           const std::string& input,
                           int start,
                           int end,
                           const std::string& text,
                           const std::string& comment = "") {
  auto cand = New<SimpleCandidate>("calculator", start, end, text, comment);
  cand->set_preedit(input);
  translation->Append(cand);
}

static void add_result(an<FifoTranslation>& translation,
                       const std::string& input,
                       int start,
                       int end,
                       const std::string& result,
                       const std::string& expression = "") {
  add_candidates(translation, input, start, end, result);
  if (!expression.empty()) {
    add_candidates(translation, input, start, end, expression + "=" + result);
  }
}

bool is_number_str(const std::string& s) {
  if (s.empty())
    return false;
  size_t i = 0;
  if (s[i] == '-')
    ++i;
  bool has_dot = false, has_digit = false;
  for (; i < s.size(); ++i) {
    if (std::isdigit(s[i])) {
      has_digit = true;
    } else if (s[i] == '.' && !has_dot) {
      has_dot = true;
    } else {
      return false;
    }
  }
  return has_digit;
}

static size_t find_matching_paren(const std::string& s, size_t open_pos) {
  int depth = 0;
  for (size_t i = open_pos; i < s.size(); ++i) {
    if (s[i] == '(')
      ++depth;
    if (s[i] == ')') {
      --depth;
      if (depth == 0)
        return i;
    }
  }
  return std::string::npos;
}

static bool has_unmatched_open_paren(const std::string& s) {
  int depth = 0;
  for (char c : s) {
    if (c == '(')
      ++depth;
    if (c == ')') {
      if (depth == 0)
        return true;
      --depth;
    }
  }
  return depth != 0;
}

an<Translation> CalculatorTranslator::Query(const string& input,
                                            const Segment& segment) {
  if (!segment.HasTag(tag_))
    return nullptr;
  if (!initialized_)
    Initialize();

  std::string express = strip_prefix(input, prefix_);

  std::string display_expr = express;

  const string& full_input = engine_->context()->input();
  size_t caret_pos = engine_->context()->caret_pos();
  int candidate_end = segment.end;

  if (caret_pos >= full_input.length()) {
    const_cast<Segment*>(&segment)->prompt =
        tips_.empty() ? "〔计算器〕" : "〔" + tips_ + "〕";
  }

  if (caret_pos < full_input.length()) {
    std::string full_express = strip_prefix(full_input, prefix_);
    if (full_express != express && !full_express.empty()) {
      express = full_express;
      candidate_end = static_cast<int>(segment.start + prefix_.length() +
                                       express.length());
    }
  }

  string preedit_text =
      (caret_pos < full_input.length()) ? "" : input;

  bool is_op_first = false;
  std::string prefix_buf;
  if (!express.empty() &&
      std::string("+-*/%^").find(express[0]) != std::string::npos) {
    is_op_first = true;
    if (express.size() == 1 && calc_prefix_expired()) {
      calc_prefix_clear();
      return nullptr;
    }
    prefix_buf = calc_prefix();
    if (express.size() == 1 && !g_chain_result.empty()) {
      calc_prefix_set(g_chain_result);
      calc_prefix_touch();
      prefix_buf = g_chain_result;
      g_chain_result.clear();
    }
    if (prefix_buf.empty()) {
      return nullptr;
    }
    express = prefix_buf + express;
    if (is_op_first && !express.empty() &&
        std::string("+-*/%^").find(express.back()) != std::string::npos) {
      auto translation = New<FifoTranslation>();
      add_candidates(translation, preedit_text, segment.start, candidate_end, prefix_buf,
                     "继续输入");
      return translation;
    }
    g_last_op_input = input;
  } else if (!express.empty()) {
    calc_prefix_clear();
    g_chain_result.clear();
    g_last_op_input.clear();
  }
  if (express.empty())
    return nullptr;

  if (has_unmatched_open_paren(express))
    return nullptr;

  auto translation = New<FifoTranslation>();

  auto const_it = kConstants.find(express);
  if (const_it != kConstants.end()) {
    double val = const_it->second;
    std::string formatted = CalculatorParser::FormatNumber(val);
    auto f_it = kCalcFunctions.find(express);
    std::string desc;
    if (f_it != kCalcFunctions.end())
      desc = f_it->second.desc;
    add_result(translation, preedit_text, segment.start, candidate_end, formatted,
               display_expr);
    return translation;
  }

  if (express == "help") {
    for (const auto& [name, info] : kCalcFunctions) {
      add_candidates(translation, preedit_text, segment.start, candidate_end,
                     make_sig(name, info));
    }
    return translation;
  }

  if (std::isalpha(express[0])) {
    std::string func_name;
    std::string param_part;

    auto paren_open = express.find('(');
    auto paren_close = (paren_open != std::string::npos)
                           ? find_matching_paren(express, paren_open)
                           : std::string::npos;

    if (paren_open != std::string::npos && paren_close != std::string::npos &&
        paren_close > paren_open) {
      func_name = express.substr(0, paren_open);
      param_part = express.substr(paren_open + 1, paren_close - paren_open - 1);
    }

    if (!func_name.empty()) {
      auto fit = kCalcFunctions.find(func_name);
      if (fit != kCalcFunctions.end()) {
        auto& info = fit->second;
        std::vector<std::string> params;

        if (!param_part.empty()) {
          bool in_quotes = false;
          char quote_char = 0;
          std::string current;
          for (char c : param_part) {
            if (in_quotes) {
              if (c == quote_char) {
                in_quotes = false;
                params.push_back(current);
                current.clear();
              } else {
                current += c;
              }
            } else {
              if (c == '"' || c == '\'') {
                in_quotes = true;
                quote_char = c;
                if (!current.empty()) {
                  params.push_back(current);
                  current.clear();
                }
              } else if (c == ',') {
                if (!current.empty()) {
                  params.push_back(current);
                  current.clear();
                }
              } else {
                current += c;
              }
            }
          }
          if (!current.empty()) {
            params.push_back(current);
          }
        }

        for (auto& p : params) {
          while (!p.empty() && p.front() == ' ')
            p.erase(0, 1);
          while (!p.empty() && p.back() == ' ')
            p.pop_back();
        }

        for (auto& p : params) {
          if (!is_number_str(p) &&
              !(p.size() >= 2 &&
                (p.front() == '\'' || p.front() == '"'))) {
            CalculatorParser parser;
            std::string evaluated = parser.Evaluate(p);
            if (is_number_str(evaluated)) {
              p = evaluated;
            }
          }
        }

        if (params.empty() && info.min_args > 0) {
          add_candidates(translation, preedit_text, segment.start, candidate_end,
                         func_name + ": " + info.desc);
          return translation;
        }

        if ((int)params.size() < info.min_args) {
          add_candidates(translation, preedit_text, segment.start, candidate_end,
                         "错误: 函数 " + func_name + " 需要至少 " +
                             std::to_string(info.min_args) + " 个参数");
          return translation;
        }
        if (info.max_args > 0 && (int)params.size() > info.max_args) {
          add_candidates(translation, preedit_text, segment.start, candidate_end,
                         "错误: 函数 " + func_name + " 最多接受 " +
                             std::to_string(info.max_args) + " 个参数");
          return translation;
        }

        if (info.need_quote) {
          size_t q_idx = 0;
          for (auto& p : params) {
            if (!is_number_str(p) &&
                (p.size() < 2 || (p.front() != '"' && p.front() != '\''))) {
              if (q_idx == 1 && info.min_args >= 2) {
              } else {
                p = "'" + p + "'";
              }
            }
            ++q_idx;
          }
        }

        try {
          std::string result = info.impl(params);
          add_result(translation, preedit_text, segment.start, candidate_end, result,
                     func_name + "(" + param_part + ")");
        } catch (...) {
          add_candidates(translation, preedit_text, segment.start, candidate_end,
                         "函数执行错误: " + func_name);
        }
        return translation;
      }
    }

    auto fit = kCalcFunctions.find(express);
    if (fit != kCalcFunctions.end()) {
      add_candidates(translation, preedit_text, segment.start, candidate_end,
                     express + ": " + fit->second.desc);
      return translation;
    }
  }

  try {
    CalculatorParser parser;
    std::string result = parser.Evaluate(express);
    if (is_op_first && is_number_str(result)) {
      g_chain_result = result;
    }
    add_result(translation, preedit_text, segment.start, candidate_end, result,
               display_expr);
  } catch (...) {
    LOG(ERROR) << "calc: Evaluate exception for express='" << express << "'";
    add_candidates(translation, preedit_text, segment.start, candidate_end, "计算错误");
  }
  return translation;
}

}  // namespace rime
