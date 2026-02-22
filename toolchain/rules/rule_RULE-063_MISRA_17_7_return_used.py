#!/usr/bin/env python3
from common import parse_args, check_unused_result_heuristic, report

RULE_ID = "RULE-063"
ORIGINAL_ID = "MISRA-17.7"
DESCRIPTION = "Use return values (heuristic)"

if __name__ == "__main__":
    args = parse_args()
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
