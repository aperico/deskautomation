# Coding Guidelines

This document lists only the rules that are automatically verifiable by our pipeline tools (Cppcheck with MISRA/CERT addons). If a rule is not enforced by a tool in CI, it is not listed here.

---

## Tool-Enforced Rules

1) **Include guards on headers**: Every header must use `#pragma once` or include guards. (Checked by Cppcheck)
2) **Braces on all control statements**: Always use braces for `if/else/for/while/do` blocks, even single statements. (Cppcheck/MISRA)
3) **No unused code**: Remove unused variables, functions, and dead/commented-out code. (Cppcheck)
4) **No global variable misuse**: Avoid non-const globals; Cppcheck flags risky globals. (Cppcheck)
5) **Static analysis clean**: Code must be free of errors/warnings in `cppcheck --enable=all --addon=misra --addon=cert`. (CI gate)
6) **No uninitialized reads**: Do not read variables before initialization. (Cppcheck/MISRA)
7) **Null-safety**: Check pointers before dereference; avoid null dereference. (Cppcheck/MISRA)
8) **Bounds & shifts**: No out-of-bounds array access; no invalid/oversized bit shifts. (Cppcheck/MISRA)
9) **Division safety**: Guard against division or modulo by zero. (Cppcheck)
10) **Header hygiene**: Remove unused/duplicate includes; headers must be self-contained. (Cppcheck)
11) **No C-style casts**: Use C++ casts; C-style casts are prohibited. (MISRA)
12) **No goto**: `goto` is disallowed. (MISRA)
13) **No dead code**: Remove unreachable code. (Cppcheck)
14) **No UB from overflow/enums**: Avoid signed overflow, invalid enum values, and undefined shifts. (Cppcheck/MISRA)
15) **Sanitizer clean (when enabled)**: No findings from AddressSanitizer/UBSan runs in CI. (CI gate)

---

**Note:** Additional style preferences (naming, function length, documentation style, magic numbers, formatting) are valuable but not currently enforced by our CI tools; follow team conventions where helpful.