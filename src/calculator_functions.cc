#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "calculator_functions.h"

namespace rime {

static double fn(double x) {
  if (std::isinf(x) || std::isnan(x))
    return x;
  if (x == std::floor(x))
    return x;
  double t = std::round(x * 1e12) / 1e12;
  return t;
}

static std::string fs(double n) {
  std::string s = std::to_string(n);
  auto i = s.find('.');
  if (i == std::string::npos)
    return s;
  while (s.size() > 1 && s.back() == '0')
    s.pop_back();
  if (s.back() == '.')
    s.pop_back();
  return s;
}

static std::string fmt(double x) {
  double v = fn(x);
  if (v == std::floor(v))
    return std::to_string((long long)v);
  return fs(v);
}

static double to_double(const std::string& s) {
  return std::stod(s);
}

static int to_int(const std::string& s) {
  return std::stoi(s);
}

static bool is_int(double x) {
  return x == std::floor(x) && !std::isinf(x);
}

static double gcd_d(double a, double b) {
  a = std::abs(a);
  b = std::abs(b);
  while (b != 0) {
    double t = b;
    b = std::fmod(a, b);
    a = t;
  }
  return a;
}

static std::string fs_int(double x) {
  return std::to_string((long long)std::floor(x));
}

// ===== Constants =====

const std::map<std::string, double> kConstants = {
    {"e", M_E},  {"pi", M_PI}, {"b", 1e2},  {"q", 1e3}, {"k", 1e3}, {"w", 1e4},
    {"tw", 1e5}, {"m", 1e6},   {"tm", 1e7}, {"y", 1e8}, {"g", 1e9},
};

// ===== Basic Math =====

static std::string f_sin(const std::vector<std::string>& a) {
  return fmt(std::sin(to_double(a[0])));
}

static std::string f_cos(const std::vector<std::string>& a) {
  return fmt(std::cos(to_double(a[0])));
}

static std::string f_tan(const std::vector<std::string>& a) {
  return fmt(std::tan(to_double(a[0])));
}

static std::string f_asin(const std::vector<std::string>& a) {
  return fmt(std::asin(to_double(a[0])));
}

static std::string f_acos(const std::vector<std::string>& a) {
  return fmt(std::acos(to_double(a[0])));
}

static std::string f_atan(const std::vector<std::string>& a) {
  return fmt(std::atan(to_double(a[0])));
}

static std::string f_atan2(const std::vector<std::string>& a) {
  double y = to_double(a[0]), x = to_double(a[1]);
  if (x == 0 && y == 0)
    return "NaN";
  if (x == 0 && y != 0)
    return fmt(y > 0 ? M_PI / 2 : -M_PI / 2);
  double r = std::atan(y / x);
  if (x < 0)
    r += (y >= 0 ? M_PI : -M_PI);
  return fmt(r);
}

static std::string f_sinh(const std::vector<std::string>& a) {
  return fmt(std::sinh(to_double(a[0])));
}

static std::string f_cosh(const std::vector<std::string>& a) {
  return fmt(std::cosh(to_double(a[0])));
}

static std::string f_tanh(const std::vector<std::string>& a) {
  return fmt(std::tanh(to_double(a[0])));
}

static std::string f_deg(const std::vector<std::string>& a) {
  return fmt(to_double(a[0]) * 180.0 / M_PI);
}

static std::string f_rad(const std::vector<std::string>& a) {
  return fmt(to_double(a[0]) * M_PI / 180.0);
}

static std::string f_exp(const std::vector<std::string>& a) {
  return fmt(std::exp(to_double(a[0])));
}

static std::string f_ldexp(const std::vector<std::string>& a) {
  return fmt(to_double(a[0]) * std::pow(2.0, to_double(a[1])));
}

static std::string f_sqrt(const std::vector<std::string>& a) {
  double x = to_double(a[0]);
  if (x < 0)
    return "错误：参数必须为非负数";
  return fmt(std::sqrt(x));
}

static std::string f_csqrt(const std::vector<std::string>& a) {
  double ar = 0, ai = 0;
  if (!a.empty())
    ar = to_double(a[0]);
  if (a.size() > 1)
    ai = to_double(a[1]);
  double t1 = (std::sqrt(ar * ar + ai * ai) + ar) / 2;
  double t2 = (std::sqrt(ar * ar + ai * ai) - ar) / 2;
  double x1 = fn(std::sqrt(t1)), x2 = fn(-std::sqrt(t1));
  double y1 = fn(std::sqrt(t2)), y2 = fn(-std::sqrt(t2));
  if (ar == 0 && ai == 0)
    return "0";
  if (ai == 0) {
    if (ar > 0)
      return fs(x1) + " , " + fs(x2);
    return fs(y1) + "i , " + fs(y2) + "i";
  }
  if (ai > 0)
    return fs(x1) + "+" + fs(y1) + "i , " + fs(x2) + "-" + fs(-y2) + "i";
  return fs(x1) + "-" + fs(-y2) + "i , " + fs(x2) + "+" + fs(y1) + "i";
}

static std::string f_nroot(const std::vector<std::string>& a) {
  double x = to_double(a[0]), n = to_double(a[1]);
  if ((int)n % 2 == 0 && x < 0)
    return "错误：无实数解";
  if (x < 0)
    return fmt(-std::pow(-x, 1.0 / n));
  return fmt(std::pow(x, 1.0 / n));
}

static std::string f_ceil(const std::vector<std::string>& a) {
  return fmt(std::ceil(to_double(a[0])));
}

static std::string f_floor(const std::vector<std::string>& a) {
  return fmt(std::floor(to_double(a[0])));
}

static std::string f_log(const std::vector<std::string>& a) {
  double x = to_double(a[0]), y = to_double(a[1]);
  if (x <= 0 || y <= 0)
    return "错误：底数和真数必须为正数";
  return fmt(std::log(y) / std::log(x));
}

static std::string f_loge(const std::vector<std::string>& a) {
  double x = to_double(a[0]);
  if (x <= 0)
    return "错误：真数必须为正数";
  return fmt(std::log(x));
}

static std::string f_logt(const std::vector<std::string>& a) {
  double x = to_double(a[0]);
  if (x <= 0)
    return "错误：真数必须为正数";
  return fmt(std::log10(x));
}

// ===== Power Sums =====

static std::string f_sqsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(n * (n + 1) * (2 * n + 1) / 6);
}

static std::string f_cbsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(std::pow(n * (n + 1), 2) / 4);
}

static std::string f_qpsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(n * (n + 1) * (2 * n + 1) * (3 * n * n + 3 * n - 1) / 30);
}

static std::string f_osqsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(n * (4 * n * n - 1) / 3);
}

static std::string f_esqsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(2 * n * (n + 1) * (2 * n + 1) / 3);
}

static std::string f_ocbsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(n * n * (2 * n * n - 1));
}

static std::string f_ecbsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(2 * std::pow(n * (n + 1), 2));
}

static std::string f_oqpsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt((48 * std::pow(n, 5) - 40 * std::pow(n, 3) + 7 * n) / 15);
}

static std::string f_eqpsum(const std::vector<std::string>& a) {
  double n = to_double(a[0]);
  if (n < 1 || !is_int(n))
    return "错误：参数必须为正整数";
  return fmt(8 * n * (n + 1) * (2 * n + 1) * (3 * n * n + 3 * n - 1) / 15);
}

// ===== Combinatorics / Number Theory =====

static std::string f_fact(const std::vector<std::string>& a) {
  double x = to_double(a[0]);
  if (x < 0 || !is_int(x))
    return "错误：参数必须为非负整数";
  if (x == 0 || x == 1)
    return "1";
  double r = 1;
  for (int i = 2; i <= (int)x; ++i)
    r *= i;
  return fmt(r);
}

static std::string f_perm(const std::vector<std::string>& a) {
  double n = to_double(a[0]), r = to_double(a[1]);
  if (n < 0 || r < 0 || !is_int(n) || !is_int(r))
    return "错误：参数必须为非负整数";
  if (r > n)
    return "错误：第二个参数不能大于第一个参数";
  if (r == 0)
    return "1";
  if (r == 1)
    return fmt(n);
  double v = 1;
  for (int i = (int)(n - r + 1); i <= (int)n; ++i)
    v *= i;
  return fmt(v);
}

static std::string f_comb(const std::vector<std::string>& a) {
  double n = to_double(a[0]), r = to_double(a[1]);
  if (n < 0 || r < 0 || !is_int(n) || !is_int(r))
    return "错误：参数必须为非负整数";
  if (r > n)
    return "错误：第二个参数不能大于第一个参数";
  r = std::min(r, n - r);
  if (r == 0 || r == n)
    return "1";
  if (r == 1 || r == n - 1)
    return fmt(n);
  double v = 1;
  for (int i = 1; i <= (int)r; ++i)
    v = v * (n - r + i) / i;
  return fmt(v);
}

static std::string f_gcd(const std::vector<std::string>& a) {
  double r = std::abs(to_double(a[0]));
  for (size_t i = 1; i < a.size(); ++i)
    r = gcd_d(r, std::abs(to_double(a[i])));
  return fmt(r);
}

static std::string f_lcm(const std::vector<std::string>& a) {
  double r = std::abs(to_double(a[0]));
  for (size_t i = 1; i < a.size(); ++i) {
    double b = std::abs(to_double(a[i]));
    r = r * b / gcd_d(r, b);
  }
  return fmt(r);
}

