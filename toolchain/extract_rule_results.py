#!/usr/bin/env python3
"""Generate a summary table from rule log files in toolchain/results."""

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

    rows = []
    for log_path in logs:
        rule_id = log_path.stem
        status = read_status(log_path)
        rows.append((rule_id, status, log_path.name))

    lines = []
    lines.append("| Rule ID | Status | Log |")
    lines.append("|---------|--------|-----|")
    for rule_id, status, log_name in rows:
        lines.append(f"| {rule_id} | {status} | {log_name} |")

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
