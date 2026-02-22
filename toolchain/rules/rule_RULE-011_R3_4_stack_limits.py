#!/usr/bin/env python3
from common import parse_args, check_stack_usage, report

RULE_ID = "RULE-011"
ORIGINAL_ID = "R3.4"
DESCRIPTION = "Stack usage per function <= 512 bytes (heuristic)"

MAX_BYTES = 512

if __name__ == "__main__":
    args = parse_args()
    findings = check_stack_usage(args.src_path, MAX_BYTES)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
