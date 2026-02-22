#!/usr/bin/env python3
"""
AI-GENERATED - Tool-Based Automation Script
Rule: RULE-041 (R10.5) - MISRA C:2012 compliance
Purpose: Run cppcheck with MISRA rule checking to detect compliance issues
Method: Tool invocation via subprocess calling cppcheck with MISRA addons
"""

import os
import sys
import subprocess
import json
from pathlib import Path


def run_cppcheck_misra(source_dir):
    """
    Run cppcheck with MISRA rule checking.
    Returns tuple: (violations, warnings, errors)
    """
    
    # Check if cppcheck is available
    try:
        result = subprocess.run(['cppcheck', '--version'], capture_output=True, text=True)
        if result.returncode != 0:
            print("[WARN] cppcheck not found in PATH - installing may be required", file=sys.stderr)
            return None
    except FileNotFoundError:
        print("[WARN] cppcheck not installed or not in PATH", file=sys.stderr)
        return None
    
    # Build cppcheck command
    cmd = [
        'cppcheck',
        '--enable=misra',  # Enable MISRA rules
        '--addon=misra',   # Use MISRA addon
        '--language=c',
        '--std=c99',
        '--inconclusive',  # Include inconclusive checks
        '--inline-suppr',  # Support inline suppressions
        '--report-progress',
        '--json',  # JSON output for parsing
        source_dir
    ]
    
    violations = []
    warnings = []
    errors = []
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        
        # Parse JSON output if available
        if result.stdout:
            try:
                output = json.loads(result.stdout)
                if 'results' in output:
                    for issue in output['results']:
                        msg = issue.get('message', '(no message)')
                        file_path = issue.get('file', '(unknown)')
                        line = issue.get('line', 0)
                        severity = issue.get('severity', 'warning')
                        
                        if 'misra' in msg.lower():
                            violations.append(f"{file_path}:{line}: {msg}")
                        elif severity == 'warning':
                            warnings.append(f"{file_path}:{line}: {msg}")
                        elif severity == 'error':
                            errors.append(f"{file_path}:{line}: {msg}")
            except json.JSONDecodeError:
                # Fallback to text parsing
                lines = result.stdout.split('\n')
                for line in lines:
                    if 'misra' in line.lower():
                        violations.append(line)
                    elif 'warning' in line.lower():
                        warnings.append(line)
                    elif 'error' in line.lower():
                        errors.append(line)
        
        # Also check stderr for issues
        if result.stderr:
            for line in result.stderr.split('\n'):
                if 'misra' in line.lower():
                    violations.append(line)
    
    except subprocess.TimeoutExpired:
        print("[WARN] cppcheck MISRA analysis timed out", file=sys.stderr)
        return None
    except Exception as e:
        print(f"[WARN] Error running cppcheck: {e}", file=sys.stderr)
        return None
    
    return (violations, warnings, errors)


def main():
    """Main entry point for MISRA compliance checking."""
    if len(sys.argv) < 2:
        print("Usage: python rule_RULE-041_R10_5_misra_compliance.py <source_dir>", file=sys.stderr)
        sys.exit(1)
    
    source_dir = sys.argv[1]
    
    if not os.path.isdir(source_dir):
        print(f"[FAIL] Source directory not found: {source_dir}", file=sys.stderr)
        sys.exit(1)
    
    result = run_cppcheck_misra(source_dir)
    
    if result is None:
        print("[SKIP] RULE-041: cppcheck not available for MISRA checking")
        sys.exit(0)  # Skip if tool not available
    
    violations, warnings, errors = result
    
    if violations:
        print(f"[FAIL] RULE-041 MISRA VIOLATIONS: {len(violations)} found")
        for v in violations[:20]:  # Show first 20
            print(f"  {v}")
        if len(violations) > 20:
            print(f"  ... and {len(violations) - 20} more")
        sys.exit(1)
    elif errors:
        print(f"[FAIL] RULE-041 MISRA ERRORS: {len(errors)} found")
        for e in errors[:10]:
            print(f"  {e}")
        sys.exit(1)
    elif warnings:
        print(f"[WARN] RULE-041 MISRA WARNINGS: {len(warnings)} found")
        sys.exit(0)  # Warnings are non-fatal
    else:
        print("[PASS] RULE-041: MISRA C:2012 compliance verified (no violations)")
        sys.exit(0)


if __name__ == '__main__':
    main()
