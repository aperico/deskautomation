#!/usr/bin/env python3
from common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-051"
ORIGINAL_ID = "MISRA-2.2"
DESCRIPTION = "No dead code (cppcheck)"

patterns = [r"deadcode", r"unused", r"redundant"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
