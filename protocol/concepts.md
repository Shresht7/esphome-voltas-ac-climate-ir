# IR Protocol

This document defines general IR and Pronto Hex terminology used during this project. Voltas-specific captures, experiments, hypotheses, and conclusions are recorded in [`analysis.ipynb`](analysis.ipynb).

Infrared (IR) communication is a ubiquitous method for remote control of consumer electronics, including TVs, audio systems, and air conditioners. IR remotes work by emitting pulses of infrared light that encode information in the timing and duration of the pulses. The AC unit's IR receiver detects these pulses and decodes them to perform the corresponding actions.

## Infrared Light

Infrared radiation (light) sits just below the visible spectrum (below red) and is as such invisible to the human eye. While we cannot see Infrared light, many cameras (including smartphone cameras) can detect it, which is why you can see the IR LED on a remote when you look at it through a camera.

It is everywhere, anything that emits heat (including our bodies) emits IR radiation. This can be a problem for IR receivers, as they can pick up ambient IR noise from the environment. To mitigate this, IR remotes use a technique called modulation, where the IR signal is turned on and off at a specific frequency (the carrier frequency) to distinguish it from ambient IR noise.

## Infrared Modulation

IR Remotes blink the LED extremely fast (usually at 38kHz). This is called the **carrier frequency** and is used to distinguish IR signals from ambient light. The IR receiver is designed to filter out any signals that do not match the expected carrier frequency, allowing it to focus on the intended signals from the remote. (Usually between 30-60kHz, with 38kHz being the most common).

The data is encoded in the timing of the on/off bursts of the IR LED. The exact timings and encoding scheme can vary widely between different manufacturers and models.

In serial communication, we use the terminology of **marks** and **spaces** to describe these bursts. The 'space' is the default signal, which is the "off" state in the transmitter's case. No light is emitted during a space. During a 'mark', the transmitter actively pulses the IR LED at the carrier frequency.

On the receiver's end, a 'space' is usually represented as 'HIGH' (no signal detected) and a 'mark' is represented as 'LOW' (signal detected). However, these are not the actual `0`s and `1`s we want to transmit. Carrier modulation determines how the LED pulses during a mark; the protocol's data encoding determines how mark and space durations represent bits.

## Pronto Hex Codes

AC remotes are often **stateful**: they send the **entire state** (mode, temperature, fan, swing, etc.) rather than actions such as "temp up" or "temp down".

A common format for representing IR codes is Pronto Hex: a sequence of 4-character hexadecimal words encoding the signal timings. In the common raw format, the first four words form the header; the remaining words represent mark/space duration pairs.

- The first word is the format code. `0000` identifies the common raw Pronto format used by this project.
- The second word is a frequency divisor. For example, `006D` is hexadecimal `109`; `1 / (109 * 0.241246 us)` gives a carrier frequency of approximately 38 kHz.
- The third and fourth words declare the number of burst pairs in the introductory and repeat sequences, respectively.
- The remaining words are pairs of mark and space durations. Each duration is expressed as a count of the period derived from the frequency divisor.

## Bit Encoding (Pulse Distance Modulation)

Pulse distance modulation is a common IR encoding scheme. The duration of the space (the time between marks) determines whether a bit is a "0" or a "1". For example:
- A short space (e.g., 600µs) might represent a "0"
- A long space (e.g., 1600µs) might represent a "1"

## Checksum

Many IR protocols include a checksum or CRC to verify the integrity of the data. This can be a simple sum of the data bytes modulo 256 or a more complex algorithm. Its presence and algorithm must be established from captures for a specific protocol.

---

## 📕 References

- https://www.sbprojects.net/knowledge/ir/index.php
- https://www.sbprojects.net/knowledge/ir/nec.php
- https://www.remotecentral.com/features/irdisp1.htm
