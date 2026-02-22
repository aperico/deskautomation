#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-058"
ORIGINAL_ID = "MISRA-8.3"
DESCRIPTION = "Consistent function declarations (cppcheck/misra)"

patterns = [r"misra.*8\.3", r"function.*declaration", r"inconsistent"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    else:
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
