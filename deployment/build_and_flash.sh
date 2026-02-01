#!/bin/bash
# DrishtiGuide Build and Flash Script
# Automated compilation and flashing for all devices

set -e  # Exit on any error

# Configuration
TRANSMITTER_SKETCH="src/esp8266-nodes/transmitter/transmitter.ino"
RECEIVER_SKETCH="src/esp8266-nodes/receiver/receiver.ino"
MAIN_CONTROLLER_SKETCH="src/esp32-main-controller/main_controller.ino"

# Default ports (adjust as needed)
TRANSMITTER_PORT="/dev/ttyUSB0"
RECEIVER_PORT="/dev/ttyUSB1"
MAIN_CONTROLLER_PORT="/dev/ttyUSB2"

# Board configurations
ESP8266_BOARD="esp8266:esp8266:nodemcuv2"
ESP32_BOARD="esp32:esp32:devkitv1"

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

# Check dependencies
check_dependencies() {
    log_info "Checking dependencies..."
    
    if ! command -v arduino-cli &> /dev/null; then
        log_error "Arduino CLI not found. Please install it first."
        log_info "Visit: https://arduino.github.io/arduino-cli/latest/installation/"
        exit 1
    fi
    
    log_success "Arduino CLI found: $(arduino-cli version)"
    
    # Check if cores are installed
    if ! arduino-cli core list | grep -q "esp8266:esp8266"; then
        log_error "ESP8266 core not installed. Run setup_dev_environment.sh first."
        exit 1
    fi
    
    if ! arduino-cli core list | grep -q "esp32:esp32"; then
        log_error "ESP32 core not installed. Run setup_dev_environment.sh first."
        exit 1
    fi
    
    log_success "Required Arduino cores are installed"
}

# Detect available ports
detect_ports() {
    log_info "Detecting available serial ports..."
    
    # Get list of ports
    ports_output=$(arduino-cli board list)
    
    # Parse ports (simplified parsing)
    available_ports=()
    while IFS= read -r line; do
        if [[ $line =~ ^/dev/ ]] || [[ $line =~ ^COM ]]; then
            port=$(echo $line | awk '{print $1}')
            available_ports+=("$port")
        fi
    done <<< "$ports_output"
    
    if [ ${#available_ports[@]} -eq 0 ]; then
        log_warning "No serial ports detected"
        return 1
    fi
    
    log_info "Found ${#available_ports[@]} serial ports:"
    for port in "${available_ports[@]}"; do
        echo "  $port"
    done
    
    return 0
}

# Compile sketch
compile_sketch() {
    local sketch_path=$1
    local board_fqbn=$2
    local sketch_name=$(basename "$sketch_path")
    
    log_info "Compiling $sketch_name..."
    
    if arduino-cli compile --fqbn "$board_fqbn" "$sketch_path"; then
        log_success "$sketch_name compiled successfully"
        return 0
    else
        log_error "Failed to compile $sketch_name"
        return 1
    fi
}

# Flash firmware to device
flash_device() {
    local sketch_path=$1
    local port=$2
    local board_fqbn=$3
    local device_name=$4
    
    log_info "Flashing $device_name to $port..."
    
    # Wait a moment for port to be ready
    sleep 2
    
    if arduino-cli upload --fqbn "$board_fqbn" --port "$port" "$sketch_path"; then
        log_success "$device_name flashed successfully"
        return 0
    else
        log_error "Failed to flash $device_name"
        return 1
    fi
}

# Build all sketches
build_all() {
    log_info "Building all sketches..."
    
    local success_count=0
    local total_count=3
    
    # Build transmitter
    if compile_sketch "$TRANSMITTER_SKETCH" "$ESP8266_BOARD"; then
        ((success_count++))
    fi
    
    # Build receiver
    if compile_sketch "$RECEIVER_SKETCH" "$ESP8266_BOARD"; then
        ((success_count++))
    fi
    
    # Build main controller
    if compile_sketch "$MAIN_CONTROLLER_SKETCH" "$ESP32_BOARD"; then
        ((success_count++))
    fi
    
    if [ $success_count -eq $total_count ]; then
        log_success "All sketches compiled successfully"
        return 0
    else
        log_error "Only $success_count/$total_count sketches compiled successfully"
        return 1
    fi
}

# Flash all devices
flash_all() {
    log_info "Flashing all devices..."
    
    if ! detect_ports; then
        log_error "No ports available for flashing"
        return 1
    fi
    
    local success_count=0
    local total_count=3
    
    # Flash transmitter
    if [ -e "$TRANSMITTER_PORT" ]; then
        if flash_device "$TRANSMITTER_SKETCH" "$TRANSMITTER_PORT" "$ESP8266_BOARD" "Transmitter"; then
            ((success_count++))
        fi
        sleep 3  # Wait between devices
    else
        log_warning "Transmitter port $TRANSMITTER_PORT not found, skipping..."
    fi
    
    # Flash receiver
    if [ -e "$RECEIVER_PORT" ]; then
        if flash_device "$RECEIVER_SKETCH" "$RECEIVER_PORT" "$ESP8266_BOARD" "Receiver"; then
            ((success_count++))
        fi
        sleep 3  # Wait between devices
    else
        log_warning "Receiver port $RECEIVER_PORT not found, skipping..."
    fi
    
    # Flash main controller
    if [ -e "$MAIN_CONTROLLER_PORT" ]; then
        if flash_device "$MAIN_CONTROLLER_SKETCH" "$MAIN_CONTROLLER_PORT" "$ESP32_BOARD" "Main Controller"; then
            ((success_count++))
        fi
    else
        log_warning "Main controller port $MAIN_CONTROLLER_PORT not found, skipping..."
    fi
    
    if [ $success_count -eq $total_count ]; then
        log_success "All devices flashed successfully"
        return 0
    else
        log_warning "Only $success_count/$total_count devices flashed successfully"
        return 1
    fi
}

# Interactive flashing
interactive_flash() {
    log_info "Starting interactive flash mode..."
    
    if ! detect_ports; then
        return 1
    fi
    
    echo ""
    echo "Available devices to flash:"
    echo "1. Transmitter (ESP8266)"
    echo "2. Receiver (ESP8266)"
    echo "3. Main Controller (ESP32)"
    echo "4. Flash All"
    echo "5. Exit"
    echo ""
    
    read -p "Select option (1-5): " choice
    
    case $choice in
        1)
            read -p "Enter port (default: $TRANSMITTER_PORT): " port
            port=${port:-$TRANSMITTER_PORT}
            flash_device "$TRANSMITTER_SKETCH" "$port" "$ESP8266_BOARD" "Transmitter"
            ;;
        2)
            read -p "Enter port (default: $RECEIVER_PORT): " port
            port=${port:-$RECEIVER_PORT}
            flash_device "$RECEIVER_SKETCH" "$port" "$ESP8266_BOARD" "Receiver"
            ;;
        3)
            read -p "Enter port (default: $MAIN_CONTROLLER_PORT): " port
            port=${port:-$MAIN_CONTROLLER_PORT}
            flash_device "$MAIN_CONTROLLER_SKETCH" "$port" "$ESP32_BOARD" "Main Controller"
            ;;
        4)
            flash_all
            ;;
        5)
            log_info "Exiting..."
            exit 0
            ;;
        *)
            log_error "Invalid selection"
            exit 1
            ;;
    esac
}

