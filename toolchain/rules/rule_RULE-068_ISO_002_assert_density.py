#!/usr/bin/env python3
from common import parse_args, check_assert_density, report

RULE_ID = "RULE-068"
ORIGINAL_ID = "ISO-002"
DESCRIPTION = "Defensive programming via assertions (density)"

MIN_ASSERTS_PER_FUNCTION = 1.0

if __name__ == "__main__":
    args = parse_args()
    findings = check_assert_density(args.src_path, MIN_ASSERTS_PER_FUNCTION)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
