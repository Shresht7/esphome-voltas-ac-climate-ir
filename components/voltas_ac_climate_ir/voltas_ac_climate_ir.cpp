#include "esphome/core/log.h"

#include "voltas_ac_climate_ir.h"

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        static const char *TAG = "voltas_ac_climate_ir.climate";

        esphome::climate::ClimateTraits VoltasACClimateIR::traits()
        {
            // Get the base traits from ClimateIR
            auto traits = esphome::climate::ClimateTraits();

            // Explicitly tell Home Assistant we ONLY support OFF and COOL
            traits.set_supported_modes({esphome::climate::CLIMATE_MODE_OFF, esphome::climate::CLIMATE_MODE_COOL});

            return traits;
        }

        void VoltasACClimateIR::transmit_state()
        {
            // TODO: Implement the logic to transmit the IR signal based on the current state of the climate entity (mode, temperature, fan speed, etc.)

            ESP_LOGD(TAG, "Transmitting IR signal for Voltas AC Climate Controller with current settings...");
            ESP_LOGD(TAG, "Current Mode: %d", this->mode);
            ESP_LOGD(TAG, "Current Target Temperature: %.1f", this->target_temperature);
            // Add more logs as needed to debug the state being transmitted
        }

    } // namespace voltas_ac_climate_ir
} // namespace esphome
