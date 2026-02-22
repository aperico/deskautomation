#!/usr/bin/env python3# AI-GENERATEDfrom common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-060"
ORIGINAL_ID = "MISRA-13.2"
DESCRIPTION = "Single side effect per expression (cppcheck/misra)"

patterns = [r"misra.*13\.2", r"sideeffect"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
