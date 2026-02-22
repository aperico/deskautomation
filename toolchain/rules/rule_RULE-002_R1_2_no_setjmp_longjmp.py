#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-002"
ORIGINAL_ID = "R1.2"
DESCRIPTION = "No setjmp/longjmp"

patterns = [r"\bsetjmp\b", r"\blongjmp\b"]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
