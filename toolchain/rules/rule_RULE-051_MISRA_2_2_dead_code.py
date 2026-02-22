#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_cppcheck_pattern, report

RULE_ID = "RULE-051"
ORIGINAL_ID = "MISRA-2.2"
DESCRIPTION = "No dead code (cppcheck)"

patterns = [r"unreachable", r"dead code", r"after break", r"after return"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cppcheck_pattern(args.src_path, patterns)
    kwargs = {"status_override": status} if status else {}
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings or [], **kwargs)
