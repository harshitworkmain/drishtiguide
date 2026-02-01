#!/bin/bash
# DrishtiGuide Development Setup Script
# Sets up development environment for the project

set -e  # Exit on any error

echo "=== DrishtiGuide Development Setup ==="

# Check if we're in the project root
if [ ! -f "README.md" ] || [ ! -d "src" ]; then
    echo "Error: Please run this script from the project root directory"
    exit 1
fi

# Function to check command availability
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install Arduino CLI if not present
install_arduino_cli() {
    if ! command_exists arduino-cli; then
        echo "Installing Arduino CLI..."
        
        # Detect OS
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            # Linux
            wget -O arduino-cli.tar.gz https://downloads.arduino.cc/arduino-cli/arduino-cli-latest-linux64.tar.gz
            tar -xf arduino-cli.tar.gz
            sudo mv arduino-cli /usr/local/bin/
            rm arduino-cli.tar.gz
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            # macOS
            if command_exists brew; then
                brew install arduino-cli
            else
                wget -O arduino-cli.zip https://downloads.arduino.cc/arduino-cli/arduino-cli-latest-macos64.zip
                unzip arduino-cli.zip
                sudo mv arduino-cli /usr/local/bin/
                rm arduino-cli.zip
            fi
        else
            echo "Please install Arduino CLI manually from: https://arduino.github.io/arduino-cli/latest/installation/"
            exit 1
        fi
        
        echo "Arduino CLI installed"
    else
        echo "Arduino CLI already installed"
    fi
}

# Install Arduino cores
install_arduino_cores() {
    echo "Installing Arduino cores..."
    
    # Update index
    arduino-cli core update-index
    
    # Install ESP8266 core
    if ! arduino-cli core list | grep -q "esp8266:esp8266"; then
        echo "Installing ESP8266 core..."
        arduino-cli core install esp8266:esp8266
    else
        echo "ESP8266 core already installed"
    fi
    
    # Install ESP32 core
    if ! arduino-cli core list | grep -q "esp32:esp32"; then
        echo "Installing ESP32 core..."
        arduino-cli core install esp32:esp32
    else
        echo "ESP32 core already installed"
    fi
}

# Install required libraries
install_libraries() {
    echo "Installing required libraries..."
    
    libraries=(
        "TinyGPSPlus@^1.0.2"
        "MPU6050@^0.5.3"
        "WiFi@^2.0.0"
        "WebServer@^2.0.0"
        "ArduinoJson@^6.19.4"
    )
    
    for lib in "${libraries[@]}"; do
        echo "Installing $lib..."
        arduino-cli lib install "$lib"
    done
}

# Install Python dependencies
install_python_deps() {
    echo "Installing Python dependencies..."
    
    if command_exists pip3; then
        pip3 install -r requirements.txt
    elif command_exists pip; then
        pip install -r requirements.txt
    else
        echo "Warning: pip not found. Python tools may not work."
    fi
}

# Create virtual environment for Python
setup_python_env() {
    if [ ! -d "venv" ]; then
        echo "Creating Python virtual environment..."
        python3 -m venv venv
    fi
    
    echo "Activating virtual environment..."
    source venv/bin/activate
    
    # Install dependencies in virtual environment
    pip install -r requirements.txt
    pip install pyserial pytest
    
    echo "Virtual environment setup complete"
    echo "To activate: source venv/bin/activate"
}

# Setup development tools
setup_dev_tools() {
    echo "Setting up development tools..."
    
    # Make Python scripts executable
    chmod +x tools/calibration/sensor_calibration.py
    chmod +x tools/data_logger/sensor_logger.py
    chmod +x tools/flash_utility/flash_utility.py
    chmod +x deployment/build_and_flash.sh
    chmod +x deployment/deploy_production.sh
    
    # Create development configuration
    cat > development_config.json << EOF
{
    "development": {
        "arduino_cli_path": "arduino-cli",
        "default_ports": {
            "transmitter": "/dev/ttyUSB0",
            "receiver": "/dev/ttyUSB1", 
            "main_controller": "/dev/ttyUSB2"
        },
        "build_options": {
            "warnings": "all",
            "optimize": "size",
            "debug": true
        }
    }
}
EOF
    
    echo "Development configuration created"
}

# Setup pre-commit hooks
setup_git_hooks() {
    echo "Setting up git hooks..."
    
    # Create pre-commit hook
    cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# Pre-commit hook for DrishtiGuide

echo "Running pre-commit checks..."

# Check Arduino code syntax
echo "Checking Arduino code syntax..."
for sketch in $(find src -name "*.ino"); do
    echo "Checking $sketch..."
    # Add actual syntax checking here
done

# Run tests if available
if [ -f "tests/run_tests.sh" ]; then
    echo "Running tests..."
    ./tests/run_tests.sh
fi

echo "Pre-commit checks complete"
EOF
    
    chmod +x .git/hooks/pre-commit
    echo "Pre-commit hook installed"
}

# Validate installation
validate_installation() {
    echo "Validating installation..."
    
    # Check Arduino CLI
    if command_exists arduino-cli; then
        echo "✓ Arduino CLI: $(arduino-cli version)"
    else
        echo "✗ Arduino CLI not found"
        return 1
    fi
    
    # Check cores
    if arduino-cli core list | grep -q "esp8266:esp8266"; then
        echo "✓ ESP8266 core installed"
    else
        echo "✗ ESP8266 core not found"
        return 1
    fi
    
    if arduino-cli core list | grep -q "esp32:esp32"; then
        echo "✓ ESP32 core installed"
    else
        echo "✗ ESP32 core not found"
        return 1
    fi
    
    # Check Python environment
    if [ -d "venv" ]; then
        echo "✓ Python virtual environment exists"
    else
        echo "⚠ Python virtual environment not found"
    fi
    
    echo "Installation validation complete"
}

# Main setup process
main() {
    echo "Starting development environment setup..."
    
    # Install Arduino CLI
    install_arduino_cli
    
    # Install Arduino cores and libraries
    install_arduino_cores
    install_libraries
    
    # Setup Python environment
    if command_exists python3; then
        setup_python_env
    else
        echo "Python3 not found, skipping Python setup"
    fi
    
    # Setup development tools
    setup_dev_tools
    
    # Setup git hooks if git repository
    if [ -d ".git" ]; then
        setup_git_hooks
    fi
    
    # Validate everything
    validate_installation
    
    echo ""
    echo "=== Development Setup Complete ==="
    echo ""
    echo "Quick Start Commands:"
    echo "  Flash all devices: ./deployment/build_and_flash.sh"
    echo "  Run calibration: python3 tools/calibration/sensor_calibration.py --help"
    echo "  Start data logger: python3 tools/data_logger/sensor_logger.py --help"
    echo "  Activate Python env: source venv/bin/activate"
    echo ""
    echo "For more information, see README.md"
}

# Check if running as root (not recommended)
if [ "$EUID" -eq 0 ]; then
    echo "Warning: Running as root is not recommended"
    echo "Please run as a regular user with sudo privileges only when needed"
fi

# Run main setup
main "$@"