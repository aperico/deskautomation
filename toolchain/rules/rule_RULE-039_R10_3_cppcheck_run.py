#!/usr/bin/env python3
from common import parse_args, check_cppcheck_report, report

RULE_ID = "RULE-039"
ORIGINAL_ID = "R10.3"
DESCRIPTION = "Cppcheck report exists"

if __name__ == "__main__":
    args = parse_args()
    xml_files, status = check_cppcheck_report(args.src_path)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    findings = []
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
