#pragma once

#include <cstdint>
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        class VoltasIRFrame
        {

        private:
            // IR FRAME
            // --------

            // Array to hold the 10 bytes of the IR frame
            uint8_t frame_[10];

            // CHECKSUM
            // --------

            // Method to calculate the checksum for the frame
            uint8_t calculate_checksum() const;
            uint8_t calculate_checksum(const uint8_t *bytes) const;

            // Method to update the checksum in the frame
            void update_checksum();

        public:
            // Constructor to initialize the frame with default values
            VoltasIRFrame();

            // POWER
            // -----

            // Method to set the power state (on/off)
            void set_power(bool on);

            // Returns the current power state (on/off) from the frame
            bool get_power() const;

            // TEMPERATURE
            // -----------

            // Method to set the temperature  (in Celsius)
            void set_temperature(uint8_t temperature);

            // Returns the current temperature (in Celsius) from the frame
            uint8_t get_temperature() const;

            // FAN
            // ---

            // Method to set the fan speed
            void set_fan_speed(uint8_t fan_speed);

            // Returns the current fan speed from the frame
            uint8_t get_fan_speed() const;

            // MODE
            // ----

            // Method to set the mode (e.g., COOL, HEAT, FAN_ONLY)
            void set_mode(uint8_t mode);

            // Returns the current mode from the frame
            uint8_t get_mode() const;

            // PAYLOAD ENCODER / DECODER
            // -------------------------

            // Returns the IR Frame Payload (10 bytes - with the checksum computed)
            const uint8_t *payload() const;

            // Encodes the frame into IR signal timings (in microseconds) with PDM (constant mark + short/long space per bit) for transmission
            void encode(remote_base::RemoteTransmitData *data) const;

            // Decodes the received IR into the frame, validating the checksum and returning true if successful
            bool decode(remote_base::RemoteReceiveData *data);
        };

    } // namespace voltas_ac_climate_ir

} // namespace esphome
