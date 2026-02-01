# Receiver Node Schematic

## 📍 Overview
Haptic feedback receiver node with 5 vibration motors controlled by ESP8266 NodeMCU via ESP-NOW wireless communication.

## 🔌 Component Connections

### ESP8266 NodeMCU to Motor Driver Circuit
```
NodeMCU          Motor Driver
-------          ------------
D1 (GPIO5) →  1kΩ → 2N2222 Base → Motor1(-) → Motor1(+) → 3.3V
D2 (GPIO4) →  1kΩ → 2N2222 Base → Motor2(-) → Motor2(+) → 3.3V
D3 (GPIO0) →  1kΩ → 2N2222 Base → Motor3(-) → Motor3(+) → 3.3V
D4 (GPIO2) →  1kΩ → 2N2222 Base → Motor4(-) → Motor4(+) → 3.3V
D5 (GPIO15)→  1kΩ → 2N2222 Base → Motor5(-) → Motor5(+) → 3.3V
GND        →  2N2222 Emitters (all tied together)
```

### Power Distribution
```
3.3V Power Rail
├── NodeMCU VCC
├── All Motor Positive Terminals
├── Status LED (via resistor)
└── 100μF Capacitor → GND (power filtering)

Ground Rail
├── NodeMCU GND
├── All 2N2222 Emitters
├── 100μF Capacitor (-) terminal
└── Battery Negative
```

## 📋 Detailed Pinout

| NodeMCU Pin | Function | Driver Circuit | Notes |
|--------------|----------|----------------|-------|
| 3V3 | Motor Power | All motor positives | Motor power supply |
| GND | Common Ground | All transistor emitters | Common ground reference |
| D1 (GPIO5) | Motor 1 Control | 2N2222 driver | Distance 80-100cm |
| D2 (GPIO4) | Motor 2 Control | 2N2222 driver | Distance 60-80cm |
| D3 (GPIO0) | Motor 3 Control | 2N2222 driver | Distance 40-60cm |
| D4 (GPIO2) | Motor 4 Control | 2N2222 driver | Distance 20-40cm |
| D5 (GPIO15) | Motor 5 Control | 2N2222 driver | Distance <20cm |
| D0 (GPIO16) | Status LED | LED + resistor | System status |

## ⚡ Power Requirements

### Motor Power Consumption
```
Motor Type: Miniature Pancake Vibration Motor
Operating Voltage: 3.3V
Current Draw: 50-100mA per motor
Maximum All Motors: 500mA
Peak Current: 800mA (startup surge)
```

### Power Supply Design
```
Battery: 3.7V 2000mAh Li-ion
Buck Converter: 3.7V → 3.3V @ 1A max
Current Capability: 1000mA (200% safety margin)
Run Time: ~4 hours (all motors active)
```

## 🔧 Motor Driver Circuit

### Individual Motor Driver
```
ESP8266 GPIO
     │
     └── 1kΩ Base Resistor
          │
          └── 2N2222 NPN Transistor Base
               │
     2N2222 NPN Transistor Emitter → GND
               │
     2N2222 NPN Transistor Collector
               │
               └── Vibration Motor Negative
```

### Component Values
```
Base Resistor (Rb): 1kΩ (limits base current)
Transistor (Q1): 2N2222 NPN (800mA max collector current)
Motor: 10mm pancake vibration motor (50-100mA @ 3.3V)
Flyback Diode: 1N4148 (optional, across motor terminals)
```

## 📊 Activation Logic

### Distance-to-Motor Mapping
```
Distance Range    | Active Motors | GPIO States | User Feedback
80cm - 100cm    | Motor 1       | D1=HIGH     | Light vibration
60cm - 80cm     | Motors 1-2    | D1=D2=HIGH  | Medium-light
40cm - 60cm     | Motors 1-3    | D1=D2=D3=HIGH| Medium
20cm - 40cm     | Motors 1-4    | D1-4=HIGH    | Medium-heavy
< 20cm          | Motors 1-5    | D1-5=HIGH    | Heavy vibration
```

### Motor Timing
```
Activation Delay: <50ms (from ESP-NOW packet reception)
Motor Pulse Duration: 100ms (configurable)
Motor Refresh Rate: 5Hz (maximum to prevent motor overheating)
Cooldown Between Activations: 50ms minimum
```

## 🛡️ Protection Circuits

### Transistor Protection
```
1. Base Resistor (1kΩ): Limits base current to ~3mA
2. Flyback Diode (1N4148): Across motor terminals
   - Protects transistor from back EMF
   - Cathode to transistor collector
   - Anode to transistor emitter (GND)
3. Base-Emitter Resistor (10kΩ): Prevents accidental activation
```

### Power Protection
```
1. Input Capacitor (100μF): Smooths current spikes
2. TVS Diode (3.6V): Overvoltage protection
3. PTC Resettable Fuse: 1A hold, 2A trip
```

## 📐 PCB Layout Guidelines

### Component Placement
1. **ESP8266 Module**: Center of PCB
2. **Motor Connectors**: Edge of PCB (wearable mounting)
3. **Driver Transistors**: Close to motor connectors
4. **Power Input**: Bottom edge with protection

