#!/usr/bin/env python3
from common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-058"
ORIGINAL_ID = "MISRA-11.3"
DESCRIPTION = "No object pointer casting (cppcheck/misra)"

# More specific patterns for MISRA 11.3 object pointer casting
# Case 1: XML elements with misra 11.3 rule ID
# Case 2: cppcheck messages about object pointer casting
# Case 3: Casts between object pointer types (void* <-> struct*/class*)
patterns = [r'id="misra.*11\.3"', r'<error.*MISRA.*11\.3', r'cast.*object.*pointer', r'cast.*pointer.*type']

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
