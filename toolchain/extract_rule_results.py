#!/usr/bin/env python3
"""Generate a grouped summary table from rule log files in toolchain/results."""

import argparse
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser(description="Extract rule results summary")
    parser.add_argument(
        "--results-dir",
        default="toolchain/results",
        help="Directory containing rule logs"
    )
    parser.add_argument(
        "--output",
        default="toolchain/results/rules_summary.md",
        help="Summary output file"
    )
    return parser.parse_args()


def read_status(log_path):
    status = "UNKNOWN"
    with open(log_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            if line.startswith("STATUS:"):
                status = line.split(":", 1)[1].strip()
                break
    return status


def main():
    args = parse_args()
    results_dir = Path(args.results_dir)
    if not results_dir.exists():
        print(f"Results directory not found: {results_dir}")
        return 1

    logs = sorted(results_dir.glob("RULE-*.log"))
    if not logs:
        print("No rule logs found.")
        return 1

    rule_groups = {
        "nasa": [
            "RULE-001", "RULE-002", "RULE-007", "RULE-009", "RULE-011",
            "RULE-012", "RULE-014", "RULE-023", "RULE-024", "RULE-026",
            "RULE-029", "RULE-033", "RULE-034", "RULE-037", "RULE-038",
            "RULE-039", "RULE-040",
        ],
        "misra": [
            "RULE-049", "RULE-050", "RULE-051", "RULE-054", "RULE-055",
            "RULE-056", "RULE-058", "RULE-060", "RULE-062", "RULE-063",
            "RULE-064", "RULE-065", "RULE-066",
        ],
        "iso": [
            "RULE-067", "RULE-068",
        ],
        "compiler": [
            "RULE-023", "RULE-024", "RULE-037", "RULE-055",
            "RULE-056", "RULE-062",
        ],
        "cppcheck": [
            "RULE-039", "RULE-040", "RULE-049", "RULE-050", "RULE-051",
            "RULE-054", "RULE-055", "RULE-056", "RULE-058", "RULE-060",
            "RULE-062", "RULE-064",
        ],
        "heuristic": [
            "RULE-011", "RULE-012", "RULE-014", "RULE-023", "RULE-024",
            "RULE-026", "RULE-033", "RULE-034", "RULE-063", "RULE-064",
            "RULE-067", "RULE-068",
        ],
    }

    status_by_rule = {}
    for log_path in logs:
        rule_id = log_path.stem
        status_by_rule[rule_id] = read_status(log_path)

    lines = []
    lines.append("# Rule Summary")
    grouped = set()
    for group_name, group_rule_ids in rule_groups.items():
        if not any(rule_id in status_by_rule for rule_id in group_rule_ids):
            continue
        lines.append("")
        lines.append(f"## Group: {group_name}")
        lines.append("| Rule ID | Status | Log |")
        lines.append("|---------|--------|-----|")
        for rule_id in group_rule_ids:
            if rule_id not in status_by_rule:
                continue
            log_name = f"{rule_id}.log"
            lines.append(f"| {rule_id} | {status_by_rule[rule_id]} | {log_name} |")
            grouped.add(rule_id)

    ungrouped = sorted(rule_id for rule_id in status_by_rule.keys() if rule_id not in grouped)
    if ungrouped:
        lines.append("")
        lines.append("## Group: other")
        lines.append("| Rule ID | Status | Log |")
        lines.append("|---------|--------|-----|")
        for rule_id in ungrouped:
            log_name = f"{rule_id}.log"
            lines.append(f"| {rule_id} | {status_by_rule[rule_id]} | {log_name} |")

    summary = "\n".join(lines) + "\n"
    print(summary)

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(summary)

    print(f"Summary written to: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
