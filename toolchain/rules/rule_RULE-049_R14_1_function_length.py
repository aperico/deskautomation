#!/usr/bin/env python3
# AI-GENERATED
"""
RULE-049: R14.1 - Limited Function Length (Code Quality Metrics)
================================================================
Functions must not exceed 60 lines of executable code.

Threshold:
  - ⚠️  Yellow flag: 40+ lines
  - 🔴 Red flag: 60+ lines

Rationale:
  - Functions >60 lines violate single responsibility principle
  - Easier to understand, test, and review if they fit on screen
  - Correlates with cyclomatic complexity maintaining low values

Automation:
  - Counts executable lines per function (excluding comments, blanks)
  - Reports violations with line counts
"""

#!/usr/bin/env python3
from common import parse_args, check_function_length, report

RULE_ID = "RULE-049"
ORIGINAL_ID = "R14.1"
DESCRIPTION = "Function length <= 60 lines (Code Quality Metrics)"

MAX_LINES = 60

if __name__ == "__main__":
    args = parse_args()
    findings = check_function_length(args.src_path, MAX_LINES)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
