#!/usr/bin/env python3
"""Build Sandbox19's vertical relay switchgear cabinet."""

from generate_relay_meshes import Mesh


def main():
	mesh = Mesh()
	# A narrow vertical service cabinet replaces one repeated tall supply case.
	# All parts remain inside a closed solid envelope for the Bullet convex hull.
	mesh.box((1.42, 1.62, 0.78), (0, 0, 0), 0.060, "Relay/GeneratorBody")
	mesh.box((1.56, 0.14, 0.88), (0, -0.80, 0), 0.025, "Relay/GeneratorFrame")
	mesh.box((1.54, 0.15, 0.86), (0, 0.80, 0), 0.025, "Relay/GeneratorCap")
	for x in (-0.68, 0.68):
		mesh.box((0.12, 1.54, 0.84), (x, -0.01, 0), 0.022, "Relay/GeneratorFrame")

	# Twin front doors, cooling slots and a compact status cluster make the unit
	# read as switchgear rather than another storage case.
	mesh.box((1.08, 1.30, 0.045), (0, -0.02, -0.405), 0.012, "Relay/GeneratorVent")
	mesh.box((0.055, 1.18, 0.070), (0, -0.03, -0.445), 0.010, "Relay/GeneratorFrame")
	for y in (-0.48, -0.34, 0.30, 0.44):
		mesh.box((0.38, 0.050, 0.075), (0.30, y, -0.448), 0.009, "Relay/GeneratorFrame")
	mesh.box((0.28, 0.20, 0.028), (-0.28, 0.38, -0.452), 0.006, "Relay/GeneratorMark")
	for y in (0.03, -0.08, -0.19):
		mesh.box((0.22, 0.035, 0.028), (-0.28, y, -0.452), 0.005, "Relay/GeneratorCap")
	mesh.box((0.30, 0.055, 0.030), (0, 0.66, -0.452), 0.007, "Relay/CrateMark")

	print(mesh.write("relay_switchgear.mesh"))


if __name__ == "__main__":
	main()
