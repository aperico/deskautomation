#!/usr/bin/env python3
from common import parse_args, check_cmake_flags, check_unused_result_heuristic, report

RULE_ID = "RULE-024"
ORIGINAL_ID = "R7.1"
DESCRIPTION = "Check all return values (heuristic)"

if __name__ == "__main__":
    args = parse_args()
    findings, status = check_cmake_flags(args.src_path, ["-Wunused-result"])
    if status == "SKIP":
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, [], status_override="SKIP")
    if findings:
        report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
    allowlist = {
        "pinMode",
        "digitalWrite",
        "analogWrite",
        "init_inputs",
        "init_outputs",
        "transition_to",
        "handle_fault",
        "HAL_setMotorType",
        "HAL_setMotor",
        "HAL_setLED",
        "HAL_init",
    }
    findings = check_unused_result_heuristic(args.src_path, allowlist=allowlist)
    report(RULE_ID, ORIGINAL_ID, DESCRIPTION, findings)
