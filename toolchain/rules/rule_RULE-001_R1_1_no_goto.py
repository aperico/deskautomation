#!/usr/bin/env python3
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-001"
ORIGINAL_ID = "R1.1"
DESCRIPTION = "No goto statements"

patterns = [r"\bgoto\b"]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
