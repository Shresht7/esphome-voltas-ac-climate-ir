#include "voltas_ir_frame.h"

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
            update_checksum(); // Calculate and set the checksum for the initial frame
        }

        bool VoltasIRFrame::get_power() const
        {
            return (frame_[2] & 0b10000000) != 0; // Check if the power bit (bit 7) in Byte 2 is set
        }

        void VoltasIRFrame::set_power(bool on)
        {
            frame_[2] = (on ? 0b10000000 : 0b00000000); // Set the power state in Byte 2
            update_checksum();                          // Recalculate the checksum after changing the power state
        }

        uint8_t VoltasIRFrame::calculate_checksum(const uint8_t *bytes) const
        {
            uint8_t checksum = 0;
            for (uint8_t i = 0; i <= 8; i++)
            {
                checksum += bytes[i]; // Sum the first 9 bytes of the frame
            }
            // Get the one's complement of the sum
            return ~checksum;
        }

        uint8_t VoltasIRFrame::calculate_checksum() const
        {
            return calculate_checksum(frame_);
        }

        void VoltasIRFrame::update_checksum()
        {
            frame_[9] = calculate_checksum(); // Set the checksum in Byte 9
        }

        const uint8_t *VoltasIRFrame::payload() const
        {
            const_cast<VoltasIRFrame *>(this)->update_checksum(); // Ensure the checksum is up-to-date before returning the payload
            return frame_;                                        // Return the pointer to the frame array
        }

        void VoltasIRFrame::encode(remote_base::RemoteTransmitData *data) const
        {
            data->reset(); // Clear any existing data in the RemoteTransmitData object

            data->set_carrier_frequency(CARRIER_FREQUENCY); // Set the carrier frequency for the IR transmission
            data->reserve(FRAME_SIZE);                      // Reserve space for 81 mark/space pairs (80 bits + 1 footer)

            // Construct the 10-byte payload with the checksum computed
            const uint8_t *payload = this->payload();

            // Iterate over each byte in the payload and encode it into mark/space pairs
            for (uint8_t b = 0; b < FRAME_BYTES; b++)
            {
                // For each byte, iterate over its bits from MSB to LSB adding the corresponding mark and space durations to the data
                for (uint8_t mask = 0x80; mask; mask >>= 1)
                {
                    data->mark(DURATION_MARK); // Add the constant mark duration

                    if (payload[b] & mask)
                    {
                        data->space(DURATION_SPACE_LONG); // Add a long space for bit 1
                    }
                    else
                    {
                        data->space(DURATION_SPACE_SHORT); // Add a short space for bit 0
                    }
                }
            }

            // Footer: Final Mark and Trailing Space to end the frame
            data->item(DURATION_MARK, DURATION_FOOTER);
        }

        bool VoltasIRFrame::decode(remote_base::RemoteReceiveData *data)
        {
            uint8_t bytes[10] = {0}; // Temporary array to hold the decoded bytes

            for (uint8_t b = 0; b < FRAME_BYTES; b++)
            {
                for (uint8_t mask = 0x80; mask; mask >>= 1)
                {
                    if (data->expect_item(DURATION_MARK, DURATION_SPACE_LONG))
                    {
                        bytes[b] |= mask; // Set the corresponding bit in the byte if a long space is detected
                    }
                    else if (data->expect_item(DURATION_MARK, DURATION_SPACE_SHORT))
                    {
                        // Bit is 0; do nothing as the bit is already cleared
                    }
                    else
                    {
                        return false; // If neither a long nor short space is detected, decoding fails
                    }
                }
            }

            // Validate that the first byte is the expected constant (0b00110011)
            if (bytes[0] != 0b00110011)
            {
                return false; // Invalid frame: first byte does not match the expected constant
            }

            // Validate the checksum by comparing the calculated checksum with the received checksum in byte 9
            if (bytes[9] != calculate_checksum(bytes))
            {
                return false; // Invalid frame: checksum does not match
            }

            // If all validations pass, copy the decoded bytes into the frame
            for (uint8_t i = 0; i < FRAME_BYTES; i++)
            {
                frame_[i] = bytes[i];
            }
            return true; // Decoding successful
        }

    } // namespace voltas_ac_climate_ir

} // namespace esphome
