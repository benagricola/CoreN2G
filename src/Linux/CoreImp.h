#ifndef LINUX_COREIMP_H
#define LINUX_COREIMP_H

#include <Core.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <climits>
#include <functional>
#include <vector>
#include <map>
#include <mutex>

// Mock hardware implementation for Linux emulation

// Pin definitions for Linux emulation
using Pin = int32_t;
constexpr Pin NoPin = -1;

// Mock IO port enumeration
enum class PortNumber : uint8_t {
    A = 0, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, Z,
    MAX_PORTS
};

// Pin mode definitions
enum PinMode : uint8_t {
    INPUT = 0,
    OUTPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN,
    AIN,
    OUTPUT_PWM,
    OUTPUT_LOW_OPEN_DRAIN,
    OUTPUT_HIGH_OPEN_DRAIN
};

// Interrupt mode definitions
enum class InterruptMode : uint8_t {
    NONE = 0,
    RISING,
    FALLING,
    CHANGE,
    LOW,
    HIGH,
    INTERRUPT_MODE_COUNT
};

// Pin function
enum class PinFunction : uint8_t {
    NONE = 0,
    DIGITAL_IN,
    DIGITAL_OUT,
    ANALOG_IN,
    ANALOG_OUT,
    PWM_OUT,
    SERVO,
    INTERRUPT_IN
};

// Naming style
constexpr size_t PinPackageNameLength = 8;

// Mock functions for pin configuration
bool IsValidPin(Pin p) noexcept;
void SetPinFunction(Pin pin, PinFunction function, const char *owner = nullptr) noexcept;
void ResetPin(Pin pin) noexcept;
bool IsAnalogSupported(Pin p) noexcept;

// Mock registry for tracking attached pin functions and state
class PinRegistry {
public:
    static PinRegistry& Instance() noexcept;

    // Register a pin for a particular function
    void RegisterPin(Pin pin, PinFunction function, const char *owner = nullptr) noexcept;

    // Set and get pin state
    void SetPinState(Pin pin, bool state) noexcept;
    bool GetPinState(Pin pin) const noexcept;

    // Set and get analog values
    void SetAnalogValue(Pin pin, float value) noexcept;
    float GetAnalogValue(Pin pin) const noexcept;

    // Reset a pin to default state
    void ResetPin(Pin pin) noexcept;

    // For mock implementation - customize pin behavior
    using DigitalReadFunction = std::function<bool(Pin)>;
    using AnalogReadFunction = std::function<float(Pin)>;

    void SetDigitalReadFunction(Pin pin, DigitalReadFunction func) noexcept;
    void SetAnalogReadFunction(Pin pin, AnalogReadFunction func) noexcept;

private:
    PinRegistry() = default;
    ~PinRegistry() = default;

    mutable std::mutex mutex;

    struct PinState {
        PinFunction function = PinFunction::NONE;
        const char* owner = nullptr;
        bool digitalValue = false;
        float analogValue = 0.0f;
        DigitalReadFunction digitalReadFunc;
        AnalogReadFunction analogReadFunc;
    };

    std::map<Pin, PinState> pins;
};

#endif // LINUX_COREIMP_H