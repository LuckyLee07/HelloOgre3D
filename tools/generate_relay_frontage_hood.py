#!/usr/bin/env python3
"""Build the two solid relay-side frontage hoods used by Sandbox19."""

from generate_relay_meshes import Mesh


def main():
	mesh = Mesh()
	# A shallow concrete crown projects forward from the side volume. Its dark
	# underside and front metal lip create depth without entering the walkway.
	mesh.box((5.50, 0.24, 1.12), (0, 0.09, 0), 0.045, "Relay/ConcreteShade", 0.35)
	mesh.box((5.40, 0.10, 1.13), (0, -0.08, -0.02), 0.025, "Relay/Trim")
	mesh.box((5.60, 0.17, 0.17), (0, -0.10, -0.57), 0.025, "Relay/EquipmentBox")
	for x in (-2.67, 2.67):
		mesh.box((0.18, 0.35, 1.13), (x, 0.12, -0.02), 0.025, "Relay/Trim")
	print(mesh.write("relay_frontage_hood.mesh"))


if __name__ == "__main__":
	main()
