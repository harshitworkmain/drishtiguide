#!/usr/bin/env python3
"""
DrishtiGuide Data Logger

Logs real-time sensor data from ESP-NOW communication for analysis
and debugging. Supports multiple data formats and export options.
"""

import serial
import json
import csv
import time
import argparse
import threading
from datetime import datetime
from collections import deque
import statistics

class DataLogger:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial_conn = None
        self.logging = False
        self.data_buffer = deque(maxlen=1000)
        self.session_start = None
        
        # Statistics
        self.packets_received = 0
        self.packets_dropped = 0
        self.last_packet_time = None
        
    def connect(self):
        """Connect to the device"""
        try:
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=1)
            print(f"Connected to {self.port} at {self.baudrate} baud")
            return True
        except serial.SerialException as e:
            print(f"Failed to connect: {e}")
            return False
    
    def disconnect(self):
        """Stop logging and close connection"""
        self.logging = False
        if self.serial_conn:
            self.serial_conn.close()
            print("Disconnected")
    
    def parse_sensor_data(self, line):
        """Parse sensor data from serial output"""
        try:
            # Try to extract structured data
            if 'Distance:' in line or 'distance' in line.lower():
                data = {'timestamp': datetime.now().isoformat()}
                
                # Extract distance
                if 'Distance:' in line:
                    parts = line.split('Distance:')[1].strip().split()
                    if parts:
                        data['distance'] = float(parts[0].replace('cm', ''))
                
                # Extract battery if present
                if 'Battery:' in line or 'battery' in line.lower():
                    parts = line.split('Battery:') if 'Battery:' in line else line.split('battery:')
                    if len(parts) > 1:
                        battery_part = parts[1].strip().split()[0]
                        data['battery'] = float(battery_part.replace('%', ''))
                
                # Extract temperature if present
                if 'Temp:' in line or 'temp' in line.lower():
                    parts = line.split('Temp:') if 'Temp:' in line else line.split('temp:')
                    if len(parts) > 1:
                        temp_part = parts[1].strip().split()[0]
                        data['temperature'] = float(temp_part.replace('°C', '').replace('C', ''))
                
                # Extract delivery status
                if 'Delivery success' in line:
                    data['transmission_status'] = 'success'
                elif 'Delivery fail' in line:
                    data['transmission_status'] = 'fail'
                
                return data if len(data) > 1 else None
            
            # Parse JSON data if present
            if line.strip().startswith('{') and line.strip().endswith('}'):
                try:
                    json_data = json.loads(line.strip())
                    json_data['timestamp'] = datetime.now().isoformat()
                    return json_data
                except json.JSONDecodeError:
                    pass
            
            return None
            
        except Exception as e:
            print(f"Error parsing line: {e}")
            return None
    
    def log_data_csv(self, filename):
        """Log data to CSV file"""
        print(f"Logging to {filename}")
        
        with open(filename, 'w', newline='') as csvfile:
            fieldnames = ['timestamp', 'distance', 'battery', 'temperature', 'transmission_status']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            
            while self.logging:
                try:
                    if self.serial_conn.in_waiting > 0:
                        line = self.serial_conn.readline().decode('utf-8').strip()
                        
                        data = self.parse_sensor_data(line)
                        if data:
                            writer.writerow(data)
                            csvfile.flush()
                            
                            self.packets_received += 1
                            self.last_packet_time = datetime.now()
                            
                            # Print progress
                            if self.packets_received % 100 == 0:
                                print(f"Logged {self.packets_received} packets")
                
                except Exception as e:
                    print(f"Logging error: {e}")
                    self.packets_dropped += 1
                
                time.sleep(0.01)  # Small delay to prevent CPU overload
    
    def log_data_json(self, filename):
        """Log data to JSON file"""
        print(f"Logging to {filename}")
        
        with open(filename, 'w') as jsonfile:
            jsonfile.write('[\n')
            first_entry = True
            
            while self.logging:
                try:
                    if self.serial_conn.in_waiting > 0:
                        line = self.serial_conn.readline().decode('utf-8').strip()
                        
                        data = self.parse_sensor_data(line)
                        if data:
                            if not first_entry:
                                jsonfile.write(',\n')
                            
                            json.dump(data, jsonfile, indent=2)
                            first_entry = False
                            jsonfile.flush()
                            
                            self.packets_received += 1
                            self.last_packet_time = datetime.now()
                
                except Exception as e:
                    print(f"Logging error: {e}")
                    self.packets_dropped += 1
                
                time.sleep(0.01)
            
            jsonfile.write('\n]')
    
    def real_time_monitor(self):
        """Real-time data monitoring with statistics"""
        print("\n=== REAL-TIME MONITOR ===")
        print("Press Ctrl+C to stop monitoring\n")
        
        while self.logging:
            try:
                if self.serial_conn.in_waiting > 0:
                    line = self.serial_conn.readline().decode('utf-8').strip()
                    
                    data = self.parse_sensor_data(line)
                    if data:
                        self.data_buffer.append(data)
                        self.packets_received += 1
                        self.last_packet_time = datetime.now()
                        
                        # Print formatted data
                        self.print_data_summary(data)
                
                # Update statistics periodically
                if self.packets_received > 0 and self.packets_received % 50 == 0:
                    self.print_statistics()
                
                time.sleep(0.01)
                
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f"Monitoring error: {e}")
                self.packets_dropped += 1
        
        self.print_statistics()
    
    def print_data_summary(self, data):
        """Print a formatted summary of received data"""
        output = f"[{data['timestamp'][-12:]}] "
        
        if 'distance' in data:
            output += f"Distance: {data['distance']:6.1f}cm "
        
        if 'battery' in data:
            output += f"Battery: {data['battery']:3.0f}% "
        
        if 'temperature' in data:
            output += f"Temp: {data['temperature']:5.1f}°C "
        
        if 'transmission_status' in data:
            status = "✓" if data['transmission_status'] == 'success' else "✗"
            output += f"TX: {status} "
        
        print(output)
    
    def print_statistics(self):
        """Print data statistics"""
        if not self.data_buffer:
            return
        
        recent_data = list(self.data_buffer)[-100:]  # Last 100 packets
        
        stats = {
            'total_packets': self.packets_received,
            'dropped_packets': self.packets_dropped,
            'success_rate': 0,
            'avg_distance': 0,
            'min_distance': float('inf'),
            'max_distance': 0,
            'avg_battery': 0,
            'data_rate': 0
        }
        
        distances = [d['distance'] for d in recent_data if 'distance' in d]
        batteries = [d['battery'] for d in recent_data if 'battery' in d]
        transmissions = [d['transmission_status'] for d in recent_data if 'transmission_status' in d]
        
        if distances:
            stats['avg_distance'] = statistics.mean(distances)
            stats['min_distance'] = min(distances)
            stats['max_distance'] = max(distances)
        
        if batteries:
            stats['avg_battery'] = statistics.mean(batteries)
        
        if transmissions:
            successes = transmissions.count('success')
            stats['success_rate'] = (successes / len(transmissions)) * 100
        
        if self.session_start:
            elapsed = (datetime.now() - self.session_start).total_seconds()
            if elapsed > 0:
                stats['data_rate'] = self.packets_received / elapsed
        
        print(f"\n=== STATISTICS ===")
        print(f"Total Packets: {stats['total_packets']}")
        print(f"Dropped Packets: {stats['dropped_packets']}")
        print(f"Success Rate: {stats['success_rate']:.1f}%")
        print(f"Data Rate: {stats['data_rate']:.1f} packets/sec")
        if distances:
            print(f"Distance - Avg: {stats['avg_distance']:.1f}cm, Min: {stats['min_distance']:.1f}cm, Max: {stats['max_distance']:.1f}cm")
        if batteries:
            print(f"Average Battery: {stats['avg_battery']:.1f}%")
        print("=" * 50)
    
    def analyze_data(self, data_file):
        """Analyze existing log file"""
        print(f"Analyzing data from {data_file}")
        
        try:
            if data_file.endswith('.csv'):
                self.analyze_csv_file(data_file)
            elif data_file.endswith('.json'):
                self.analyze_json_file(data_file)
            else:
                print("Unsupported file format. Use .csv or .json")
        
        except Exception as e:
            print(f"Error analyzing file: {e}")
    
    def analyze_csv_file(self, filename):
        """Analyze CSV log file"""
        distances = []
        batteries = []
        temperatures = []
        
        with open(filename, 'r') as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if row.get('distance'):
                    distances.append(float(row['distance']))
                if row.get('battery'):
                    batteries.append(float(row['battery']))
                if row.get('temperature'):
                    temperatures.append(float(row['temperature']))
        
        self.print_analysis_results(distances, batteries, temperatures)
    
    def analyze_json_file(self, filename):
        """Analyze JSON log file"""
        distances = []
        batteries = []
        temperatures = []
        
        with open(filename, 'r') as jsonfile:
            data = json.load(jsonfile)
            for entry in data:
                if entry.get('distance'):
                    distances.append(float(entry['distance']))
                if entry.get('battery'):
                    batteries.append(float(entry['battery']))
                if entry.get('temperature'):
                    temperatures.append(float(entry['temperature']))
        
        self.print_analysis_results(distances, batteries, temperatures)
    
    def print_analysis_results(self, distances, batteries, temperatures):
        """Print analysis results"""
        print("\n=== DATA ANALYSIS RESULTS ===")
        
        if distances:
            print(f"\nDistance Analysis ({len(distances)} samples):")
            print(f"  Mean: {statistics.mean(distances):.2f}cm")
            print(f"  Median: {statistics.median(distances):.2f}cm")
            print(f"  Std Dev: {statistics.stdev(distances):.2f}cm")
            print(f"  Range: {min(distances):.2f}cm - {max(distances):.2f}cm")
        
        if batteries:
            print(f"\nBattery Analysis ({len(batteries)} samples):")
            print(f"  Mean: {statistics.mean(batteries):.2f}%")
            print(f"  Range: {min(batteries):.2f}% - {max(batteries):.2f}%")
        
        if temperatures:
            print(f"\nTemperature Analysis ({len(temperatures)} samples):")
            print(f"  Mean: {statistics.mean(temperatures):.2f}°C")
            print(f"  Range: {min(temperatures):.2f}°C - {max(temperatures):.2f}°C")
        
        print("\n" + "=" * 50)
    
    def start_logging(self, output_format='csv', filename=None):
        """Start data logging"""
        if not filename:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"sensor_log_{timestamp}.{output_format}"
        
        self.session_start = datetime.now()
        self.logging = True
        
        print(f"Starting {output_format.upper()} logging...")
        print(f"Output file: {filename}")
        print("Press Ctrl+C to stop logging\n")
        
        try:
            if output_format == 'csv':
                self.log_data_csv(filename)
            elif output_format == 'json':
                self.log_data_json(filename)
            elif output_format == 'monitor':
                self.real_time_monitor()
            else:
                print(f"Unsupported format: {output_format}")
        
        except KeyboardInterrupt:
            print(f"\nLogging stopped by user")
        finally:
            self.logging = False
            self.disconnect()
            self.print_final_statistics()
    
    def print_final_statistics(self):
        """Print final logging statistics"""
        if self.session_start:
            duration = datetime.now() - self.session_start
            print(f"\n=== FINAL STATISTICS ===")
            print(f"Session Duration: {duration}")
            print(f"Total Packets: {self.packets_received}")
            print(f"Dropped Packets: {self.packets_dropped}")
            
            if self.packets_received > 0:
                rate = self.packets_received / duration.total_seconds()
                print(f"Average Data Rate: {rate:.2f} packets/sec")

def main():
    parser = argparse.ArgumentParser(description='DrishtiGuide Data Logger')
    parser.add_argument('port', help='Serial port (e.g., COM3 or /dev/ttyUSB0)')
    parser.add_argument('--baudrate', type=int, default=115200, help='Baud rate (default: 115200)')
    parser.add_argument('--format', choices=['csv', 'json', 'monitor'], default='monitor',
                       help='Output format (default: monitor)')
    parser.add_argument('--output', help='Output filename (auto-generated if not specified)')
    parser.add_argument('--analyze', help='Analyze existing log file')
    
    args = parser.parse_args()
    
    logger = DataLogger(args.port, args.baudrate)
    
    if args.analyze:
        logger.analyze_data(args.analyze)
    else:
        if logger.connect():
            logger.start_logging(args.format, args.output)
        else:
            print("Failed to connect to device")

if __name__ == '__main__':
    main()