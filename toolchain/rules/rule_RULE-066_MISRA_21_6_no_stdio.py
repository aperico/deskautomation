#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-066"
ORIGINAL_ID = "MISRA-21.6"
DESCRIPTION = "No standard I/O functions"

patterns = [r"\bprintf\b", r"\bscanf\b", r"\bfopen\b", r"\bfclose\b"]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
