#!/usr/bin/env python3
"""
Build pipeline subscript.

Handles all build-related operations:
- clean: Clean build directory
- configure: Run CMake configuration
- build: Build the project
- clean-build: Clean + configure + build
- rebuild: Full rebuild (alias for clean-build)
"""

import sys
import shutil
import re
from pathlib import Path
from datetime import datetime
from typing import Optional

# Import common utilities
from pipeline_common import (
    Config, Colors, colorize,
    write_success, write_info, write_error, write_header,
    run_command
)


class BuildPipeline:
    """Build pipeline operations."""
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config if config else Config()
    
    def clean(self) -> bool:
        """Clean build directory."""
        write_header("CLEANING BUILD DIRECTORY")
        
        if self.config.build_dir.exists():
            write_info("Removing build directory...")
            try:
                # Use platform-specific robust deletion
                import platform
                if platform.system() == 'Windows':
                    # Use PowerShell for robust deletion on Windows
                    import subprocess
                    cmd = f'powershell -Command "Remove-Item -Path \\"{self.config.build_dir}\\" -Recurse -Force -ErrorAction SilentlyContinue"'
                    subprocess.run(cmd, shell=True, check=False)
                    # Give Windows time to release handles
                    import time
                    time.sleep(0.5)
                    # Verify deletion
                    if self.config.build_dir.exists():
                        shutil.rmtree(self.config.build_dir, ignore_errors=True)
                else:
                    shutil.rmtree(self.config.build_dir)
                
                write_success("Build directory cleaned")
            except Exception as e:
                write_error(f"Failed to remove build directory: {e}")
                write_info("Attempting to continue anyway...")
                # Don't fail the pipeline, just warn
        else:
            write_info("Build directory does not exist, nothing to clean")
        
        # Recreate build directory
        self.config.build_dir.mkdir(exist_ok=True)
        write_success("Build directory created")
        return True
    
    def configure(self) -> bool:
        """Configure CMake."""
        write_header("CONFIGURING CMAKE")
        
        write_info("Running CMake configuration...")
        config_result_file = self.config.results_dir / "cmake_configure.txt"
        
        start_time = datetime.now()
        exit_code, output = run_command(
            ["cmake", "-S", ".", "-B", "build"],
            cwd=self.config.workspace_root
        )
        duration = (datetime.now() - start_time).total_seconds()
        
        # Save configuration output to file
        header = f"""{'=' * 80}
CMAKE CONFIGURATION LOG
{'=' * 80}
Command: cmake -S . -B build
Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
Workspace: {self.config.workspace_root}

This log contains the output from CMake configuration phase, including
compiler detection, dependency resolution, and build system generation.
{'=' * 80}

"""
        with open(config_result_file, 'w', encoding='utf-8') as f:
            f.write(header)
            f.write(output)
        
        # Check for warnings
        warnings = [line for line in output.split('\n') if re.search(r'warning', line, re.IGNORECASE)]
        if warnings:
            print(colorize("\nConfiguration Warnings:", Colors.YELLOW))
            for warning in warnings[:10]:  # Limit to first 10
                print(colorize(f"  {warning}", Colors.YELLOW))
        
        if exit_code != 0:
            write_error("CMake configuration failed")
            print(output)
            write_info(f"Configuration log saved to: {config_result_file}")
            return False
        
        write_success(f"CMake configuration completed in {duration:.2f} seconds")
        write_info(f"Configuration log saved to: {config_result_file}")
        return True
    
    def build(self) -> bool:
        """Build project."""
        write_header("BUILDING PROJECT")
        
        write_info("Building project (Release configuration)...")
        build_result_file = self.config.results_dir / "build_log.txt"
        
        exit_code, output = run_command(
            ["cmake", "--build", "build", "--config", "Release"],
            cwd=self.config.workspace_root
        )
        
        # Save build output to file
        header = f"""{'=' * 80}
BUILD LOG
{'=' * 80}
Command: cmake --build build --config Release
Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
Workspace: {self.config.workspace_root}
Configuration: Release

This log contains the complete build output including compilation of all
source files, linking, and any compiler warnings or errors encountered.
{'=' * 80}

"""
        with open(build_result_file, 'w', encoding='utf-8') as f:
            f.write(header)
            f.write(output)
        
        # Extract and display compiler diagnostics
        diagnostic_patterns = [
            r'warning', r'error', r'note:', r'deprecated', r'error:',
            r'^\s*\^', r'In function', r'undefined reference',
            r'multiple definition', r'conversion', r'initializer',
            r'unused', r'format'
        ]
        diagnostics = []
        for line in output.split('\n'):
            if any(re.search(pattern, line, re.IGNORECASE) for pattern in diagnostic_patterns):
                diagnostics.append(line)
        
        if diagnostics:
            print(colorize("\nCompiler Diagnostics:", Colors.YELLOW))
            for diag in diagnostics[:20]:  # Limit output
                if re.search(r'error', diag, re.IGNORECASE) and not re.search(r'In function', diag):
                    print(colorize(f"  {diag}", Colors.RED))
                elif re.search(r'warning', diag, re.IGNORECASE):
                    print(colorize(f"  {diag}", Colors.YELLOW))
                elif re.search(r'note:|undefined reference|multiple definition|conversion|deprecated', diag, re.IGNORECASE):
                    print(colorize(f"  {diag}", Colors.CYAN))
                else:
                    print(colorize(f"  {diag}", Colors.GRAY))
        
        if exit_code != 0:
            write_error("Build failed")
            print(colorize("\nBuild Errors:", Colors.RED))
            for line in output.split('\n')[:30]:
                print(colorize(f"  {line}", Colors.RED))
            write_info(f"Build log saved to: {build_result_file}")
            return False
        
        write_success("Build completed successfully")
        if not diagnostics:
            write_success("No compiler warnings detected")
        write_info(f"Build log saved to: {build_result_file}")
        return True


def main():
    """Main entry point for standalone execution."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Build pipeline subscript')
    parser.add_argument(
        'command',
        choices=['clean', 'configure', 'build', 'clean-build', 'rebuild'],
        help='Build command to execute'
    )
    
    args = parser.parse_args()
    
    print(colorize(f"\nBuild Pipeline - {args.command}", Colors.MAGENTA))
    
    config = Config()
    pipeline = BuildPipeline(config)
    
    start_time = datetime.now()
    success = True
    
    try:
        if args.command == 'clean':
            success = pipeline.clean()
        
        elif args.command == 'configure':
            success = pipeline.configure()
        
        elif args.command == 'build':
            success = pipeline.build()
        
        elif args.command in ['clean-build', 'rebuild']:
            success = pipeline.clean()
            if success:
                success = pipeline.configure()
            if success:
                success = pipeline.build()
        
        if not success:
            write_error("Build pipeline failed")
            return 1
        
        duration = (datetime.now() - start_time).total_seconds()
        print()
        write_success(f"Build pipeline completed in {duration:.2f} seconds")
        return 0
        
    except KeyboardInterrupt:
        write_error("\nBuild pipeline interrupted by user")
        return 1
    except Exception as e:
        write_error(f"Build pipeline failed: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
