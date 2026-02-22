#!/usr/bin/env python3
"""
Local development pipeline script for deskautomation project.

This script provides common development tasks for building, testing, and analyzing the project.
Test suite validates both motor driver configurations (MT_BASIC: L298N and MT_ROBUST: IBT_2)
to ensure motor-type-agnostic application logic.

This is the main orchestrator that delegates to specialized subscripts:
    - pipeline_build.py: Build operations
    - pipeline_test.py: Test operations
    - pipeline_analysis.py: Static analysis operations

Requirements:
    - Python 3.7+
    - MSYS2 with MinGW-w64 toolchain
    - CMake
    - cppcheck (for static analysis)

Usage:
    python toolchain/run-pipeline.py <command>
    python toolchain/run-pipeline.py --help
"""

import sys
import argparse
from datetime import datetime

# Import pipeline subscripts
from pipeline_common import Config, Colors, colorize, write_success, write_error
from pipeline_build import BuildPipeline
from pipeline_test import TestPipeline
from pipeline_analysis import AnalysisPipeline


# ============================================================================
# Help Display
# ============================================================================
def show_help():
    """Display help message."""
    help_text = """
Local Development Pipeline Script
==================================

Usage:
    python toolchain/run-pipeline.py <command>

Available Commands:
    clean-build       Clean build directory and build the project
    rebuild           Full rebuild (clean + configure + build)
    test              Run all test cases (validates both MT_BASIC and MT_ROBUST motor types)
                      - Builds and tests with MT_BASIC (L298N) configuration
                      - Rebuilds and tests with MT_ROBUST (IBT_2) configuration
                      - Restores MT_BASIC as default after completion
    unit              Run unit tests only
    component         Run component tests only
    integration       Run integration tests only
    clean-test        Clean, build, and run all test cases with dual motor validation
    static-analysis   Run static analysis (cppcheck)
    rules-all         Run all automated rule checks and generate grouped summary
    all               Run everything (rebuild + dual motor test + static analysis + rules)
    help              Show this help message

Examples:
    python toolchain/run-pipeline.py clean-build
    python toolchain/run-pipeline.py test
    python toolchain/run-pipeline.py unit
    python toolchain/run-pipeline.py clean-test
    python toolchain/run-pipeline.py rules-all
    python toolchain/run-pipeline.py all

Dual Motor Type Testing
=======================
The test pipeline automatically validates all 37 tests (unit, component, and integration)
against both supported motor driver configurations:
  - MT_BASIC  (L298N motor driver)
  - MT_ROBUST (IBT_2 motor driver)

This ensures the application logic is truly motor-type-agnostic and behaves
identically regardless of which motor driver is deployed.

Test Results
============
Results for each motor type are saved separately:
  - results/test_results_MT_BASIC.txt
  - results/test_results_MT_ROBUST.txt

Requirements:
    - Python 3.7+
    - MSYS2 with MinGW-w64 toolchain
    - CMake
    - cppcheck (for static analysis)

Architecture
============
This pipeline orchestrates specialized subscripts:
  - pipeline_build.py: Build operations (clean, configure, build)
  - pipeline_test.py: Test operations (unit, component, integration, dual motor)
  - pipeline_analysis.py: Static analysis operations (cppcheck)
  - pipeline_common.py: Shared utilities and configuration

Each subscript can also be run independently for specific operations.
"""
    print(help_text)


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='Local development pipeline for deskautomation project',
        add_help=False
    )
    parser.add_argument(
        'command',
        nargs='?',
        default='help',
        choices=[
            'clean-build', 'rebuild',
            'test', 'unit', 'component', 'integration', 'clean-test',
            'static-analysis', 'rules-all', 'all', 'help'
        ],
        help='Command to execute'
    )
    parser.add_argument(
        '-h', '--help',
        action='store_true',
        help='Show this help message'
    )
    
    args = parser.parse_args()
    
    if args.help or args.command == 'help':
        show_help()
        return 0
    
    print(colorize("\nLocal Development Pipeline", Colors.MAGENTA))
    print(colorize(f"Command: {args.command}\n", Colors.MAGENTA))
    
    # Initialize configuration
    config = Config()
    
    # Initialize pipeline components
    build_pipeline = BuildPipeline(config)
    test_pipeline = TestPipeline(config)
    analysis_pipeline = AnalysisPipeline(config)
    
    start_time = datetime.now()
    success = True
    
    try:
        # Execute requested command
        if args.command == 'clean-build':
            success = build_pipeline.clean()
            if success:
                success = build_pipeline.configure()
            if success:
                success = build_pipeline.build()
        
        elif args.command == 'rebuild':
            success = build_pipeline.clean()
            if success:
                success = build_pipeline.configure()
            if success:
                success = build_pipeline.build()
        
        elif args.command == 'test':
            success = test_pipeline.test_dual_motor()
        
        elif args.command == 'unit':
            success = test_pipeline.run_tests(test_filter='UnitTests')
        
        elif args.command == 'component':
            success = test_pipeline.run_tests(test_filter='ComponentTests')
        
        elif args.command == 'integration':
            success = test_pipeline.run_tests(test_filter='IntegrationTests')
        
        elif args.command == 'clean-test':
            success = build_pipeline.clean()
            if success:
                success = build_pipeline.configure()
            if success:
                success = build_pipeline.build()
            if success:
                success = test_pipeline.test_dual_motor()
        
        elif args.command == 'static-analysis':
            success = analysis_pipeline.static_analysis()

        elif args.command == 'rules-all':
            success = analysis_pipeline.run_all_groups(str(config.source_dir))
        
        elif args.command == 'all':
            # Run complete pipeline
            success = build_pipeline.clean()
            if success:
                success = build_pipeline.configure()
            if success:
                success = build_pipeline.build()
            if success:
                success = test_pipeline.test_dual_motor()
            if success:
                success = analysis_pipeline.static_analysis()
            if success:
                success = analysis_pipeline.run_all_groups(str(config.source_dir))
        
        if not success:
            write_error("Pipeline failed")
            return 1
        
        duration = (datetime.now() - start_time).total_seconds()
        print()
        write_success(f"Pipeline completed successfully in {duration:.2f} seconds")
        return 0
        
    except KeyboardInterrupt:
        write_error("\nPipeline interrupted by user")
        return 1
    except Exception as e:
        write_error(f"Pipeline failed: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
