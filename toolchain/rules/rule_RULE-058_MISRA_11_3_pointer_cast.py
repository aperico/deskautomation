#!/usr/bin/env python3
from common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-058"
ORIGINAL_ID = "MISRA-11.3"
DESCRIPTION = "No object pointer casting (cppcheck/misra)"

patterns = [r"misra.*11\.3", r"cast"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
