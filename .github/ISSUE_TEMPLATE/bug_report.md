name: Bug Report
description: File a bug report to help us improve DrishtiGuide
title: "[BUG]: "
labels: ["bug", "triage"]
body:
  - type: markdown
    attributes:
      value: |
        Thanks for taking the time to fill out this bug report! Please provide as much detail as possible.

  - type: input
    id: contact
    attributes:
      label: Contact Details
      description: How can we contact you if we need more information?
      placeholder: "harshit.workmain@gmail.com"
    validations:
      required: false

  - type: dropdown
    id: component
    attributes:
      label: Component
      description: Which component is affected?
      options:
        - Transmitter Node (ESP8266)
        - Receiver Node (ESP8266)
        - Main Controller (ESP32)
        - Web Interface
        - Documentation
        - Other
    validations:
      required: true

  - type: textarea
    id: bug-description
    attributes:
      label: Bug Description
      description: A clear and concise description of what the bug is.
      placeholder: "Describe the bug..."
    validations:
      required: true

  - type: textarea
    id: steps-to-reproduce
    attributes:
      label: Steps to Reproduce
      description: Steps to reproduce the behavior
      placeholder: |
        1. Power on the device
        2. Connect to WiFi hotspot
        3. Access web interface
        4. See error
    validations:
      required: true

  - type: textarea
    id: expected-behavior
    attributes:
      label: Expected Behavior
      description: A clear description of what you expected to happen
      placeholder: "The web interface should load and show GPS data..."
    validations:
      required: true

  - type: textarea
    id: actual-behavior
    attributes:
      label: Actual Behavior
      description: What actually happened
      placeholder: "The web interface shows a 404 error..."
    validations:
      required: true

  - type: textarea
    id: logs
    attributes:
      label: Relevant Logs
      description: Please copy and paste any relevant log output
      render: shell

  - type: dropdown
    id: hardware-version
    attributes:
      label: Hardware Version
      description: Which hardware version are you using?
      options:
        - Prototype v1.0
        - Custom build
        - Development board
        - Not sure
    validations:
      required: false

  - type: dropdown
    id: firmware-version
    attributes:
      label: Firmware Version
      description: Which firmware version are you using?
      options:
        - Latest from main branch
        - v1.0
        - Development version
        - Not sure
    validations:
      required: false

  - type: dropdown
    id: operating-system
    attributes:
      label: Operating System
      description: What operating system are you using?
      options:
        - Windows 10/11
        - macOS
        - Linux
        - Raspberry Pi OS
        - Other
    validations:
      required: false

  - type: textarea
    id: additional-context
    attributes:
      label: Additional Context
      description: Add any other context about the problem here
      placeholder: "Additional information..."

  - type: checkboxes
    id: terms
    attributes:
      label: Confirmation
      description: Please confirm the following
      options:
        - label: I have searched existing issues for similar bugs
          required: true
        - label: I have provided all requested information
          required: true
        - label: This is not a security issue (security issues should be reported privately)
          required: true