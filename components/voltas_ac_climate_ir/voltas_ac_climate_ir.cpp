#include <cmath>

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

        // TRANSMIT
        // --------

        void VoltasACClimateIR::transmit_state()
        {
            VoltasIRFrame frame;

            // Set the power state based on the current mode
            frame.set_power(this->mode != esphome::climate::CLIMATE_MODE_OFF);

            // Set the temperature based on the current target temperature
            uint8_t temperature = clamp_temperature(this->target_temperature);
            frame.set_temperature(temperature);

            // Construct the IR Payload
            const uint8_t *payload = frame.payload();

            // Log the payload for debugging
            ESP_LOGD(TAG, "Transmitting IR Frame: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                     payload[0], payload[1], payload[2], payload[3], payload[4],
                     payload[5], payload[6], payload[7], payload[8], payload[9]);

            // Transmit the IR Payload
            auto transmit = this->transmitter_->transmit();
            frame.encode(transmit.get_data());
            transmit.perform();
        }

        // RECEIVE
        // -------

        bool VoltasACClimateIR::on_receive(remote_base::RemoteReceiveData data)
        {
            VoltasIRFrame frame; // Create a new frame to decode the received data

            // Decode the received IR data into the frame
            if (!frame.decode(&data))
            {
                ESP_LOGD(TAG, "Failed to decode received IR frame.");
                return false; // Decoding failed, return false to indicate unsuccessful reception
            }

            ESP_LOGD(TAG, "Received IR Frame: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                     frame.payload()[0], frame.payload()[1], frame.payload()[2], frame.payload()[3], frame.payload()[4],
                     frame.payload()[5], frame.payload()[6], frame.payload()[7], frame.payload()[8], frame.payload()[9]);

            // Update the climate state based on the received frame
            this->mode = frame.get_power() ? esphome::climate::CLIMATE_MODE_COOL : esphome::climate::CLIMATE_MODE_OFF;

            // Update the target temperature based on the received frame
            const uint8_t received_temperature = frame.get_temperature();
            if (received_temperature < MIN_TEMPERATURE || received_temperature > MAX_TEMPERATURE)
            {
                ESP_LOGD(TAG, "Received temperature %d is out of range (%f - %f)", received_temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
                return false; // Temperature out of range, return false to indicate unsuccessful reception
            }
            this->target_temperature = static_cast<float>(received_temperature);

            this->publish_state(); // Publish the updated state to Home Assistant
            return true;           // Indicate successful reception
        }

        // HELPERS
        // -------

        uint8_t VoltasACClimateIR::clamp_temperature(float temperature)
        {
            // Clamp the temperature to the valid range and round to the nearest step
            if (temperature < MIN_TEMPERATURE)
                return static_cast<uint8_t>(MIN_TEMPERATURE);
            if (temperature > MAX_TEMPERATURE)
                return static_cast<uint8_t>(MAX_TEMPERATURE);
            return static_cast<uint8_t>(round(temperature / TEMPERATURE_STEP) * TEMPERATURE_STEP);
        }

    } // namespace voltas_ac_climate_ir
} // namespace esphome
