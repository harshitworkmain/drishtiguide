# Guardian Cloud Web Interface

Live Web Monitor for the DrishtiGuide Smart Assistive Stick, featuring a dark-mode glassmorphic interface, real-time Leaflet.js mapping, and sub-second updates powered by Firebase Realtime Database.

## 🌐 Live Access
- **Production Dashboard**: [https://stick-theta.vercel.app/](https://stick-theta.vercel.app/)
- **Upstream Repository**: [https://github.com/agent-loop/stick](https://github.com/agent-loop/stick)

## 🌟 Key Features
- **Real-Time Geolocation Tracking**: Powered by Leaflet.js with CartoDB Dark Matter tiles. Smooth panning marker updates as the GPS coordinates shift.
- **Bi-directional Emergency Notification & Resolution**:
  - Automatically pulses an `emergency-glow` alert badge and reveals the emergency resolution button when `fallDetected == true`.
  - Caregivers can click **RESOLVE EMERGENCY** to update the cloud database (`fallDetected: false`), signaling the stick to stop its audible alarm.
- **Zero-Dependency Build**: Written in vanilla HTML5, Tailwind CSS via CDN, and the Firebase JavaScript SDK v8. Deploys statically in seconds on Vercel.

## 🚀 Deployment to Vercel

```bash
cd src/cloud-interface
vercel deploy --prod
```

Or connect the repository on the Vercel dashboard and set the root directory to `src/cloud-interface`.

