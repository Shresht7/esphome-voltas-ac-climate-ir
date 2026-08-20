#include "esphome/core/log.h"

#include "voltas_ac_climate_ir.h"
#include "voltas_ir_frame.h"

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
            VoltasIRFrame frame;

            // Set the power state based on the current mode
            frame.set_power(this->mode != esphome::climate::CLIMATE_MODE_OFF);

            const uint8_t *payload = frame.payload();
            ESP_LOGD(TAG, "Transmitting IR Frame: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                     payload[0], payload[1], payload[2], payload[3], payload[4],
                     payload[5], payload[6], payload[7], payload[8], payload[9]);
        }

    } // namespace voltas_ac_climate_ir
} // namespace esphome
