#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_build_warnings, report

RULE_ID = "RULE-038"
ORIGINAL_ID = "R10.2"
DESCRIPTION = "Zero compiler warnings (requires build log)"

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_build_warnings(args.src_path)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
