#!/usr/bin/env python3
from common import parse_args, check_cmake_flags, check_cppcheck_pattern, report

RULE_ID = "RULE-062"
ORIGINAL_ID = "MISRA-16.3"
DESCRIPTION = "Switch clauses end with break (cppcheck/misra)"

patterns = [r"misra.*16\.3", r"switch" ]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cmake_flags(args.src_path, ["-Wimplicit-fallthrough"])
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    if findings:
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
