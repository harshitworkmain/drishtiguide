#!/usr/bin/env python3
"""
DrishtiGuide Flash Utility

Simplified utility for flashing firmware to ESP8266/ESP32 devices
with automatic board detection and configuration management.
"""

import os
import sys
import subprocess
import json
import time
import argparse
from pathlib import Path

class FlashUtility:
    def __init__(self):
        self.arduino_cli_path = self.find_arduino_cli()
        self.supported_boards = {
            'esp8266': {
                'nodemcu': 'esp8266:esp8266:nodemcuv2',
                'generic': 'esp8266:esp8266:generic'
            },
            'esp32': {
                'devkitv1': 'esp32:esp32:devkitv1',
                'generic': 'esp32:esp32:generic'
            }
        }
        self.projects = {
            'transmitter': 'src/esp8266-nodes/transmitter/transmitter.ino',
            'receiver': 'src/esp8266-nodes/receiver/receiver.ino',
            'main_controller': 'src/esp32-main-controller/main_controller.ino'
        }
        
    def find_arduino_cli(self):
        """Find Arduino CLI executable"""
        possible_paths = [
            'arduino-cli',
            r'C:\Program Files\Arduino CLI\arduino-cli.exe',
            r'C:\Program Files (x86)\Arduino CLI\arduino-cli.exe',
            '/usr/local/bin/arduino-cli',
            '/usr/bin/arduino-cli'
        ]
        
        for path in possible_paths:
            try:
                result = subprocess.run([path, 'version'], 
                                      capture_output=True, text=True, timeout=5)
                if result.returncode == 0:
                    print(f"Found Arduino CLI: {path}")
                    return path
            except (subprocess.TimeoutExpired, FileNotFoundError):
                continue
        
        print("Arduino CLI not found. Please install Arduino CLI first.")
        print("Visit: https://arduino.github.io/arduino-cli/latest/installation/")
        sys.exit(1)
    
    def check_dependencies(self):
        """Check if required boards and libraries are installed"""
        print("Checking dependencies...")
        
        # Check board installation
        try:
            result = subprocess.run([self.arduino_cli_path, 'core', 'list'],
                                  capture_output=True, text=True)
            boards = result.stdout
            
            if 'esp8266:esp8266' not in boards:
                print("Installing ESP8266 core...")
                subprocess.run([self.arduino_cli_path, 'core', 'install', 'esp8266:esp8266'],
                            check=True)
                print("ESP8266 core installed")
            
            if 'esp32:esp32' not in boards:
                print("Installing ESP32 core...")
                subprocess.run([self.arduino_cli_path, 'core', 'install', 'esp32:esp32'],
                            check=True)
                print("ESP32 core installed")
        
        except subprocess.CalledProcessError as e:
            print(f"Error installing boards: {e}")
            return False
        
        # Check libraries
        required_libs = [
            'TinyGPSPlus@^1.0.2',
            'MPU6050@^0.5.3',
            'WiFi@^2.0.0',
            'WebServer@^2.0.0'
        ]
        
        for lib in required_libs:
            try:
                subprocess.run([self.arduino_cli_path, 'lib', 'install', lib],
                             check=True, capture_output=True)
                print(f"Library {lib} installed/verified")
            except subprocess.CalledProcessError:
                print(f"Error installing library: {lib}")
        
        return True
    
    def detect_ports(self):
        """Detect available serial ports"""
        print("Detecting serial ports...")
        
        try:
            result = subprocess.run([self.arduino_cli_path, 'board', 'list'],
                                  capture_output=True, text=True)
            lines = result.stdout.strip().split('\n')
            
            ports = []
            for line in lines[2:]:  # Skip header lines
                if line.strip():
                    parts = line.split()
                    if len(parts) >= 2:
                        port = parts[0]
                        board_info = ' '.join(parts[1:]) if len(parts) > 1 else 'Unknown'
                        ports.append({'port': port, 'board': board_info})
            
            return ports
        
        except subprocess.CalledProcessError:
            return []
    
    def compile_sketch(self, sketch_path, board_fqbn):
        """Compile Arduino sketch"""
        print(f"Compiling {sketch_path} for {board_fqbn}...")
        
        try:
            result = subprocess.run([
                self.arduino_cli_path, 'compile',
                '--fqbn', board_fqbn,
                sketch_path
            ], capture_output=True, text=True, timeout=300)
            
            if result.returncode == 0:
                print("Compilation successful")
                return True
            else:
                print(f"Compilation failed: {result.stderr}")
                return False
        
        except subprocess.TimeoutExpired:
            print("Compilation timed out")
            return False
        except FileNotFoundError:
            print("Arduino CLI not found")
            return False
    
    def flash_firmware(self, sketch_path, port, board_fqbn):
        """Flash firmware to device"""
        print(f"Flashing {sketch_path} to {port} ({board_fqbn})...")
        
        try:
            # First compile
            if not self.compile_sketch(sketch_path, board_fqbn):
                return False
            
            # Then flash
            result = subprocess.run([
                self.arduino_cli_path, 'upload',
                '--fqbn', board_fqbn,
                '--port', port,
                sketch_path
            ], capture_output=True, text=True, timeout=120)
            
            if result.returncode == 0:
                print("Flashing successful")
                return True
            else:
                print(f"Flashing failed: {result.stderr}")
                return False
        
        except subprocess.TimeoutExpired:
            print("Flashing timed out")
            return False
    
    def flash_all_devices(self):
        """Flash all devices in sequence"""
        print("=== FLASH ALL DEVICES ===")
        
        # Detect ports
        ports = self.detect_ports()
        if len(ports) < 3:
            print(f"Warning: Expected at least 3 devices, found {len(ports)}")
        
        # Flash order: transmitter, receiver, main controller
        flash_sequence = [
            ('transmitter', 'esp8266', 'nodemcu'),
            ('receiver', 'esp8266', 'nodemcu'),
            ('main_controller', 'esp32', 'devkitv1')
        ]
        
        success_count = 0
        for project_name, chip_type, board_type in flash_sequence:
            if not ports:
                print(f"No more ports available for {project_name}")
                break
            
            port_info = ports.pop(0)
            sketch_path = self.projects.get(project_name)
            board_fqbn = self.supported_boards[chip_type][board_type]
            
            print(f"\n--- Flashing {project_name} ---")
            print(f"Port: {port_info['port']}")
            print(f"Detected: {port_info['board']}")
            
            if self.flash_firmware(sketch_path, port_info['port'], board_fqbn):
                success_count += 1
                print(f"✓ {project_name} flashed successfully")
                time.sleep(2)  # Wait between devices
            else:
                print(f"✗ {project_name} failed to flash")
        
        print(f"\nFlashing complete: {success_count}/{len(flash_sequence)} devices")
    
    def create_flash_script(self):
        """Create a batch/sh script for easy flashing"""
        script_content = """#!/bin/bash
# DrishtiGuide Auto-Flash Script
# Generated by flash_utility.py

echo "=== DrishtiGuide Auto-Flash ==="

# Check for Arduino CLI
if ! command -v arduino-cli &> /dev/null; then
    echo "Arduino CLI not found. Please install it first."
    exit 1
fi

# Install dependencies
echo "Installing dependencies..."
arduino-cli core update-index
arduino-cli core install esp8266:esp8266
arduino-cli core install esp32:esp32
arduino-cli lib install TinyGPSPlus@^1.0.2
arduino-cli lib install MPU6050@^0.5.3

# Detect devices
echo "Detecting devices..."
arduino-cli board list

# Flash transmitter
echo "Flashing transmitter..."
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 src/esp8266-nodes/transmitter/
arduino-cli upload --fqbn esp8266:esp8266:nodemcuv2 --port /dev/ttyUSB0 src/esp8266-nodes/transmitter/

# Flash receiver
echo "Flashing receiver..."
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 src/esp8266-nodes/receiver/
arduino-cli upload --fqbn esp8266:esp8266:nodemcuv2 --port /dev/ttyUSB1 src/esp8266-nodes/receiver/

# Flash main controller
echo "Flashing main controller..."
arduino-cli compile --fqbn esp32:esp32:devkitv1 src/esp32-main-controller/
arduino-cli upload --fqbn esp32:esp32:devkitv1 --port /dev/ttyUSB2 src/esp32-main-controller/

echo "Flashing complete!"
"""
        
        # Write script for Linux/Mac
        with open('flash_drishtiguide.sh', 'w') as f:
            f.write(script_content)
        os.chmod('flash_drishtiguide.sh', 0o755)
        
        # Write script for Windows
        windows_script = script_content.replace('#!/bin/bash', '@echo off')
        windows_script = windows_script.replace('arduino-cli', 'arduino-cli.exe')
        windows_script = windows_script.replace('/dev/ttyUSB', 'COM')
        
        with open('flash_drishtiguide.bat', 'w') as f:
            f.write(windows_script)
        
        print("Flash scripts created:")
        print("  Linux/Mac: flash_drishtiguide.sh")
        print("  Windows: flash_drishtiguide.bat")
    
    def interactive_mode(self):
        """Interactive flashing mode"""
        print("=== INTERACTIVE FLASH MODE ===")
        
        # Show available projects
        print("\nAvailable projects:")
        for i, project in enumerate(self.projects.keys(), 1):
            print(f"{i}. {project}")
        
        # Select project
        try:
            choice = int(input("\nSelect project number: ")) - 1
            project_name = list(self.projects.keys())[choice]
        except (ValueError, IndexError):
            print("Invalid selection")
            return
        
        # Show available ports
        ports = self.detect_ports()
        if not ports:
            print("No ports detected")
            return
        
        print("\nAvailable ports:")
        for i, port in enumerate(ports, 1):
            print(f"{i}. {port['port']} ({port['board']})")
        
        # Select port
        try:
            choice = int(input("\nSelect port number: ")) - 1
            selected_port = ports[choice]['port']
        except (ValueError, IndexError):
            print("Invalid selection")
            return
        
        # Determine board type
        board_fqbn = None
        if 'esp8266' in ports[choice]['board'].lower():
            board_fqbn = 'esp8266:esp8266:nodemcuv2'
        elif 'esp32' in ports[choice]['board'].lower():
            board_fqbn = 'esp32:esp32:devkitv1'
        else:
            print("Unable to determine board type")
            return
        
        # Flash the selected project
        sketch_path = self.projects[project_name]
        self.flash_firmware(sketch_path, selected_port, board_fqbn)

