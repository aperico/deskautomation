#!/usr/bin/env python3
"""
Common utilities and configuration for pipeline scripts.

This module provides shared functionality used across all pipeline subscripts.
"""

import subprocess
import sys
from pathlib import Path
from typing import List, Dict, Optional, Tuple


# ============================================================================
# Configuration
# ============================================================================
class Config:
    """Pipeline configuration settings."""
    
    def __init__(self):
        self.script_dir = Path(__file__).parent.resolve()
        self.workspace_root = self.script_dir.parent  # toolchain -> workspace root
        self.build_dir = self.workspace_root / "build"
        self.source_dir = self.workspace_root / "src"
        self.results_dir = self.script_dir / "results"  # toolchain/results/
        self.mingw_path = Path("C:/msys64/mingw64/bin")
        
        # Create results directory
        self.results_dir.mkdir(exist_ok=True)
        
        # Setup environment
        self.setup_environment()
    
    def setup_environment(self):
        """Set up environment variables for build tools."""
        import os
        mingw_bin = str(self.mingw_path)
        if mingw_bin not in os.environ.get('PATH', ''):
            os.environ['PATH'] = f"{mingw_bin};{os.environ.get('PATH', '')}"


# ============================================================================
# Color Output
# ============================================================================
class Colors:
    """ANSI color codes for terminal output."""
    
    # Standard colors
    GREEN = '\033[92m'
    CYAN = '\033[96m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    MAGENTA = '\033[95m'
    GRAY = '\033[90m'
    WHITE = '\033[97m'
    RESET = '\033[0m'
    
    @staticmethod
    def is_enabled():
        """Check if color output is supported."""
        return hasattr(sys.stdout, 'isatty') and sys.stdout.isatty()


def colorize(text: str, color: str) -> str:
    """Apply color to text if supported."""
    if Colors.is_enabled():
        return f"{color}{text}{Colors.RESET}"
    return text


def write_success(message: str):
    """Print success message."""
    print(colorize(f"[OK] {message}", Colors.GREEN))


def write_info(message: str):
    """Print info message."""
    print(colorize(f"[INFO] {message}", Colors.CYAN))


def write_error(message: str):
    """Print error message."""
    print(colorize(f"[ERROR] {message}", Colors.RED))


def write_header(message: str):
    """Print section header."""
    separator = "=" * 72
    print()
    print(colorize(separator, Colors.YELLOW))
    print(colorize(message, Colors.YELLOW))
    print(colorize(separator, Colors.YELLOW))
    print()


# ============================================================================
# Command Execution
# ============================================================================
def run_command(
    cmd: List[str],
    cwd: Optional[Path] = None,
    env: Optional[Dict[str, str]] = None,
    capture_output: bool = True
) -> Tuple[int, str]:
    """
    Execute a command and return exit code and output.
    
    Args:
        cmd: Command and arguments as list
        cwd: Working directory
        env: Environment variables
        capture_output: Whether to capture stdout/stderr
        
    Returns:
        Tuple of (exit_code, output)
    """
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            env=env,
            capture_output=capture_output,
            text=True,
            encoding='utf-8',
            errors='replace'
        )
        output = ""
        if capture_output:
            output = result.stdout + result.stderr
        return result.returncode, output
    except Exception as e:
        write_error(f"Command execution failed: {e}")
        return 1, str(e)
