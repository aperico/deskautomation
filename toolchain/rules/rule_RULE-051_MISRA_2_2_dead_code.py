#!/usr/bin/env python3# AI-GENERATEDfrom common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-051"
ORIGINAL_ID = "MISRA-2.2"
DESCRIPTION = "No dead code (cppcheck)"

# More specific patterns for MISRA 2.2 dead code detection
# Case 1: Unreachable code after break/return
# Case 2: Code that is guaranteed to never execute (constant conditions)
# Case 3: Unreachable code (explicit from cppcheck)
patterns = [r'id="unreachableCode"', r'<error.*id="unreachableCode"', r'unreachable', r'after\s+break', r'after\s+return']

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