static std::string f_mod(const std::vector<std::string>& a) {
  double x = to_double(a[0]), y = to_double(a[1]);
  double r = std::fmod(x, y);
  if (r < 0)
    r += y;
  return fmt(r);
}

// ===== Sequences =====

static std::string f_arith_sum(const std::vector<std::string>& a) {
  double a1 = to_double(a[0]), d = to_double(a[1]), n = to_double(a[2]);
  if (n < 1 || !is_int(n))
    return "错误：n必须是正整数";
  if (a1 == 0 && d == 0)
    return "0";
  if (d == 0)
    return fmt(a1 * n);
  return fmt(n * a1 + n * (n - 1) * d / 2);
}

static std::string f_geo_sum(const std::vector<std::string>& a) {
  double a1 = to_double(a[0]), q = to_double(a[1]), n = to_double(a[2]);
  if (n < 1 || !is_int(n))
    return "错误：n必须是正整数";
  if (a1 == 0)
    return "0";
  if (q == 0)
    return fmt(a1);
  if (q == 1)
    return fmt(a1 * n);
  return fmt(a1 * (1 - std::pow(q, n)) / (1 - q));
}

static std::string f_seq(const std::vector<std::string>& a) {
  double i = to_double(a[0]), ai = to_double(a[1]);
  double k = to_double(a[2]), ak = to_double(a[3]);
  int b = to_int(a[4]);
  if (i < 1 || !is_int(i) || k < 1 || !is_int(k))
    return "错误：i和k必须是正整数";
  if (ai == ak && i == k)
    return "错误：不能同时相等";
  if (ai != ak && i == k)
    return "错误：同一项数对应不同的项值";
  if (b == 0) {
    double d = fn((ak - ai) / (k - i));
    double a1 = ai - (i - 1) * d;
    double s = fn(a1 - d);
    if (d == 0)
      return "aₙ=" + fmt(a1);
    if (d == 1)
      return s == 0 ? "aₙ=n" : (s > 0 ? "aₙ=n+" + fmt(s) : "aₙ=n-" + fmt(-s));
    if (d == -1)
      return s == 0 ? "aₙ=-n"
                    : (s > 0 ? "aₙ=-n+" + fmt(s) : "aₙ=-n-" + fmt(-s));
    return s == 0 ? "aₙ=" + fmt(d) + "n"
                  : (s > 0 ? "aₙ=" + fmt(d) + "n+" + fmt(s)
                           : "aₙ=" + fmt(d) + "n-" + fmt(-s));
  }
  if (b == 1) {
    if (ai == 0 || ak == 0)
      return "错误：等比数列中不能有0项";
    double s = fn(ak / ai), nd = fn(k - i);
    if (s < 0 && (int)nd % 2 == 0)
      return "无法求解通项公式";
    double q = fn(std::pow(s < 0 ? -s : s, 1.0 / nd));
    if (s < 0)
      q = -q;
    double a1 = fn(ai / std::pow(q, i - 1));
    auto seq_fmt = [&]() -> std::string {
      if (a1 == q)
        return q == 1 ? "aₙ=" + fs(q)
                      : (q > 0 ? "aₙ=" + fs(q) + "ⁿ" : "aₙ=(" + fs(q) + ")ⁿ");
      if (a1 == -q)
        return q == 1 ? "aₙ=-" + fs(q)
                      : (q == -1 ? "aₙ=(" + fs(q) + ")ⁿ⁻¹"
                                 : (q > 0 ? "aₙ=-" + fs(q) + "ⁿ"
                                          : "aₙ=-(" + fs(q) + ")ⁿ"));
      if (q > 0) {
        if (a1 == 1)
          return "aₙ=" + fs(q) + "ⁿ⁻¹";
        if (a1 == -1)
          return "aₙ=-" + fs(q) + "ⁿ⁻¹";
        return "aₙ=" + fs(a1) + "×" + fs(q) + "ⁿ⁻¹";
      }
      if (a1 == 1)
        return "aₙ=(" + fs(q) + ")ⁿ⁻¹";
      if (a1 == -1)
        return "aₙ=-(" + fs(q) + ")ⁿ⁻¹";
      return "aₙ=" + fs(a1) + "×(" + fs(q) + ")ⁿ⁻¹";
    };
    return seq_fmt();
  }
  return "错误：参数b必须是0或1";
}

// ===== Statistics =====

static std::string f_avg(const std::vector<std::string>& a) {
  if (a.empty())
    return "错误：需要至少一个参数";
  double s = 0;
  for (auto& v : a)
    s += to_double(v);
  return fmt(s / a.size());
}

static std::string f_var(const std::vector<std::string>& a) {
  if (a.empty())
    return "错误：需要至少一个参数";
  double s = 0;
  for (auto& v : a)
    s += to_double(v);
  double mean = s / a.size();
  double ss = 0;
  for (auto& v : a) {
    double d = to_double(v) - mean;
    ss += d * d;
  }
  return fmt(ss / a.size());
}

// ===== Equations =====

static std::string f_eq1(const std::vector<std::string>& a) {
  double A = to_double(a[0]), B = to_double(a[1]);
  if (A == 0)
    return B == 0 ? "方程有无数解" : "方程无解";
  return "x=" + fmt(-B / A);
}

static std::string f_eq2(const std::vector<std::string>& a) {
  double A = to_double(a[0]), B = to_double(a[1]), C = to_double(a[2]);
  double D = to_double(a[3]), E = to_double(a[4]), F = to_double(a[5]);
  double det = A * D - B * C;
  if (det == 0) {
    if (A * F - C * E == 0 && B * E - D * F == 0)
      return "方程组有无穷多解";
    return "方程组无解";
  }
  double x = fn((D * E - B * F) / det);
  double y = fn((A * F - C * E) / det);
  return "x=" + fmt(x) + "\ny=" + fmt(y);
}

static std::string f_eq2d(const std::vector<std::string>& a) {
  double A = to_double(a[0]), B = to_double(a[1]), C = to_double(a[2]);
  if (A == 0)
    return "错误：二次项系数不能为0";
  double delta = B * B - 4 * A * C;
  double P = fn(-B / (2 * A));
  if (delta == 0)
    return "x₁=x₂=" + fmt(P);
  if (delta > 0) {
    double Q = fn(std::sqrt(delta) / (2 * A));
    return "x₁=" + fmt(P + Q) + "\nx₂=" + fmt(P - Q);
  }
  double Q = fn(std::sqrt(-delta) / (2 * A));
  std::string x1, x2;
  if (P == 0) {
    if (fabs(Q - 1) < 1e-12) {
      x1 = "i";
      x2 = "-i";
    } else if (fabs(Q + 1) < 1e-12) {
      x1 = "-i";
      x2 = "i";
    } else {
      x1 = fs(Q) + "i";
      x2 = "-" + fs(Q) + "i";
    }
  } else {
    if (fabs(Q - 1) < 1e-12) {
      x1 = fs(P) + "+i";
      x2 = fs(P) + "-i";
    } else if (fabs(Q + 1) < 1e-12) {
      x1 = fs(P) + "-i";
      x2 = fs(P) + "+i";
    } else if (Q > 0) {
      x1 = fs(P) + "+" + fs(Q) + "i";
      x2 = fs(P) + "-" + fs(Q) + "i";
    } else {
      x1 = fs(P) + "-" + fs(-Q) + "i";
      x2 = fs(P) + "+" + fs(-Q) + "i";
    }
  }
  return "x₁=" + x1 + "\nx₂=" + x2;
}

