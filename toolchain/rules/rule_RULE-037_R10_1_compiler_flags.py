#!/usr/bin/env python3
from common import parse_args, check_cmake_flags, report

RULE_ID = "RULE-037"
ORIGINAL_ID = "R10.1"
DESCRIPTION = "Required compiler flags present"

REQUIRED_FLAGS = ["-Wall", "-Wextra", "-Werror", "-pedantic"]

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cmake_flags(args.src_path, REQUIRED_FLAGS)
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
