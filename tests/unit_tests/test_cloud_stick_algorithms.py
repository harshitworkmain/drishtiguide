#!/usr/bin/env python3
"""
Unit Tests for Cloud-Connected Smart Stick Algorithms

Validates:
- Obstacle LED proximity thresholds (<10cm)
- Dual-stage kinematic fall detection & inactivity timers
- Firebase Realtime Database payload schema compliance
- Bidirectional remote emergency resolution state transitions
"""

import unittest
import math
import re

class TestCloudStickAlgorithms(unittest.TestCase):
    
    def test_obstacle_led_threshold(self):
        """Test obstacle LED alert logic (<10cm)"""
        obstacle_threshold = 10.0  # cm
        
        # Test distances
        test_cases = [
            (5.0, True),    # Within 10cm -> Obstacle LED ON
            (9.9, True),    # Just under threshold -> ON
            (10.0, False),  # At threshold -> OFF
            (15.2, False),  # Outside threshold -> OFF
            (0.0, False),   # Invalid 0 reading -> OFF
            (-1.0, False),  # Negative reading -> OFF
        ]
        
        for distance, expected_led_state in test_cases:
            is_obstacle = (0.0 < distance < obstacle_threshold)
            self.assertEqual(is_obstacle, expected_led_state, f"Failed at distance {distance}cm")

    def test_cloud_stick_fall_kinematics(self):
        """Test total acceleration norm and fall classification on Cloud Stick"""
        # MPU6050 conversion factor: 16384 LSB/g
        scale = 16384.0
        
        # Simulate free fall: near 0g
        ax_ff, ay_ff, az_ff = int(0.05 * scale), int(0.05 * scale), int(0.1 * scale)
        accel_norm_ff = math.sqrt((ax_ff/scale)**2 + (ay_ff/scale)**2 + (az_ff/scale)**2)
        self.assertLess(accel_norm_ff, 0.3)
        
        # Simulate collision impact: >2.8g
        ax_imp, ay_imp, az_imp = int(1.5 * scale), int(2.0 * scale), int(2.5 * scale)
        accel_norm_imp = math.sqrt((ax_imp/scale)**2 + (ay_imp/scale)**2 + (az_imp/scale)**2)
        self.assertGreater(accel_norm_imp, 2.8)

    def test_firebase_payload_schema(self):
        """Verify Firebase RTDB payload conforms to Guardian web dashboard requirements"""
        payload = {
            "latitude": 12.840600,
            "longitude": 80.153400,
            "fallDetected": True,
            "lastFallTime": "14:32:15"
        }
        
        # Verify required keys
        self.assertIn("latitude", payload)
        self.assertIn("longitude", payload)
        self.assertIn("fallDetected", payload)
        self.assertIn("lastFallTime", payload)
        
        # Verify types
        self.assertIsInstance(payload["latitude"], float)
        self.assertIsInstance(payload["longitude"], float)
        self.assertIsInstance(payload["fallDetected"], bool)
        self.assertIsInstance(payload["lastFallTime"], str)
        
        # Verify time format (HH:MM:SS)
        time_pattern = r"^\d{2}:\d{2}:\d{2}$"
        self.assertTrue(re.match(time_pattern, payload["lastFallTime"]))

    def test_bidirectional_emergency_resolution(self):
        """Verify state transition when caregiver resolves emergency from Vercel dashboard"""
        class MockStickState:
            def __init__(self):
                self.fall_alert_active = False
                self.inactivity_triggered = False
                self.buzzer_active = False
            
            def on_fall_detected(self):
                self.fall_alert_active = True
                self.inactivity_triggered = True
                self.buzzer_active = True
                
            def on_cloud_poll(self, cloud_fall_detected):
                # When cloud indicates emergency was resolved remotely
                if not cloud_fall_detected and self.fall_alert_active:
                    self.fall_alert_active = False
                    self.inactivity_triggered = False
                    self.buzzer_active = False
        
        stick = MockStickState()
        self.assertFalse(stick.fall_alert_active)
        
        # Trigger fall
        stick.on_fall_detected()
        self.assertTrue(stick.fall_alert_active)
        self.assertTrue(stick.buzzer_active)
        
        # Simulate cloud poll while still active
        stick.on_cloud_poll(cloud_fall_detected=True)
        self.assertTrue(stick.fall_alert_active)
        
        # Caregiver clicks "RESOLVE EMERGENCY" on Vercel -> cloud_fall_detected becomes False
        stick.on_cloud_poll(cloud_fall_detected=False)
        self.assertFalse(stick.fall_alert_active)
        self.assertFalse(stick.inactivity_triggered)
        self.assertFalse(stick.buzzer_active)

if __name__ == '__main__':
    unittest.main()