static std::string f_eq3(const std::vector<std::string>& a) {
  double a3 = to_double(a[0]), b = to_double(a[1]);
  double c = to_double(a[2]), d = to_double(a[3]);
  if (a3 == 0)
    return "错误：系数a不能为零";
  double A = b * b - 3 * a3 * c;
  double B = b * c - 9 * a3 * d;
  double Ce = c * c - 3 * b * d;
  double Delta = B * B - 4 * A * Ce;
  if (A == 0 && B == 0) {
    double x = fn(-b / (3 * a3));
    return "x₁=x₂=x₃=" + fmt(x);
  }
  if (Delta > 0) {
    double Y1 = A * b + 3 * a3 * (-B + std::sqrt(Delta)) / 2;
    double Y2 = A * b + 3 * a3 * (-B - std::sqrt(Delta)) / 2;
    double y1 = Y1 >= 0 ? std::pow(Y1, 1.0 / 3) : -std::pow(-Y1, 1.0 / 3);
    double y2 = Y2 >= 0 ? std::pow(Y2, 1.0 / 3) : -std::pow(-Y2, 1.0 / 3);
    double x1 = fn((-b - y1 - y2) / (3 * a3));
    double P = fn((-b + 0.5 * (y1 + y2)) / (3 * a3));
    double Q = fn((0.5 * std::sqrt(3) * (y1 - y2)) / (3 * a3));
    std::string x2, x3;
    if (P == 0) {
      if (fabs(Q - 1) < 1e-12) {
        x2 = "i";
        x3 = "-i";
      } else if (fabs(Q + 1) < 1e-12) {
        x2 = "-i";
        x3 = "i";
      } else {
        x2 = fs(Q) + "i";
        x3 = "-" + fs(Q) + "i";
      }
    } else if (fabs(Q - 1) < 1e-12) {
      x2 = fs(P) + "+i";
      x3 = fs(P) + "-i";
    } else if (fabs(Q + 1) < 1e-12) {
      x2 = fs(P) + "-i";
      x3 = fs(P) + "+i";
    } else if (Q > 0) {
      x2 = fs(P) + "+" + fs(Q) + "i";
      x3 = fs(P) + "-" + fs(Q) + "i";
    } else {
      x2 = fs(P) + "-" + fs(-Q) + "i";
      x3 = fs(P) + "+" + fs(-Q) + "i";
    }
    return "x₁=" + fmt(x1) + "\nx₂=" + x2 + "\nx₃=" + x3;
  }
  if (Delta == 0 && A != 0) {
    double K = B / A;
    double x1 = fn(-b / a3 + K);
    double x2 = fn(-0.5 * K);
    return "x₁=" + fmt(x1) + "\nx₂=x₃=" + fmt(x2);
  }
  if (Delta < 0 && A > 0) {
    double T_num = 2 * A * b - 3 * a3 * B;
    double T_den = 2 * std::sqrt(A * A * A);
    if (T_den == 0)
      return "求解失败";
    double T_val = T_num / T_den;
    T_val = std::max(-1.0, std::min(1.0, T_val));
    double M = std::acos(T_val);
    double S = std::cos(M / 3);
    double R = std::sin(M / 3);
    double x1 = fn((-b - 2 * std::sqrt(A) * S) / (3 * a3));
    double x2 = fn((-b + std::sqrt(A) * (S + std::sqrt(3) * R)) / (3 * a3));
    double x3 = fn((-b + std::sqrt(A) * (S - std::sqrt(3) * R)) / (3 * a3));
    return "x₁=" + fmt(x1) + "\nx₂=" + fmt(x2) + "\nx₃=" + fmt(x3);
  }
  return "求解失败";
}

static std::string f_eq4(const std::vector<std::string>& a) {
  auto sgn = [](double x) -> double {
    if (x == 0)
      return 0;
    return x > 0 ? 1 : -1;
  };
  double a4 = to_double(a[0]), b = to_double(a[1]);
  double c = to_double(a[2]), d = to_double(a[3]), e = to_double(a[4]);
  if (a4 == 0)
    return "错误：系数a不能为零";

  double D = 3 * b * b - 8 * a4 * c;
  double E = -b * b * b + 4 * a4 * b * c - 8 * a4 * a4 * d;
  double F = 3 * b * b * b * b + 16 * a4 * a4 * c * c - 16 * a4 * b * b * c +
             16 * a4 * a4 * b * d - 64 * a4 * a4 * a4 * e;
  double A = D * D - 3 * F;
  double B = D * F - 9 * E * E;
  double Ce = F * F - 3 * D * E * E;
  double Delta = B * B - 4 * A * Ce;

  if (D == 0 && E == 0 && F == 0) {
    double x = fn(-b / (4 * a4));
    return "x₁=x₂=x₃=x₄=" + fmt(x);
  }

  auto fmt_complex = [](double real, double imag) -> std::string {
    double r = fn(real), q = fn(imag);
    if (r == 0) {
      if (fabs(q - 1) < 1e-12)
        return "i";
      if (fabs(q + 1) < 1e-12)
        return "-i";
      return fs(q) + "i";
    }
    if (fabs(q - 1) < 1e-12)
      return fs(r) + "+i";
    if (fabs(q + 1) < 1e-12)
      return fs(r) + "-i";
    if (q > 0)
      return fs(r) + "+" + fs(q) + "i";
    return fs(r) + "-" + fs(-q) + "i";
  };

  if (fabs(D * E * F) > 1e-12 && A == 0 && B == 0 && Ce == 0) {
    double x1 = fn((-b * D + 9 * E) / (4 * a4 * D));
    double x2 = fn((-b * D - 3 * E) / (4 * a4 * D));
    return "x₁=" + fmt(x1) + "\nx₂=x₃=x₄=" + fmt(x2);
  }

  if (E == 0 && F == 0 && D != 0) {
    if (D > 0) {
      double x1 = fn((-b + std::sqrt(D)) / (4 * a4));
      double x2 = fn((-b - std::sqrt(D)) / (4 * a4));
      return "x₁=x₂=" + fmt(x1) + "\nx₃=x₄=" + fmt(x2);
    } else {
      double P = fn(-b / (4 * a4));
      double Q = fn(std::sqrt(-D) / (4 * a4));
      return "x₁=x₂=" + fmt_complex(P, Q) + "\nx₃=x₄=" + fmt_complex(P, -Q);
    }
  }

  if (fabs(A * B * Ce) > 1e-12 && Delta == 0) {
    double P = -b / (4 * a4);
    double Q = 2 * A * E / (4 * a4 * B);
    double x1 = fn(P - Q);
    if (A * B > 0) {
      double R = std::sqrt(2 * B / A) / (4 * a4);
      double x2 = fn(P + Q + R);
      double x3 = fn(P + Q - R);
      return "x₁=x₂=" + fmt(x1) + "\nx₃=" + fmt(x2) + "\nx₄=" + fmt(x3);
    } else {
      double R = fn(std::sqrt(-2 * B / A) / (4 * a4));
      return "x₁=x₂=" + fmt(x1) + "\nx₃=" + fmt_complex(P + Q, R) +
             "\nx₄=" + fmt_complex(P + Q, -R);
    }
  }

  if (Delta > 0) {
    double z1 = A * D + 3 * ((-B + std::sqrt(Delta)) / 2);
    double z2 = A * D + 3 * ((-B - std::sqrt(Delta)) / 2);
    double z3 = (z1 >= 0 ? std::pow(z1, 1.0 / 3) : -std::pow(-z1, 1.0 / 3)) +
                (z2 >= 0 ? std::pow(z2, 1.0 / 3) : -std::pow(-z2, 1.0 / 3));
    double z = D * D - D * z3 + z3 * z3 - 3 * A;
    double P = -b / (4 * a4);
    double Q = sgn(E) * std::sqrt((D + z3) / 3) / (4 * a4);
    double R1 = std::sqrt((2 * D - z3 + 2 * std::sqrt(z)) / 3) / (4 * a4);
    double R2 = fn(std::sqrt((-2 * D + z3 + 2 * std::sqrt(z)) / 3) / (4 * a4));
    double x1 = fn(P + Q + R1);
    double x2 = fn(P + Q - R1);
    return "x₁=" + fmt(x1) + "\nx₂=" + fmt(x2) +
           "\nx₃=" + fmt_complex(P - Q, R2) + "\nx₄=" + fmt_complex(P - Q, -R2);
  }

  if (Delta < 0) {
    double T = (3 * B - 2 * A * D) / (2 * A * std::sqrt(A));
    T = std::max(-1.0, std::min(1.0, T));
    double M = std::acos(T);
    double N = std::cos(M / 3);
    double O = std::sin(M / 3);
    double y1 = (D - 2 * std::sqrt(A) * N) / 3;
    double y2 = (D + std::sqrt(A) * (N + std::sqrt(3) * O)) / 3;
    double y3 = (D + std::sqrt(A) * (N - std::sqrt(3) * O)) / 3;

    if (E == 0 && D > 0 && F > 0) {
      double x1 = fn((-b + std::sqrt(D + 2 * std::sqrt(F))) / (4 * a4));
      double x2 = fn((-b - std::sqrt(D + 2 * std::sqrt(F))) / (4 * a4));
      double x3 = fn((-b + std::sqrt(D - 2 * std::sqrt(F))) / (4 * a4));
      double x4 = fn((-b - std::sqrt(D - 2 * std::sqrt(F))) / (4 * a4));
      return "x₁=" + fmt(x1) + "\nx₂=" + fmt(x2) + "\nx₃=" + fmt(x3) +
             "\nx₄=" + fmt(x4);
    }
    if (E == 0 && D < 0 && F > 0) {
      double P = fn(-b / (4 * a4));
      double Q1 = fn(std::sqrt(-D + 2 * std::sqrt(F)) / (4 * a4));
      double Q2 = fn(std::sqrt(-D - 2 * std::sqrt(F)) / (4 * a4));
      return "x₁=" + fmt_complex(P, Q1) + "\nx₂=" + fmt_complex(P, -Q1) +
             "\nx₃=" + fmt_complex(P, Q2) + "\nx₄=" + fmt_complex(P, -Q2);
    }
    if (E == 0 && F < 0) {
      double P = -b / (4 * a4);
      double Q1 = std::sqrt(2 * D + 2 * std::sqrt(A - F)) / (8 * a4);
      double Q2 = fn(std::sqrt(-2 * D + 2 * std::sqrt(A - F)) / (8 * a4));
      return "x₁=" + fmt_complex(P + Q1, Q2) +
             "\nx₂=" + fmt_complex(P + Q1, -Q2) +
             "\nx₃=" + fmt_complex(P - Q1, Q2) +
             "\nx₄=" + fmt_complex(P - Q1, -Q2);
    }
    if (E != 0) {
      if (D > 0 && F > 0) {
        double P = -b / (4 * a4);
        double Q1 = sgn(E) * std::sqrt(y1) / (4 * a4);
        double Q2 = (std::sqrt(y2) + std::sqrt(y3)) / (4 * a4);
        double Q3 = (std::sqrt(y2) - std::sqrt(y3)) / (4 * a4);
        double x1 = fn(P + Q1 + Q2);
        double x2 = fn(P + Q1 - Q2);
        double x3 = fn(P - Q1 + Q3);
        double x4 = fn(P - Q1 - Q3);
        return "x₁=" + fmt(x1) + "\nx₂=" + fmt(x2) + "\nx₃=" + fmt(x3) +
               "\nx₄=" + fmt(x4);
      } else {
        double P = -b / (4 * a4);
        double Q1 = std::sqrt(y2) / (4 * a4);
        double Q2 = sgn(E) * std::sqrt(-y1) / (4 * a4);
        double Q3 = std::sqrt(-y3) / (4 * a4);
        return "x₁=" + fmt_complex(P - Q1, Q2 + Q3) +
               "\nx₂=" + fmt_complex(P - Q1, -(Q2 + Q3)) +
               "\nx₃=" + fmt_complex(P + Q1, Q2 - Q3) +
               "\nx₄=" + fmt_complex(P + Q1, -(Q2 - Q3));
      }
    }
  }
  return "求解失败";
}

