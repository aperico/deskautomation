#!/usr/bin/env python3
from common import parse_args, check_function_length, report

RULE_ID = "RULE-012"
ORIGINAL_ID = "R4.1"
DESCRIPTION = "Function length <= 60 lines"

MAX_LINES = 60

if __name__ == "__main__":
    args = parse_args()
    findings = check_function_length(args.src_path, MAX_LINES)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
