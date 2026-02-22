#!/usr/bin/env python3
"""
AI-GENERATED - Tool-Based & Pattern Match Automation Script
Rule: RULE-066 (MISRA-14.3) - No invariant control expressions
Purpose: Detect if(1), while(true) and other constant condition expressions
Method: cppcheck tool + pattern matching for impossible conditions
"""

import os
import sys
import subprocess
import re
from pathlib import Path


def check_invariant_conditions(file_path):
    """
    Scan file for invariant control expressions: if(1), while(true), if(false), etc.
    Returns list of violations: (line_num, code, severity)
    """
    violations = []
    
    # Patterns for invariant conditions
    patterns = [
        (r'\bif\s*\(\s*1\s*\)', 'if(1) - always true'),
        (r'\bif\s*\(\s*0\s*\)', 'if(0) - always false'),
        (r'\bwhile\s*\(\s*1\s*\)', 'while(1) - infinite loop'),
        (r'\bwhile\s*\(\s*0\s*\)', 'while(0) - never executes'),
        (r'\bwhile\s*\(\s*true\s*\)', 'while(true) - infinite loop'),
        (r'\bif\s*\(\s*true\s*\)', 'if(true) - always true'),
        (r'\bif\s*\(\s*false\s*\)', 'if(false) - always false'),
        (r'\bwhile\s*\(\s*false\s*\)', 'while(false) - never executes'),
        (r'\bdo\s*\{.*?\}\s*while\s*\(\s*0\s*\)', 'do-while(0) - unusual construct'),
    ]
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line_num, line in enumerate(lines, start=1):
            # Skip comments
            if line.strip().startswith('//'):
                continue
            
            for pattern, description in patterns:
                if re.search(pattern, line, re.IGNORECASE):
                    # Verify it's not in a comment
                    code_part = line.split('//')[0]  # Remove comments
                    if re.search(pattern, code_part, re.IGNORECASE):
                        severity = 'ERROR' if 'infinite' in description else 'WARNING'
                        violations.append((line_num, line.strip(), description, severity))
                        break  # Only report once per line
    
    except Exception as e:
        print(f"Warning: Could not process {file_path}: {e}", file=sys.stderr)
    
    return violations


def run_cppcheck_invariant_analysis(source_dir):
    """
    Run cppcheck to detect impossible conditions.
    """
    violations = []
    
    try:
        cmd = [
            'cppcheck',
            '--enable=all',
            '--language=c',
            '--std=c99',
            '--suppress=missingIncludeSystem',
            '--suppress=unusedFunction',
            '--suppress=unusedVariable',
            source_dir
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        
        # Parse output for condition warnings
        for line in result.stdout.split('\n'):
            if 'condition' in line.lower() or 'invariant' in line.lower():
                violations.append(line)
            if 'always true' in line or 'always false' in line:
                violations.append(line)
    
    except (FileNotFoundError, subprocess.TimeoutExpired):
        pass  # cppcheck not available
    
    return violations


def main():
    """Main entry point for invariant condition detection."""
    if len(sys.argv) < 2:
        print("Usage: python rule_RULE-066_MISRA_14_3_invariant_control.py <source_dir>", file=sys.stderr)
        sys.exit(1)
    
    source_dir = sys.argv[1]
    
    if not os.path.isdir(source_dir):
        print(f"[FAIL] Source directory not found: {source_dir}", file=sys.stderr)
        sys.exit(1)
    
    violations_found = False
    total_errors = 0
    total_warnings = 0
    
    # Pattern-based detection
    files_to_check = []
    for ext in ['*.c', '*.cpp', '*.cc', '*.h', '*.hpp']:
        files_to_check.extend(Path(source_dir).rglob(ext))
    
    for file_path in sorted(files_to_check):
        violations = check_invariant_conditions(str(file_path))
        
        if violations:
            for line_num, code, description, severity in violations:
                violations_found = True
                rel_path = os.path.relpath(file_path, source_dir)
                
                if severity == 'ERROR':
                    total_errors += 1
                    print(f"[FAIL] {rel_path}:{line_num} {description}")
                else:
                    total_warnings += 1
                    print(f"[WARN] {rel_path}:{line_num} {description}")
                
                print(f"       Code: {code}")
    
    # Run cppcheck for additional analysis
    if total_errors == 0 and total_warnings == 0:
        cppcheck_findings = run_cppcheck_invariant_analysis(source_dir)
        if cppcheck_findings:
            print("[INFO] cppcheck additional findings:")
            for finding in cppcheck_findings[:5]:
                print(f"  {finding}")
    
    if total_errors > 0:
        print(f"\n[FAIL] RULE-066 VIOLATION: {total_errors} invariant conditions found")
        sys.exit(1)
    elif total_warnings > 0:
        print(f"\n[WARN] RULE-066 ADVISORY: {total_warnings} questionable conditions detected")
        sys.exit(0)
    else:
        print("[PASS] RULE-066: No invariant control expressions detected")
        sys.exit(0)


if __name__ == '__main__':
    main()