// ===== Geometry - Lines =====

static std::string line_eq(double x1, double y1, double k) {
  if (std::isinf(k) || k != k)
    return "x=" + fs(x1);
  std::string eq = "y=";
  if (k == 0) {
    eq += fs(y1);
    return eq;
  }
  double b = fn(y1 - k * x1);
  if (fabs(k - 1) < 1e-12)
    eq += "x";
  else if (fabs(k + 1) < 1e-12)
    eq += "-x";
  else if (k > 0)
    eq += fs(k) + "x";
  else
    eq += "-" + fs(-k) + "x";
  if (b != 0) {
    if (b > 0)
      eq += "+" + fs(b);
    else
      eq += "-" + fs(-b);
  }
  return eq;
}

static std::string line_general(double A, double B, double C) {
  if (A == 0 && B == 0)
    return "错误：直线方程系数A和B不能同时为0";
  double g = gcd_d(std::abs(A), gcd_d(std::abs(B), std::abs(C)));
  if (A < 0) {
    A = -A;
    B = -B;
    C = -C;
  }
  A = fn(A / g);
  B = fn(B / g);
  C = fn(C / g);
  if (A != 0 && B == 0 && C == 0)
    return "x=0";
  if (A != 0 && B == 0)
    return "x=" + fs(fn(-C / A));
  if (A == 0 && B != 0 && C == 0)
    return "y=0";
  if (A == 0 && B != 0)
    return "y=" + fs(fn(-C / B));
  std::string r;
  if (A == 1)
    r = "x";
  else
    r = fs(A) + "x";
  if (B == 1)
    r += "+y";
  else if (B == -1)
    r += "-y";
  else if (B > 0)
    r += "+" + fs(B) + "y";
  else
    r += "-" + fs(-B) + "y";
  if (C != 0) {
    if (C > 0)
      r += "+" + fs(C) + "=0";
    else
      r += "-" + fs(-C) + "=0";
  } else
    r += "=0";
  return r;
}

static std::string f_line_ps(const std::vector<std::string>& a) {
  double k = to_double(a[0]), x1 = to_double(a[1]), y1 = to_double(a[2]);
  return "直线方程: " + line_eq(x1, y1, k);
}

static std::string f_line_2p(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  if (x1 == x2 && y1 == y2)
    return "错误：两点坐标完全相同，无法确定直线方程";
  if (x1 == x2)
    return "直线方程: x=" + fs(x1);
  double k = fn((y2 - y1) / (x2 - x1));
  return "直线方程: " + line_eq(x1, y1, k);
}