# Clean build artifacts
clean_build() {
    log_info "Cleaning build artifacts..."
    
    # Remove build directories
    find . -name "build" -type d -exec rm -rf {} + 2>/dev/null || true
    
    # Remove temporary files
    find . -name "*.tmp" -delete 2>/dev/null || true
    find . -name "*.o" -delete 2>/dev/null || true
    
    log_success "Build artifacts cleaned"
}

# Show help
show_help() {
    echo "DrishtiGuide Build and Flash Script"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  build        Build all sketches without flashing"
    echo "  flash        Flash all devices (requires ports to be connected)"
    echo "  interactive  Interactive mode for selective flashing"
    echo "  clean        Clean build artifacts"
    echo "  all          Build and flash all devices"
    echo "  help         Show this help message"
    echo ""
    echo "Environment Variables:"
    echo "  TRANSMITTER_PORT    Port for transmitter (default: $TRANSMITTER_PORT)"
    echo "  RECEIVER_PORT       Port for receiver (default: $RECEIVER_PORT)"
    echo "  MAIN_CONTROLLER_PORT Port for main controller (default: $MAIN_CONTROLLER_PORT)"
    echo ""
    echo "Examples:"
    echo "  $0 all                    # Build and flash everything"
    echo "  $0 build                  # Build only"
    echo "  TRANSMITTER_PORT=/dev/ttyACM0 $0 flash  # Custom port"
}

# Main function
main() {
    echo "=== DrishtiGuide Build and Flash Utility ==="
    echo ""
    
    # Check dependencies
    check_dependencies
    
    # Parse command line arguments
    case "${1:-help}" in
        build)
            build_all
            ;;
        flash)
            flash_all
            ;;
        interactive)
            interactive_flash
            ;;
        clean)
            clean_build
            ;;
        all)
            build_all
            if [ $? -eq 0 ]; then
                flash_all
            fi
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"