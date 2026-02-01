name: Feature Request
description: Suggest an idea for DrishtiGuide
title: "[FEATURE]: "
labels: ["enhancement", "triage"]
body:
  - type: markdown
    attributes:
      value: |
        Thanks for suggesting a new feature for DrishtiGuide! Please provide as much detail as possible.

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
      description: Which component would this feature affect?
      options:
        - Transmitter Node (ESP8266)
        - Receiver Node (ESP8266)
        - Main Controller (ESP32)
        - Web Interface
        - Mobile App
        - Documentation
        - Testing
        - Other
    validations:
      required: true

  - type: textarea
    id: feature-description
    attributes:
      label: Feature Description
      description: A clear and concise description of the feature you'd like to see
      placeholder: "Add a voice guidance system using text-to-speech..."
    validations:
      required: true

  - type: dropdown
    id: problem-type
    attributes:
      label: Problem Type
      description: What type of problem does this feature solve?
      options:
        - User Experience Improvement
        - Performance Enhancement
        - New Capability
        - Accessibility Enhancement
        - Power Management
        - Security Improvement
        - Developer Experience
        - Maintenance/Support
        - Other
    validations:
      required: true

  - type: textarea
    id: problem-details
    attributes:
      label: Problem Details
      description: Describe the problem this feature would solve
      placeholder: "Currently, users with limited hearing cannot benefit from audio alerts..."
    validations:
      required: true

  - type: textarea
    id: proposed-solution
    attributes:
      label: Proposed Solution
      description: How would you like this feature to be implemented?
      placeholder: "Add a DFPlayer mini module and integrate text-to-speech alerts..."
    validations:
      required: true

  - type: textarea
    id: alternatives-considered
    attributes:
      label: Alternatives Considered
      description: Describe any alternative solutions or features you've considered
      placeholder: "I considered using Bluetooth audio streaming but it adds complexity..."

  - type: textarea
    id: use-cases
    attributes:
      label: Use Cases
      description: Describe specific use cases where this feature would be valuable
      placeholder: "1. Elderly users with hearing impairment\n2. Users in noisy environments..."

  - type: dropdown
    id: priority
    attributes:
      label: Priority
      description: How important is this feature?
      options:
        - Critical (blocks usage)
        - High (major improvement)
        - Medium (nice to have)
        - Low (minor enhancement)
    validations:
      required: true

  - type: dropdown
    id: complexity
    attributes:
      label: Implementation Complexity
      description: How complex do you think this would be to implement?
      options:
        - Simple (few lines of code)
        - Moderate (requires some refactoring)
        - Complex (requires significant changes)
        - Very Complex (requires architecture changes)
    validations:
      required: true

  - type: checkboxes
    id: contribution
    attributes:
      label: Contribution
      description: Would you be willing to help implement this feature?
      options:
        - label: I can implement this feature myself
        - label: I can help test the implementation
        - label: I can provide feedback during development
        - label: I can contribute documentation
        - label: I cannot contribute at this time

  - type: textarea
    id: additional-context
    attributes:
      label: Additional Context
      description: Add any other context, screenshots, or examples about the feature request
      placeholder: "Additional information, diagrams, mockups..."

  - type: checkboxes
    id: terms
    attributes:
      label: Confirmation
      description: Please confirm the following
      options:
        - label: I have searched existing issues for similar feature requests
          required: true
        - label: I have provided sufficient detail for the feature to be understood
          required: true
        - label: This feature aligns with the project's accessibility goals
          required: true