static std::string f_dist(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  if (x1 == x2 && y1 == y2)
    return "两点重合，距离为0";
  return fmt(std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

static std::string f_pbisec(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  if (x1 == x2 && y1 == y2)
    return "错误：两点重合，无法求解垂直平分线方程";
  double x3 = fn((x1 + x2) / 2), y3 = fn((y1 + y2) / 2);
  if (x1 == x2)
    return "垂直平分线方程为：y=" + fs(y3);
  double k = (y2 - y1) / (x2 - x1);
  if (k == 0)
    return "垂直平分线方程为：x=" + fs(x3);
  double kl = fn(-1 / k);
  return "垂直平分线方程为：" + line_eq(x3, y3, kl);
}

static std::string f_rotate(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  double ang = to_double(a[4]) * M_PI / 180.0;
  double x = fn(x2 + (x1 - x2) * std::cos(ang) - (y1 - y2) * std::sin(ang));
  double y = fn(y2 + (x1 - x2) * std::sin(ang) + (y1 - y2) * std::cos(ang));
  return "点P(" + fs(x1) + "," + fs(y1) + ")绕点Q(" + fs(x2) + "," + fs(y2) +
         ")旋转" + fs(to_double(a[4])) + "°后的P'坐标为(" + fs(x) + "," +
         fs(y) + ")";
}

static std::string f_lines(const std::vector<std::string>& a) {
  double A1 = to_double(a[0]), B1 = to_double(a[1]), C1 = to_double(a[2]);
  double A2 = to_double(a[3]), B2 = to_double(a[4]), C2 = to_double(a[5]);
  if ((A1 == 0 && B1 == 0) || (A2 == 0 && B2 == 0))
    return "错误：直线方程的系数不能同时为零";
  bool px = (A1 * B2 == A2 * B1) && (A1 * C2 != A2 * C1);
  bool ch =
      (A1 * B2 == A2 * B1) && (C1 * B2 == C2 * B1) && (C1 * A2 == C2 * A1);
  if (ch)
    return "两直线重合，距离为0";
  if (px) {
    if (B1 != B2) {
      double k = std::max(B1, B2) / std::min(B1, B2);
      if (B1 < B2) {
        A1 *= k;
        B1 *= k;
        C1 *= k;
      } else {
        C2 *= k;
      }
    }
    double D = fn(std::abs(C2 - C1) / std::sqrt(A1 * A1 + B1 * B1));
    return "两直线平行，距离为" + fs(D);
  }
  double x = fn((B1 * C2 - B2 * C1) / (A1 * B2 - A2 * B1));
  double y = fn((C1 * A2 - C2 * A1) / (A1 * B2 - A2 * B1));
  return "两直线相交，交点坐标为(" + fs(x) + "," + fs(y) + ")";
}

static std::string f_lsym(const std::vector<std::string>& a) {
  double A1 = to_double(a[0]), B1 = to_double(a[1]), C1 = to_double(a[2]);
  double A2 = to_double(a[3]), B2 = to_double(a[4]), C2 = to_double(a[5]);
  if ((A1 == 0 && B1 == 0) || (A2 == 0 && B2 == 0))
    return "错误：直线方程的系数不能同时为零";
  double a1 = A2 * A2 + B2 * B2;
  double b = 2 * (A1 * A2 + B1 * B2);
  double A3 = a1 * A1 - b * A2;
  double B3 = a1 * B1 - b * B2;
  double C3 = a1 * C1 - b * C2;
  double a2 = A1 * A1 + B1 * B1;
  double A4 = a2 * A2 - b * A1;
  double B4 = a2 * B2 - b * B1;
  double C4 = a2 * C2 - b * C1;
  return "直线l₁关于l₂的对称直线l₃的方程为：" + line_general(A3, B3, C3) +
         "\n直线l₂关于l₁的对称直线l₄的方程为：" + line_general(A4, B4, C4);
}

// ===== Geometry - Quadratic / Vertex =====

static std::string quad_eq(double a3, double b, double c) {
  std::string r = "y=";
  if (a3 == 1)
    r += "x²";
  else if (a3 == -1)
    r += "-x²";
  else
    r += fs(a3) + "x²";
  if (b != 0) {
    if (b == 1)
      r += "+x";
    else if (b == -1)
      r += "-x";
    else if (b > 0)
      r += "+" + fs(b) + "x";
    else
      r += "-" + fs(-b) + "x";
  }
  if (c != 0) {
    if (c > 0)
      r += "+" + fs(c);
    else
      r += "-" + fs(-c);
  }
  return r;
}

static std::string f_quad_v(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  if (x1 == x2 || y1 == y2)
    return "错误：两个点的横坐标不能相同";
  double aa = fn((y2 - y1) / ((x2 - x1) * (x2 - x1)));
  double b = fn(-2 * aa * x1);
  double cc = fn(y1 + aa * x1 * x1);
  return "二次函数解析式为：" + quad_eq(aa, b, cc);
}

static std::string f_quad_3p(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  double x3 = to_double(a[4]), y3 = to_double(a[5]);
  double detA = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
  if (detA == 0)
    return "错误：三个点共线或重合，无法求解二次函数解析式";
  double A[3][3] = {{x1 * x1, x1, 1}, {x2 * x2, x2, 1}, {x3 * x3, x3, 1}};
  double Bb[3] = {y1, y2, y3};
  auto det3 = [](double m[3][3]) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
           m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
  };
  double matD = det3(A);
  double Ax[3][3] = {{Bb[0], A[0][1], A[0][2]},
                     {Bb[1], A[1][1], A[1][2]},
                     {Bb[2], A[2][1], A[2][2]}};
  double Ay[3][3] = {{A[0][0], Bb[0], A[0][2]},
                     {A[1][0], Bb[1], A[1][2]},
                     {A[2][0], Bb[2], A[2][2]}};
  double Az[3][3] = {{A[0][0], A[0][1], Bb[0]},
                     {A[1][0], A[1][1], Bb[1]},
                     {A[2][0], A[2][1], Bb[2]}};
  double aa = fn(det3(Ax) / matD);
  double b = fn(det3(Ay) / matD);
  double cc = fn(det3(Az) / matD);
  return "二次函数解析式为：" + quad_eq(aa, b, cc);
}

// ===== Geometry - Circles =====

static std::string circle_std(double h, double k, double r2) {
  if (h == 0) {
    if (k > 0)
      return "x²+(y-" + fs(k) + ")²=" + fs(r2);
    if (k == 0)
      return "x²+y²=" + fs(r2);
    return "x²+(y+" + fs(-k) + ")²=" + fs(r2);
  }
  if (k == 0) {
    if (h > 0)
      return "(x-" + fs(h) + ")²+y²=" + fs(r2);
    return "(x+" + fs(-h) + ")²+y²=" + fs(r2);
  }
  if (h > 0 && k > 0)
    return "(x-" + fs(h) + ")²+(y-" + fs(k) + ")²=" + fs(r2);
  if (h > 0 && k < 0)
    return "(x-" + fs(h) + ")²+(y+" + fs(-k) + ")²=" + fs(r2);
  if (h < 0 && k > 0)
    return "(x+" + fs(-h) + ")²+(y-" + fs(k) + ")²=" + fs(r2);
  return "(x+" + fs(-h) + ")²+(y+" + fs(-k) + ")²=" + fs(r2);
}

static std::string circle_gen(double D, double E, double F) {
  std::string r = "x²+y²";
  if (D == -1)
    r += "-x";
  else if (D == 1)
    r += "+x";
  else if (D > 0)
    r += "+" + fs(D) + "x";
  else if (D < 0)
    r += "-" + fs(-D) + "x";
  if (E == -1)
    r += "-y";
  else if (E == 1)
    r += "+y";
  else if (E > 0)
    r += "+" + fs(E) + "y";
  else if (E < 0)
    r += "-" + fs(-E) + "y";
  if (F > 0)
    r += "+" + fs(F) + "=0";
  else if (F < 0)
    r += "-" + fs(-F) + "=0";
  else
    r += "=0";
  return r;
}

static std::string f_circle_r(const std::vector<std::string>& a) {
  double h = to_double(a[0]), k = to_double(a[1]), r = to_double(a[2]);
  if (r <= 0)
    return "错误：半径必须大于0";
  double r2 = fn(r * r);
  double D = fn(-2 * h), E = fn(-2 * k), F = fn(h * h + k * k - r * r);
  return "标准方程: " + circle_std(h, k, r2) +
         "\n一般方程: " + circle_gen(D, E, F);
}

static std::string f_circle_c2p(const std::vector<std::string>& a) {
  double h = to_double(a[0]), k = to_double(a[1]);
  double x1 = to_double(a[2]), y1 = to_double(a[3]);
  double x2 = to_double(a[4]), y2 = to_double(a[5]);
  if ((x1 == x2 && y1 == y2) || (x1 == h && y1 == k) || (x2 == h && y2 == k))
    return "错误：三个坐标中不能有任意两个点坐标完全相同";
  double d1 = std::sqrt((x1 - h) * (x1 - h) + (y1 - k) * (y1 - k));
  double d2 = std::sqrt((x2 - h) * (x2 - h) + (y2 - k) * (y2 - k));
  if (fabs(d1 - d2) > 1e-12)
    return "错误：给定的圆心坐标和两个点无法构成圆";
  double r2 = fn(d1 * d1);
  double D = fn(-2 * h), E = fn(-2 * k), F = fn(h * h + k * k - r2);
  return "标准方程: " + circle_std(h, k, r2) +
         "\n一般方程: " + circle_gen(D, E, F);
}

static std::string f_circle_3p(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  double x3 = to_double(a[4]), y3 = to_double(a[5]);
  double det = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
  if (det == 0)
    return "错误：三个点共线或重合，无法构成圆";
  double B1 = -(x1 * x1 + y1 * y1);
  double B2 = -(x2 * x2 + y2 * y2);
  double B3 = -(x3 * x3 + y3 * y3);
  double A[3][3] = {{x1, y1, 1}, {x2, y2, 1}, {x3, y3, 1}};
  auto det3 = [](double m[3][3]) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
           m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
  };
  double detA = det3(A);
  double AD[3][3] = {
      {B1, A[0][1], A[0][2]}, {B2, A[1][1], A[1][2]}, {B3, A[2][1], A[2][2]}};
  double AE[3][3] = {
      {A[0][0], B1, A[0][2]}, {A[1][0], B2, A[1][2]}, {A[2][0], B3, A[2][2]}};
  double AF[3][3] = {
      {A[0][0], A[0][1], B1}, {A[1][0], A[1][1], B2}, {A[2][0], A[2][1], B3}};
  double D = fn(det3(AD) / detA);
  double E = fn(det3(AE) / detA);
  double F = fn(det3(AF) / detA);
  double h = fn(-D / 2), k = fn(-E / 2), r2 = fn(h * h + k * k - F);
  return "标准方程: " + circle_std(h, k, r2) +
         "\n一般方程: " + circle_gen(D, E, F);
}

static std::string f_circles(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]), r1 = to_double(a[2]);
  double x2 = to_double(a[3]), y2 = to_double(a[4]), r2 = to_double(a[5]);
  if (r1 <= 0 || r2 <= 0)
    return "错误：半径必须为正数";
  if (x1 == x2 && y1 == y2 && r1 == r2)
    return "两圆重合";
  double d = fn(std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
  if (d > (r1 + r2))
    return "两圆外离，圆心距为" + fs(d) + "，无交点";
  if (d < std::abs(r1 - r2))
    return "两圆内含，圆心距为" + fs(d) + "，无交点";
  double ap = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
  double h = std::sqrt(r1 * r1 - ap * ap);
  double m = (x2 - x1) / d, n = (y2 - y1) / d;
  double xj1 = fn(x1 + ap * m + h * n), yj1 = fn(y1 + ap * n - h * m);
  double xj2 = fn(x1 + ap * m - h * n), yj2 = fn(y1 + ap * n + h * m);
  if (fabs(xj1) < 1e-8)
    xj1 = 0;
  if (fabs(yj1) < 1e-8)
    yj1 = 0;
  if (fabs(xj2) < 1e-8)
    xj2 = 0;
  if (fabs(yj2) < 1e-8)
    yj2 = 0;
  double dj =
      fn(std::sqrt((xj2 - xj1) * (xj2 - xj1) + (yj2 - yj1) * (yj2 - yj1)));
  if (fabs(d - (r1 + r2)) < 1e-12)
    return "两圆外切，圆心距为" + fs(d) + "\n交点坐标为(" + fs(xj1) + "," +
           fs(yj1) + ")";
  if (fabs(d - std::abs(r1 - r2)) < 1e-12)
    return "两圆内切，圆心距为" + fs(d) + "\n交点坐标为(" + fs(xj1) + "," +
           fs(yj1) + ")";
  return "两圆相交，圆心距为" + fs(d) + "\n交点坐标为(" + fs(xj1) + "," +
         fs(yj1) + ")和(" + fs(xj2) + "," + fs(yj2) + ")\n相交弦弦长为" +
         fs(dj);
}

static std::string f_circles_g(const std::vector<std::string>& a) {
  double D1 = to_double(a[0]), E1 = to_double(a[1]), F1 = to_double(a[2]);
  double D2 = to_double(a[3]), E2 = to_double(a[4]), F2 = to_double(a[5]);
  double x1 = -D1 / 2, y1 = -E1 / 2, r1 = std::sqrt(x1 * x1 + y1 * y1 - F1);
  double x2 = -D2 / 2, y2 = -E2 / 2, r2 = std::sqrt(x2 * x2 + y2 * y2 - F2);
  std::vector<std::string> args = {fs(x1), fs(y1), fs(r1),
                                   fs(x2), fs(y2), fs(r2)};
  return f_circles(args);
}

// ===== Geometry - Triangles =====

static std::string f_tri_sss(const std::vector<std::string>& a) {
  double s1 = to_double(a[0]), s2 = to_double(a[1]), s3 = to_double(a[2]);
  if (s1 + s2 <= s3 || s1 + s3 <= s2 || s2 + s3 <= s1)
    return "错误：不能构成三角形";
  double p = (s1 + s2 + s3) / 2;
  return fmt(std::sqrt(p * (p - s1) * (p - s2) * (p - s3)));
}

static std::string f_tri_area(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  double x3 = to_double(a[4]), y3 = to_double(a[5]);
  double det = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
  if (det == 0)
    return "错误：三个点重合或共线，不能构成三角形";
  return fmt(std::abs(det / 2));
}

