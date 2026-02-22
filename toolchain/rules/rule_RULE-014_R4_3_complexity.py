#!/usr/bin/env python3
from common import parse_args, check_complexity, report

RULE_ID = "RULE-014"
ORIGINAL_ID = "R4.3"
DESCRIPTION = "Cyclomatic complexity <= 10 (heuristic)"

MAX_COMPLEXITY = 10

if __name__ == "__main__":
    args = parse_args()
    findings = check_complexity(args.src_path, MAX_COMPLEXITY)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
