#!/usr/bin/env python3
from common import parse_args, check_null_deref_heuristic, report

RULE_ID = "RULE-026"
ORIGINAL_ID = "R7.3"
DESCRIPTION = "Pointer parameters must be NULL-checked (heuristic)"

if __name__ == "__main__":
    args = parse_args()
    findings = check_null_deref_heuristic(args.src_path)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
