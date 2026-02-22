#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_pointer_arithmetic, report

RULE_ID = "RULE-064"
ORIGINAL_ID = "MISRA-18.1"
DESCRIPTION = "Pointer arithmetic within bounds (heuristic)"

if __name__ == "__main__":
    args = parse_args()
    findings = check_pointer_arithmetic(args.src_path)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
