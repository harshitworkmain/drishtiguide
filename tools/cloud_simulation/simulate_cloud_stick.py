#!/usr/bin/env python3
"""
DrishtiGuide Cloud Stick Simulator

Simulates the ESP32 Cloud Smart Stick sending telemetry and emergency alerts
to Firebase Realtime Database. Allows testing the live Guardian Vercel Dashboard
(https://stick-theta.vercel.app/) without physical hardware.
"""

import time
import json
import argparse
import urllib.request
import urllib.error
from datetime import datetime

DEFAULT_DB_URL = "https://stick-ff202-default-rtdb.asia-southeast1.firebasedatabase.app"
DEFAULT_AUTH = "i7NcmzVbSeDBNOmsDtOmBYM8AlR8AK9famr3ixfx"

class CloudStickSimulator:
    def __init__(self, db_url=DEFAULT_DB_URL, auth=DEFAULT_AUTH):
        self.db_url = db_url.rstrip('/')
        self.auth = auth
        self.current_lat = 12.8406
        self.current_lon = 80.1534
        self.fall_detected = False
        self.last_fall_time = "N/A"

    def _make_request(self, path, method="GET", data=None):
        url = f"{self.db_url}{path}.json"
        if self.auth:
            url += f"?auth={self.auth}"

        headers = {"Content-Type": "application/json"}
        payload = json.dumps(data).encode("utf-8") if data is not None else None
        req = urllib.request.Request(url, data=payload, headers=headers, method=method)

        try:
            with urllib.request.urlopen(req, timeout=5) as response:
                res_body = response.read().decode("utf-8")
                return json.loads(res_body) if res_body and res_body != "null" else None
        except urllib.error.URLError as e:
            print(f"[Error] Network/HTTP error: {e}")
            return None

    def get_stick_data(self):
        """Fetch current /stick node from Firebase RTDB"""
        return self._make_request("/stick", method="GET")

    def sync_telemetry(self, lat, lon, fall_detected=False, fall_time=None):
        """Update telemetry values in Firebase RTDB"""
        payload = {
            "latitude": round(lat, 6),
            "longitude": round(lon, 6),
            "fallDetected": bool(fall_detected),
            "lastFallTime": fall_time or self.last_fall_time
        }
        res = self._make_request("/stick", method="PUT", data=payload)
        return res is not None

    def trigger_fall(self, lat=None, lon=None):
        """Simulate a critical fall event"""
        lat = lat or self.current_lat
        lon = lon or self.current_lon
        now_str = datetime.now().strftime("%H:%M:%S")
        self.fall_detected = True
        self.last_fall_time = now_str

        print(f"\n[SIMULATOR] 🚨 TRIGGERING FALL at {lat}, {lon} (Time: {now_str})")
        success = self.sync_telemetry(lat, lon, fall_detected=True, fall_time=now_str)
        if success:
            print("[SIMULATOR] Emergency alert dispatched to Firebase.")
            print("Check https://stick-theta.vercel.app/ to verify emergency glow and 'RESOLVE EMERGENCY' button!")
        return success

    def resolve_emergency(self):
        """Clear emergency flag"""
        self.fall_detected = False
        print("\n[SIMULATOR] Resolving emergency status...")
        return self._make_request("/stick", method="PATCH", data={"fallDetected": False}) is not None

    def run_walk_simulation(self, steps=10, interval=3):
        """Simulate user walking along a path with periodic telemetry updates"""
        print(f"\n=== Starting Walk Simulation ({steps} steps, {interval}s interval) ===")
        print(f"Tracking on dashboard: https://stick-theta.vercel.app/\n")

        for step in range(steps):
            # Simulate slight GPS walking delta
            self.current_lat += 0.00008
            self.current_lon += 0.00006

            print(f"[{step + 1}/{steps}] Syncing GPS: Lat={self.current_lat:.6f}, Lon={self.current_lon:.6f}")
            self.sync_telemetry(self.current_lat, self.current_lon, self.fall_detected, self.last_fall_time)

            # Check if remote resolution was performed on Vercel
            current_cloud_state = self.get_stick_data()
            if current_cloud_state and not current_cloud_state.get("fallDetected", False) and self.fall_detected:
                print(">> Remote resolution detected from dashboard! Alarm silenced.")
                self.fall_detected = False

            time.sleep(interval)

        print("\n=== Walk Simulation Completed ===")

def main():
    parser = argparse.ArgumentParser(description="DrishtiGuide Cloud Stick Simulator")
    parser.add_argument("--db-url", default=DEFAULT_DB_URL, help="Firebase RTDB URL")
    parser.add_argument("--auth", default=DEFAULT_AUTH, help="Firebase Auth token / secret")
    parser.add_argument("--action", choices=["walk", "fall", "resolve", "read"], default="read",
                        help="Action to simulate")
    parser.add_argument("--steps", type=int, default=10, help="Number of walk steps")
    parser.add_argument("--interval", type=int, default=3, help="Interval between steps in seconds")

    args = parser.parse_args()

    sim = CloudStickSimulator(db_url=args.db_url, auth=args.auth)

    if args.action == "read":
        print(f"Reading current cloud state from {args.db_url}/stick.json...")
        data = sim.get_stick_data()
        print(f"Current State:\n{json.dumps(data, indent=2)}")
    elif args.action == "fall":
        sim.trigger_fall()
    elif args.action == "resolve":
        sim.resolve_emergency()
    elif args.action == "walk":
        sim.run_walk_simulation(steps=args.steps, interval=args.interval)

if __name__ == "__main__":
    main()

