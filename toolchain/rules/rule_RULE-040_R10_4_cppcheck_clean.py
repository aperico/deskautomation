#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_cppcheck_errors, report

RULE_ID = "RULE-040"
ORIGINAL_ID = "R10.4"
DESCRIPTION = "Zero cppcheck errors"

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_errors(args.src_path)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
