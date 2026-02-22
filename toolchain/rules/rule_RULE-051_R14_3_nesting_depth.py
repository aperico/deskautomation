#!/usr/bin/env python3
# AI-GENERATED
"""
RULE-051: R14.3 - Limited Nesting Depth (Code Quality Metrics)
==============================================================
Block nesting must not exceed 4 levels.

Threshold:
  - ⚠️  Yellow flag: 4 nesting levels
  - 🔴 Red flag: 5+ nesting levels

Rationale:
  - Each nesting level increases cognitive load exponentially
  - Humans reliably understand only 3-4 levels of nesting
  - High nesting correlates with error-prone code
  - Dramatic readability improvement by extracting helper functions

Automation:
  - Analyzes control flow structure (if/for/while/switch depth)
  - Reports maximum nesting level per function
"""

#!/usr/bin/env python3
from common import parse_args, check_nesting_depth, report

RULE_ID = "RULE-051"
ORIGINAL_ID = "R14.3"
DESCRIPTION = "Nesting depth <= 4 levels (Code Quality Metrics)"

MAX_NESTING_DEPTH = 4

if __name__ == "__main__":
    args = parse_args()
    findings = check_nesting_depth(args.src_path, MAX_NESTING_DEPTH)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
