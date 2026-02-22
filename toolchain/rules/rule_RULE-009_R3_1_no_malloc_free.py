#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, find_regex_in_files, report

RULE_ID = "RULE-009"
ORIGINAL_ID = "R3.1"
DESCRIPTION = "No malloc/calloc/realloc/free"

patterns = [r"\bmalloc\b", r"\bcalloc\b", r"\brealloc\b", r"\bfree\b"]

if __name__ == "__main__":
    args = parse_args()
    findings = find_regex_in_files(args.src_path, patterns)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
