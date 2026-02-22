#!/usr/bin/env python3
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-029"
ORIGINAL_ID = "R8.2"
DESCRIPTION = "No function-like macros"

patterns = [r"#define\s+\w+\s*\("]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
