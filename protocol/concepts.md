# IR Protocol

Infrared (IR) communication is an ubiquitous method for remote control of consumer electronics, including TVs, audio systems, and air conditioners. IR remotes work by emitting pulses of infrared light that encode information in the timing and duration of the pulses. The AC unit's IR receiver detects these pulses and decodes them to perform the corresponding actions.

## Infrared Light

Infrared radiation (light) sits just below the visible spectrum (below red) and is as such invisible to the human eye. While we cannot see Infrared light, many cameras (including smartphone cameras) can detect it, which is why you can see the IR LED on a remote when you look at it through a camera.

It is everywhere, anything that emits heat (including our bodies) emits IR radiation. This can be a problem for IR receivers, as they can pick up ambient IR noise from the environment. To mitigate this, IR remotes use a technique called modulation, where the IR signal is turned on and off at a specific frequency (the carrier frequency) to distinguish it from ambient IR noise.

## Infrared Modulation

IR Remotes blink the LED extremely fast (usually at 38kHz). This is called the **carrier frequency** and is used to distinguish IR signals from ambient light. The IR receiver is designed to filter out any signals that do not match the expected carrier frequency, allowing it to focus on the intended signals from the remote. (Usually between 30-60kHz, with 38kHz being the most common).

The data is encoded in the timing of the on/off bursts of the IR LED. The exact timings and encoding scheme can vary widely between different manufacturers and models. 

In serial communication, we use the terminology of **marks** and **spaces** to describe these bursts. The 'space' is the default signal, which is the "off" state in the transmitter's case. No light is being emitted during a space. During a 'mark', the transmitter actively pulses the IR LED at the carrier frequency.

On the receiver's end, a 'space' is usually represented as 'HIGH' (no signal detected) and a 'mark' is represented as 'LOW' (signal detected). However, these are not the actual `0`s and `1`s we want to transmit. The real relationship between the marks and spaces and the bits they represent is determined by the modulation scheme used by the remote, which varies by manufacturer and model.

## Pronto Hex Codes

AC Remotes are **stateful**, the remote sends the **entire state** (mode, temp, fan, swing, etc) and not actions like "temp up" or "temp down".

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

---

## 📕 References

- https://www.sbprojects.net/knowledge/ir/index.php
- https://www.sbprojects.net/knowledge/ir/nec.php
