#!/bin/bash
# Test Runner for DrishtiGuide Project
# Runs all unit and integration tests

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if Python 3 is available
check_python() {
    if command -v python3 &> /dev/null; then
        PYTHON_CMD="python3"
    elif command -v python &> /dev/null; then
        PYTHON_CMD="python"
        # Check if it's Python 3
        python_version=$($PYTHON_CMD --version 2>&1 | grep -o 'Python [0-9]\+\.[0-9]\+' | cut -d' ' -f2)
        if [[ $python_version < 3 ]]; then
            log_error "Python 3 is required. Found Python $python_version"
            exit 1
        fi
    else
        log_error "Python 3 is not installed"
        exit 1
    fi
    
    log_success "Python found: $($PYTHON_CMD --version)"
}

# Check if required packages are installed
check_dependencies() {
    log_info "Checking test dependencies..."
    
    # Check for pytest
    if ! $PYTHON_CMD -c "import pytest" 2>/dev/null; then
        log_warning "pytest not found. Installing..."
        pip3 install pytest || pip install pytest
    fi
    
    # Check for serial
    if ! $PYTHON_CMD -c "import serial" 2>/dev/null; then
        log_warning "pyserial not found. Installing..."
        pip3 install pyserial || pip install pyserial
    fi
    
    log_success "Dependencies checked"
}

# Run unit tests
run_unit_tests() {
    log_info "Running unit tests..."
    
    unit_test_dir="tests/unit_tests"
    if [ ! -d "$unit_test_dir" ]; then
        log_error "Unit test directory not found: $unit_test_dir"
        return 1
    fi
    
    # Find and run all unit test files
    unit_test_files=$(find "$unit_test_dir" -name "test_*.py")
    
    if [ -z "$unit_test_files" ]; then
        log_warning "No unit test files found"
        return 0
    fi
    
    unit_success=0
    unit_total=0
    
    for test_file in $unit_test_files; do
        log_info "Running unit tests from $(basename "$test_file")..."
        
        if $PYTHON_CMD -m pytest "$test_file" -v --tb=short; then
            log_success "Unit tests passed: $(basename "$test_file")"
            ((unit_success++))
        else
            log_error "Unit tests failed: $(basename "$test_file")"
        fi
        ((unit_total++))
        
        echo ""  # Add spacing
    done
    
    log_info "Unit tests: $unit_success/$unit_total passed"
    
    if [ $unit_success -eq $unit_total ]; then
        log_success "All unit tests passed!"
        return 0
    else
        log_error "Some unit tests failed!"
        return 1
    fi
}

# Run integration tests
run_integration_tests() {
    log_info "Running integration tests..."
    
    integration_test_dir="tests/integration_tests"
    if [ ! -d "$integration_test_dir" ]; then
        log_error "Integration test directory not found: $integration_test_dir"
        return 1
    fi
    
    # Find and run all integration test files
    integration_test_files=$(find "$integration_test_dir" -name "test_*.py")
    
    if [ -z "$integration_test_files" ]; then
        log_warning "No integration test files found"
        return 0
    fi
    
    integration_success=0
    integration_total=0
    
    for test_file in $integration_test_files; do
        log_info "Running integration tests from $(basename "$test_file")..."
        
        if $PYTHON_CMD -m pytest "$test_file" -v --tb=short; then
            log_success "Integration tests passed: $(basename "$test_file")"
            ((integration_success++))
        else
            log_error "Integration tests failed: $(basename "$test_file")"
        fi
        ((integration_total++))
        
        echo ""  # Add spacing
    done
    
    log_info "Integration tests: $integration_success/$integration_total passed"
    
    if [ $integration_success -eq $integration_total ]; then
        log_success "All integration tests passed!"
        return 0
    else
        log_error "Some integration tests failed!"
        return 1
    fi
}

# Run performance tests
run_performance_tests() {
    log_info "Running performance tests..."
    
    performance_test_dir="tests/performance_tests"
    if [ ! -d "$performance_test_dir" ]; then
        log_warning "Performance test directory not found: $performance_test_dir"
        return 0
    fi
    
    # Find and run all performance test files
    performance_test_files=$(find "$performance_test_dir" -name "test_*.py")
    
    if [ -z "$performance_test_files" ]; then
        log_warning "No performance test files found"
        return 0
    fi
    
    performance_success=0
    performance_total=0
    
    for test_file in $performance_test_files; do
        log_info "Running performance tests from $(basename "$test_file")..."
        
        if $PYTHON_CMD -m pytest "$test_file" -v --tb=short; then
            log_success "Performance tests passed: $(basename "$test_file")"
            ((performance_success++))
        else
            log_error "Performance tests failed: $(basename "$test_file")"
        fi
        ((performance_total++))
        
        echo ""  # Add spacing
    done
    
    log_info "Performance tests: $performance_success/$performance_total passed"
    
    if [ $performance_success -eq $performance_total ]; then
        log_success "All performance tests passed!"
        return 0
    else
        log_error "Some performance tests failed!"
        return 1
    fi
}

# Run code coverage
run_coverage() {
    log_info "Running tests with code coverage..."
    
    if ! $PYTHON_CMD -c "import coverage" 2>/dev/null; then
        log_warning "coverage.py not found. Installing..."
        pip3 install coverage || pip install coverage
    fi
    
    # Run coverage on all tests
    coverage run -m pytest tests/ -v
    coverage report -m
    
    # Generate HTML report
    coverage html
    log_success "HTML coverage report generated in htmlcov/"
}

