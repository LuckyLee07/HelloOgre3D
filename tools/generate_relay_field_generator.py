#!/usr/bin/env python3
"""Build Sandbox19's low field generator for the near approach."""

from generate_relay_meshes import Mesh


def main():
	mesh = Mesh()
	# A horizontal silhouette replaces one repeated tall supply case. The unit is
	# closed and solid so its authored vertices can also define the Bullet hull.
	mesh.box((1.86, 0.82, 0.92), (0, -0.04, 0), 0.065, "Relay/GeneratorBody")
	mesh.box((2.10, 0.16, 1.02), (0, -0.53, 0), 0.025, "Relay/GeneratorFrame")
	for x in (-0.97, 0.97):
		mesh.box((0.15, 1.02, 1.08), (x, -0.02, 0), 0.025, "Relay/GeneratorFrame")
	mesh.box((1.28, 0.14, 0.72), (0, 0.44, 0), 0.025, "Relay/GeneratorCap")

	# Recessed intake, horizontal vanes and a small status panel face the player.
	mesh.box((1.04, 0.54, 0.05), (-0.24, -0.04, -0.48), 0.012, "Relay/GeneratorVent")
	for y in (-0.22, -0.08, 0.06, 0.20):
		mesh.box((0.90, 0.055, 0.075), (-0.24, y, -0.525), 0.010, "Relay/GeneratorFrame")
	mesh.box((0.42, 0.54, 0.05), (0.57, -0.04, -0.48), 0.012, "Relay/GeneratorFrame")
	mesh.box((0.20, 0.13, 0.025), (0.57, 0.08, -0.522), 0.006, "Relay/GeneratorMark")
	for y in (-0.13, -0.22):
		mesh.box((0.16, 0.035, 0.025), (0.57, y, -0.522), 0.005, "Relay/GeneratorCap")

	print(mesh.write("relay_field_generator.mesh"))


if __name__ == "__main__":
	main()
