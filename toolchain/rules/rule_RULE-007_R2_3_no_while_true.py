#!/usr/bin/env python3
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-007"
ORIGINAL_ID = "R2.3"
DESCRIPTION = "No while(true)/for(;;) except main"

patterns = [r"\bwhile\s*\(\s*true\s*\)", r"\bfor\s*\(\s*;\s*;\s*\)"]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
