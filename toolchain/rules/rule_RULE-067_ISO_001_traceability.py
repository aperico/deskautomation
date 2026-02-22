#!/usr/bin/env python3
# AI-GENERATED
from common import parse_args, check_traceability, report

RULE_ID = "RULE-067"
ORIGINAL_ID = "ISO-001"
DESCRIPTION = "Requirement traceability (SWReq- tags)"

if __name__ == "__main__":
    args = parse_args()
    findings = check_traceability(args.src_path)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
