#!/usr/bin/env python3
"""Build the solid louvered service bay for Sandbox19's outer relay wing."""

from generate_relay_meshes import Mesh


def main():
	mesh = Mesh()
	# The wing wall behind this assembly is already solid. The shallow panel,
	# raised rim and slats form one convex-colliding service fixture.
	mesh.box((2.72, 2.26, 0.16), (0, 0, 0), 0.045, "Relay/Trim")
	mesh.box((2.40, 1.96, 0.045), (0, -0.02, -0.105), 0.015, "Relay/CrateRubber")
	mesh.box((2.65, 0.17, 0.23), (0, 1.08, -0.015), 0.025, "Relay/ConcreteShade")
	mesh.box((2.47, 0.10, 0.15), (0, -1.02, -0.09), 0.015, "Relay/EquipmentBox")
	for y in (-0.73, -0.49, -0.25, -0.01, 0.23, 0.47, 0.71):
		mesh.box((1.75, 0.115, 0.15), (-0.22, y, -0.21), 0.018, "Relay/EquipmentBox")
	mesh.box((0.20, 1.79, 0.11), (1.07, -0.03, -0.18), 0.018, "Relay/Trim")
	for y in (-0.50, -0.28, -0.06):
		mesh.box((0.12, 0.06, 0.025), (1.07, y, -0.25), 0.008, "Relay/GoalMark")
	print(mesh.write("relay_service_bay.mesh"))


if __name__ == "__main__":
	main()
