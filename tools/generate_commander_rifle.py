#!/usr/bin/env python3
"""Build the small, static commander rifle shell used over the existing animated gun.

The original weapon skeleton remains the muzzle and reload authority. This
mesh improves the visible receiver/stock silhouette at Sandbox19 camera scale.
"""

from generate_relay_meshes import Mesh


def main():
    rifle = Mesh()
    armor = "Relay/CommanderRifleArmor"
    metal = "Relay/CommanderRifleMetal"
    dark = "Relay/CommanderRifleDark"
    mark = "Relay/CommanderRifleMark"

    # Local +Z follows the authored gun barrel. Keep the muzzle section of the
    # animated source mesh exposed, so shots still appear at the real muzzle.
    rifle.box((0.135, 0.108, 0.37), (0.012, 0.033, 0.108), 0.018, armor)
    rifle.box((0.118, 0.090, 0.215), (0.013, 0.009, -0.170), 0.015, dark)
    rifle.box((0.148, 0.035, 0.085), (0.013, 0.022, -0.258), 0.008, metal)
    rifle.box((0.125, 0.065, 0.190), (0.011, 0.039, 0.369), 0.013, metal)
    rifle.box((0.083, 0.030, 0.370), (0.013, 0.106, 0.095), 0.007, dark)
    rifle.box((0.072, 0.061, 0.155), (0.013, 0.161, 0.096), 0.008, metal)
    rifle.box((0.085, 0.123, 0.070), (0.013, -0.105, 0.010), 0.012, dark)
    rifle.box((0.087, 0.143, 0.088), (0.013, -0.115, 0.157), 0.011, metal)
    for x in (-0.064, 0.088):
        rifle.box((0.012, 0.030, 0.165), (x, 0.070, 0.096), 0.004, mark)
        rifle.box((0.012, 0.038, 0.098), (x, 0.012, 0.327), 0.004, dark)
    print(rifle.write("commander_rifle_shell.mesh"))


if __name__ == "__main__":
    main()
