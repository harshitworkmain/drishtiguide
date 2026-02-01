#ifndef BUZZER_CONTROL_H
#define BUZZER_CONTROL_H

#include <Arduino.h>

// Buzzer patterns
typedef enum {
    BUZZ_PATTERN_SINGLE,
    BUZZ_PATTERN_DOUBLE,
    BUZZ_PATTERN_TRIPLE,
    BUZZ_PATTERN_LONG,
    BUZZ_PATTERN_SOS,
    BUZZ_PATTERN_EMERGENCY,
    BUZZ_PATTERN_WARNING,
    BUZZ_PATTERN_SUCCESS,
    BUZZ_PATTERN_ERROR,
    BUZZ_PATTERN_CUSTOM
} BuzzerPattern;

// Buzzer state
typedef enum {
    BUZZ_STATE_IDLE,
    BUZZ_STATE_PLAYING,
    BUZZ_STATE_PAUSED,
    BUZZ_STATE_EMERGENCY
} BuzzerState;

// Custom pattern structure
typedef struct {
    uint16_t onDuration;
    uint16_t offDuration;
    uint8_t repeatCount;
    uint16_t pauseDuration;
} BuzzerPatternData;

class BuzzerController {
private:
    uint8_t buzzerPin;
    BuzzerState currentState;
    
    // Timing variables
    uint32_t startTime;
    uint32_t lastToggle;
    uint32_t emergencyStartTime;
    
    // Current pattern
    BuzzerPattern currentPattern;
    BuzzerPatternData customPattern;
    uint8_t currentRepeat;
    bool isOn;
    
    // Emergency handling
    bool emergencyActive;
    uint8_t emergencyBeepCount;
    uint32_t lastEmergencyBeep;
    
    // Queue for multiple patterns
    BuzzerPattern patternQueue[8];
    uint8_t queueHead;
    uint8_t queueTail;
    uint8_t queueCount;
    
    // Private methods
    void playSingleBeep();
    void playDoubleBeep();
    void playTripleBeep();
    void playLongBeep();
    void playSOS();
    void playEmergency();
    void playWarning();
    void playSuccess();
    void playError();
    void playCustomPattern();
    
    void toggleBuzzer(bool state);
    void nextInQueue();
    bool isPatternComplete();
    void startPattern(BuzzerPattern pattern);

public:
    BuzzerController(uint8_t pin);
    
    // Initialization
    void begin();
    void test();
    void stop();
    
    // Pattern playback
    void beep();
    void beep(uint16_t duration);
    void beepPattern(BuzzerPattern pattern);
    void beepCustom(uint16_t onTime, uint16_t offTime, uint8_t repeats = 1);
    
    // Emergency alerts
    void emergencyAlert();
    void warningAlert();
    void successAlert();
    void errorAlert();
    void sosAlert();
    
    // Queue management
    void addToQueue(BuzzerPattern pattern);
    void clearQueue();
    bool isQueueEmpty();
    uint8_t getQueueSize();
    
    // State management
    void update();
    BuzzerState getState();
    bool isPlaying();
    bool isIdle();
    void pause();
    void resume();
    void stopPattern();
    
    // Configuration
    void setVolume(uint8_t volume);
    void setFrequency(uint16_t frequency);
    void setEmergencyMode(bool enabled);
    void setBuzzerPin(uint8_t pin);
    
    // Advanced features
    void setCustomPattern(const BuzzerPatternData& pattern);
    void savePattern(BuzzerPattern pattern, const BuzzerPatternData& data);
    BuzzerPatternData loadPattern(BuzzerPattern pattern);
    
    // Music functionality
    void playTone(uint16_t frequency, uint16_t duration);
    void playMelody(const uint16_t* notes, const uint16_t* durations, uint8_t length);
    void playNote(char note, uint16_t duration);
    
    // Diagnostics
    void printStatus();
    bool testBuzzer();
    void runSoundTest();
    
    // Callbacks
    void setPatternCompleteCallback(void (*callback)(BuzzerPattern));
    void setEmergencyCallback(void (*callback)());
    
    // Timers and scheduling
    void scheduleBeep(uint32_t delayMs, BuzzerPattern pattern);
    void cancelScheduledBeep();
    bool hasScheduledBeep();
    uint32_t getNextBeepTime();
};

// Predefined melody functions
namespace BuzzerMelodies {
    void playStartupJingle(BuzzerController& buzzer);
    void playShutdownJingle(BuzzerController& buzzer);
    void playAlertTone(BuzzerController& buzzer);
    void playConnectSound(BuzzerController& buzzer);
    void playDisconnectSound(BuzzerController& buzzer);
    void playLowBatteryWarning(BuzzerController& buzzer);
}

extern BuzzerController buzzer;

#endif // BUZZER_CONTROL_H