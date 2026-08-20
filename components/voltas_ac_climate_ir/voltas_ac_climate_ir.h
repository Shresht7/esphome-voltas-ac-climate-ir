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

// Supports Fan-Only Mode
static constexpr bool SUPPORTS_FAN_ONLY = true;

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
                                      MIN_TEMPERATURE,   // Minimum Temperature
                                      MAX_TEMPERATURE,   // Maximum Temperature
                                      TEMPERATURE_STEP,  // Temperature Step
                                      false,             // Supports Dry Mode? (Hardcoded to false for now)
                                      SUPPORTS_FAN_ONLY, // Supports Fan Only Mode? (Hardcoded to false for now)
                                      FAN_SPEEDS         // Supported Fan Speeds
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
        };

    } // namespace voltas_ac_climate_ir

} // namespace esphome
