#include "voltas_ir_frame.h"

static constexpr uint32_t CARRIER_FREQUENCY = 38000;            // Carrier Frequency in Hz (38 kHz)
static constexpr double TIMEBASE = 1000000 / CARRIER_FREQUENCY; // Timebase in microseconds (µs)

static constexpr uint32_t PRONTO_MARK = 40;        // Pronto mark duration in ticks
static constexpr uint32_t PRONTO_SPACE_SHORT = 22; // Pronto short space duration in ticks
static constexpr uint32_t PRONTO_SPACE_LONG = 99;  // Pronto long space duration in ticks
static constexpr uint32_t PRONTO_FOOTER = 0x180;   // Pronto footer duration in ticks

static constexpr uint32_t DURATION_MARK = PRONTO_MARK * TIMEBASE;               // Duration of the mark in microseconds (µs)
static constexpr uint32_t DURATION_SPACE_SHORT = PRONTO_SPACE_SHORT * TIMEBASE; // Duration of the short space in microseconds (µs)
static constexpr uint32_t DURATION_SPACE_LONG = PRONTO_SPACE_LONG * TIMEBASE;   // Duration of the long space in microseconds (µs)
static constexpr uint32_t DURATION_FOOTER = PRONTO_FOOTER * TIMEBASE;           // Duration of the footer (trailing space) in microseconds (µs)

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        VoltasIRFrame::VoltasIRFrame() : frame_{
                                             0b00110011, // Byte 0: Constant. Unknown Purpose. Possibly Identifier or Signature?
                                             0b00101000, // Byte 1: Encodes the Fan Mode (bits 7-5) and Mode (bits 3-0). Default Fan: High, Mode: Cool
                                             0b00000000, // Byte 2: Encodes the Power State (bit 7), Turbo Mode (bit 5) and Vertical Swing (bits 2-0). Default: Off
                                             0b00011000, // Byte 3: Encodes the Temperature in degree Celcius (bits 4-0). Default: 24°C
                                             0b00111011, // Byte 4: Unknown Purpose. Possibly Timer/Sleep/Other?
                                             0b00111011, // Byte 5: Unknown Purpose. Possibly Timer/Sleep/Other?
                                             0b00111011, // Byte 6: Unknown Purpose. Possibly Timer/Sleep/Other?
                                             0b00010001, // Byte 7: Unknown Purpose. Possibly Timer/Sleep/Other?
                                             0b00000000, // Byte 8: Lamp/Light (bit 5), Horizontal Swing (bit 4). Default: Off
                                             0b00000000} // Byte 9: Checksum. Will be calculated and set in the constructor
        {
            // Initialize the frame with default values
            checksum(); // Calculate and set the checksum for the initial frame
        }

        void VoltasIRFrame::set_power(bool on)
        {
            frame_[2] = (on ? 0b10000000 : 0b00000000); // Set the power state in Byte 2
            checksum();                                 // Recalculate the checksum after changing the power state
        }

        void VoltasIRFrame::checksum()
        {
            uint8_t checksum = 0;
            for (uint8_t i = 0; i <= 8; i++)
            {
                checksum += frame_[i]; // Sum the first 9 bytes of the frame
            }
            // Get the one's complement of the sum
            checksum = ~checksum;
            frame_[9] = checksum; // Set the checksum in Byte 9
        }

        const uint8_t *VoltasIRFrame::payload() const
        {
            const_cast<VoltasIRFrame *>(this)->checksum(); // Ensure the checksum is up-to-date before returning the payload
            return frame_;                                 // Return the pointer to the frame array
        }

    } // namespace voltas_ac_climate_ir

} // namespace esphome
