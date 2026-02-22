#!/usr/bin/env python3
"""
AI-GENERATED - Pattern Match Automation Script
Rule: RULE-030 (R8.3) - Named constants only
Purpose: Detect numeric literals in #define statements - should use const/enum instead
Method: Pattern matching for #define with numeric values
"""

import os
import sys
import re
from pathlib import Path


def check_numeric_defines(file_path):
    """
    Analyze a C/C++ header/source file for numeric #define statements.
    Returns list of violations: (line_num, define_name, value)
    """
    violations = []
    
    # Pattern: #define NAME numeric_value (excluding strings, function-like macros)
    define_pattern = re.compile(
        r'#\s*define\s+'
        r'(?P<name>[A-Z_][A-Z0-9_]*)'  # Constant name (UPPER_SNAKE_CASE)
        r'(?!\s*\()'  # Negative lookahead: not function-like macro
        r'\s+'
        r'(?P<value>-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?|0x[0-9a-fA-F]+|0b[01]+)'  # numeric value
        r'(?:\s|$)'  # Followed by whitespace or EOL
    )
    
    # Exceptions: bitfield definitions, bit shift operations are OK
    exception_pattern = re.compile(r'<<|>>|\(|\)|&|\||~|\^')
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line_num, line in enumerate(lines, start=1):
            match = define_pattern.search(line)
            if match:
                define_name = match.group('name')
                value = match.group('value')
                
                # Skip if it's a bitwise/shift operation (allowed exceptions)
                if exception_pattern.search(line[match.end():]):
                    continue
                
                # Skip standard headers and system constants
                if define_name.startswith('_') or define_name in ['MAX', 'MIN']:
                    continue
                
                violations.append((line_num, define_name, value, line.strip()))
    
    except Exception as e:
        print(f"Warning: Could not process {file_path}: {e}", file=sys.stderr)
    
    return violations


def main():
    """Main entry point for numeric #define detection."""
    if len(sys.argv) < 2:
        print("Usage: python rule_RULE-030_R8_3_named_constants.py <source_dir>", file=sys.stderr)
        sys.exit(1)
    
    source_dir = sys.argv[1]
    violations_found = False
    total_violations = 0
    
    # Find all C/C++ header and source files
    files_to_check = []
    for ext in ['*.h', '*.hpp', '*.c', '*.cpp', '*.cc']:
        files_to_check.extend(Path(source_dir).rglob(ext))
    
    for file_path in sorted(files_to_check):
        violations = check_numeric_defines(str(file_path))
        
        if violations:
            violations_found = True
            for line_num, define_name, value, code in violations:
                total_violations += 1
                rel_path = os.path.relpath(file_path, source_dir)
                print(f"[FAIL] {rel_path}:{line_num} Numeric #define '{define_name}' should use const int or enum")
                print(f"       Code: {code}")
                print(f"       Suggestion: const int {define_name} = {value};")
    
    if violations_found:
        print(f"\n[FAIL] RULE-030 VIOLATION: Found {total_violations} numeric #define statements")
        sys.exit(1)
    else:
        print("[PASS] RULE-030: All constants properly defined (using const/enum, not #define)")
        sys.exit(0)


if __name__ == '__main__':
    main()
