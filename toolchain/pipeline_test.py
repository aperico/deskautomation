#!/usr/bin/env python3
"""
Test pipeline subscript.

Handles all test-related operations:
- test: Run all tests with dual motor type validation
- unit: Run unit tests only
- component: Run component tests only
- integration: Run integration tests only
- clean-test: Clean, build, and run all tests
"""

import sys
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


class TestPipeline:
    """Test pipeline operations."""
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config if config else Config()
    
    def set_motor_type(self, motor_type: str) -> bool:
        """
        Set motor type in motor_config.cpp.
        
        Args:
            motor_type: Either 'MT_BASIC' or 'MT_ROBUST'
            
        Returns:
            True if successful
        """
        motor_config_file = self.config.source_dir / "motor_config.cpp"
        
        if not motor_config_file.exists():
            write_error(f"motor_config.cpp not found at {motor_config_file}")
            return False
        
        try:
            content = motor_config_file.read_text(encoding='utf-8')
            
            # Replace the motor type definition
            if motor_type == "MT_BASIC":
                new_content = re.sub(
                    r'#define MOTOR_TYPE MT_ROBUST',
                    '#define MOTOR_TYPE MT_BASIC',
                    content
                )
            elif motor_type == "MT_ROBUST":
                new_content = re.sub(
                    r'#define MOTOR_TYPE MT_BASIC',
                    '#define MOTOR_TYPE MT_ROBUST',
                    content
                )
            else:
                write_error(f"Invalid motor type: {motor_type} (must be MT_BASIC or MT_ROBUST)")
                return False
            
            motor_config_file.write_text(new_content, encoding='utf-8')
            write_info(f"Motor type set to: {motor_type}")
            return True
        except Exception as e:
            write_error(f"Failed to set motor type: {e}")
            return False
    
    def run_tests(self, test_filter: Optional[str] = None, motor_type: Optional[str] = None) -> bool:
        """
        Run tests with optional filter.
        
        Args:
            test_filter: CTest regex filter (e.g., 'UnitTests' or 'Component.*')
            motor_type: Motor type label for logging (e.g., 'MT_BASIC')
            
        Returns:
            True if tests passed
        """
        filter_label = f" ({test_filter})" if test_filter else ""
        motor_label = f" - {motor_type}" if motor_type else ""
        
        write_info(f"Running test suite{filter_label}{motor_label}...")
        
        # Build command
        cmd = ["ctest", "--test-dir", "build", "-C", "Release", "--output-on-failure"]
        if test_filter:
            cmd.extend(["-R", test_filter])
        
        # Determine result file name
        if motor_type:
            result_file = self.config.results_dir / f"test_results_{motor_type}.txt"
        elif test_filter:
            result_file = self.config.results_dir / f"test_results_{test_filter}.txt"
        else:
            result_file = self.config.results_dir / "test_results.txt"
        
        exit_code, output = run_command(cmd, cwd=self.config.workspace_root)
        
        # Save test output to file
        header = f"""{'=' * 80}
TEST RESULTS{motor_label}
{'=' * 80}
Command: {' '.join(cmd)}
Filter: {test_filter if test_filter else 'All tests'}
Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
Workspace: {self.config.workspace_root}
Configuration: Release

This log contains the results of test execution.
{'=' * 80}

"""
        with open(result_file, 'w', encoding='utf-8') as f:
            f.write(header)
            f.write(output)
        
        # Parse test summary
        test_summary_lines = [
            line for line in output.split('\n')
            if re.search(r'tests passed|failed|Test #|\d+% tests passed', line)
        ]
        
        if test_summary_lines:
            print(colorize(f"\nTest Summary{filter_label}:", Colors.CYAN))
            for line in test_summary_lines:
                print(colorize(f"  {line}", Colors.CYAN))
        
        if exit_code != 0:
            write_error(f"Tests failed{filter_label}")
            print(colorize("\nFailed Test Output:", Colors.RED))
            failed_tests = [
                line for line in output.split('\n')
                if re.search(r'FAILED|failed', line, re.IGNORECASE)
            ][:10]
            for line in failed_tests:
                print(colorize(f"  {line}", Colors.RED))
            
            write_info(f"Detailed test results saved to: {result_file}")
            return False
        
        write_success(f"All tests passed{filter_label}")
        write_info(f"Test results saved to: {result_file}")
        return True
    
    def test_dual_motor(self) -> bool:
        """Run tests for both motor type configurations."""
        write_header("RUNNING TESTS - DUAL MOTOR TYPE VALIDATION")
        
        write_info("Testing both motor driver configurations (MT_BASIC and MT_ROBUST)...")
        
        motor_types = ["MT_BASIC", "MT_ROBUST"]
        all_tests_passed = True
        test_results = []
        
        for motor_type in motor_types:
            write_header(f"TESTING: {motor_type} Configuration")
            
            # Set motor type
            if not self.set_motor_type(motor_type):
                all_tests_passed = False
                continue
            
            # Rebuild with new motor type
            write_info(f"Rebuilding with {motor_type}...")
            exit_code, output = run_command(
                ["cmake", "--build", "build", "--config", "Release"],
                cwd=self.config.workspace_root
            )
            
            if exit_code != 0:
                write_error(f"Build failed for {motor_type} configuration")
                print(output)
                all_tests_passed = False
                continue
            
            write_success(f"Build successful for {motor_type}")
            
            # Run tests
            if not self.run_tests(motor_type=motor_type):
                all_tests_passed = False
            else:
                # Track result for summary
                result_file = self.config.results_dir / f"test_results_{motor_type}.txt"
                if result_file.exists():
                    content = result_file.read_text(encoding='utf-8')
                    for line in content.split('\n'):
                        if re.search(r'\d+% tests passed.*out of \d+', line):
                            test_results.append({"MotorType": motor_type, "Result": line.strip()})
                            break
        
        # Restore to default (MT_BASIC)
        write_info("Restoring default configuration (MT_BASIC)...")
        if not self.set_motor_type("MT_BASIC"):
            write_error("Failed to restore MT_BASIC configuration")
            return False
        
        # Display comprehensive summary
        write_header("DUAL MOTOR TYPE TEST SUMMARY")
        print(colorize("\nTest Results:", Colors.CYAN))
        for result in test_results:
            print(colorize(f"  [{result['MotorType']}] {result['Result']}", Colors.CYAN))
        
        if not all_tests_passed:
            write_error("One or more motor type configurations failed testing")
            write_info("Test results:")
            write_info(f"  - MT_BASIC:  {self.config.results_dir / 'test_results_MT_BASIC.txt'}")
            write_info(f"  - MT_ROBUST: {self.config.results_dir / 'test_results_MT_ROBUST.txt'}")
            return False
        
        write_success("All tests passed for both motor type configurations (MT_BASIC and MT_ROBUST)")
        write_info("Test results:")
        write_info(f"  - MT_BASIC:  {self.config.results_dir / 'test_results_MT_BASIC.txt'}")
        write_info(f"  - MT_ROBUST: {self.config.results_dir / 'test_results_MT_ROBUST.txt'}")
        return True


def main():
    """Main entry point for standalone execution."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Test pipeline subscript')
    parser.add_argument(
        'command',
        choices=['test', 'unit', 'component', 'integration'],
        help='Test command to execute'
    )
    
    args = parser.parse_args()
    
    print(colorize(f"\nTest Pipeline - {args.command}", Colors.MAGENTA))
    
    config = Config()
    pipeline = TestPipeline(config)
    
    start_time = datetime.now()
    success = True
    
    try:
        if args.command == 'test':
            # Run dual motor type validation
            success = pipeline.test_dual_motor()
        
        elif args.command == 'unit':
            success = pipeline.run_tests(test_filter='UnitTests')
        
        elif args.command == 'component':
            success = pipeline.run_tests(test_filter='ComponentTests')
        
        elif args.command == 'integration':
            success = pipeline.run_tests(test_filter='IntegrationTests')
        
        if not success:
            write_error("Test pipeline failed")
            return 1
        
        duration = (datetime.now() - start_time).total_seconds()
        print()
        write_success(f"Test pipeline completed in {duration:.2f} seconds")
        return 0
        
    except KeyboardInterrupt:
        write_error("\nTest pipeline interrupted by user")
        return 1
    except Exception as e:
        write_error(f"Test pipeline failed: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
