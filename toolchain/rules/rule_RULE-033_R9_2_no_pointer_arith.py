#!/usr/bin/env python3
from common import parse_args, check_pointer_arithmetic, report

RULE_ID = "RULE-033"
ORIGINAL_ID = "R9.2"
DESCRIPTION = "No pointer arithmetic (heuristic)"

if __name__ == "__main__":
    args = parse_args()
    findings = check_pointer_arithmetic(args.src_path)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