def main():
    parser = argparse.ArgumentParser(description='DrishtiGuide Flash Utility')
    parser.add_argument('--project', choices=['transmitter', 'receiver', 'main_controller'],
                       help='Project to flash')
    parser.add_argument('--port', help='Serial port to use')
    parser.add_argument('--board', help='Board FQBN (e.g., esp8266:esp8266:nodemcuv2)')
    parser.add_argument('--all', action='store_true', help='Flash all devices')
    parser.add_argument('--interactive', action='store_true', help='Interactive mode')
    parser.add_argument('--create-script', action='store_true', help='Create flash script')
    parser.add_argument('--check-deps', action='store_true', help='Check dependencies only')
    
    args = parser.parse_args()
    
    flash_util = FlashUtility()
    
    if args.create_script:
        flash_util.create_flash_script()
        return
    
    if args.check_deps:
        flash_util.check_dependencies()
        return
    
    # Check dependencies
    if not flash_util.check_dependencies():
        print("Dependency check failed")
        return
    
    if args.interactive:
        flash_util.interactive_mode()
    elif args.all:
        flash_util.flash_all_devices()
    elif args.project and args.port and args.board:
        sketch_path = flash_util.projects.get(args.project)
        if sketch_path:
            flash_util.flash_firmware(sketch_path, args.port, args.board)
        else:
            print(f"Unknown project: {args.project}")
    else:
        print("No action specified. Use --help for options.")
        print("Try: --interactive for interactive mode or --all to flash all devices")

if __name__ == '__main__':
    main()