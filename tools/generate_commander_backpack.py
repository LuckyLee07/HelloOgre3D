#!/usr/bin/env python3
"""Build the commander's small bone-mounted dorsal radio pack.

The existing soldier mesh and skeleton stay intact. Local +Z is the display
face; the factory turns it toward the follow camera. Dimensions are metres.
"""

from generate_relay_meshes import Mesh


def main():
    pack = Mesh()
    body = "Relay/CommanderPackBody"
    armor = "Relay/CommanderPackArmor"
    dark = "Relay/CommanderPackDark"
    display = "Relay/CommanderPackDisplay"

    # A shallow dark chassis sits clear of the existing back plate. Two armor
    # shoulders preserve the source soldier's white/graphite material rhythm.
    pack.box((0.47, 0.56, 0.17), (0.0, -0.04, 0.065), 0.045, dark)
    pack.box((0.40, 0.43, 0.045), (0.0, -0.015, 0.163), 0.014, body)
    for side in (-1, 1):
        x = side * 0.187
        pack.box((0.073, 0.44, 0.10), (x, -0.025, 0.145), 0.015, armor)
        pack.box((0.106, 0.10, 0.12), (side * 0.178, 0.217, 0.109), 0.012, armor)
    pack.box((0.35, 0.08, 0.10), (0.0, -0.290, 0.126), 0.012, armor)

    # The bright inset is large enough to read at the 1600x900 follow camera.
    # It uses a lit cyan material, not a dynamic scene light.
    pack.box((0.226, 0.284, 0.033), (0.0, 0.012, 0.207), 0.011, dark)
    pack.box((0.167, 0.220, 0.008), (0.0, 0.014, 0.228), 0.002, display)
    pack.box((0.195, 0.023, 0.016), (0.0, 0.157, 0.221), 0.003, armor)
    pack.box((0.195, 0.023, 0.016), (0.0, -0.130, 0.221), 0.003, armor)
    scale = 0.78
    pack.vertices = [tuple(value * scale for value in vertex[:3]) + vertex[3:]
                     for vertex in pack.vertices]
    print(pack.write("commander_backpack.mesh"))


if __name__ == "__main__":
    main()
