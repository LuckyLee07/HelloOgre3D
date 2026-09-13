#!/usr/bin/env python3
"""Author Sandbox19's solid props. No DCC, images or external meshes required.

Writes Ogre MeshSerializer v1.100 chunks (see vendored OgreMeshFileFormat.h).
All dimensions are metres, Y up, centred pivot; triangles wind outwards.
Run from any directory. Runtime consumes the committed .mesh files.
"""
from collections import defaultdict
from itertools import product
from pathlib import Path
import json
import math
import struct

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "media/models/sandbox19"


def dot(a, b):
    return sum(x * y for x, y in zip(a, b))


def cross(a, b):
    return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])


def unit(a):
    length = math.sqrt(dot(a, a))
    return tuple(v / length for v in a)


def chunk(tag, data):
    return struct.pack("<HI", tag, len(data) + 6) + data


class Mesh:
    def __init__(self):
        self.vertices = []
        self.groups = defaultdict(list)

    def face(self, points, normal, material, uv_scale):
        normal = unit(normal)
        axis = min(range(3), key=lambda i: abs(normal[i]))
        seed = tuple(1 if i == axis else 0 for i in range(3))
        tangent = unit(cross(seed, normal))
        bitangent = cross(normal, tangent)
        center = tuple(sum(p[i] for p in points) / len(points) for i in range(3))
        points = sorted(points, key=lambda p: math.atan2(
            dot(tuple(p[i]-center[i] for i in range(3)), bitangent),
            dot(tuple(p[i]-center[i] for i in range(3)), tangent)))
        start = len(self.vertices)
        # Ogre's existing normal shader uses cross(tangent, normal).
        for p in points:
            self.vertices.append((*p, *normal, dot(p, tangent)*uv_scale,
                                  -dot(p, bitangent)*uv_scale, *tangent))
        for i in range(1, len(points)-1):
            self.groups[material].extend((start, start+i, start+i+1))

    def box(self, size, center, bevel, material, uv_scale=0.5):
        half = tuple(v * 0.5 for v in size)
        b = min(bevel, min(half) * 0.8)
        points = []
        for signs in product((-1, 1), repeat=3):
            for full_axis in range(3):
                points.append(tuple(signs[i] * (half[i] - (0 if i == full_axis else b))
                                    for i in range(3)))
        # Six face rectangles, twelve edge strips, eight corner triangles.
        for normal in product((-1, 0, 1), repeat=3):
            active = sum(v != 0 for v in normal)
            if active == 0:
                continue
            limit = sum(half[i] * abs(normal[i]) for i in range(3)) - b*(active-1)
            face = [p for p in points if abs(dot(p, normal)-limit) < 1e-6]
            if len(face) >= 3:
                translated = [tuple(p[i]+center[i] for i in range(3)) for p in face]
                self.face(translated, normal, material, uv_scale)

    def write(self, name):
        elements = [(2, 1, 0), (2, 4, 12), (1, 7, 24), (2, 9, 32)]
        declaration = b"".join(chunk(0x5110, struct.pack("<5H", 0, t, s, o, 0))
                               for t, s, o in elements)
        raw = b"".join(struct.pack("<11f", *v) for v in self.vertices)
        geometry = struct.pack("<I", len(self.vertices)) + chunk(0x5100, declaration)
        geometry += chunk(0x5200, struct.pack("<HH", 0, 44) + chunk(0x5210, raw))
        mesh = b"\0" + chunk(0x5000, geometry)
        for material, indices in self.groups.items():
            assert max(indices) < 65536
            sub = material.encode() + b"\n\1" + struct.pack("<I", len(indices)) + b"\0"
            sub += struct.pack("<" + "H" * len(indices), *indices)
            mesh += chunk(0x4000, sub)
        lower = [min(v[i] for v in self.vertices) for i in range(3)]
        upper = [max(v[i] for v in self.vertices) for i in range(3)]
        radius = max(math.sqrt(dot(v[:3], v[:3])) for v in self.vertices)
        mesh += chunk(0x9000, struct.pack("<7f", *lower, *upper, radius))
        (OUT / name).write_bytes(struct.pack("<H", 0x1000) + b"[MeshSerializer_v1.100]\n" + chunk(0x3000, mesh))
        return {"file": name, "vertices": len(self.vertices),
                "triangles": sum(len(g)//3 for g in self.groups.values()),
                "materials": list(self.groups), "bounds_min": lower, "bounds_max": upper}


def supply_crate(height):
    m = Mesh()
    # Centre the total one-piece asset around Y=0; feet touch -height/2.
    bottom, top = -height/2, height/2
    m.box((1.56, height-0.14, 1.02), (0, 0.02, 0), 0.065, "Relay/CrateBody")
    m.box((1.65, 0.13, 1.10), (0, top-0.065, 0), 0.025, "Relay/CrateFrame")
    m.box((1.59, 0.034, 1.06), (0, top-0.15, 0), 0.01, "Relay/CrateRubber")
    for x in (-0.61, 0.61):
        m.box((0.23, height-0.07, 1.08), (x, -0.015, 0), 0.024, "Relay/CrateFrame")
        m.box((0.26, 0.12, 1.10), (x, bottom+0.06, 0), 0.018, "Relay/CrateRubber")
    for z in (-0.522, 0.522):
        m.box((0.86, height-0.37, 0.022), (0, -0.05, z), 0.008, "Relay/CratePanel")
        for x in (-0.47, 0.47):
            m.box((0.11, 0.21, 0.045), (x, top-0.21, z), 0.012, "Relay/CrateLatch")
        m.box((0.34, 0.055, 0.06), (0, top-0.30, z), 0.012, "Relay/CrateRubber")
        # Restrained technical ID bars are actual shallow geometry, not decals.
        for i, width in enumerate((0.17, 0.26, 0.12)):
            m.box((width, 0.021, 0.009), (-0.17, -0.12-i*0.045, z*1.029),
                  0.002, "Relay/CrateMark")
    return m


def barrier(length=4.2, tie_x=1.42):
    m = Mesh()
    # Solid chamfered barriers share one silhouette at two courtyard scales.
    m.box((length, 1.25, 1.15), (0, 0, 0), 0.085, "Relay/Cover", 0.25)
    # Recess-like dark tie holes stay inside the
    # former cap envelope. Small details are part of the same convex solid.
    for z in (-0.571, 0.571):
        for x in (-tie_x, tie_x):
            m.box((0.045, 0.045, 0.012), (x, 0.24, z), 0.013, "Relay/CrateRubber")
    return m


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    manifest = [barrier().write("relay_barrier.mesh"),
                barrier(2.4, 0.78).write("relay_barrier_short.mesh"),
                supply_crate(1.05).write("relay_supply_crate.mesh"),
                supply_crate(1.65).write("relay_supply_crate_tall.mesh")]
    (OUT / "manifest.json").write_bytes((json.dumps(manifest, indent=2) + "\n").replace("\n", "\r\n").encode())
    for item in manifest:
        print(item)


if __name__ == "__main__":
    main()
