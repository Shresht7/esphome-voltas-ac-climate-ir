#pragma once

#include <cstdint>
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        // PROTOCOL CONSTANTS
        // ------------------

        static constexpr uint32_t CARRIER_FREQUENCY = 38000;              // Carrier Frequency in Hz (38 kHz)
        static constexpr double TIMEBASE = 1000000.0 / CARRIER_FREQUENCY; // Timebase in microseconds (µs)

        static constexpr uint32_t PRONTO_MARK = 40;        // Pronto mark duration in ticks
        static constexpr uint32_t PRONTO_SPACE_SHORT = 22; // Pronto short space duration in ticks
        static constexpr uint32_t PRONTO_SPACE_LONG = 99;  // Pronto long space duration in ticks
        static constexpr uint32_t PRONTO_FOOTER = 0x180;   // Pronto footer duration in ticks

        static constexpr uint32_t DURATION_MARK = PRONTO_MARK * TIMEBASE;               // Duration of the mark in microseconds (µs)
        static constexpr uint32_t DURATION_SPACE_SHORT = PRONTO_SPACE_SHORT * TIMEBASE; // Duration of the short space in microseconds (µs)
        static constexpr uint32_t DURATION_SPACE_LONG = PRONTO_SPACE_LONG * TIMEBASE;   // Duration of the long space in microseconds (µs)
        static constexpr uint32_t DURATION_FOOTER = PRONTO_FOOTER * TIMEBASE;           // Duration of the footer (trailing space) in microseconds (µs)

        static constexpr uint8_t FRAME_BYTES = 10;                             // Number of bytes in the IR Payload Frame
        static constexpr uint16_t FRAME_BITS = FRAME_BYTES * 8;                // Number of bits in the IR Payload Frame
        static constexpr uint8_t FOOTER_SIZE = 1;                              // Footer size in bytes (mark + trailing space)
        static constexpr uint16_t FRAME_SIZE = 2 * (FRAME_BITS + FOOTER_SIZE); // Total number of mark/space pairs in the frame (2(80 + 1) = 2 * 81 = 162)

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

            // SWING
            // -----

            // Method to set the vertical swing state
            void set_vertical_swing(bool on);

            // Returns the current vertical swing state from the frame
            bool get_vertical_swing() const;

            // Method to set the horizontal swing state
            void set_horizontal_swing(bool on);

            // Returns the current horizontal swing state from the frame
            bool get_horizontal_swing() const;

            // TURBO
            // -----

            // Method to set the turbo mode state
            void set_turbo(bool on);

            // Returns the current turbo mode state from the frame
            bool get_turbo() const;

            // ECO-SAVER
            // ---------

            // Method to set the eco-saver mode state
            void set_eco_saver(bool on);

            // Returns the current eco-saver mode state from the frame
            bool get_eco_saver() const;

            // LAMP
            // ----

            // Method to set the lamp mode state
            void set_lamp(bool on);

            // Returns the current lamp mode state from the frame
            bool get_lamp() const;

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
