#!/usr/bin/env python3
# AI-GENERATED
"""
RULE-050: R14.2 - Limited Function Parameters (Code Quality Metrics)
====================================================================
Functions must not exceed 4 parameters.

Threshold:
  - ⚠️  Yellow flag: 4 parameters
  - 🔴 Red flag: 5+ parameters

Rationale:
  - More than 4 parameters makes functions difficult to understand
  - Hard to test with many combinations of inputs
  - Forces better data encapsulation using structs

Automation:
  - Counts parameters per function definition
  - Reports violations with parameter list
"""

#!/usr/bin/env python3
from common import parse_args, check_parameter_count, report

RULE_ID = "RULE-050"
ORIGINAL_ID = "R14.2"
DESCRIPTION = "Function parameters <= 4 (Code Quality Metrics)"

MAX_PARAMETERS = 4

if __name__ == "__main__":
    args = parse_args()
    findings = check_parameter_count(args.src_path, MAX_PARAMETERS)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
