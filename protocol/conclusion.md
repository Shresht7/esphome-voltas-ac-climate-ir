# IR Protocol Conclusion

Summary of the reverse-engineered Voltas AC IR protocol. The full discovery process, experiments, and validation are recorded in [`analysis.ipynb`](analysis.ipynb); general IR and Pronto terminology is defined in [`concepts.md`](concepts.md).

---

## Physical Layer

Frames are transmitted on a **38 kHz carrier** (`006D` frequency code) using **Pulse Distance Modulation (PDM)**: a constant mark separates each space, and the space length carries the data.

- Short space → bit `0`
- Long space → bit `1`

## Frame Structure

Each frame is **166 Pronto words**:

- 4-word header: `0000 006D 0051 0000`
- 81 mark/space burst pairs (the `0051` declaration)
- 80 payload spaces decode to **ten bytes**

## Byte Map

Bits are numbered 7 (MSB) to 0 (LSB). Derived from all 22 captures.

| Byte | Bits | Value(s) observed                               | Meaning                                            |
| ---- | ---- | ----------------------------------------------- | -------------------------------------------------- |
| 0    | all  | `00110011` (`0x33`)                             | Constant; role not established                     |
| 1    | 7–5  | `100` Low, `010` Medium, `001` High, `111` Auto | Fan speed (one-hot per speed; Auto sets all three) |
| 1    | 3–0  | `1000` Cool, `0100` Dry, `0010` Sun, `0001` Fan | Mode (one-hot; bit 4 not used)                     |
| 2    | 7    | `1` ON, `0` OFF                                 | Power                                              |
| 2    | 5    | `1` = Turbo                                     | Turbo                                              |
| 2    | 2–0  | `111` = vertical swing                          | Vertical swing (bits 0–2)                          |
| 3    | 4–0  | `00010000`–`00011100` (`0x10`–`0x1C`)           | Set temperature, 16–28 °C direct value             |
| 3    | 6    | `1` = EcoSaver                                  | EcoSaver (adds `0x40`)                             |
| 4–6  | all  | `00111011` (`0x3B`)                             | Constant; role not established                     |
| 7    | all  | `00010001` (`0x11`)                             | Constant; role not established                     |
| 8    | 5    | `1` = lamp                                      | Lamp                                               |
| 8    | 4    | `1` = horizontal swing                          | Horizontal swing                                   |
| 9    | —    | one's-complement of sum                         | Checksum (see below)                               |

### Byte 1 detail

Fan and mode are independent fields in the same byte:

| Capture        | Byte 1 | Fan bits (7–5) | Mode bits (3–0) |
| -------------- | ------ | -------------- | --------------- |
| `FAN_16C_High` | `0x28` | `001` High     | `1000` Cool     |
| `FAN_16C_Med`  | `0x48` | `010` Medium   | `1000` Cool     |
| `FAN_16C_Low`  | `0x88` | `100` Low      | `1000` Cool     |
| `FAN_16C_Auto` | `0xE8` | `111` Auto     | `1000` Cool     |
| `MODE_CH_21`   | `0x28` | `001` High     | `1000` Cool     |
| `MODE_DL_24`   | `0x84` | `100` Low      | `0100` Dry      |
| `MODE_SA_23`   | `0xE2` | `111` Auto     | `0010` Sun      |
| `MODE_FH_00`   | `0x21` | `001` High     | `0001` Fan      |

The mode captures confirm that modes which change temperature or fan speed (e.g. Dry defaults to a Low fan, Sun to Auto) simply update those bytes.

## Checksum

Byte 9 is a **one's-complement checksum**:

```
byte 9 = 255 − (sum of bytes 0–8, mod 256)
```

Equivalently, the ten payload bytes always sum to **511** (0x1FF). Verified against all 22 captures.

It is a **weak integrity check**: it detects accidental corruption but not errors that preserve the byte sum. Synthesized frames must recompute byte 9 after setting the other nine bytes.

## Encoding Model

The remote is **stateful**: it transmits the entire resulting state (mode, temperature, fan, features) rather than a button action. Separate captures of identical states produced identical frames.

---

## Open Items

- Timer, sleep, light intensity, and other remote functions were not captured.
- Roles of the constant bytes 0 (`0x33`) and 4–7 (`0x3B 0x3B 0x3B 0x11`) are not established.