### PCB Dimensions: 60mm × 40mm
- **Thickness**: 1.6mm FR4
- **Copper Weight**: 1oz (standard)
- **Surface Finish**: HASL (lead-free)

### Thermal Considerations
- **Transistor Cooling**: 2mm thermal pads for each transistor
- **Power Traces**: 1mm width for motor power
- **Ground Plane**: Both sides with thermal vias

## 📏 Mechanical Design

### Wearable Mounting
```
PCB Orientation: Landscape (60mm wide × 40mm high)
Mounting Points: 4 corners, M2 screws
Strap Attachment: 2 side hooks for velcro straps
Connector Access: Bottom edge for power/data
```

### Motor Placement
```
Motor 1: Left shoulder area
Motor 2: Left upper chest  
Motor 3: Center chest
Motor 4: Right upper chest
Motor 5: Right shoulder area
```

## 🧪 Testing Points

### Test Point Access
| TP | Function | Expected Signal | Test Condition |
|----|----------|----------------|-----------------|
| TP1 | Motor 1 Signal | 0V/3.3V | Motor activation |
| TP2 | Motor 2 Signal | 0V/3.3V | Motor activation |
| TP3 | Motor 3 Signal | 0V/3.3V | Motor activation |
| TP4 | Motor 4 Signal | 0V/3.3V | Motor activation |
| TP5 | Motor 5 Signal | 0V/3.3V | Motor activation |
| TP6 | 3.3V Power | 3.2V - 3.4V | Power on |
| TP7 | Ground | 0V | Always |

### Signal Testing
```
Oscilloscope Settings:
- Voltage Range: 5V/div
- Time Base: 10ms/div  
- Trigger: Rising edge, 2V

Expected Waveform:
- 0V → 3.3V transition: <1μs rise time
- 3.3V duration: 100ms (motor on)
- 3.3V → 0V transition: <1μs fall time
- 0V duration: Variable (motor off)
```

## 🔧 Troubleshooting

### Motor Not Vibrating
**Check:**
1. 3.3V at motor positive terminal
2. GPIO output with multimeter/logic analyzer
3. Transistor base voltage (~0.7V when active)
4. Motor resistance (should be 20-50Ω)

**Common Issues:**
- Transistor not conducting (bad solder joint)
- Motor connection reversed
- Insufficient base current (wrong resistor value)

### Weak Vibration
**Check:**
1. Power supply voltage (must be 3.3V minimum)
2. Motor current draw (should be 50-100mA)
3. Transistor saturation (Vce < 0.3V when on)

**Solutions:**
- Increase motor supply voltage (up to 5V)
- Use higher gain transistor (2N2222A)
- Parallel transistors for more current

### ESP8266 Reset Issues
**Check:**
1. Motor startup current spikes
2. Power supply decoupling
3. Ground connection integrity

**Solutions:**
- Add 1000μF capacitor at power input
- Use separate power supplies for ESP8266 and motors
- Add soft-start circuitry

## 📋 Bill of Materials (Receiver)

| Ref | Component | Value/Type | Package | Quantity | Notes |
|-----|-----------|-------------|----------|-----------|--------|
| U1 | ESP8266 NodeMCU | ESP-12E | 1 | Main controller |
| Q1-Q5 | NPN Transistor | 2N2222 | 5 | Motor drivers |
| R1-R5 | Resistor | 1kΩ, 1/4W | 5 | Base resistors |
| R6 | Resistor | 220Ω, 1/4W | 1 | LED current limit |
| D1 | LED | 3mm Red | 1 | Status indicator |
| M1-M5 | Vibration Motor | 10mm Pancake | 5 | Haptic feedback |
| C1 | Capacitor | 100μF, 16V | 1 | Power filtering |
| J1-J5 | Header | 2-pin | 5 | Motor connections |
| J6 | Header | 2-pin | 1 | Power input |
| TP1-TP7| Test Point | - | 7 | Testing access |

## 🔌 Connector Specifications

### Motor Connectors (J1-J5)
```
Pin 1: Motor Positive (+) → 3.3V
Pin 2: Motor Negative (-) → Transistor Collector
```

### Power Connector (J6)
```
Pin 1: VCC (+) → 3.7V (from battery)
Pin 2: GND (-) → Battery negative
```

## 🔚 Assembly Instructions

### Step 1: Power Circuit
1. Solder power connector (J6)
2. Install 100μF capacitor (C1)
3. Create power and ground planes

### Step 2: Driver Circuits
1. Solder base resistors (R1-R5)
2. Install NPN transistors (Q1-Q5)
3. Add flyback diodes if used

### Step 3: Control Circuit
1. Solder ESP8266 module
2. Connect GPIO pins to base resistors
3. Install status LED (D1) with resistor (R6)

### Step 4: Final Assembly
1. Solder motor connectors (J1-J5)
2. Add test points (TP1-TP7)
3. Install mounting hardware
4. Clean and inspect solder joints

---

*This schematic provides the complete design for the haptic feedback receiver node. For integration with the transmitter and testing procedures, see the main project documentation.*