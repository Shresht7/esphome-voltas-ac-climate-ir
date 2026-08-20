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
            // Array to hold the 10 bytes of the IR frame
            uint8_t frame_[10];

            // Method to calculate and set the checksum for the frame
            void checksum();

        public:
            // Constructor to initialize the frame with default values
            VoltasIRFrame();

            // Method to set the power state (on/off)
            void set_power(bool on);

            // Returns the IR Frame Payload (10 bytes - with the checksum computed)
            const uint8_t *payload() const;

            // Encodes the frame into IR signal timings (in microseconds) with PDM (constant mark + short/long space per bit) for transmission
            void encode(remote_base::RemoteTransmitData *data) const;
        };

    } // namespace voltas_ac_climate_ir

} // namespace esphome
