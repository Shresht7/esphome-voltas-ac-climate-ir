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

        // CONSTRUCTOR
        // -----------

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

        // POWER
        // -----

        bool VoltasIRFrame::get_power() const
        {
            return (frame_[2] & 0b10000000) != 0; // Check if the power bit (bit 7) in Byte 2 is set
        }

        void VoltasIRFrame::set_power(bool on)
        {
            frame_[2] = (frame_[2] & 0b01111111) | (on ? 0b10000000 : 0b00000000); // Set the power state bit in Byte 2
            update_checksum();                                                     // Recalculate the checksum after changing the power state
        }

        // TEMPERATURE
        // -----------

        uint8_t VoltasIRFrame::get_temperature() const
        {
            return frame_[3] & 0b00011111; // Extract the temperature value from Byte 3 (bits 4-0)
        }

        void VoltasIRFrame::set_temperature(uint8_t temperature)
        {
            frame_[3] = (frame_[3] & 0b11100000) | (temperature & 0b00011111); // Set the temperature value in Byte 3 (bits 4-0)
            update_checksum();                                                 // Recalculate the checksum after changing the temperature
        }

        // FAN SPEED
        // ---------

        uint8_t VoltasIRFrame::get_fan_speed() const
        {
            return (frame_[1] >> 5) & 0b00000111; // Extract the fan speed value from Byte 1 (bits 7-5)
        }

        void VoltasIRFrame::set_fan_speed(uint8_t fan_speed)
        {
            frame_[1] = (frame_[1] & 0b00011111) | ((fan_speed & 0b00000111) << 5); // Set the fan speed value in Byte 1 (bits 7-5)
            update_checksum();                                                      // Recalculate the checksum after changing the fan speed
        }

        // MODE
        // ----

        void VoltasIRFrame::set_mode(uint8_t mode)
        {
            frame_[1] = (frame_[1] & 0b11110000) | (mode & 0b00001111); // Set the mode value in Byte 1 (bits 3-0)
            update_checksum();                                          // Recalculate the checksum after changing the mode
        }

        uint8_t VoltasIRFrame::get_mode() const
        {
            return frame_[1] & 0b00001111; // Extract the mode value from Byte 1 (bits 3-0)
        }

        // SWING
        // -----

        bool VoltasIRFrame::get_vertical_swing() const
        {
            return (frame_[2] & 0b00000111) == 0b00000111; // Check if the vertical swing bits (bits 2-0) in Byte 2 are set to 111
        }

        void VoltasIRFrame::set_vertical_swing(bool on)
        {
            frame_[2] = (frame_[2] & 0b11111000) | (on ? 0b00000111 : 0b00000000); // Set the vertical swing bits in Byte 2 (bits 2-0)
            update_checksum();                                                     // Recalculate the checksum after changing the vertical swing state
        }

        bool VoltasIRFrame::get_horizontal_swing() const
        {
            return (frame_[8] & 0b00010000) != 0; // Check if the horizontal swing bit (bit 4) in Byte 8 is set
        }

        void VoltasIRFrame::set_horizontal_swing(bool on)
        {
            frame_[8] = (frame_[8] & 0b11101111) | (on ? 0b00010000 : 0b00000000); // Set the horizontal swing bit in Byte 8 (bit 4)
            update_checksum();                                                     // Recalculate the checksum after changing the horizontal swing state
        }

        // TURBO
        // -----

        bool VoltasIRFrame::get_turbo() const
        {
            return (frame_[2] & 0b00100000) != 0; // Check if the turbo bit (bit 5) in Byte 2 is set
        }

        void VoltasIRFrame::set_turbo(bool on)
        {
            frame_[2] = (frame_[2] & 0b11011111) | (on ? 0b00100000 : 0b00000000); // Set the turbo bit in Byte 2 (bit 5)
            update_checksum();                                                     // Recalculate the checksum after changing the turbo state
        }

        // CHECKSUM
        // --------

        uint8_t VoltasIRFrame::calculate_checksum(const uint8_t *bytes) const
        {
            uint8_t checksum = 0;
            for (uint8_t i = 0; i < 9; i++)
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
                for (uint8_t mask = 0b10000000; mask; mask >>= 1)
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

            constexpr uint32_t TOLERANCE_PERCENT = 25;                                              // Allowable tolerance for timing variations in percentage
            constexpr uint32_t MIN_SHORT = (100 - TOLERANCE_PERCENT) * DURATION_SPACE_SHORT / 100U; // Minimum duration for a short space
            constexpr uint32_t MAX_SHORT = (100 + TOLERANCE_PERCENT) * DURATION_SPACE_SHORT / 100U; // Maximum duration for a short space
            constexpr uint32_t MIN_LONG = (100 - TOLERANCE_PERCENT) * DURATION_SPACE_LONG / 100U;   // Minimum duration for a long space
            constexpr uint32_t MAX_LONG = (100 + TOLERANCE_PERCENT) * DURATION_SPACE_LONG / 100U;   // Maximum duration for a long space

            for (uint8_t b = 0; b < FRAME_BYTES; b++)
            {
                for (uint8_t mask = 0b10000000; mask; mask >>= 1)
                {
                    // Find the next mark, skipping past any leading noise
                    while (data->is_valid() && !data->peek_mark(DURATION_MARK))
                    {
                        data->advance(); // Skip invalid marks
                    }

                    if (!data->is_valid())
                    {
                        return false; // Invalid frame: not enough data to decode
                    }

                    // Consume the mark
                    data->advance();

                    // Sum the gap (absolute durations) until the next mark to determine the space duration
                    uint32_t space_duration = 0;
                    while (data->is_valid() && !data->peek_mark(DURATION_MARK))
                    {
                        space_duration += abs(data->peek()); // Accumulate the space duration
                        data->advance();                     // Move to the next timing
                    }

                    if (!data->is_valid())
                    {
                        return false; // Invalid frame: not enough data to decode
                    }

                    // Classify the space duration as a short or long space and set the corresponding bit in the byte
                    if (space_duration >= MIN_SHORT && space_duration <= MAX_SHORT)
                    {
                        // Short space (bit 0), do nothing as the bit is already 0
                    }
                    else if (space_duration >= MIN_LONG && space_duration <= MAX_LONG)
                    {
                        // Long space (bit 1), set the corresponding bit in the byte
                        bytes[b] |= mask;
                    }
                    else
                    {
                        return false; // Invalid frame: space duration does not match expected short or long space
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
