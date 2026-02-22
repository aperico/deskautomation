#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-065"
ORIGINAL_ID = "MISRA-21.3"
DESCRIPTION = "No dynamic allocation functions"

patterns = [r"\bmalloc\b", r"\bcalloc\b", r"\brealloc\b", r"\bfree\b"]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
