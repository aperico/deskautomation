#!/usr/bin/env python3# AI-GENERATEDfrom common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-050"
ORIGINAL_ID = "MISRA-2.1"
DESCRIPTION = "No unreachable code (cppcheck)"

patterns = [r"unreachable", r"unreachableCode"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
