# IR Protocol

AC Remotes are **stateful**, the remote sends the **entire state** (mode, temp, fan, swing, etc) and not actions like "temp up" or "temp down".

## Infrared Modulation

IR Remotes blink the LED extremely fast (usually at 38kHz). This is called the **carrier frequency** and is used to distinguish IR signals from ambient light.

The data is encoded in the timing of the on/off bursts of the IR LED. For example, a "1" might be represented by a 600µs burst followed by a 600µs pause, while a "0" might be represented by a 600µs burst followed by a 1600µs pause. The exact timings and encoding scheme can vary widely between different manufacturers and models.

Terms:
- `mark`: IR LED is on (emitting light)
- `space`: IR LED is off (not emitting light)

## Pronto Hex Codes

A common format for representing IR codes is the Pronto Hex format (4-character hexadecimal numbers), which encodes the timings of the marks and spaces in a hexadecimal string. The first few values represent the carrier frequency and the number of marks and spaces, followed by pairs of values representing the duration of each mark and space.

- The first word is always `0000`.
- The second word is the frequency in kHz. For example, `006D` means 38kHz (1 / (0.241246 * 38) = 38kHz).
- The remaining words are pairs of Marks and Spaces measured in "ticks" (1 tick at 38kHz ≈ 26.3 microseconds).

## Bit Encoding (Pulse Distance Modulation)

In most AC remotes, the bits are encoded using pulse distance modulation, where the duration of the space (the time between marks) determines whether it's a "0" or a "1". For example:
- A short space (e.g., 600µs) might represent a "0"
- A long space (e.g., 1600µs) might represent a "1"

## Checksum

Many IR protocols include a checksum or CRC to verify the integrity of the data. This is often calculated as a simple sum of the data bytes modulo 256, or using more complex algorithms.

## Isolation Strategy

To deciper the protocol, we can use a diff-based approach:
1. Capture IR codes for different actions (e.g., power on, power off, temperature up, temperature down, fan speed change).
2. Convert the Pronto Hex codes to binary or a more human-readable format.
3. Compare the codes to identify which bits change for each action. For example, if only a few bits change when the temperature changes, those bits likely represent the temperature setting.
4. Use the observed patterns to reverse-engineer the meaning of each bit and how the state is encoded in the IR signal.
5. Validate the findings by sending custom IR codes and observing the behavior of the AC unit.
6. Document the protocol and implement it in the ESPHome component.
7. Profit?