static std::string f_tri_cr(const std::vector<std::string>& a) {
  double s1 = to_double(a[0]), s2 = to_double(a[1]), s3 = to_double(a[2]);
  if (s1 <= 0 || s2 <= 0 || s3 <= 0)
    return "错误：边长必须为正数";
  if (s1 + s2 <= s3 || s1 + s3 <= s2 || s2 + s3 <= s1)
    return "错误：给定的边长不能构成三角形";
  double p = (s1 + s2 + s3) / 2;
  double area = std::sqrt(p * (p - s1) * (p - s2) * (p - s3));
  double ir = fn(area / p);
  double cr = fn((s1 * s2 * s3) / (4 * area));
  return "内切圆半径为" + fs(ir) + "\n外接圆半径为" + fs(cr);
}

static std::string f_tri_crp(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  double x3 = to_double(a[4]), y3 = to_double(a[5]);
  if (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2) == 0)
    return "错误：三个点共线或重合，无法构成三角形";
  auto dist = [](double ax, double ay, double bx, double by) {
    return std::sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
  };
  double s1 = dist(x1, y1, x2, y2);
  double s2 = dist(x2, y2, x3, y3);
  double s3 = dist(x1, y1, x3, y3);
  std::vector<std::string> args = {fs(s1), fs(s2), fs(s3)};
  return f_tri_cr(args);
}

static std::string f_tri_center(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double x2 = to_double(a[2]), y2 = to_double(a[3]);
  double x3 = to_double(a[4]), y3 = to_double(a[5]);
  double det = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
  if (det == 0)
    return "错误：三个点共线或重合，无法构成三角形";
  double xg = fn((x1 + x2 + x3) / 3), yg = fn((y1 + y2 + y3) / 3);
  auto dist = [](double ax, double ay, double bx, double by) {
    return std::sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
  };
  double sa = dist(x2, y2, x3, y3);
  double sb = dist(x1, y1, x3, y3);
  double sc = dist(x1, y1, x2, y2);
  double xn = fn((sa * x1 + sb * x2 + sc * x3) / (sa + sb + sc));
  double yn = fn((sa * y1 + sb * y2 + sc * y3) / (sa + sb + sc));
  double d1 = 2 * det;
  double xw =
      fn(((x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) +
          (x3 * x3 + y3 * y3) * (y1 - y2)) /
         d1);
  double yw =
      fn(((x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) +
          (x3 * x3 + y3 * y3) * (x2 - x1)) /
         d1);
  double s1 = x1 * (x2 * (y1 - y2) + x3 * (y3 - y1)) +
              (y2 - y3) * (x2 * x3 + (y1 - y2) * (y1 - y3));
  double s2 = x1 * x1 * (x2 - x3) +
              x1 * (x3 * x3 - x2 * x2 + y1 * y2 - y1 * y3) + x2 * x2 * x3 -
              x2 * (x3 * x3 + y1 * y2 - y2 * y3) + x3 * y3 * (y1 - y2);
  double xc = fn(s1 / -det), yc = fn(s2 / det);
  return "重心(" + fs(xg) + "," + fs(yg) + ")\n内心(" + fs(xn) + "," + fs(yn) +
         ")\n外心(" + fs(xw) + "," + fs(yw) + ")\n垂心(" + fs(xc) + "," +
         fs(yc) + ")";
}

static std::string f_poly(const std::vector<std::string>& a) {
  double n = to_double(a[0]), s = to_double(a[1]);
  if (n < 1 || !is_int(n) || s <= 0)
    return "错误：边数n必须为正整数，边长a必须为正数";
  return fmt((n * s * s) / (4 * std::tan(M_PI / n)));
}

// ===== Geometry - Point-Line =====

static std::string f_pld(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double A = to_double(a[2]), B = to_double(a[3]), C = to_double(a[4]);
  if (A == 0 && B == 0)
    return "错误：直线方程的系数不能同时为零";
  double S = A * x1 + B * y1 + C;
  if (S == 0)
    return "点在直线上，距离为0，无法求解对称点坐标";
  double D = fn(std::abs(S) / std::sqrt(A * A + B * B));
  double s = S / (A * A + B * B);
  double x = fn(x1 - 2 * A * s), y = fn(y1 - 2 * B * s);
  return "点到直线距离为" + fs(D) + "\n点关于直线的对称点坐标为(" + fs(x) +
         "," + fs(y) + ")";
}

static std::string f_plsl(const std::vector<std::string>& a) {
  double x1 = to_double(a[0]), y1 = to_double(a[1]);
  double A = to_double(a[2]), B = to_double(a[3]), C = to_double(a[4]);
  if (A == 0 && B == 0)
    return "直线方程的系数不能同时为零";
  double A1 = A, B1 = B, C1 = -(2 * A * x1 + 2 * B * y1 + C);
  return "直线l关于点P的对称直线l'的方程为：" + line_general(A1, B1, C1);
}

// ===== Determinant =====

static std::string f_det(const std::vector<std::string>& a) {
  int n = a.size();
  int side = (int)std::sqrt(n);
  if (side * side != n)
    return "给出的元素不能组成一个方阵。";
  std::function<double(std::vector<std::vector<double>>)> determinant =
      [&](std::vector<std::vector<double>> m) -> double {
    int sz = (int)m.size();
    if (sz == 2)
      return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    double d = 0;
    for (int j = 0; j < sz; ++j) {
      std::vector<std::vector<double>> sub;
      for (int i2 = 1; i2 < sz; ++i2) {
        std::vector<double> row;
        for (int k = 0; k < sz; ++k)
          if (k != j)
            row.push_back(m[i2][k]);
        sub.push_back(row);
      }
      double sign = (j % 2 == 0) ? 1 : -1;
      d += sign * m[0][j] * determinant(sub);
    }
    return d;
  };
  std::vector<std::vector<double>> mat(side, std::vector<double>(side));
  int idx = 0;
  for (int i = 0; i < side; ++i)
    for (int j = 0; j < side; ++j)
      mat[i][j] = to_double(a[idx++]);
  return fmt(determinant(mat));
}

// ===== Random =====

static std::random_device rd;
static std::mt19937 gen(rd());

static std::string f_rand(const std::vector<std::string>& a) {
  if (a.size() == 0) {
    std::uniform_real_distribution<> dis(0, 1);
    return fmt(dis(gen));
  }
  if (a.size() == 1) {
    int m = to_int(a[0]);
    std::uniform_int_distribution<> dis(1, m);
    return std::to_string(dis(gen));
  }
  int m = to_int(a[0]), n = to_int(a[1]);
  std::uniform_int_distribution<> dis(m, n);
  return std::to_string(dis(gen));
}

static std::string f_randn(const std::vector<std::string>& a) {
  if (a.size() != 3 && a.size() != 4)
    return "参数数量必须为3或4";
  int min_v, max_v, count;
  bool unique = false;
  if (a.size() == 3) {
    int digits = to_int(a[0]);
    if (digits < 1 || digits > 18)
      return "位数必须为1~18的正整数";
    count = to_int(a[1]);
    unique = (to_int(a[2]) == 0);
    if (digits == 1) {
      min_v = 1;
      max_v = 9;
    } else {
      min_v = (int)std::pow(10, digits - 1);
      max_v = (int)std::pow(10, digits) - 1;
    }
  } else {
    min_v = to_int(a[0]);
    max_v = to_int(a[1]);
    count = to_int(a[2]);
    if (min_v > max_v)
      std::swap(min_v, max_v);
    unique = (to_int(a[3]) == 0);
  }
  if (count < 1)
    return "数量必须为正整数";
  if (unique && count > (max_v - min_v + 1))
    return "唯一性要求下，数量不能超过范围大小";
  std::string result;
  std::set<int> used;
  std::uniform_int_distribution<> dis(min_v, max_v);
  for (int i = 0; i < count; ++i) {
    if (i > 0) {
      if (i % 10 == 0)
        result += "\n";
      else
        result += ",";
    }
    int num;
    if (unique) {
      do {
        num = dis(gen);
      } while (used.count(num));
      used.insert(num);
    } else {
      num = dis(gen);
    }
    result += std::to_string(num);
  }
  return result;
}

// ===== Primes / Factors =====

static std::string f_pfact(const std::vector<std::string>& a) {
  long long n = std::stoll(a[0]);
  if (n <= 0)
    return "参数必须是正整数";
  if (n == 1)
    return "1";
  std::string superscript[10] = {"⁰", "¹", "²", "³", "⁴",
                                 "⁵", "⁶", "⁷", "⁸", "⁹"};
  auto to_sup = [&](int num) -> std::string {
    std::string s = std::to_string(num), r;
    for (char c : s)
      r += superscript[c - '0'];
    return r;
  };
  std::map<long long, int> factors;
  while (n % 2 == 0) {
    factors[2]++;
    n /= 2;
  }
  for (long long d = 3; d * d <= n; d += 2) {
    while (n % d == 0) {
      factors[d]++;
      n /= d;
    }
  }
  if (n > 1)
    factors[n]++;
  std::string result;
  for (auto& p : factors) {
    if (!result.empty())
      result += "×";
    result += std::to_string(p.first);
    if (p.second > 1)
      result += to_sup(p.second);
  }
  return result;
}

