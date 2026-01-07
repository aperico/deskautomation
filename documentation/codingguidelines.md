# Coding Guidelines

This document defines the most important coding style rules for the Automated Mechanical Desk Lift project. All rules are measurable and enforceable by static analysis or linting tools (e.g., Cppcheck, clang-tidy).

---

## 1. Naming Conventions
- Use `CamelCase` for class and struct names (e.g., `MotorController`).
- Use `snake_case` for variable and function names (e.g., `read_sensor_value`).
- All constants and macros must be in `UPPER_SNAKE_CASE` (e.g., `MAX_SPEED`).

## 2. Braces and Indentation
- Use Allman style braces: open braces on a new line.
- Indent with 2 or 4 spaces (no tabs).

## 3. File Organization
- Each header file must have include guards or `#pragma once`.
- Each source file should include only necessary headers.

## 4. Function Rules
- Functions must not exceed 40 lines (excluding comments and blank lines).
- Each function must have a single, clear responsibility.

## 5. Variable Declarations
- Declare variables as close as possible to their first use.
- Do not use global variables unless absolutely necessary.

## 6. Magic Numbers
- Do not use magic numbers; use named constants instead.

## 7. Comments and Documentation
- All public functions and classes must have Doxygen-style comments.
- Use `//` for single-line comments and `/* ... */` for multi-line.

## 8. Control Structures
- Always use braces `{}` for `if`, `else`, `for`, `while`, and `do` blocks, even if the block is a single statement.

## 9. Static Analysis
- Code must not have errors or warnings from `cppcheck --enable=all`.
- Code must not have errors or warnings from `clang-tidy` with default checks.

## 10. Unused Code
- Remove commented-out code and unused variables/functions.

---

**Note:** These rules are enforced in CI using Cppcheck and can be extended with other tools as needed.