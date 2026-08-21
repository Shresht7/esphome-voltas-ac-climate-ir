#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate_ir/climate_ir.h"

static constexpr float MIN_TEMPERATURE = 16.0f; // Minimum temperature in Celsius
static constexpr float MAX_TEMPERATURE = 30.0f; // Maximum temperature in Celsius
static constexpr float TEMPERATURE_STEP = 1.0f; // Temperature step in Celsius

// Supported fan speeds
static constexpr auto FAN_SPEEDS = {
    esphome::climate::CLIMATE_FAN_LOW,
    esphome::climate::CLIMATE_FAN_MEDIUM,
    esphome::climate::CLIMATE_FAN_HIGH,
    esphome::climate::CLIMATE_FAN_AUTO,
};

// Supports Dry Mode
static constexpr bool SUPPORTS_DRY_MODE = true;

// Supports Fan-Only Mode
static constexpr bool SUPPORTS_FAN_ONLY = true;

// Supported Climate Modes
static constexpr auto SUPPORTED_MODES = {
    esphome::climate::CLIMATE_MODE_OFF,
    esphome::climate::CLIMATE_MODE_COOL,
    esphome::climate::CLIMATE_MODE_DRY,
    // esphome::climate::CLIMATE_MODE_HEAT,
    esphome::climate::CLIMATE_MODE_FAN_ONLY,
};

// Supported Swing Modes
static constexpr auto SUPPORTED_SWING_MODES = {
    esphome::climate::CLIMATE_SWING_OFF,
    esphome::climate::CLIMATE_SWING_VERTICAL,
    esphome::climate::CLIMATE_SWING_HORIZONTAL,
    esphome::climate::CLIMATE_SWING_BOTH,
};

// Supported Presets
static constexpr auto SUPPORTED_PRESETS = {
    esphome::climate::CLIMATE_PRESET_NONE,
    esphome::climate::CLIMATE_PRESET_ECO,
    esphome::climate::CLIMATE_PRESET_BOOST,
};

// Swing-to-frame bit mapping
struct SwingBits
{
    bool vertical;   // Vertical swing state
    bool horizontal; // Horizontal swing state
};

// Preset-to-frame bit mapping
struct PresetBits
{
    bool turbo;     // Turbo mode state
    bool eco_saver; // Eco-saver mode state
};

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        // Inherit from ClimateIR
        class VoltasACClimateIR : public esphome::climate_ir::ClimateIR
        {
        public:
            // The constructor configures the capabilities shown in Home Assistant, such as supported modes, temperature range, etc.
            VoltasACClimateIR() : esphome::climate_ir::ClimateIR(
                                      MIN_TEMPERATURE,       // Minimum Temperature
                                      MAX_TEMPERATURE,       // Maximum Temperature
                                      TEMPERATURE_STEP,      // Temperature Step
                                      SUPPORTS_DRY_MODE,     // Supports Dry Mode
                                      SUPPORTS_FAN_ONLY,     // Supports Fan Only Mode
                                      FAN_SPEEDS,            // Supported Fan Speeds
                                      SUPPORTED_SWING_MODES, // Supported Swing Modes
                                      SUPPORTED_PRESETS      // Supported Presets like Boost, Eco, etc.
                                  )
            {
            }

            // Override traits to specify the capabilities of this climate controller
            esphome::climate::ClimateTraits traits() override;

            // Override the transmit method - called wheneer Home Assistant changes a setting
            void transmit_state() override;

            // Override the receive method - called whenever an IR frame is received
            bool on_receive(remote_base::RemoteReceiveData data) override;

            // Clamp the temperature to the valid range and round to the nearest step
            static uint8_t clamp_temperature(float temperature);

            // Helper function to get the fan speed from the fan mode
            static uint8_t get_fan_speed_from_mode(esphome::climate::ClimateFanMode fan_mode);

            // Helper function to get the fan mode from the fan speed
            static esphome::climate::ClimateFanMode get_fan_mode_from_speed(uint8_t fan_speed);

            // Helper function to get the mode from the climate mode
            static uint8_t get_bits_from_climate_mode(esphome::climate::ClimateMode mode);

            // Helper function to get the climate mode from the mode
            static esphome::climate::ClimateMode get_climate_mode_from_bits(uint8_t mode);

            // Helper function to get the vertical and horizontal swing states from the swing mode
            static SwingBits get_swing_bits(esphome::climate::ClimateSwingMode swing_mode);

            // Helper function to get swing mode from the vertical and horizontal swing states
            static esphome::climate::ClimateSwingMode get_swing_mode_from_bits(uint8_t vertical_swing, uint8_t horizontal_swing);

            // Helper function to get the turbo and eco-saver states from the preset
            static PresetBits get_preset_bits(esphome::climate::ClimatePreset preset);

            // Helper function to get the preset from the turbo and eco-saver states
            static esphome::climate::ClimatePreset get_preset_from_bits(bool turbo, bool eco_saver);
        };

    } // namespace voltas_ac_climate_ir

} // namespace esphome
