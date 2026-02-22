#!/usr/bin/env python3
# AI-GENERATED
"""
RULE-052: R14.4 - Limited File Length (Code Quality Metrics)
==========================================================
Source files must not exceed 800 lines of code.

Threshold:
  - ⚠️  Yellow flag: 600+ lines
  - 🔴 Red flag: 800+ lines

Rationale:
  - Large files often contain multiple responsibilities
  - Indicates need for module refactoring/splitting
  - Improves navigation and understanding
  - Reduces recompilation impact
  - Mirrors function length rule but at module level

Automation:
  - Counts executable code lines per file (excluding comments, blanks)
  - Reports violations with line counts
"""

#!/usr/bin/env python3
from common import parse_args, check_file_length, report

RULE_ID = "RULE-052"
ORIGINAL_ID = "R14.4"
DESCRIPTION = "File length <= 800 lines (Code Quality Metrics)"

MAX_FILE_LENGTH = 800

if __name__ == "__main__":
    args = parse_args()
    findings = check_file_length(args.src_path, MAX_FILE_LENGTH)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
