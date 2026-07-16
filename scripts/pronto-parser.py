"""
This script is a parser for the Pronto Hex format, which is commonly used for infrared (IR) remote control signals.
The Pronto Hex format represents IR signals as a series of hexadecimal values, which can be used to reproduce the original signal.
"""

# /// script
# requires-python = ">=3.12"
# dependencies = [
#     "defcmd"
# ]
# ///

from defcmd import cmd

def parse_pronto(code: str) -> dict:
    """
    Parses a Pronto Hex code string and returns a dictionary containing the frequency, number of burst pairs, and the list of mark/space pairs.
    """

    # Remove any whitespace and split the code into words
    words = code.strip().split()

    # words[0]: format code (should be 0000 for Pronto Hex)
    # words[1]: frequency (in kHz)
    # words[2]: number of burst pairs in the first sequence
    # words[3]: number of repeat pairs
    # words[4:]: Alternating mark/space in ticks

    # Extract the format code and ensure it is valid
    format_code = int(words[0], 16)

    # Ensure the format code is correct
    if words[0] != "0000":
        raise ValueError("Invalid Pronto Hex format code. Expected '0000'.")

    # Extract the frequency
    frequency_code = int(words[1], 16)              
    frequency = 1000 / (frequency_code * 0.241246)  # Convert to kHz using standard prontol hardware timebase in microseconds/tick.

    # Extract the number of burst pairs
    num_burst_pairs = int(words[2], 16)

    # Extract the number of repeat pairs
    repeat_pairs = int(words[3], 16)

    pairs = []
    for i in range(num_burst_pairs):
        idx = 4 + i * 2
        if idx + 1 >= len(words):
            raise ValueError("Not enough data for the specified number of burst pairs.")

        mark = int(words[idx], 16)
        space = int(words[idx + 1], 16)
        pairs.append((mark, space))

    return {
        "format_code": format_code,
        "frequency_code": frequency_code,
        "frequency": frequency,
        "num_burst_pairs": num_burst_pairs,
        "repeat_pairs": repeat_pairs,
        "pairs": pairs,
        "actual_pairs_count": len(pairs)
    }


@cmd
def parse(hex_code: str):
    """Parse a Pronto hex string and display its structure."""
    result = parse_pronto(hex_code)
    display(result, show_all_pairs=True)


def display(result, show_all_pairs: bool = False):
    """Pretty-print parsed result"""
    timebase = result['frequency_code'] * 0.241246  # in microseconds/tick
    total_ms = sum(m + s for m, s in result['pairs']) * timebase / 1000
    
    print(f"  Format:           0x{result['format_code']:04X}")
    print(f"  Freq code:        0x{result['frequency_code']:04X}")
    print(f"  Frequency:        {result['frequency']:.1f} kHz")
    print(f"  Timebase:         {timebase} µs/tick")
    print(f"  Burst pairs:      {result['num_burst_pairs']} (declared) / {result['actual_pairs_count']} (actual)")
    print(f"  Repeat pairs:     {result['repeat_pairs']}")
    print(f"  Total duration:   {total_ms:.0f} ms")
    
    if show_all_pairs:
        print("\n\nPairs:")
        print(f"{'Mark':>5} {'Space':>5}")
        for mark, space in result['pairs']:
            print(f"{mark:>5} {space:>5}")

# ----
# MAIN
# ----

if __name__ == "__main__":
    parse.run()
