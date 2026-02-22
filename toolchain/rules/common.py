#!/usr/bin/env python3
"""Common utilities for rule checks.

All rule scripts expect a single positional argument: src/path
"""

import argparse
import re
import sys
from pathlib import Path

DEFAULT_EXTENSIONS = [".c", ".h", ".cpp", ".hpp"]


def parse_args():
    parser = argparse.ArgumentParser(description="Rule check")
    parser.add_argument("src_path", help="Path to source root or file")
    return parser.parse_args()


def iter_source_files(root_path, extensions=None):
    extensions = extensions or DEFAULT_EXTENSIONS
    root = Path(root_path)
    if root.is_file():
        if root.suffix in extensions:
            return [root]
        return []
    files = []
    for ext in extensions:
        files.extend(root.rglob(f"*{ext}"))
    return files


def read_text(path):
    return Path(path).read_text(encoding="utf-8", errors="ignore")


def find_repo_root(start_path):
    start = Path(start_path).resolve()
    if start.is_file():
        start = start.parent
    for parent in [start] + list(start.parents):
        if (parent / "CMakeLists.txt").exists():
            return parent
    return start


def find_regex_in_files(root_path, patterns, extensions=None, flags=0):
    findings = []
    files = iter_source_files(root_path, extensions)
    for path in files:
        text = read_text(path)
        for pattern in patterns:
            for match in re.finditer(pattern, text, flags):
                line_no = text.count("\n", 0, match.start()) + 1
                line = text.splitlines()[line_no - 1].strip()
                findings.append((path, line_no, line))
    return findings


def report(rule_id, original_id, description, findings, status_override=None):
    if status_override:
        status = status_override
    else:
        status = "FAIL" if findings else "PASS"
    print(f"RULE: {rule_id} ({original_id})")
    print(f"DESC: {description}")
    print(f"STATUS: {status}")
    if findings:
        print("FINDINGS:")
        for path, line_no, line in findings:
            print(f"- {path}:{line_no}: {line}")
    if status == "FAIL":
        sys.exit(1)
    if status == "SKIP":
        sys.exit(2)
    sys.exit(0)


def check_cmake_flags(root_path, required_flags):
    repo_root = find_repo_root(root_path)
    cmake_path = repo_root / "CMakeLists.txt"
    if not cmake_path.exists():
        return None, "SKIP"
    text = read_text(cmake_path)
    missing = [flag for flag in required_flags if flag not in text]
    if missing:
        findings = [(cmake_path, 1, f"Missing flags: {', '.join(missing)}")]
        return findings, None
    return [], None


def check_build_warnings(root_path):
    repo_root = find_repo_root(root_path)
    log_paths = [
        repo_root / "toolchain" / "results" / "build.log",
        repo_root / "toolchain" / "results" / "build_log.txt",
        repo_root / "toolchain" / "results" / "build_warnings.log",
    ]
    header_prefixes = (
        "build log",
        "build warnings log",
        "command:",
        "timestamp:",
        "workspace:",
        "configuration:",
        "this log contains",
    )
    existing = [path for path in log_paths if path.exists()]
    if existing:
        log_path = max(existing, key=lambda path: path.stat().st_mtime)
        text = read_text(log_path)
        if log_path.name == "build_warnings.log":
            if "no warnings detected" in text.lower():
                return [], None
        for line in text.splitlines():
            stripped = line.strip()
            if not stripped or stripped.startswith("="):
                continue
            if stripped.lower().startswith(header_prefixes):
                continue
            if re.search(r"\bwarning\b", stripped, re.IGNORECASE):
                return [(log_path, 1, "Warnings found in build log")], None
        return [], None
    return None, "SKIP"


def check_cppcheck_report(root_path):
    repo_root = find_repo_root(root_path)
    results_dir = repo_root / "toolchain" / "results"
    if not results_dir.exists():
        return None, "SKIP"
    xml_files = list(results_dir.glob("cppcheck*.xml"))
    static_xml = results_dir / "static_analysis.xml"
    if static_xml.exists():
        xml_files.append(static_xml)
    if not xml_files:
        return None, "SKIP"
    return xml_files, None


