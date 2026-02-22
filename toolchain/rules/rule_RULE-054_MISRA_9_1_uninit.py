#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-054"
ORIGINAL_ID = "MISRA-9.1"
DESCRIPTION = "No uninitialized auto vars (cppcheck)"

patterns = [r"uninit", r"uninitialized"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
