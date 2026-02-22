#!/usr/bin/env python3
"""
AI-GENERATED - Pattern Match Automation Script
Rule: RULE-035 (R9.4) - No multi-level pointers
Purpose: Detect and restrict double/triple pointers (**ptr, ***ptr) usage
Method: Regex pattern matching for ** and *** declarations
"""

import os
import sys
import re
from pathlib import Path


def check_multilevel_pointers(file_path):
    """
    Scan C/C++ file for multi-level pointer declarations (**, ***, etc).
    Returns list of violations: (line_num, declaration, pointer_level)
    """
    violations = []
    
    # Pattern for multi-level pointers: **, ***, etc.
    # But NOT argv[] or command-line arg patterns
    multilevel_pattern = re.compile(r'\*{2,}')
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line_num, line in enumerate(lines, start=1):
            # Skip comments and empty lines
            if line.strip().startswith('//') or line.strip().startswith('/*') or not line.strip():
                continue
            
            # Check for ** or *** patterns
            if '**' in line:
                # Exception: argv (standard C idiom for command-line args)
                if 'char **argv' in line or 'const char **argv' in line:
                    continue
                
                # Exception: void ** (opaque pointer casts in specific contexts)
                if 'void **' in line and '(void **)' in line:
                    # Only flag if it's not in a cast context
                    if '{' in line or '=' in line.split('(void **)')[0]:
                        pass  # Skip if it looks like a cast
                    else:
                        violations.append((line_num, line.strip(), 2))
                else:
                    violations.append((line_num, line.strip(), 2))
            
            # Check for *** (triple pointers)
            if '***' in line:
                violations.append((line_num, line.strip(), 3))
    
    except Exception as e:
        print(f"Warning: Could not process {file_path}: {e}", file=sys.stderr)
    
    return violations


def main():
    """Main entry point for multi-level pointer detection."""
    if len(sys.argv) < 2:
        print("Usage: python rule_RULE-035_R9_4_no_multilevel_pointers.py <source_dir>", file=sys.stderr)
        sys.exit(1)
    
    source_dir = sys.argv[1]
    violations_found = False
    total_violations = 0
    
    # Find all C/C++ header and source files
    files_to_check = []
    for ext in ['*.h', '*.hpp', '*.c', '*.cpp', '*.cc']:
        files_to_check.extend(Path(source_dir).rglob(ext))
    
    for file_path in sorted(files_to_check):
        violations = check_multilevel_pointers(str(file_path))
        
        if violations:
            violations_found = True
            for line_num, code, level in violations:
                total_violations += 1
                rel_path = os.path.relpath(file_path, source_dir)
                stars = '*' * level
                # Sanitize code for ASCII-safe output (Windows cp1252 compatibility)
                code_safe = code.encode('ascii', 'replace').decode('ascii')
                print(f"[WARN] {rel_path}:{line_num} Multi-level pointer ({stars}) detected")
                print(f"       Code: {code_safe}")
                print(f"       Note: Review for safety-critical implications")
    
    if violations_found:
        print(f"\n[WARN] RULE-035 ADVISORY: Found {total_violations} multi-level pointer uses")
        print("       (May be acceptable for argv or internal use; verify with code review)")
        sys.exit(0)  # Not fatal, advisory level
    else:
        print("[PASS] RULE-035: No problematic multi-level pointers found")
        sys.exit(0)


if __name__ == '__main__':
    main()
