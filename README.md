# Rime 科学计算器插件

基于 C++ 实现的 Rime 输入法科学计算器，移植自 [super_calculator.lua](https://github.com/amzxyz/Rime)

## 功能

- **四则运算**：`1+2*3`、`10/3`、`2^10`
- **80 个数学函数**：三角函数、对数、阶乘、排列组合、方程求解、几何计算等
- **两种触发模式**：
  - **V 模式**：输入 `V` 前缀手动唤起（如 `V1+2`）
  - **数字上屏模式**：中文模式下直接输入数字（如 `84+5`），数字自动上屏，按运算符唤起计算器

## 编译

```sh
cd librime
make                    # release
```

输出：`build/lib/rime-plugins/librime-calculator.so`

### 依赖

- C++17
- librime 源码树（本仓库内编译）

## 安装

```sh
# 找到 librime.so 所在目录
find /usr -name "librime.so*" 2>/dev/null

# 复制插件到对应 rime-plugins 目录
sudo cp build/lib/rime-plugins/librime-calculator.so /usr/lib/rime-plugins/
```

## 配置

在输入方案的 `.schema.yaml` 中添加：

```yaml
engine:
  processors:
    - digit_catcher          # 必须放在 recognizer 之前
    - recognizer
    # ... 其他 processors
  segmentors:
    - matcher                # 必须
    # ... 其他 segmentors
  translators:
    - calculator_translator  # 建议放在 echo_translator 之前
    - echo_translator
    # ... 其他 translators

recognizer:
  patterns:
    calculator: "^(V.*|[0-9]+[\\+\\-\\*/\\^].*|[\\+\\-\\*/\\^].*)$"

calculator:
  tag: calculator
  prefix: "V"
  tips: "计算器"
```

### 配置项说明

| 配置 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `calculator/tag` | string | `calculator` | segment 标签，一般无需改动 |
| `calculator/prefix` | string | `V` | V 模式的手动前缀，留空则不启用 |
| `calculator/tips` | string | `"计算器"` | 候选窗口提示文字 |

## 使用

### V 模式

输入 `V` 后接表达式或函数：

| 输入 | 输出 |
|------|------|
| `V1+2*3` | `7` |
| `Vsin(pi/6)` | `0.5` |
| `Vavg3,6,9` | `6` |
| `Veq2d1,-5,6` | `x₁=3  x₂=2` |

### 数字上屏模式

中文模式下直接输入数字，数字自动上屏（提交到应用），按运算符唤起计算器：

| 按键序列 | 应用看到 | 计算器候选 |
|----------|---------|-----------|
| `8 4 + 5` | `84` | `+5=89` |
| `1 0 0 - 2 0` | `100` | `-20=80` |

> **注意**：数字上屏模式仅在中文模式（非 ascii_mode）下生效。ascii_mode 中数字由 AsciiComposer 处理。

### 函数调用

函数使用"函数名+参数"格式，多个参数用逗号分隔：

```
输入：Vavg3,6,9
候选 1：6.5
候选 2：avg(3,6,9)=6.5
```

无参数输入时显示函数描述：

```
输入：Vsin
候选：sin: 正弦
```

带字符串参数的函数（`base`、`unit`）需用引号括起字符串：

```
Vunit100,'km','m'      → 100000
Vbase'1A',16,2          → 11010
```

### 常量

直接输入常量名即可查看：

| 输入 | 值 | 说明 |
|------|------|------|
| `Ve` | 2.71828 | 自然常数 |
| `Vpi` | 3.14159 | 圆周率 |
| `Vw` | 10000 | 万 |
| `Vy` | 1e8 | 亿 |

常量也可参与表达式运算：`Vpi*2`、`Ve^2`

## 全部函数

### 基础数学

`sin` `cos` `tan` `asin` `acos` `atan` `atan2` `sinh` `cosh` `tanh`
`deg` `rad` `exp` `ldexp` `log` `loge` `logt` `sqrt` `csqrt` `nroot`
`ceil` `floor`

### 幂方求和

`sqsum` `cbsum` `qpsum` `osqsum` `esqsum` `ocbsum` `ecbsum` `oqpsum` `eqpsum`

### 组合数论

`fact` `perm` `comb` `gcd` `lcm` `mod`

### 数列

`seq` `arith_sum` `geo_sum`

### 统计

`avg` `var`

### 方程求解

`eq1` `eq2` `eq2d` `eq3` `eq4`

### 解析几何

`line_ps` `line_2p` `quad_v` `quad_3p` `circle_r` `circle_c2p` `circle_3p`
`circles` `circles_g` `dist` `pbisec` `rotate` `lines` `lsym` `pld` `plsl`

### 三角几何

`tri_sss` `tri_area` `tri_center` `tri_cr` `tri_crp` `poly`

### 工具

`rand` `randn` `pytha` `pfact` `primes` `det` `base` `unit`

## 架构

```
processor 流水线:
  digit_catcher  →  捕获数字按键，CommitText 上屏，存入共享缓冲
  recognizer     →  识别 V 前缀/运算符，标记 segment
  matcher        →  根据 pattern 给 segment 打 "calculator" 标签

translator:
  calculator_translator → 回读已上屏数字，拼接表达式，求值，生成候选
```

**共享存储**：`digit_catcher` 与 `calculator_translator` 通过静态变量 `g_calc_prefix` 通信，2 秒超时自动清理。

## 文件结构

```
plugins/calculator/
├── CMakeLists.txt
└── src/
    ├── calculator_module.cc         # 模块注册
    ├── calculator_translator.h/cc   # Translator 主逻辑
    ├── calculator_parser.h/cc       # 数学表达式解析器（Shunting-yard）
    ├── calculator_functions.h/cc    # 80 个计算函数
    └── digit_catcher.h/cc           # 数字捕获 Processor
```

## License

同 librime 项目。
