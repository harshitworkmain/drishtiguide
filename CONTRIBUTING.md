# Contributing to DrishtiGuide

Thank you for your interest in contributing to DrishtiGuide! This document provides guidelines and information for contributors.

## 🤝 How to Contribute

### Reporting Bugs
- Use the [bug report template](.github/ISSUE_TEMPLATE/bug_report.md)
- Provide detailed reproduction steps
- Include system information (OS, Arduino version, hardware)
- Add relevant logs or screenshots

### Suggesting Features
- Use the [feature request template](.github/ISSUE_TEMPLATE/feature_request.md)
- Describe the problem your feature solves
- Provide use cases and examples
- Consider accessibility impact

### Code Contributions
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Follow the coding standards below
5. Test your changes thoroughly
6. Commit your changes (`git commit -m 'Add amazing feature'`)
7. Push to the branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

## 📋 Development Workflow

### Prerequisites
- Arduino IDE 1.8.19+ or PlatformIO
- ESP8266 and ESP32 board packages installed
- Required libraries installed (see [requirements.txt](../requirements.txt))
- Git version control

### Local Development Setup
```bash
# Clone your fork
git clone https://github.com/harshitworkmain/drishtiguide.git
cd drishtiguide

# Add remote upstream
git remote add upstream https://github.com/harshitworkmain/drishtiguide.git

# Create feature branch
git checkout -b feature/your-feature-name

# Set up development environment
./deployment/setup_dev_environment.sh
```

### Testing
Before submitting a PR, ensure:
- [ ] Code compiles without warnings
- [ ] All tests pass (`./tests/run_tests.sh`)
- [ ] New features have unit tests
- [ ] Documentation is updated
- [ ] Hardware compatibility is considered

## 📝 Coding Standards

### Arduino/C++ Guidelines
- **Indentation**: 2 spaces (no tabs)
- **Line Length**: Maximum 100 characters
- **Naming Conventions**:
  - Variables: `camelCase` (e.g., `sensorData`)
  - Functions: `camelCase` (e.g., `readSensors()`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_DISTANCE`)
  - Classes: `PascalCase` (e.g., `SensorManager`)
- **Comments**: Use `//` for single line, `/* */` for multi-line
- **File Headers**: Include purpose and author information

### Code Style Example
```cpp
/**
 * Distance sensor handler for ultrasonic measurements
 * Author: Harshit Singh <harshit.workmain@gmail.com>
 * Date: 2025-02-01
 */

#include "config.h"

// Constants
#define TRIGGER_PULSE_US 10
#define SPEED_OF_SOUND 0.034

// Function to measure distance
int measureDistance() {
    // Send trigger pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(TRIGGER_PULSE_US);
    digitalWrite(TRIG_PIN, LOW);
    
    // Read echo pulse
    long duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT_US);
    
    // Calculate distance
    return duration * SPEED_OF_SOUND / 2;
}
```

## 📚 Documentation Standards

### Markdown Guidelines
- Use ATX-style headers (`#`, `##`, `###`)
- Include code examples with language specification
- Add images and diagrams where helpful
- Use bullet points for lists
- Include table of contents for long documents

### Comment Standards
```cpp
// Single line comment
/*
 * Multi-line comment
 * explaining complex functionality
 */
```

## 🔧 Technical Requirements

### ESP8266 Development
- **Board**: NodeMCU 1.0 (ESP-12E Module)
- **CPU Frequency**: 80 MHz or 160 MHz
- **Flash Size**: 4MB minimum
- **Upload Speed**: 115200 baud

### ESP32 Development  
- **Board**: ESP32 DevKit V1
- **CPU Frequency**: 240 MHz
- **Flash Size**: 4MB minimum
- **Upload Speed**: 921600 baud

### Memory Constraints
- **ESP8266**: ~80KB available RAM
- **ESP32**: ~520KB available RAM
- **Optimization**: Avoid dynamic memory allocation in loops
- **Stack Usage**: Keep functions lightweight

## 🧪 Testing Guidelines

### Unit Tests
- Test individual functions and classes
- Mock hardware dependencies
- Test edge cases and error conditions
- Maintain 90%+ code coverage

### Integration Tests
- Test component interactions
- Validate ESP-NOW communication
- Test web interface integration
- Verify end-to-end functionality

### Hardware Testing
- Test on actual hardware when possible
- Validate sensor calibrations
- Test power consumption
- Verify timing constraints

## 📦 Library Management

### Adding New Libraries
1. Update [requirements.txt](../requirements.txt)
2. Test with multiple ESP boards
3. Update [Hardware Specifications](../hardware/bill_of_materials/hardware_specifications.md)
4. Add library installation instructions to [Installation Guide](../docs/installation_guide.md)

### Version Compatibility
- Specify minimum Arduino IDE version
- Document ESP board version compatibility
- Note any breaking changes
- Provide migration guides

## 🔄 Pull Request Process

### Before Submitting
1. **Sync with upstream**:
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Run full test suite**:
   ```bash
   ./tests/run_tests.sh all
   ```

3. **Check code formatting**:
   ```bash
   # (Optional) Use arduino-formatter if available
   ```

### Pull Request Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass  
- [ ] Hardware tested (if applicable)

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review of the code
- [ ] Documentation updated
- [ ] No new warnings introduced
```

## 🏷️ Labeling

### PR Labels
- `bug`: Bug fixes and issue resolution
- `enhancement`: New features and improvements
- `documentation`: Documentation changes
- `hardware`: Hardware design and schematics
- `ci/cd`: Build and deployment changes
- `good first issue`: Suitable for new contributors

### Issue Labels
- `bug`: Confirmed bugs
- `enhancement`: Feature requests
- `question**: Support questions
- `documentation`: Documentation issues
- `hardware`: Hardware-related issues
- `help wanted`: Open for community contribution

## 🎯 Priority Areas

We welcome contributions in these areas:
- **Accessibility Features**: New ways to assist visually impaired users
- **Power Optimization**: Battery life improvements
- **Sensor Integration**: Support for additional sensors
- **Mobile Applications**: Smartphone companion apps
- **Cloud Integration**: Remote monitoring and data analytics
- **Machine Learning**: Pattern recognition and predictive features

## 📞 Getting Help

### Support Channels
- **GitHub Issues**: [Create an issue](https://github.com/harshitworkmain/drishtiguide/issues)
- **Email**: harshit.workmain@gmail.com
- **Discussions**: [GitHub Discussions](https://github.com/harshitworkmain/drishtiguide/discussions)

### Resources
- [System Architecture](../docs/system_architecture.md)
- [API Documentation](../docs/api_documentation.md)
- [Hardware Specifications](../hardware/bill_of_materials/hardware_specifications.md)
- [Installation Guide](../docs/installation_guide.md)

## 📜 License

By contributing to this project, you agree that your contributions will be licensed under the [MIT License](../LICENSE).

## 🙏 Recognition

Contributors will be acknowledged in:
- README.md contributors section
- Release notes
- Project documentation
- Annual project summary

Thank you for making DrishtiGuide better for everyone! 🚀