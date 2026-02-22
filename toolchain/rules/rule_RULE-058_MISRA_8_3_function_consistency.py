#!/usr/bin/env python3
# AI-GENERATED
"""
RULE-058 (MISRA-8.3): Consistent function declarations
Verify function signatures match between .h and .c files
"""
import os
import re
import sys
from collections import defaultdict

RULE_ID = "RULE-058"
ORIGINAL_ID = "MISRA-8.3"
DESCRIPTION = "Consistent function declarations (.h vs .c)"

def extract_function_signatures(filepath):
    """Extract function signatures from a file"""
    signatures = {}
    
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Match function declarations/definitions
        # Pattern: return_type function_name(params);
        pattern = r'(?:static\s+)?(?:inline\s+)?(?:const\s+)?(\w+(?:\s+[\*\s]*)*)\s+(\w+)\s*\(([^)]*)\)'
        
        for match in re.finditer(pattern, content):
            return_type = match.group(1).strip()
            func_name = match.group(2)
            params = match.group(3).strip()
            
            # Normalize whitespace in params
            params_normalized = re.sub(r'\s+', ' ', params)
            
            signatures[func_name] = {
                'return_type': return_type,
                'params': params_normalized,
                'line': content[:match.start()].count('\n') + 1
            }
    except Exception:
        pass
    
    return signatures

def check_function_consistency(root_path):
    """Compare function signatures in .h and .c files"""
    findings = []
    headers = {}  # { basename: {func_name: signature}}
    sources = {}  # { basename: {func_name: signature}}
    
    # Extract all header signatures
    for dirpath, dirnames, filenames in os.walk(root_path):
        dirnames[:] = [d for d in dirnames if d not in ['build', 'tests', '.git', 'node_modules', '__pycache__']]
        
        for filename in filenames:
            if filename.endswith('.h') or filename.endswith('.hpp'):
                filepath = os.path.join(dirpath, filename)
                basename = os.path.basename(filename)
                headers[basename] = extract_function_signatures(filepath)
            elif filename.endswith('.c') or filename.endswith('.cpp'):
                filepath = os.path.join(dirpath, filename)
                basename = os.path.basename(filename)
                sources[basename] = extract_function_signatures(filepath)
    
    # Compare .h and corresponding .c files
    for h_file, h_sigs in headers.items():
        # Look for corresponding .c file
        c_file = h_file.replace('.hpp', '.cpp').replace('.h', '.c')
        
        if c_file in sources:
            c_sigs = sources[c_file]
            
            # Check each function in header
            for func_name, h_sig in h_sigs.items():
                if func_name in c_sigs:
                    c_sig = c_sigs[func_name]
                    
                    # Compare return types (normalize)
                    h_ret = re.sub(r'\s+', ' ', h_sig['return_type'])
                    c_ret = re.sub(r'\s+', ' ', c_sig['return_type'])
                    
                    # Compare parameters
                    h_params = h_sig['params']
                    c_params = c_sig['params']
                    
                    if h_ret != c_ret or h_params != c_params:
                        findings.append({
                            'function': func_name,
                            'header_file': h_file,
                            'source_file': c_file,
                            'h_return': h_ret,
                            'c_return': c_ret,
                            'h_params': h_params,
                            'c_params': c_params,
                            'message': 'Function signature mismatch'
                        })
    
    return findings

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <source_root>")
        sys.exit(1)
    
    src_path = sys.argv[1]
    findings = check_function_consistency(src_path)
    
    print(f"\n{'='*70}")
    print(f"RULE {RULE_ID} ({ORIGINAL_ID}): {DESCRIPTION}")
    print(f"{'='*70}")
    
    if findings:
        print(f"[FAIL] VIOLATIONS FOUND: {len(findings)}")
        for finding in findings[:10]:
            print(f"\n  Function: {finding['function']}")
            print(f"  Header ({finding['header_file']}):")
            print(f"    {finding['h_return']} ({finding['h_params']})")
            print(f"  Source ({finding['source_file']}):")
            print(f"    {finding['c_return']} ({finding['c_params']})")
        if len(findings) > 10:
            print(f"\n  ... and {len(findings) - 10} more")
        sys.exit(1)
    else:
        print(f"[PASS] Function signatures are consistent")
        sys.exit(0)
