#!/usr/bin/env python3
# AI-GENERATED
"""
RULE-053: R14.5 - Minimum Assertion Density (Code Quality Metrics)
==============================================================
Functions must contain minimum 2 assertions on average.

Threshold:
  - 🟡 Warning if < 2 assertions per function

Rationale:
  - Defensive programming requires invariant checking
  - Critical for ISO 25119 safety requirements
  - Low assertion density = low defensive rigor
  - Catches assumptions that may be invalid
  - Helps document implicit assumptions explicitly

Automation:
  - Counts assert() calls per function
  - Reports functions below minimum threshold
  - Useful for identifying functions needing more validation
"""

#!/usr/bin/env python3
from common import parse_args, check_assertion_density_per_function, report

RULE_ID = "RULE-053"
ORIGINAL_ID = "R14.5"
DESCRIPTION = "Assertions >= 2 per function (Code Quality Metrics)"

MIN_ASSERTIONS = 2

if __name__ == "__main__":
    args = parse_args()
    findings = check_assertion_density_per_function(args.src_path, MIN_ASSERTIONS)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