def check_cppcheck_errors(root_path):
    xml_files, status = check_cppcheck_report(root_path)
    if status == "SKIP":
        return None, "SKIP"
    findings = []
    for xml_path in xml_files:
        try:
            import xml.etree.ElementTree as ET
            tree = ET.parse(xml_path)
            root = tree.getroot()
            for error in root.findall('.//error'):
                if error.get('severity', '') == 'information':
                    continue
                findings.append((xml_path, 1, "Cppcheck reported issues"))
                break
        except Exception:
            text = read_text(xml_path)
            if "<error" in text:
                findings.append((xml_path, 1, "Cppcheck reported issues"))
    return findings, None


def check_cppcheck_pattern(root_path, patterns):
    xml_files, status = check_cppcheck_report(root_path)
    if status == "SKIP":
        return None, "SKIP"
    findings = []
    for xml_path in xml_files:
        text = read_text(xml_path)
        for pattern in patterns:
            if re.search(pattern, text):
                findings.append((xml_path, 1, f"Matched cppcheck pattern: {pattern}"))
    return findings, None


def extract_functions(text):
    functions = []
    lines = text.splitlines()
    in_func = False
    brace_depth = 0
    func_start = 0
    for i, line in enumerate(lines):
        stripped = line.strip()
        if not in_func:
            if re.match(r"^[a-zA-Z_][\w\s\*]*\s+[a-zA-Z_][\w]*\s*\([^;]*\)\s*\{\s*$", stripped):
                in_func = True
                brace_depth = 1
                func_start = i
        else:
            brace_depth += stripped.count("{")
            brace_depth -= stripped.count("}")
            if brace_depth == 0:
                func_body = "\n".join(lines[func_start:i + 1])
                functions.append((func_start + 1, i + 1, func_body))
                in_func = False
    return functions


def check_function_length(root_path, max_lines):
    findings = []
    for path in iter_source_files(root_path):
        text = read_text(path)
        for start, end, body in extract_functions(text):
            code_lines = [l for l in body.splitlines() if l.strip() and not l.strip().startswith("//")]
            if len(code_lines) > max_lines:
                findings.append((path, start, f"Function length {len(code_lines)} > {max_lines}"))
    return findings


def check_complexity(root_path, max_complexity):
    findings = []
    keywords = ["if", "for", "while", "case", "&&", "||", "?"]
    for path in iter_source_files(root_path):
        text = read_text(path)
        for start, end, body in extract_functions(text):
            complexity = 1
            for kw in keywords:
                complexity += body.count(kw)
            if complexity > max_complexity:
                findings.append((path, start, f"Cyclomatic complexity {complexity} > {max_complexity}"))
    return findings


def check_stack_usage(root_path, max_bytes):
    findings = []
    type_sizes = {
        "char": 1,
        "int8_t": 1,
        "uint8_t": 1,
        "bool": 1,
        "int16_t": 2,
        "uint16_t": 2,
        "int32_t": 4,
        "uint32_t": 4,
        "float": 4,
        "double": 8,
    }
    array_decl = re.compile(r"^\s*(?:const\s+)?([A-Za-z_][\w:]*)\s+\w+\s*\[\s*(\d+)\s*\]\s*(?:=\s*\{)?\s*;\s*$")
    for path in iter_source_files(root_path):
        text = read_text(path)
        lines = text.splitlines()
        for start, end, body in extract_functions(text):
            total_bytes = 0
            body_lines = body.splitlines()
            for idx, line in enumerate(body_lines, start=start):
                match = array_decl.match(line)
                if not match:
                    continue
                type_name = match.group(1)
                count = int(match.group(2))
                elem_size = type_sizes.get(type_name)
                if elem_size is None:
                    continue
                total_bytes += elem_size * count
            if total_bytes > max_bytes:
                findings.append((path, start, f"Estimated stack arrays {total_bytes} bytes > {max_bytes}"))
    return findings


def check_uninitialized_heuristic(root_path):
    findings = []
    pattern = re.compile(r"^\s*(?!extern|static|const|typedef|struct|enum|union|volatile)\w[\w\s\*]*\s+\w+\s*;\s*$")
    for path in iter_source_files(root_path):
        if path.suffix in [".h", ".hpp"]:
            continue
        lines = read_text(path).splitlines()
        for idx, line in enumerate(lines, start=1):
            stripped = line.strip()
            if not stripped:
                continue
            if stripped.startswith("return "):
                continue
            if "(" in line or ")" in line:
                continue
            if pattern.match(line):
                findings.append((path, idx, "Possible uninitialized declaration"))
    return findings


