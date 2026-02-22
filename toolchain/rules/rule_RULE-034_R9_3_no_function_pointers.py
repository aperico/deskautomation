#!/usr/bin/env python3
from common import parse_args, check_function_pointers, report

RULE_ID = "RULE-034"
ORIGINAL_ID = "R9.3"
DESCRIPTION = "No function pointers"

if __name__ == "__main__":
    args = parse_args()
    findings = check_function_pointers(args.src_path)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
