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
            auto traits = esphome::climate_ir::ClimateIR::traits();

            // Explicitly tell Home Assistant which modes are supported by this climate controller
            traits.set_supported_modes(SUPPORTED_MODES);

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

            // Set the fan speed based on the current fan mode (defaulting to AUTO if not set)
            uint8_t fan_speed = get_fan_speed_from_mode(this->fan_mode.value_or(esphome::climate::CLIMATE_FAN_AUTO));
            frame.set_fan_speed(fan_speed);

            // Set the mode based on the current climate mode (only if not OFF)
            if (this->mode != esphome::climate::CLIMATE_MODE_OFF)
            {
                uint8_t climate_mode = get_bits_from_climate_mode(this->mode);
                frame.set_mode(climate_mode);
            }

            // Set the vertical and horizontal swing states based on the current swing mode
            const SwingBits swing_bits = get_swing_bits(this->swing_mode);
            frame.set_vertical_swing(swing_bits.vertical);
            frame.set_horizontal_swing(swing_bits.horizontal);

            // Set the turbo and eco-saver states based on the current preset
            const PresetBits preset = get_preset_bits(this->preset.value_or(esphome::climate::CLIMATE_PRESET_NONE));
            frame.set_turbo(preset.turbo);
            frame.set_eco_saver(preset.eco_saver);

            // Set the lamp state (preserved across transmissions; toggled via the physical remote)
            frame.set_lamp(this->lamp_state_);

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

            // Validate the target temperature before mutating any state, so a rejected frame leaves no partial updates behind
            const uint8_t received_temperature = frame.get_temperature();
            if (received_temperature < MIN_TEMPERATURE || received_temperature > MAX_TEMPERATURE)
            {
                ESP_LOGD(TAG, "Received temperature %d is out of range (%f - %f)", received_temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
                return false; // Temperature out of range, return false to indicate unsuccessful reception
            }

            // Update the climate state based on the received frame
            if (!frame.get_power())
            {
                this->mode = esphome::climate::CLIMATE_MODE_OFF; // If power is off, set mode to OFF
            }
            else
            {
                // If power is on, update the mode based on the received frame
                uint8_t received_mode = frame.get_mode();
                this->mode = get_climate_mode_from_bits(received_mode);
            }

            // Update the target temperature based on the received frame
            this->target_temperature = static_cast<float>(received_temperature);

            // Update the fan mode based on the received frame (defaulting to AUTO if the received fan speed is unsupported)
            const uint8_t received_fan_speed = frame.get_fan_speed();
            this->fan_mode = get_fan_mode_from_speed(received_fan_speed);

            // Update the swing mode based on the received frame
            const bool received_vertical_swing = frame.get_vertical_swing();
            const bool received_horizontal_swing = frame.get_horizontal_swing();
            this->swing_mode = get_swing_mode_from_bits(received_vertical_swing, received_horizontal_swing);

            // Update the preset based on the received frame
            const bool received_turbo = frame.get_turbo();
            const bool received_eco_saver = frame.get_eco_saver();
            this->preset = get_preset_from_bits(received_turbo, received_eco_saver);

            // Store the lamp state so subsequent transmissions preserve it
            this->lamp_state_ = frame.get_lamp();

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

        uint8_t VoltasACClimateIR::get_fan_speed_from_mode(esphome::climate::ClimateFanMode fan_mode)
        {
            switch (fan_mode)
            {
            case esphome::climate::CLIMATE_FAN_LOW:
                return 0b100; // Low fan speed
            case esphome::climate::CLIMATE_FAN_MEDIUM:
                return 0b010; // Medium fan speed
            case esphome::climate::CLIMATE_FAN_HIGH:
                return 0b001; // High fan speed
            case esphome::climate::CLIMATE_FAN_AUTO:
                return 0b111; // Auto fan speed
            default:
                ESP_LOGW(TAG, "Unsupported fan mode: %d", static_cast<int>(fan_mode));
                return 0b111; // Default to Auto if unsupported
            }
        }

        esphome::climate::ClimateFanMode VoltasACClimateIR::get_fan_mode_from_speed(uint8_t fan_speed)
        {
            switch (fan_speed)
            {
            case 0b100:
                return esphome::climate::CLIMATE_FAN_LOW;
            case 0b010:
                return esphome::climate::CLIMATE_FAN_MEDIUM;
            case 0b001:
                return esphome::climate::CLIMATE_FAN_HIGH;
            case 0b111:
                return esphome::climate::CLIMATE_FAN_AUTO;
            default:
                ESP_LOGW(TAG, "Unsupported fan speed: %d", fan_speed);
                return esphome::climate::CLIMATE_FAN_AUTO; // Default to Auto if unsupported
            }
        }

        uint8_t VoltasACClimateIR::get_bits_from_climate_mode(esphome::climate::ClimateMode mode)
        {
            switch (mode)
            {
            case esphome::climate::CLIMATE_MODE_OFF:
                return 0b0000;
            case esphome::climate::CLIMATE_MODE_COOL:
                return 0b1000;
            case esphome::climate::CLIMATE_MODE_DRY:
                return 0b0100;
            case esphome::climate::CLIMATE_MODE_HEAT:
                return 0b0010;
            case esphome::climate::CLIMATE_MODE_FAN_ONLY:
                return 0b0001;
            default:
                ESP_LOGW(TAG, "Unsupported climate mode: %d", static_cast<int>(mode));
                return 0b1000; // Default to Cool if unsupported
            }
        }

        esphome::climate::ClimateMode VoltasACClimateIR::get_climate_mode_from_bits(uint8_t mode)
        {
            switch (mode)
            {
            case 0b0000:
                return esphome::climate::CLIMATE_MODE_OFF;
            case 0b1000:
                return esphome::climate::CLIMATE_MODE_COOL;
            case 0b0100:
                return esphome::climate::CLIMATE_MODE_DRY;
            case 0b0010:
                return esphome::climate::CLIMATE_MODE_HEAT;
            case 0b0001:
                return esphome::climate::CLIMATE_MODE_FAN_ONLY;
            default:
                ESP_LOGW(TAG, "Unsupported mode: %d", mode);
                return esphome::climate::CLIMATE_MODE_COOL; // Default to Cool if unsupported
            }
        }

        SwingBits VoltasACClimateIR::get_swing_bits(esphome::climate::ClimateSwingMode swing_mode)
        {
            switch (swing_mode)
            {
            case esphome::climate::CLIMATE_SWING_BOTH:
                return {true, true}; // Both swings are enabled
            case esphome::climate::CLIMATE_SWING_VERTICAL:
                return {true, false}; // Only vertical swing is enabled
            case esphome::climate::CLIMATE_SWING_HORIZONTAL:
                return {false, true}; // Only horizontal swing is enabled
            case esphome::climate::CLIMATE_SWING_OFF:
                return {false, false}; // Both swings are disabled
            default:
                ESP_LOGW(TAG, "Unsupported swing mode: %d", static_cast<int>(swing_mode));
                return {false, false}; // Default to disabled if unsupported
            }
        }

        esphome::climate::ClimateSwingMode VoltasACClimateIR::get_swing_mode_from_bits(bool vertical_swing, bool horizontal_swing)
        {
            if (vertical_swing && horizontal_swing)
                return esphome::climate::CLIMATE_SWING_BOTH;
            else if (vertical_swing)
                return esphome::climate::CLIMATE_SWING_VERTICAL;
            else if (horizontal_swing)
                return esphome::climate::CLIMATE_SWING_HORIZONTAL;
            else
                return esphome::climate::CLIMATE_SWING_OFF;
        }

        PresetBits VoltasACClimateIR::get_preset_bits(esphome::climate::ClimatePreset preset)
        {
            switch (preset)
            {
            case esphome::climate::CLIMATE_PRESET_BOOST:
                return {true, false}; // Turbo is enabled, eco-saver is disabled
            case esphome::climate::CLIMATE_PRESET_ECO:
                return {false, true}; // Eco-saver is enabled, turbo is disabled
            case esphome::climate::CLIMATE_PRESET_NONE:
                return {false, false}; // Turbo and eco-saver are disabled
            default:
                ESP_LOGW(TAG, "Unsupported preset: %d", static_cast<int>(preset));
                return {false, false}; // Default to disabled if unsupported
            }
        }

        esphome::climate::ClimatePreset VoltasACClimateIR::get_preset_from_bits(bool turbo, bool eco_saver)
        {
            if (turbo)
                return esphome::climate::CLIMATE_PRESET_BOOST; // Turbo takes precedence over Eco-saver
            else if (eco_saver)
                return esphome::climate::CLIMATE_PRESET_ECO;
            else
                return esphome::climate::CLIMATE_PRESET_NONE;
        }

    } // namespace voltas_ac_climate_ir
} // namespace esphome
