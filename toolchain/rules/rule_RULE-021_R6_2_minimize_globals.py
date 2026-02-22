#!/usr/bin/env python3
"""
AI-GENERATED - Pattern Match Automation Script
Rule: RULE-021 (R6.2) - Minimize global variables
Purpose: Detect and report global variable declarations to minimize global scope usage
Method: Pattern matching across source files
"""

import os
import sys
import re
from pathlib import Path

# Pattern to detect global variable declarations
# Excludes: comments, function declarations, typedefs, includes
GLOBAL_VAR_PATTERN = re.compile(
    r'^\s*'  # Start of line with optional whitespace
    r'(?!//|#|typedef|struct|union|enum)'  # Negative lookahead: exclude comments, defines, typedefs
    r'(?P<type>\w+(?:\s+[\*&])?)'  # Type (with optional pointer/reference)
    r'\s+'
    r'(?P<var>\w+)'  # Variable name
    r'\s*'
    r'(?:[=;]|$)'  # Assignment or semicolon
)

# Files to exclude from analysis
EXCLUDE_PATTERNS = ['.h', 'test', 'mock', '_test.c', 'Test.cpp']


def is_global_in_module(file_path):
    """
    Analyze a C/C++ source file for global variables.
    Returns list of violations: (line_num, variable_name, code_snippet)
    """
    violations = []
    state = {'in_function': False, 'brace_depth': 0}
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line_num, line in enumerate(lines, start=1):
            # Track if we're inside a function using brace counting
            state['brace_depth'] += line.count('{') - line.count('}')
            
            # Skip if inside a function (brace_depth > 0)
            if state['brace_depth'] > 0:
                continue
            
            # Skip comments and preprocessor
            if line.strip().startswith('//') or line.strip().startswith('#'):
                continue
            
            # Skip empty lines
            if not line.strip():
                continue
            
            # Skip function declarations (has parenthesis)
            if '(' in line and ')' in line and '{' not in line:
                continue
            
            # Check for global variable pattern
            if re.match(r'^\s*\w+.*\s+\w+\s*[=;]', line):
                # Filter out extern, static, const declarations (module-scoped or const)
                if any(keyword in line for keyword in ['extern', 'static const', 'const ']):
                    continue
                
                # Extract variable name if possible
                match = re.search(r'(\w+)\s*(?:=|;)', line.strip())
                if match:
                    var_name = match.group(1)
                    violations.append((line_num, var_name, line.strip()))
    
    except Exception as e:
        print(f"Warning: Could not process {file_path}: {e}", file=sys.stderr)
    
    return violations


def main():
    """Main entry point for pattern match automation."""
    if len(sys.argv) < 2:
        print("Usage: python rule_RULE-021_R6_2_minimize_globals.py <source_dir>", file=sys.stderr)
        sys.exit(1)
    
    source_dir = sys.argv[1]
    violations_found = False
    
    # Find all C/C++ source files
    source_files = []
    for ext in ['*.c', '*.cpp', '*.cc']:
        source_files.extend(Path(source_dir).rglob(ext))
    
    if not source_files:
        print("[PASS] No C/C++ source files found")
        sys.exit(0)
    
    total_violations = 0
    
    for file_path in sorted(source_files):
        # Skip test/mock files
        if any(pattern in str(file_path) for pattern in EXCLUDE_PATTERNS):
            continue
        
        violations = is_global_in_module(str(file_path))
        
        if violations:
            violations_found = True
            for line_num, var_name, code in violations:
                total_violations += 1
                rel_path = os.path.relpath(file_path, source_dir)
                print(f"[FAIL] {rel_path}:{line_num} Global variable '{var_name}' should be minimized")
                print(f"       {code}")
    
    if violations_found:
        print(f"\n[FAIL] RULE-021 VIOLATION: Found {total_violations} potential global variables")
        sys.exit(1)
    else:
        print("[PASS] RULE-021: Global variable scope is well-managed")
        sys.exit(0)


if __name__ == '__main__':
    main()
