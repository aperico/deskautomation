#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, iter_source_files, read_text, extract_functions, report
import re

RULE_ID = "RULE-007"
ORIGINAL_ID = "R2.3"
DESCRIPTION = "No while(true)/for(;;) except main"

if __name__ == "__main__":
    args = parse_args()
    findings = []
    patterns = [r"\bwhile\s*\(\s*true\s*\)", r"\bfor\s*\(\s*;\s*;\s*\)"]
    for path in iter_source_files(args.src_path):
        text = read_text(path)
        for start, end, body in extract_functions(text):
            func_header = text.splitlines()[start - 1] if start > 0 else ""
            if "main" in func_header:
                continue
            for pattern in patterns:
                if re.search(pattern, body):
                    findings.append((path, start, f"while(true) or for(;;) outside main"))
                    break
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