static std::string f_primes(const std::vector<std::string>& a) {
  int n = to_int(a[0]);
  if (n <= 1)
    return "参数必须是大于1的正整数";
  if (n > 26338)
    return "数字超限!";
  std::vector<bool> is_p(n + 1, true);
  std::vector<int> primes;
  for (int i = 2; i <= n; ++i) {
    if (is_p[i])
      primes.push_back(i);
    for (int p : primes) {
      long long c = (long long)i * p;
      if (c > n)
        break;
      is_p[(int)c] = false;
      if (i % p == 0)
        break;
    }
  }
  std::string result;
  for (size_t i = 0; i < primes.size(); ++i) {
    result += std::to_string(primes[i]);
    if (i + 1 < primes.size()) {
      if ((i + 1) % 10 == 0)
        result += "\n";
      else
        result += ",";
    }
  }
  return result;
}

static std::string f_pytha(const std::vector<std::string>& a) {
  if (a.size() == 1) {
    int m = to_int(a[0]);
    if (m < 1)
      return "参数必须是正整数";
    if (m % 2 == 1 && m < 3)
      return "输入1个参数时,奇数须大于等于3";
    if (m % 2 == 0 && m < 4)
      return "输入1个参数时,偶数须大于等于4";
    std::set<std::string> seen;
    std::string result;
    if (m % 2 == 1) {
      long long c = ((long long)m * m - 1) / 2, d = ((long long)m * m + 1) / 2;
      std::string key =
          std::to_string(m) + "," + std::to_string(c) + "," + std::to_string(d);
      if (!seen.count(key)) {
        seen.insert(key);
        result += "(" + std::to_string(m) + "," + std::to_string(c) + "," +
                  std::to_string(d) + ")";
      }
    } else {
      long long c = ((long long)m * m) / 4 - 1, d = ((long long)m * m) / 4 + 1;
      std::string key =
          std::to_string(m) + "," + std::to_string(c) + "," + std::to_string(d);
      if (!seen.count(key)) {
        seen.insert(key);
        result += "(" + std::to_string(m) + "," + std::to_string(c) + "," +
                  std::to_string(d) + ")";
      }
    }
    {
      int max_a = (int)(m / std::sqrt(2));
      for (int a2 = 1; a2 <= max_a; ++a2) {
        long long b_sq = (long long)m * m - (long long)a2 * a2;
        if (b_sq < 0)
          break;
        long long b = (long long)std::sqrt(b_sq);
        if (b * b == b_sq && b > a2) {
          std::string key = std::to_string(a2) + "," + std::to_string(b) + "," +
                            std::to_string(m);
          if (!seen.count(key)) {
            seen.insert(key);
            if (!result.empty())
              result += " 和 ";
            result += "(" + std::to_string(a2) + "," + std::to_string(b) + "," +
                      std::to_string(m) + ")";
          }
        }
      }
    }
    return result.empty() ? "无解" : "勾股数为: " + result;
  }
  if (a.size() == 2) {
    int a1 = to_int(a[0]), b1 = to_int(a[1]);
    if (a1 == b1)
      return "两个参数不能相等";
    std::set<std::string> seen;
    std::string result;
    {
      long long sum = (long long)a1 * a1 + (long long)b1 * b1;
      long long c = (long long)std::sqrt(sum);
      if (c * c == sum) {
        int x = a1, y = b1;
        if (x > y)
          std::swap(x, y);
        std::string key = std::to_string(x) + "," + std::to_string(y) + "," +
                          std::to_string(c);
        seen.insert(key);
        result += "(" + std::to_string(x) + "," + std::to_string(y) + "," +
                  std::to_string(c) + ")";
      }
    }
    {
      long long sq = std::abs((long long)a1 * a1 - (long long)b1 * b1);
      long long d = (long long)std::sqrt(sq);
      if (d * d == sq) {
        int x = a1, y = b1;
        if (x > y)
          std::swap(x, y);
        int mn = (int)d;
        int arr[] = {mn, x, y};
        std::sort(arr, arr + 3);
        std::string key = std::to_string(arr[0]) + "," +
                          std::to_string(arr[1]) + "," + std::to_string(arr[2]);
        if (!seen.count(key)) {
          seen.insert(key);
          if (!result.empty())
            result += " 和 ";
          result += "(" + std::to_string(arr[0]) + "," +
                    std::to_string(arr[1]) + "," + std::to_string(arr[2]) + ")";
        }
      }
    }
    {
      long long p1 = std::abs((long long)a1 * a1 - (long long)b1 * b1);
      long long p2 = 2LL * a1 * b1;
      long long hyp = (long long)a1 * a1 + (long long)b1 * b1;
      int arr[] = {(int)p1, (int)p2, (int)hyp};
      std::sort(arr, arr + 3);
      std::string key = std::to_string(arr[0]) + "," + std::to_string(arr[1]) +
                        "," + std::to_string(arr[2]);
      if (!seen.count(key)) {
        seen.insert(key);
        if (!result.empty())
          result += " 和 ";
        result += "(" + std::to_string(arr[0]) + "," + std::to_string(arr[1]) +
                  "," + std::to_string(arr[2]) + ")";
      }
    }
    return result.empty() ? "无解" : "勾股数为: " + result;
  }
  return "请输入至少1个参数";
}

// ===== Unit Conversion =====

static std::string f_unit(const std::vector<std::string>& a) {
  static std::map<std::string, double> factors = {
      {"ai", 1e-10},
      {"nm", 1e-9},
      {"wm", 1e-6},
      {"mm", 1e-3},
      {"cm", 0.01},
      {"dm", 0.1},
      {"m", 1},
      {"km", 1e3},
      {"li", 500},
      {"yc", 0.0254},
      {"ft", 0.3048},
      {"mile", 1609.344},
      {"nmi", 1852},
      {"zhang", 10.0 / 3},
      {"chi", 1.0 / 3},
      {"cun", 1.0 / 30},
      {"fen", 1.0 / 300},
      {"mm2", 1e-6},
      {"cm2", 1e-4},
      {"dm2", 1e-2},
      {"m2", 1},
      {"km2", 1e6},
      {"pfyl", 2589988.1103},
      {"hm2", 1e4},
      {"sq", 2e5 / 3},
      {"acre", 4046.8648},
      {"sm", 2000.0 / 3},
      {"gm", 100},
      {"wl", 1e-9},
      {"mm3", 1e-9},
      {"ml", 1e-6},
      {"cm3", 1e-6},
      {"cl", 1e-5},
      {"dl", 1e-4},
      {"l", 1e-3},
      {"dm3", 1e-3},
      {"hl", 0.1},
      {"m3", 1},
      {"ygl", 4.5461e-3},
      {"mgl", 3.78541e-3},
      {"km3", 1e9},
      {"wg", 1e-6},
      {"mg", 1e-3},
      {"g", 1},
      {"kg", 1e3},
      {"t", 1e6},
      {"lb", 453.59237},
      {"oz", 28.349523125},
      {"ct", 0.2},
      {"gd", 1e5},
      {"sd", 5e4},
      {"jin", 500},
      {"liang", 50},
      {"qian", 5},
      {"dr", 1.771845195},
      {"gr", 0.06479891},
  };
  double value = to_double(a[0]);
  std::string from = a[1], to = a[2];
  if (from.size() >= 2 && (from.front() == '\'' || from.front() == '"'))
    from = from.substr(1, from.size() - 2);
  if (to.size() >= 2 && (to.front() == '\'' || to.front() == '"'))
    to = to.substr(1, to.size() - 2);
  if (!factors.count(from))
    return "错误: 未知的原单位 '" + from + "'";
  if (!factors.count(to))
    return "错误: 未知的目标单位 '" + to + "'";
  if (value <= 0)
    return "错误: 第一个参数必须是有效的正数";
  double result = value * (factors[from] / factors[to]);
  double abs_r = std::abs(result);
  if (abs_r >= 1e5 || (abs_r <= 1e-3 && abs_r > 0)) {
    std::ostringstream oss;
    oss << std::scientific << result;
    std::string s = oss.str();
    auto epos = s.find('e');
    if (epos != std::string::npos) {
      std::string mant = s.substr(0, epos);
      while (mant.size() > 1 && mant.back() == '0')
        mant.pop_back();
      if (mant.back() == '.')
        mant.pop_back();
      std::string exp = s.substr(epos + 1);
      if (exp[0] == '+')
        exp = exp.substr(1);
      std::string superscript[10] = {"⁰", "¹", "²", "³", "⁴",
                                     "⁵", "⁶", "⁷", "⁸", "⁹"};
      std::string sexp;
      for (char c : exp) {
        if (c == '-')
          sexp += "⁻";
        else if (c >= '0' && c <= '9')
          sexp += superscript[c - '0'];
        else
          sexp += c;
      }
      return mant + "×10" + sexp;
    }
  }
  return fs(fn(result));
}

// ===== Base Conversion =====