# Run hardware-in-loop tests
run_hil_tests() {
    log_info "Running Hardware-in-Loop (HIL) tests..."
    
    # Check if hardware is connected
    if ! $PYTHON_CMD -c "import serial; serial.Serial('/dev/ttyUSB0', timeout=1)" 2>/dev/null; then
        log_warning "No hardware detected for HIL tests"
        return 0
    fi
    
    hil_test_dir="tests/hil_tests"
    if [ ! -d "$hil_test_dir" ]; then
        log_warning "HIL test directory not found: $hil_test_dir"
        return 0
    fi
    
    # Run HIL tests
    hil_test_files=$(find "$hil_test_dir" -name "test_*.py")
    
    for test_file in $hil_test_files; do
        log_info "Running HIL tests from $(basename "$test_file")..."
        $PYTHON_CMD "$test_file"
    done
}

# Generate test report
generate_report() {
    log_info "Generating test report..."
    
    report_file="test_report_$(date +%Y%m%d_%H%M%S).txt"
    
    {
        echo "DrishtiGuide Test Report"
        echo "Generated: $(date)"
        echo "========================================"
        echo ""
        
        echo "Test Environment:"
        echo "Python: $($PYTHON_CMD --version)"
        echo "OS: $(uname -s -r)"
        echo "Shell: $SHELL"
        echo ""
        
        echo "Test Results:"
        echo "- Unit Tests: $unit_test_result"
        echo "- Integration Tests: $integration_test_result"
        echo "- Performance Tests: $performance_test_result"
        echo "- HIL Tests: $hil_test_result"
        echo ""
        
        echo "Summary:"
        if [ "$overall_result" = "PASS" ]; then
            echo "✓ All tests passed successfully"
        else
            echo "✗ Some tests failed - check logs for details"
        fi
        
    } > "$report_file"
    
    log_success "Test report generated: $report_file"
}

# Clean test artifacts
clean_tests() {
    log_info "Cleaning test artifacts..."
    
    # Remove coverage files
    rm -f .coverage
    rm -rf htmlcov/
    rm -rf .pytest_cache/
    
    # Remove test logs
    find tests/ -name "*.log" -delete 2>/dev/null || true
    find tests/ -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
    
    log_success "Test artifacts cleaned"
}

# Show help
show_help() {
    echo "DrishtiGuide Test Runner"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  unit         Run unit tests only"
    echo "  integration  Run integration tests only"
    echo "  performance  Run performance tests only"
    echo "  hil          Run hardware-in-loop tests"
    echo "  coverage     Run tests with code coverage"
    echo "  all          Run all tests (default)"
    echo "  clean        Clean test artifacts"
    echo "  report       Generate test report"
    echo "  help         Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                # Run all tests"
    echo "  $0 unit           # Run only unit tests"
    echo "  $0 coverage        # Run with coverage report"
}

# Main function
main() {
    echo "=== DrishtiGuide Test Runner ==="
    echo ""
    
    # Check environment
    check_python
    check_dependencies
    
    # Initialize result variables
    unit_test_result="NOT RUN"
    integration_test_result="NOT RUN"
    performance_test_result="NOT RUN"
    hil_test_result="NOT RUN"
    overall_result="PASS"
    
    # Parse command line arguments
    case "${1:-all}" in
        unit)
            if run_unit_tests; then
                unit_test_result="PASS"
            else
                unit_test_result="FAIL"
                overall_result="FAIL"
            fi
            ;;
        integration)
            if run_integration_tests; then
                integration_test_result="PASS"
            else
                integration_test_result="FAIL"
                overall_result="FAIL"
            fi
            ;;
        performance)
            if run_performance_tests; then
                performance_test_result="PASS"
            else
                performance_test_result="FAIL"
                overall_result="FAIL"
            fi
            ;;
        hil)
            if run_hil_tests; then
                hil_test_result="PASS"
            else
                hil_test_result="FAIL"
                overall_result="FAIL"
            fi
            ;;
        coverage)
            run_coverage
            ;;
        all)
            # Run all test types
            if run_unit_tests; then
                unit_test_result="PASS"
            else
                unit_test_result="FAIL"
                overall_result="FAIL"
            fi
            echo ""
            
            if run_integration_tests; then
                integration_test_result="PASS"
            else
                integration_test_result="FAIL"
                overall_result="FAIL"
            fi
            echo ""
            
            if run_performance_tests; then
                performance_test_result="PASS"
            else
                performance_test_result="FAIL"
                overall_result="FAIL"
            fi
            echo ""
            
            run_hil_tests
            ;;
        clean)
            clean_tests
            exit 0
            ;;
        report)
            generate_report
            exit 0
            ;;
        help|--help|-h)
            show_help
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
    
    # Print summary
    echo ""
    echo "========================================"
    echo "TEST SUMMARY"
    echo "========================================"
    echo "Unit Tests: $unit_test_result"
    echo "Integration Tests: $integration_test_result"
    echo "Performance Tests: $performance_test_result"
    echo "HIL Tests: $hil_test_result"
    echo ""
    echo "Overall Result: $overall_result"
    echo "========================================"
    
    # Generate report if tests were run
    if [[ "$unit_test_result" != "NOT RUN" || "$integration_test_result" != "NOT RUN" ]]; then
        generate_report
    fi
    
    # Exit with appropriate code
    if [ "$overall_result" = "PASS" ]; then
        log_success "All tests completed successfully!"
        exit 0
    else
        log_error "Some tests failed!"
        exit 1
    fi
}

# Run main function with all arguments
main "$@"