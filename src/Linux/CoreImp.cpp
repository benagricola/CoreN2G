#include "CoreImp.h"
#include <iostream>

// Implementation of PinRegistry singleton
PinRegistry& PinRegistry::Instance() noexcept {
    static PinRegistry instance;
    return instance;
}

void PinRegistry::RegisterPin(Pin pin, PinFunction function, const char *owner) noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto& state = pins[pin];
    state.function = function;
    state.owner = owner;

    // Initialize with default values based on function
    switch (function) {
        case PinFunction::DIGITAL_IN:
        case PinFunction::INTERRUPT_IN:
            state.digitalValue = false;
            break;
        case PinFunction::DIGITAL_OUT:
            state.digitalValue = false;
            break;
        case PinFunction::ANALOG_IN:
            state.analogValue = 0.0f;
            break;
        case PinFunction::ANALOG_OUT:
        case PinFunction::PWM_OUT:
            state.analogValue = 0.0f;
            break;
        default:
            break;
    }

    std::cout << "Pin " << pin << " registered as "
              << static_cast<int>(function)
              << " by " << (owner ? owner : "unknown") << std::endl;
}

void PinRegistry::SetPinState(Pin pin, bool state) noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pins.find(pin);
    if (it != pins.end()) {
        it->second.digitalValue = state;
        std::cout << "Pin " << pin << " set to " << (state ? "HIGH" : "LOW") << std::endl;
    }
}

bool PinRegistry::GetPinState(Pin pin) const noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pins.find(pin);
    if (it != pins.end()) {
        // If there's a custom read function, use it
        if (it->second.digitalReadFunc) {
            return it->second.digitalReadFunc(pin);
        }
        return it->second.digitalValue;
    }
    return false;
}

void PinRegistry::SetAnalogValue(Pin pin, float value) noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pins.find(pin);
    if (it != pins.end()) {
        it->second.analogValue = value;
        std::cout << "Pin " << pin << " analog value set to " << value << std::endl;
    }
}

float PinRegistry::GetAnalogValue(Pin pin) const noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pins.find(pin);
    if (it != pins.end()) {
        // If there's a custom read function, use it
        if (it->second.analogReadFunc) {
            return it->second.analogReadFunc(pin);
        }
        return it->second.analogValue;
    }
    return 0.0f;
}

void PinRegistry::ResetPin(Pin pin) noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    pins.erase(pin);
    std::cout << "Pin " << pin << " reset" << std::endl;
}

void PinRegistry::SetDigitalReadFunction(Pin pin, DigitalReadFunction func) noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pins.find(pin);
    if (it != pins.end()) {
        it->second.digitalReadFunc = func;
    }
}

void PinRegistry::SetAnalogReadFunction(Pin pin, AnalogReadFunction func) noexcept {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pins.find(pin);
    if (it != pins.end()) {
        it->second.analogReadFunc = func;
    }
}

// Implementation of global pin functions
bool IsValidPin(Pin p) noexcept {
    return p >= 0 && p < 256; // Define a reasonable range for mock pins
}

void SetPinFunction(Pin pin, PinFunction function, const char *owner) noexcept {
    if (IsValidPin(pin)) {
        PinRegistry::Instance().RegisterPin(pin, function, owner);
    }
}

void ResetPin(Pin pin) noexcept {
    if (IsValidPin(pin)) {
        PinRegistry::Instance().ResetPin(pin);
    }
}

bool IsAnalogSupported(Pin p) noexcept {
    return IsValidPin(p) && (p >= 100); // For simplicity, pins 100+ support analog
}

// Standard GPIO function implementation that the RRF codebase expects
extern "C" {

// Digital I/O
void pinMode(Pin pin, PinMode mode) noexcept {
    PinFunction function;
    switch (mode) {
        case INPUT:
        case INPUT_PULLUP:
        case INPUT_PULLDOWN:
            function = PinFunction::DIGITAL_IN;
            break;
        case OUTPUT:
            function = PinFunction::DIGITAL_OUT;
            break;
        case AIN:
            function = PinFunction::ANALOG_IN;
            break;
        case OUTPUT_PWM:
            function = PinFunction::PWM_OUT;
            break;
        default:
            function = PinFunction::NONE;
            break;
    }

    SetPinFunction(pin, function, "pinMode");
}

void digitalWrite(Pin pin, bool state) noexcept {
    PinRegistry::Instance().SetPinState(pin, state);
}

bool digitalRead(Pin pin) noexcept {
    return PinRegistry::Instance().GetPinState(pin);
}

// Analog I/O
void analogWrite(Pin pin, float value) noexcept {
    PinRegistry::Instance().SetAnalogValue(pin, value);
}

float analogRead(Pin pin) noexcept {
    return PinRegistry::Instance().GetAnalogValue(pin);
}

// Interrupt handling
void attachInterrupt(Pin pin, void (*callback)(void), InterruptMode mode) noexcept {
    SetPinFunction(pin, PinFunction::INTERRUPT_IN, "attachInterrupt");
    // In a real implementation, we'd register the callback
    std::cout << "Interrupt attached to pin " << pin << std::endl;
}

void detachInterrupt(Pin pin) noexcept {
    ResetPin(pin);
    std::cout << "Interrupt detached from pin " << pin << std::endl;
}

}  // extern "C"