static std::string f_base(const std::vector<std::string>& a) {
  if (a.size() != 2 && a.size() != 3)
    return "参数数量必须为2或3";
  std::string number = a[0];
  int fromBase = 10, toBase;
  if (number.size() >= 2 && (number.front() == '\'' || number.front() == '"'))
    number = number.substr(1, number.size() - 2);
  if (a.size() == 3) {
    fromBase = to_int(a[1]);
    toBase = to_int(a[2]);
  } else {
    toBase = to_int(a[1]);
  }
  if (fromBase < 2 || fromBase > 36 || toBase < 2 || toBase > 36)
    return "进制范围必须在2到36之间";
  int sign = 1;
  if (number[0] == '-') {
    sign = -1;
    number = number.substr(1);
  } else if (number[0] == '+') {
    number = number.substr(1);
  }
  std::string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::string intPart, fracPart;
  auto dotPos = number.find('.');
  if (dotPos != std::string::npos) {
    intPart = number.substr(0, dotPos);
    fracPart = number.substr(dotPos + 1);
  } else {
    intPart = number;
  }
  long long decInt = 0;
  for (char c : intPart) {
    size_t p = digits.find((char)std::toupper(c));
    if (p == std::string::npos || (long long)p >= fromBase)
      return "数字中包含无效字符或超出原进制范围";
    decInt = decInt * fromBase + (long long)p;
  }
  double decFrac = 0, mult = 1.0 / fromBase;
  for (char c : fracPart) {
    size_t p = digits.find((char)std::toupper(c));
    if (p == std::string::npos || (long long)p >= fromBase)
      return "数字中包含无效字符或超出原进制范围";
    decFrac += (long long)p * mult;
    mult /= fromBase;
  }
  std::string result;
  if (sign == -1)
    result = "-";
  long long n = std::abs(decInt);
  std::string intStr;
  if (n == 0)
    intStr = "0";
  else {
    while (n > 0) {
      intStr = digits[n % toBase] + intStr;
      n /= toBase;
    }
  }
  result += intStr;
  if (!fracPart.empty()) {
    result += ".";
    double f = decFrac;
    for (int i = 0; i < 10 && f > 0; ++i) {
      f *= toBase;
      int d = (int)f;
      result += digits[d];
      f -= d;
    }
  }
  return result;
}

// ===== Function Registration Table =====

const std::map<std::string, FuncInfo> kCalcFunctions = {
    {"sin", {f_sin, "正弦", 1, 1, false}},
    {"cos", {f_cos, "余弦", 1, 1, false}},
    {"tan", {f_tan, "正切", 1, 1, false}},
    {"asin", {f_asin, "反正弦", 1, 1, false}},
    {"acos", {f_acos, "反余弦", 1, 1, false}},
    {"atan", {f_atan, "反正切", 1, 1, false}},
    {"atan2",
     {f_atan2, "返回以弧度为单位的点(x,y)相对于x轴的逆时针角度", 2, 2, false}},
    {"sinh", {f_sinh, "双曲正弦", 1, 1, false}},
    {"cosh", {f_cosh, "双曲余弦", 1, 1, false}},
    {"tanh", {f_tanh, "双曲正切", 1, 1, false}},
    {"deg", {f_deg, "弧度转换为角度", 1, 1, false}},
    {"rad", {f_rad, "角度转换为弧度", 1, 1, false}},
    {"exp", {f_exp, "返回 e^x", 1, 1, false}},
    {"ldexp", {f_ldexp, "返回 x*2^y", 2, 2, false}},
    {"log", {f_log, "x作为底数的对数", 2, 2, false}},
    {"loge", {f_loge, "e作为底数的对数", 1, 1, false}},
    {"logt", {f_logt, "10作为底数的对数", 1, 1, false}},
    {"sqrt", {f_sqrt, "计算正数的算术平方根", 1, 1, false}},
    {"csqrt", {f_csqrt, "计算复数的平方根", 1, 2, false}},
    {"nroot", {f_nroot, "计算 x 开 N 次方", 2, 2, false}},
    {"ceil", {f_ceil, "向上取整", 1, 1, false}},
    {"floor", {f_floor, "向下取整", 1, 1, false}},

    {"sqsum", {f_sqsum, "连续自然数平方和(从1开始)", 1, 1, false}},
    {"cbsum", {f_cbsum, "连续自然数立方和(从1开始)", 1, 1, false}},
    {"qpsum", {f_qpsum, "连续自然数4次方之和(从1开始)", 1, 1, false}},
    {"osqsum", {f_osqsum, "前n个奇数的平方和", 1, 1, false}},
    {"esqsum", {f_esqsum, "前n个偶数的平方和", 1, 1, false}},
    {"ocbsum", {f_ocbsum, "前n个奇数的立方和", 1, 1, false}},
    {"ecbsum", {f_ecbsum, "前n个偶数的立方和", 1, 1, false}},
    {"oqpsum", {f_oqpsum, "前n个奇数的4次方之和", 1, 1, false}},
    {"eqpsum", {f_eqpsum, "前n个偶数的4次方之和", 1, 1, false}},

    {"fact", {f_fact, "阶乘", 1, 1, false}},
    {"perm", {f_perm, "计算排列数", 2, 2, false}},
    {"comb", {f_comb, "计算组合数", 2, 2, false}},
    {"gcd", {f_gcd, "计算多个数的最大公因数", 1, -1, false}},
    {"lcm", {f_lcm, "计算多个数的最小公倍数", 1, -1, false}},
    {"mod", {f_mod, "求余函数", 2, 2, false}},

    {"arith_sum",
     {f_arith_sum, "已知等差数列的首项a₁、公差d，求指定的前n项和", 3, 3,
      false}},
    {"geo_sum",
     {f_geo_sum, "已知等比数列的首项a₁、公比q，求指定的前n项和", 3, 3, false}},
    {"seq",
     {f_seq, "已知数列的任意两项aᵢ、aₖ及对应的项数i、k，求其通项公式", 5, 5,
      false}},

    {"avg", {f_avg, "平均值", 1, -1, false}},
    {"var", {f_var, "方差", 1, -1, false}},

    {"eq1", {f_eq1, "求解一元一次方程", 2, 2, false}},
    {"eq2", {f_eq2, "求解二元一次方程组ax+by=e，cx+dy=f", 6, 6, false}},
    {"eq2d", {f_eq2d, "求解一元二次方程", 3, 3, false}},
    {"eq3", {f_eq3, "求解一元三次方程", 4, 4, false}},
    {"eq4", {f_eq4, "求解一元四次方程", 5, 5, false}},

    {"line_ps", {f_line_ps, "点斜法求解一次函数解析式", 3, 3, false}},
    {"line_2p", {f_line_2p, "两点法求解一次函数解析式", 4, 4, false}},
    {"quad_v", {f_quad_v, "顶点式求解二次函数解析式", 4, 4, false}},
    {"quad_3p", {f_quad_3p, "一般式求解二次函数解析式", 6, 6, false}},
    {"circle_r", {f_circle_r, "已知圆心坐标和半径求圆的方程", 3, 3, false}},
    {"circle_c2p",
     {f_circle_c2p, "已知圆心和圆上不同两点的坐标求圆方程", 6, 6, false}},
    {"circle_3p",
     {f_circle_3p, "已知圆上不同三点的坐标，求圆方程", 6, 6, false}},
    {"circles",
     {f_circles, "已知两圆标准方程，判断它们的位置关系", 6, 6, false}},
    {"circles_g",
     {f_circles_g, "已知两圆一般方程，判断它们的位置关系", 6, 6, false}},

    {"dist", {f_dist, "已知两点坐标，求两点间的距离", 4, 4, false}},
    {"pbisec",
     {f_pbisec, "已知两点坐标，求两点间线段的垂直平分线方程", 4, 4, false}},
    {"rotate",
     {f_rotate, "已知两点P、Q，求点P绕点Q旋转角度a后的P'坐标", 5, 5, false}},
    {"lines", {f_lines, "已知两直线方程，判断它们的位置关系", 6, 6, false}},
    {"lsym",
     {f_lsym, "已知两直线方程，求两条直线以彼此为轴的对称直线方程", 6, 6,
      false}},
    {"pld",
     {f_pld, "已知一点坐标和直线方程，求点到直线的距离及对称点坐标", 5, 5,
      false}},
    {"plsl",
     {f_plsl, "已知一点和直线，求直线关于点的对称直线方程", 5, 5, false}},

    {"tri_sss",
     {f_tri_sss, "已知三角形的三边长a、b、c，求三角形面积", 3, 3, false}},
    {"tri_area",
     {f_tri_area, "已知三角形的三个顶点坐标，求三角形面积", 6, 6, false}},
    {"tri_center",
     {f_tri_center, "已知三角形三个顶点坐标，求其「心」的坐标", 6, 6, false}},
    {"tri_cr",
     {f_tri_cr, "已知三角形三边长，求内切圆半径和外接圆半径", 3, 3, false}},
    {"tri_crp",
     {f_tri_crp, "已知三角形三个顶点坐标，求内切圆半径和外接圆半径", 6, 6,
      false}},
    {"poly", {f_poly, "已知边数n与边长a计算正多边形面积", 2, 2, false}},

    {"rand", {f_rand, "随机数", 0, 2, false}},
    {"randn", {f_randn, "批量随机数", 3, 4, false}},
    {"pytha", {f_pytha, "求解勾股数", 1, 2, false}},
    {"pfact", {f_pfact, "质因数分解", 1, 1, false}},
    {"primes", {f_primes, "找质数", 1, 1, false}},
    {"det", {f_det, "计算行列式", 1, -1, false}},
    {"base", {f_base, "数字进制转换，支持2~36进制", 2, 3, true}},
    {"unit", {f_unit, "单位换算，支持面积、质量、长度、体积", 3, 3, true}},
};

}  // namespace rime
