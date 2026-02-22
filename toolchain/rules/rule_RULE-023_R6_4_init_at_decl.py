#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_cmake_flags, check_uninitialized_heuristic, report

RULE_ID = "RULE-023"
ORIGINAL_ID = "R6.4"
DESCRIPTION = "Initialize variables at declaration (heuristic)"

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cmake_flags(args.src_path, ["-Wuninitialized"])
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    if findings:
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
    findings = check_uninitialized_heuristic(args.src_path)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
