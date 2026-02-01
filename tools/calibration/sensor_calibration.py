#!/usr/bin/env python3
"""
DrishtiGuide Sensor Calibration Tool

This script helps calibrate the ultrasonic sensors and ESP-NOW communication
for optimal performance and accuracy.
"""

import serial
import time
import json
import statistics
import argparse
import sys
from datetime import datetime

class SensorCalibrator:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial_conn = None
        self.calibration_data = {
            'timestamp': datetime.now().isoformat(),
            'ultrasonic': {},
            'espnow': {},
            'battery': {},
            'temperature': {}
        }
        
    def connect(self):
        """Connect to the device via serial"""
        try:
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=2)
            print(f"Connected to {self.port} at {self.baudrate} baud")
            time.sleep(2)  # Wait for device to boot
            return True
        except serial.SerialException as e:
            print(f"Failed to connect to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Close serial connection"""
        if self.serial_conn:
            self.serial_conn.close()
            print("Disconnected from device")
    
    def read_distance_readings(self, samples=50):
        """Read multiple distance samples for calibration"""
        print(f"Reading {samples} distance samples...")
        
        distances = []
        attempts = 0
        max_attempts = samples * 3
        
        while len(distances) < samples and attempts < max_attempts:
            try:
                if self.serial_conn.in_waiting > 0:
                    line = self.serial_conn.readline().decode('utf-8').strip()
                    
                    # Parse distance from serial output
                    if 'Distance:' in line or 'distance' in line.lower():
                        try:
                            # Extract numeric value
                            parts = line.split()
                            for part in parts:
                                if part.replace('.', '').replace('cm', '').isdigit():
                                    distance = float(part.replace('cm', ''))
                                    if 0 < distance < 500:  # Valid range
                                        distances.append(distance)
                                        print(f"Sample {len(distances)}/{samples}: {distance}cm")
                                        break
                        except ValueError:
                            continue
                
                attempts += 1
                time.sleep(0.1)
                
            except Exception as e:
                print(f"Error reading data: {e}")
                attempts += 1
        
        if len(distances) < samples:
            print(f"Warning: Only collected {len(distances)} samples out of {samples}")
        
        return distances
    
    def calibrate_ultrasonic(self):
        """Calibrate ultrasonic sensor"""
        print("\n=== ULTRASONIC SENSOR CALIBRATION ===")
        print("Place an object at known distances and measure accuracy")
        print("Press Enter after placing object at each distance...")
        
        known_distances = [10, 30, 50, 100, 200, 300]  # cm
        calibration_data = []
        
        for known_dist in known_distances:
            input(f"Place object at {known_dist}cm from sensor and press Enter...")
            
            readings = self.read_distance_readings(samples=20)
            if readings:
                measured_dist = statistics.mean(readings)
                error = measured_dist - known_dist
                std_dev = statistics.stdev(readings) if len(readings) > 1 else 0
                
                calibration_point = {
                    'known_distance': known_dist,
                    'measured_distance': round(measured_dist, 2),
                    'error': round(error, 2),
                    'std_deviation': round(std_dev, 2),
                    'samples': len(readings)
                }
                
                calibration_data.append(calibration_point)
                print(f"Known: {known_dist}cm, Measured: {measured_dist:.2f}cm, Error: {error:.2f}cm")
            else:
                print("Failed to get readings for this distance")
        
        # Calculate calibration factors
        errors = [point['error'] for point in calibration_data]
        avg_error = statistics.mean(errors) if errors else 0
        std_error = statistics.stdev(errors) if len(errors) > 1 else 0
        
        self.calibration_data['ultrasonic'] = {
            'calibration_points': calibration_data,
            'average_error': round(avg_error, 3),
            'error_std_deviation': round(std_error, 3),
            'recommended_offset': round(avg_error, 3),
            'calibration_quality': self._assess_calibration_quality(std_error)
        }
        
        print(f"\nUltrasonic Calibration Results:")
        print(f"Average Error: {avg_error:.3f}cm")
        print(f"Error Std Deviation: {std_error:.3f}cm")
        print(f"Recommended Offset: {avg_error:.3f}cm")
        print(f"Calibration Quality: {self.calibration_data['ultrasonic']['calibration_quality']}")
    
    def calibrate_battery(self):
        """Calibrate battery voltage readings"""
        print("\n=== BATTERY CALIBRATION ===")
        print("Measure actual battery voltage with multimeter")
        
        actual_voltage = input("Enter measured battery voltage (V): ")
        try:
            actual_voltage = float(actual_voltage)
        except ValueError:
            print("Invalid voltage value")
            return
        
        # Read battery readings from device
        print("Reading battery level from device...")
        battery_readings = []
        
        for _ in range(20):
            try:
                if self.serial_conn.in_waiting > 0:
                    line = self.serial_conn.readline().decode('utf-8').strip()
                    if 'battery' in line.lower() or 'Battery' in line:
                        # Extract battery percentage or voltage
                        parts = line.split()
                        for part in parts:
                            if part.replace('%', '').replace('V', '').replace('.', '').isdigit():
                                battery_readings.append(part)
                                break
                time.sleep(0.1)
            except:
                continue
        
        if battery_readings:
            avg_reading = statistics.mean([float(r.replace('%', '').replace('V', '')) 
                                       for r in battery_readings])
            
            # Calculate calibration factor
            if '%' in str(battery_readings[0]):
                # Device reports percentage
                calibration_factor = actual_voltage / (avg_reading * 0.042)  # Assuming 0-100% maps to 0-4.2V
            else:
                # Device reports voltage
                calibration_factor = actual_voltage / avg_reading
            
            self.calibration_data['battery'] = {
                'actual_voltage': actual_voltage,
                'device_reading': round(avg_reading, 3),
                'calibration_factor': round(calibration_factor, 4),
                'reading_type': 'percentage' if '%' in str(battery_readings[0]) else 'voltage'
            }
            
            print(f"Battery Calibration Factor: {calibration_factor:.4f}")
        else:
            print("No battery readings received")
    
    def test_espnow_communication(self):
        """Test ESP-NOW communication quality"""
        print("\n=== ESP-NOW COMMUNICATION TEST ===")
        print("Testing packet delivery and reliability...")
        
        success_count = 0
        failure_count = 0
        total_tests = 100
        
        print(f"Running {total_tests} communication tests...")
        
        for i in range(total_tests):
            # Monitor serial for delivery status
            start_time = time.time()
            delivery_detected = False
            
            while time.time() - start_time < 1.0:  # 1 second timeout
                try:
                    if self.serial_conn.in_waiting > 0:
                        line = self.serial_conn.readline().decode('utf-8').strip()
                        if 'Delivery success' in line or 'success' in line.lower():
                            delivery_detected = True
                            break
                        elif 'Delivery fail' in line or 'fail' in line.lower():
                            break
                except:
                    pass
                time.sleep(0.01)
            
            if delivery_detected:
                success_count += 1
                print(f"Test {i+1}/{total_tests}: ✓")
            else:
                failure_count += 1
                print(f"Test {i+1}/{total_tests}: ✗")
            
            time.sleep(0.1)  # Small delay between tests
        
        success_rate = (success_count / total_tests) * 100
        
        self.calibration_data['espnow'] = {
            'total_tests': total_tests,
            'successful_transmissions': success_count,
            'failed_transmissions': failure_count,
            'success_rate': round(success_rate, 2),
            'communication_quality': self._assess_communication_quality(success_rate)
        }
        
        print(f"\nESP-NOW Communication Results:")
        print(f"Success Rate: {success_rate:.2f}%")
        print(f"Communication Quality: {self.calibration_data['espnow']['communication_quality']}")
    
    def _assess_calibration_quality(self, std_error):
        """Assess the quality of calibration"""
        if std_error < 2:
            return "Excellent"
        elif std_error < 5:
            return "Good"
        elif std_error < 10:
            return "Fair"
        else:
            return "Poor"
    
    def _assess_communication_quality(self, success_rate):
        """Assess communication quality"""
        if success_rate >= 95:
            return "Excellent"
        elif success_rate >= 85:
            return "Good"
        elif success_rate >= 70:
            return "Fair"
        else:
            return "Poor"
    
    def save_calibration(self, filename):
        """Save calibration data to file"""
        with open(filename, 'w') as f:
            json.dump(self.calibration_data, f, indent=2)
        print(f"\nCalibration data saved to {filename}")
    
    def generate_config_header(self, filename):
        """Generate Arduino config header file"""
        config_content = f"""/*
 * Auto-generated calibration configuration
 * Generated: {self.calibration_data['timestamp']}
 * Device: DrishtiGuide Sensor Node
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

// Ultrasonic sensor calibration
"""
        
        if 'ultrasonic' in self.calibration_data:
            ultra = self.calibration_data['ultrasonic']
            config_content += f"""#define ULTRASONIC_OFFSET {ultra['recommended_offset']:.3f}
#define ULTRASONIC_CALIBRATED true
"""
        
        config_content += "\n// Battery calibration\n"
        if 'battery' in self.calibration_data:
            battery = self.calibration_data['battery']
            config_content += f"""#define BATTERY_CALIBRATION_FACTOR {battery['calibration_factor']:.4f}
#define BATTERY_CALIBRATED true
"""
        
        config_content += "\n// Communication settings\n"
        if 'espnow' in self.calibration_data:
            espnow = self.calibration_data['espnow']
            if espnow['success_rate'] < 70:
                config_content += "#define ESPNOW_RETRIES 5\n"
            else:
                config_content += "#define ESPNOW_RETRIES 3\n"
        
        config_content += "\n#endif // CALIBRATION_H\n"
        
        with open(filename, 'w') as f:
            f.write(config_content)
        print(f"Configuration header saved to {filename}")
    
    def run_full_calibration(self):
        """Run complete calibration sequence"""
        print("=== DRISHTIGUIDE SENSOR CALIBRATION ===")
        print("This tool will calibrate ultrasonic sensors and communication")
        print("Make sure the device is connected and running the transmitter sketch\n")
        
        if not self.connect():
            return False
        
        try:
            # Run calibration sequence
            self.calibrate_ultrasonic()
            self.calibrate_battery()
            self.test_espnow_communication()
            
            # Generate timestamped filenames
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            json_file = f"calibration_{timestamp}.json"
            header_file = f"calibration_{timestamp}.h"
            
            # Save results
            self.save_calibration(json_file)
            self.generate_config_header(header_file)
            
            print(f"\n=== CALIBRATION COMPLETE ===")
            print(f"Results saved to: {json_file}")
            print(f"Config header: {header_file}")
            print("Include the header file in your Arduino sketch to apply calibration")
            
            return True
            
        except KeyboardInterrupt:
            print("\nCalibration interrupted by user")
            return False
        except Exception as e:
            print(f"Error during calibration: {e}")
            return False
        finally:
            self.disconnect()

def main():
    parser = argparse.ArgumentParser(description='DrishtiGuide Sensor Calibration Tool')
    parser.add_argument('port', help='Serial port (e.g., COM3 or /dev/ttyUSB0)')
    parser.add_argument('--baudrate', type=int, default=115200, help='Baud rate (default: 115200)')
    parser.add_argument('--ultrasonic-only', action='store_true', help='Only calibrate ultrasonic sensor')
    parser.add_argument('--comm-test', action='store_true', help='Only test communication')
    
    args = parser.parse_args()
    
    calibrator = SensorCalibrator(args.port, args.baudrate)
    
    if args.ultrasonic_only:
        if calibrator.connect():
            calibrator.calibrate_ultrasonic()
            calibrator.disconnect()
    elif args.comm_test:
        if calibrator.connect():
            calibrator.test_espnow_communication()
            calibrator.disconnect()
    else:
        calibrator.run_full_calibration()

if __name__ == '__main__':
    main()