def check_unused_result_heuristic(root_path, allowlist=None):
    findings = []
    allowlist = allowlist or set()
    pattern = re.compile(r"^\s*[A-Za-z_]\w*\s*\([^;]*\)\s*;\s*$")
    skip = re.compile(r"^\s*(if|for|while|switch|return)\b")
    name_match = re.compile(r"^\s*([A-Za-z_]\w*)\s*\(")
    for path in iter_source_files(root_path):
        lines = read_text(path).splitlines()
        for idx, line in enumerate(lines, start=1):
            if skip.match(line):
                continue
            if pattern.match(line) and "(void)" not in line:
                name = None
                match = name_match.match(line)
                if match:
                    name = match.group(1)
                if name and name in allowlist:
                    continue
                findings.append((path, idx, "Possible unchecked return value"))
    return findings


def check_null_deref_heuristic(root_path):
    findings = []
    null_check_template = r"\b{var}\b\s*(==|!=)\s*(NULL|nullptr)"
    null_check_reverse = r"\b(NULL|nullptr)\s*(==|!=)\s*\b{var}\b"
    negated_check = r"!\s*\b{var}\b"
    for path in iter_source_files(root_path):
        text = read_text(path)
        for start, end, body in extract_functions(text):
            body_lines = body.splitlines()
            for offset, line in enumerate(body_lines, start=0):
                match = re.search(r"\b(\w+)\s*->", line)
                if not match:
                    continue
                var = match.group(1)
                prior = "\n".join(body_lines[:offset + 1])
                has_check = re.search(null_check_template.format(var=re.escape(var)), prior)
                if not has_check:
                    has_check = re.search(null_check_reverse.format(var=re.escape(var)), prior)
                if not has_check:
                    has_check = re.search(negated_check.format(var=re.escape(var)), prior)
                if not has_check:
                    line_no = start + offset
                    findings.append((path, line_no, f"Pointer '{var}' used without NULL check"))
    return findings


def check_pointer_arithmetic(root_path):
    findings = []
    decl_ptr = re.compile(r"\b\w[\w\s]*\*\s*(\w+)\b")
    extra_ptr = re.compile(r",\s*\*\s*(\w+)\b")
    for path in iter_source_files(root_path):
        text = read_text(path)
        pointer_vars = set()
        for line in text.splitlines():
            if "*" not in line:
                continue
            match = decl_ptr.search(line)
            if match:
                pointer_vars.add(match.group(1))
                for extra in extra_ptr.findall(line):
                    pointer_vars.add(extra)
        if not pointer_vars:
            continue
        for idx, line in enumerate(text.splitlines(), start=1):
            for var in pointer_vars:
                if re.search(rf"\b{re.escape(var)}\b\s*[\+\-]=", line):
                    findings.append((path, idx, f"Pointer arithmetic on '{var}'"))
                    break
                if re.search(rf"\b{re.escape(var)}\b\s*[\+\-]\s*\d+", line):
                    findings.append((path, idx, f"Pointer arithmetic on '{var}'"))
                    break
                if re.search(rf"\*\s*\(\s*{re.escape(var)}\s*[\+\-]", line):
                    findings.append((path, idx, f"Pointer arithmetic on '{var}'"))
                    break
    return findings


def check_function_pointers(root_path):
    patterns = [r"\(\s*\*\s*\w+\s*\)\s*\("]
    return find_regex_in_files(root_path, patterns)


def check_traceability(root_path):
    findings = []
    for path in iter_source_files(root_path, extensions=[".c", ".cpp"]):
        text = read_text(path)
        lines = text.splitlines()
        for start, end, body in extract_functions(text):
            context_start = max(0, start - 4)
            context = "\n".join(lines[context_start:start] + body.splitlines())
            if "SWReq-" not in context and "SysReq-" not in context:
                findings.append((path, start, "Missing SWReq-/SysReq- traceability tag"))
    return findings


def check_assert_density(root_path, min_asserts_per_function):
    findings = []
    for path in iter_source_files(root_path):
        text = read_text(path)
        functions = extract_functions(text)
        if not functions:
            continue
        asserts = text.count("assert(")
        density = asserts / max(1, len(functions))
        if density < min_asserts_per_function:
            findings.append((path, 1, f"Assertion density {density:.2f} < {min_asserts_per_function}"))
    return findings
