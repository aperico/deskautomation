#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_cmake_flags, check_cppcheck_pattern, report

RULE_ID = "RULE-055"
ORIGINAL_ID = "MISRA-10.1"
DESCRIPTION = "Appropriate essential types (cppcheck/misra)"

patterns = [r"misra.*10\.1", r"sign", r"conversion"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cmake_flags(args.src_path, ["-Wconversion"])
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    if findings:
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
