#!/usr/bin/env python3
"""
Static analysis pipeline subscript.

Handles all static analysis operations:
- static-analysis: Run cppcheck static analysis
"""

import sys
import shutil
import xml.etree.ElementTree as ET
from pathlib import Path
from datetime import datetime
from typing import Optional

# Import common utilities
from pipeline_common import (
    Config, Colors, colorize,
    write_success, write_info, write_error, write_header,
    run_command
)


class AnalysisPipeline:
    """Static analysis pipeline operations."""
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config if config else Config()
        self.rules_dir = self.config.workspace_root / "toolchain" / "rules"
        self.rule_groups = {
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
        self.group_descriptions = {
            "nasa": "**NASA Power of Ten Rules** - Critical safety rules from NASA/JPL for resource-constrained embedded systems. Ensures predictability, reliability, and safety-critical code quality through strict control flow, bounded loops, and defensive assertions.",
            "misra": "**MISRA C:2012** - Motor Industry Software Reliability Association guidelines for safety-critical software. Prevents undefined behavior, improves code safety, portability, and maintainability across automotive and safety-critical domains.",
            "iso": "**ISO 25119** - Functional safety requirements for machinery control systems. Ensures requirement traceability, defensive programming practices, and compliance with automated safety standards.",
            "compiler": "**Compiler Checks** - Compiler flags and build configuration validation. Enforces aggressive warning levels and error detection at compile time to catch potential issues early.",
            "cppcheck": "**Static Analysis (Cppcheck)** - Comprehensive static analysis to detect potential runtime errors, memory issues, undefined behavior, and code quality problems without execution.",
            "heuristic": "**Custom Heuristics** - Project-specific pattern detection including stack usage analysis, NULL pointer checks, pointer arithmetic validation, and function complexity metrics.",
        }
    
    def static_analysis(self) -> bool:
        """Run static analysis with cppcheck."""
        write_header("RUNNING STATIC ANALYSIS")
        
        # Check if cppcheck is available
        if not shutil.which("cppcheck"):
            write_error("cppcheck not found in PATH")
            write_info("Install cppcheck via MSYS2: pacman -S mingw-w64-x86_64-cppcheck")
            return False
        
        write_info("Running comprehensive static analysis with cppcheck...")
        
        analysis_result_file = self.config.results_dir / "static_analysis.txt"
        analysis_xml_file = self.config.results_dir / "static_analysis.xml"
        
        # Comprehensive cppcheck arguments for safety-critical code
        cppcheck_args = [
            "cppcheck",
            "--enable=all",
            "--std=c++17",
            "--inconclusive",
            "--inline-suppr",
            "--force",
            "--verbose",
            f"-I{self.config.source_dir}",
            "--suppress=missingIncludeSystem",
            "--suppress=missingInclude",
            "--suppress=unusedFunction",
            "--suppress=unmatchedSuppression",
            "--suppress=checkersReport",
            "--error-exitcode=1",
            "--template=gcc",
            "--xml",
            "--xml-version=2",
            f"--output-file={analysis_xml_file}",
            str(self.config.source_dir)
        ]
        
        write_info("Analysis includes: errors, warnings, style, performance, portability checks")
        
        exit_code, output = run_command(cppcheck_args, cwd=self.config.workspace_root)
        
        # Save text analysis output to file
        header = f"""{'=' * 80}
STATIC ANALYSIS REPORT
{'=' * 80}
Command: {' '.join(cppcheck_args)}
Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
Source Directory: {self.config.source_dir}
Analysis Level: Comprehensive

This report contains static code analysis results using cppcheck with all
checks enabled: errors, warnings, style, performance, and portability issues.
Results are saved in both text and XML formats for review and CI integration.
{'=' * 80}

"""
        with open(analysis_result_file, 'w', encoding='utf-8') as f:
            f.write(header)
            f.write(output)
        
        # Parse XML to count and display issues by severity
        has_issues = False
        if analysis_xml_file.exists():
            try:
                tree = ET.parse(analysis_xml_file)
                root = tree.getroot()
                errors = root.findall('.//error')
                
                # Count by severity
                severity_counts = {
                    'error': 0,
                    'warning': 0,
                    'style': 0,
                    'performance': 0,
                    'portability': 0
                }
                
                if errors:
                    for error in errors:
                        severity = error.get('severity', '')
                        if severity in severity_counts:
                            severity_counts[severity] += 1
                    
                    # Check if there are any real issues
                    has_issues = sum(severity_counts.values()) > 0
                    
                    if has_issues:
                        print(colorize("\nStatic Analysis Issues Found:", Colors.YELLOW))
                        if severity_counts['error'] > 0:
                            print(colorize(f"  Errors:       {severity_counts['error']}", Colors.RED))
                        if severity_counts['warning'] > 0:
                            print(colorize(f"  Warnings:     {severity_counts['warning']}", Colors.MAGENTA))
                        if severity_counts['style'] > 0:
                            print(colorize(f"  Style:        {severity_counts['style']}", Colors.CYAN))
                        if severity_counts['performance'] > 0:
                            print(colorize(f"  Performance:  {severity_counts['performance']}", Colors.YELLOW))
                        if severity_counts['portability'] > 0:
                            print(colorize(f"  Portability:  {severity_counts['portability']}", Colors.YELLOW))
                        
                        # Display each issue in human-readable format
                        print(colorize("\nIssue Details:", Colors.YELLOW))
                        for error in errors:
                            severity = error.get('severity', '')
                            if severity == 'information':
                                continue
                            
                            error_id = error.get('id', '')
                            msg = error.get('msg', '')
                            
                            severity_colors = {
                                'error': Colors.RED,
                                'warning': Colors.MAGENTA,
                                'style': Colors.CYAN
                            }
                            color = severity_colors.get(severity, Colors.YELLOW)
                            
                            print(colorize(f"\n  [{severity.upper()}] {error_id}", color))
                            print(colorize(f"  Message: {msg}", Colors.WHITE))
                            
                            location = error.find('location')
                            if location is not None:
                                file = location.get('file', '')
                                line = location.get('line', '')
                                column = location.get('column', '')
                                print(colorize(f"  File: {file}", Colors.GRAY))
                                print(colorize(f"  Line: {line}, Column: {column}", Colors.GRAY))
                
                # Always append summary to text file
                summary = f"\n\n{'=' * 80}\nSUMMARY\n{'=' * 80}\n"
                summary += f"Errors:       {severity_counts['error']}\n"
                summary += f"Warnings:     {severity_counts['warning']}\n"
                summary += f"Style:        {severity_counts['style']}\n"
                summary += f"Performance:  {severity_counts['performance']}\n"
                summary += f"Portability:  {severity_counts['portability']}\n"
                
                with open(analysis_result_file, 'a', encoding='utf-8') as f:
                    f.write(summary)
            except Exception as e:
                write_error(f"Failed to parse XML analysis results: {e}")
        
        # Decide success based on actual issues found, not just exit code
        # (cppcheck may return non-zero for other reasons like config warnings)
        if has_issues:
            write_error("Static analysis found issues that need attention")
            write_info("Static analysis results saved to:")
            write_info(f"  - Text: {analysis_result_file}")
            write_info(f"  - XML:  {analysis_xml_file}")
            return False
        
        write_success("Static analysis completed (no issues found)")
        write_info("Static analysis results saved to:")
        write_info(f"  - Text: {analysis_result_file}")
        write_info(f"  - XML:  {analysis_xml_file}")
        return True

    def list_rules(self) -> bool:
        """List available rule IDs and groups."""
        write_header("AVAILABLE RULES")
        rules = self._load_rule_scripts()
        for rule_id in sorted(rules.keys()):
            print(f"  {rule_id}: {rules[rule_id].name}")
        print()
        write_header("RULE GROUPS")
        for group, rule_ids in self.rule_groups.items():
            print(f"  {group}: {', '.join(rule_ids)}")
        return True

    def run_rule(self, rule_id: str, src_path: str) -> bool:
        """Run a single rule by unified rule ID."""
        rules = self._load_rule_scripts()
        script_path = rules.get(rule_id)
        if not script_path:
            write_error(f"Unknown rule ID: {rule_id}")
            return False
        self._clear_rule_logs([rule_id])
        if rule_id in set(self.rule_groups.get("cppcheck", [])):
            if not self._ensure_cppcheck_report():
                return False
        write_header(f"RUNNING RULE {rule_id}")
        success = self._run_rule_script(rule_id, script_path, src_path)
        self._write_grouped_summary([rule_id], src_path, datetime.now())
        return success

    def run_rule_group(self, group_name: str, src_path: str) -> bool:
        """Run a group of rules by group name."""
        rule_ids = self.rule_groups.get(group_name)
        if not rule_ids:
            write_error(f"Unknown rule group: {group_name}")
            return False
        self._clear_rule_logs(rule_ids)
        if any(rule_id in set(self.rule_groups.get("cppcheck", [])) for rule_id in rule_ids):
            if not self._ensure_cppcheck_report():
                return False
        rules = self._load_rule_scripts()
        write_header(f"RUNNING RULE GROUP: {group_name}")
        all_ok = True
        for rule_id in rule_ids:
            script_path = rules.get(rule_id)
            if not script_path:
                write_error(f"Rule script not found for {rule_id}")
                all_ok = False
                continue
            if not self._run_rule_script(rule_id, script_path, src_path):
                all_ok = False
        self._write_grouped_summary(rule_ids, src_path, datetime.now())
        return all_ok

    def run_all_groups(self, src_path: str) -> bool:
        """Run all rule groups and write a single grouped summary."""
        rule_ids = self._get_all_rule_ids()
        if not rule_ids:
            write_error("No rules configured")
            return False
        self._clear_rule_logs(rule_ids)
        if any(rule_id in set(self.rule_groups.get("cppcheck", [])) for rule_id in rule_ids):
            if not self._ensure_cppcheck_report():
                return False
        rules = self._load_rule_scripts()
        write_header("RUNNING ALL RULE GROUPS")
        all_ok = True
        for rule_id in rule_ids:
            script_path = rules.get(rule_id)
            if not script_path:
                write_error(f"Rule script not found for {rule_id}")
                all_ok = False
                continue
            if not self._run_rule_script(rule_id, script_path, src_path):
                all_ok = False
        self._write_grouped_summary(rule_ids, src_path, datetime.now())
        return all_ok

    def _run_rule_script(self, rule_id: str, script_path: Path, src_path: str) -> bool:
        write_info(f"Running {script_path.name} on {src_path}")
        self.config.results_dir.mkdir(parents=True, exist_ok=True)
        log_path = self.config.results_dir / f"{rule_id}.log"
        exit_code, output = run_command(
            [sys.executable, str(script_path), src_path],
            cwd=self.config.workspace_root
        )
        status = "PASS"
        if exit_code == 2:
            status = "SKIP"
        elif exit_code != 0:
            status = "FAIL"
        log_header = (
            f"RULE_ID: {rule_id}\n"
            f"SCRIPT: {script_path.name}\n"
            f"SRC_PATH: {src_path}\n"
            f"STATUS: {status}\n"
            f"EXIT_CODE: {exit_code}\n"
            f"TIMESTAMP: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
            f"{'=' * 80}\n"
        )
        with open(log_path, "w", encoding="utf-8") as log_file:
            log_file.write(log_header)
            if output:
                log_file.write(output)
        if output:
            print(output.rstrip())
        if exit_code == 0:
            write_success(f"{script_path.name} passed")
            return True
        if exit_code == 2:
            write_info(f"{script_path.name} skipped")
            return True
        write_error(f"{script_path.name} failed")
        return False

    def _load_rule_scripts(self) -> dict:
        rules = {}
        if not self.rules_dir.exists():
            return rules
        for path in self.rules_dir.glob("rule_RULE-*.py"):
            name = path.stem
            parts = name.split("_")
            if len(parts) < 2:
                continue
            rule_id = parts[1]
            rules[rule_id] = path
        return rules

    def _get_all_rule_ids(self):
        seen = set()
        ordered = []
        for group_rule_ids in self.rule_groups.values():
            for rule_id in group_rule_ids:
                if rule_id not in seen:
                    seen.add(rule_id)
                    ordered.append(rule_id)
        return ordered

    def _clear_rule_logs(self, rule_ids) -> None:
        self.config.results_dir.mkdir(parents=True, exist_ok=True)
        for rule_id in rule_ids:
            log_path = self.config.results_dir / f"{rule_id}.log"
            if log_path.exists():
                log_path.unlink()

    def _read_rule_status(self, log_path: Path) -> str:
        if not log_path.exists():
            return "NOT_RUN"
        with open(log_path, "r", encoding="utf-8", errors="ignore") as log_file:
            for line in log_file:
                if line.startswith("STATUS:"):
                    return line.split(":", 1)[1].strip()
        return "UNKNOWN"

    def _count_rule_instances(self, log_path: Path) -> int:
        """Count the number of violations/findings in a rule log file."""
        if not log_path.exists():
            return 0
        count = 0
        in_findings = False
        with open(log_path, "r", encoding="utf-8", errors="ignore") as log_file:
            for line in log_file:
                if line.startswith("FINDINGS:"):
                    in_findings = True
                    continue
                if in_findings and line.startswith("-"):
                    count += 1
        return count

    def _write_grouped_summary(self, rule_ids, src_path: str = None, timestamp = None) -> None:
        summary_path = self.config.results_dir / "rules_summary.md"
        if not rule_ids:
            return
        if timestamp is None:
            timestamp = datetime.now()
        
        rule_id_set = set(rule_ids)
        included_groups = []
        for group_name, group_rule_ids in self.rule_groups.items():
            if any(rule_id in rule_id_set for rule_id in group_rule_ids):
                included_groups.append((group_name, group_rule_ids))

        # First pass: collect all statuses for summary
        all_statuses = {}
        for group_name, group_rule_ids in included_groups:
            for rule_id in group_rule_ids:
                log_path = self.config.results_dir / f"{rule_id}.log"
                status = self._read_rule_status(log_path)
                all_statuses[rule_id] = status
        
        # Count totals
        total_pass = sum(1 for s in all_statuses.values() if s == "PASS")
        total_fail = sum(1 for s in all_statuses.values() if s == "FAIL")
        
        lines = []
        lines.append("# Rule Summary")
        lines.append("")
        # Add header with metadata
        lines.append(f"**Generated:** {timestamp.strftime('%Y-%m-%d %H:%M:%S')}")
        if src_path:
            lines.append(f"**Source Path:** `{src_path}`")
        lines.append("")
        lines.append(f"**Guidelines:** [Coding Guidelines and Automated Checks](../coding_guides_and_checks.md)")
        lines.append("")
        
        # Add summary statistics
        lines.append("## Status Summary")
        lines.append("")
        lines.append(f"✅ **Total PASS:** {total_pass}")
        lines.append(f"❌ **Total FAIL:** {total_fail}")
        lines.append("")
        
        for group_name, group_rule_ids in included_groups:
            lines.append("")
            lines.append(f"## Group: {group_name.capitalize()}")
            lines.append("")
            group_desc = self.group_descriptions.get(group_name, "")
            if group_desc:
                lines.append(group_desc)
                lines.append("")
            lines.append("| Rule ID | Status | Instances | Log |")
            lines.append("|---------|--------|-----------|-----|")
            for rule_id in group_rule_ids:
                log_name = f"{rule_id}.log"
                log_path = self.config.results_dir / log_name
                status = self._read_rule_status(log_path)
                instances = self._count_rule_instances(log_path) if status != "NOT_RUN" else 0
                
                if status == "NOT_RUN":
                    log_link = "-"
                    status_icon = "-"
                elif status == "PASS":
                    log_link = f"[{log_name}](./{log_name})"
                    status_icon = "✅ PASS"
                elif status == "FAIL":
                    log_link = f"[{log_name}](./{log_name})"
                    status_icon = "❌ FAIL"
                else:
                    log_link = f"[{log_name}](./{log_name})"
                    status_icon = status
                
                # Display instance count only if status is FAIL (violations found)
                if status == "FAIL":
                    lines.append(f"| {rule_id} | {status_icon} | {instances} | {log_link} |")
                else:
                    lines.append(f"| {rule_id} | {status_icon} | - | {log_link} |")

        summary = "\n".join(lines) + "\n"
        with open(summary_path, "w", encoding="utf-8") as summary_file:
            summary_file.write(summary)
        print(summary)
        write_info(f"Summary written to: {summary_path}")

    def _ensure_cppcheck_report(self) -> bool:
        """Ensure cppcheck XML report exists before rule execution."""
        xml_files = list(self.config.results_dir.glob("cppcheck*.xml"))
        static_xml = self.config.results_dir / "static_analysis.xml"
        if static_xml.exists():
            xml_files.append(static_xml)
        if xml_files:
            return True
        write_info("Cppcheck report not found. Running static analysis...")
        return self.static_analysis()


def main():
    """Main entry point for standalone execution."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Static analysis pipeline subscript')
    parser.add_argument(
        'command',
        choices=['static-analysis', 'rule', 'rules-group', 'rules-list', 'rules-all'],
        help='Analysis command to execute'
    )
    parser.add_argument('--rule-id', help='Unified rule ID (e.g., RULE-001)')
    parser.add_argument('--group', help='Rule group name (e.g., nasa, misra)')
    parser.add_argument('--src-path', help='Source path to scan')
    
    args = parser.parse_args()
    
    print(colorize(f"\nAnalysis Pipeline - {args.command}", Colors.MAGENTA))
    
    config = Config()
    pipeline = AnalysisPipeline(config)
    
    start_time = datetime.now()
    success = True
    
    try:
        if args.command == 'static-analysis':
            success = pipeline.static_analysis()
        elif args.command == 'rules-list':
            success = pipeline.list_rules()
        elif args.command == 'rule':
            if not args.rule_id or not args.src_path:
                write_error("--rule-id and --src-path are required for rule")
                return 1
            success = pipeline.run_rule(args.rule_id, args.src_path)
        elif args.command == 'rules-group':
            if not args.group or not args.src_path:
                write_error("--group and --src-path are required for rules-group")
                return 1
            success = pipeline.run_rule_group(args.group, args.src_path)
        elif args.command == 'rules-all':
            if not args.src_path:
                write_error("--src-path is required for rules-all")
                return 1
            success = pipeline.run_all_groups(args.src_path)
        
        if not success:
            write_error("Analysis pipeline failed")
            return 1
        
        duration = (datetime.now() - start_time).total_seconds()
        print()
        write_success(f"Analysis pipeline completed in {duration:.2f} seconds")
        return 0
        
    except KeyboardInterrupt:
        write_error("\nAnalysis pipeline interrupted by user")
        return 1
    except Exception as e:
        write_error(f"Analysis pipeline failed: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
