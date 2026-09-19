#!/usr/bin/env python3
"""Build Crossfire's original industrial miniature kit and authored surface maps.

Run from any directory with Python 3.10+. Uses the project's Ogre v1.100 mesh
serializer only; deterministic raster primitives use the standard library.
No DCC, downloaded asset, generative image service or runtime builder is needed.
Metres, Y up, +Z forward. Environments stand on Y=0; tile top is Y=0.
Combat actors are centred near their physics origin, with authored muzzle data.
"""
from pathlib import Path
import argparse
import hashlib
import json
import math
import struct
import zlib

import generate_relay_meshes as serializer
from generate_relay_meshes import Mesh, cross, dot, unit

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "media/models/crossfire"
TEXTURES = ROOT / "media/textures/crossfire"

# Linear material inputs. Quiet panels, dark joints and sparse status colours.
PALETTE = {
    "Porcelain": ((0.76, 0.72, 0.62), (0.14, 0.13, 0.11), 44),
    "Ivory": ((0.90, 0.86, 0.73), (0.18, 0.17, 0.14), 48),
    "Slate": ((0.16, 0.23, 0.27), (0.12, 0.16, 0.19), 34),
    "Floor": ((0.115, 0.17, 0.205), (0.035, 0.05, 0.06), 24),
    "FloorPanel": ((0.185, 0.255, 0.295), (0.04, 0.055, 0.07), 24),
    "Graphite": ((0.055, 0.080, 0.096), (0.10, 0.12, 0.14), 36),
    "Rubber": ((0.025, 0.039, 0.047), (0.005, 0.005, 0.005), 8),
    "Steel": ((0.35, 0.42, 0.44), (0.35, 0.40, 0.42), 72),
    "Teal": ((0.075, 0.41, 0.42), (0.14, 0.22, 0.22), 42),
    "Signal": ((0.17, 0.88, 0.82), (0.05, 0.12, 0.12), 24),
    "Amber": ((0.88, 0.56, 0.19), (0.14, 0.09, 0.04), 32),
    "Enemy": ((0.69, 0.18, 0.105), (0.18, 0.07, 0.035), 44),
    "EnemyLight": ((1.0, 0.41, 0.16), (0.10, 0.035, 0.01), 32),
    "Mark": ((0.65, 0.68, 0.62), (0.015, 0.015, 0.015), 12),
    "Vega": ((0.17, 0.61, 0.49), (0.10, 0.21, 0.17), 38),
    "Rook": ((0.24, 0.40, 0.71), (0.12, 0.17, 0.27), 38),
    "RookLight": ((0.38, 0.64, 0.95), (0.07, 0.12, 0.20), 24),
    "ServiceBand": ((0.065, 0.088, 0.10), (0.01, 0.015, 0.02), 12),
    "Wreck": ((0.19, 0.215, 0.225), (0.015, 0.018, 0.02), 8),
    "ShieldHit": ((1.0, 0.70, 0.22), (0.08, 0.05, 0.01), 12),
    "HullHit": ((0.75, 1.0, 0.91), (0.03, 0.05, 0.04), 12),
    "DeckService": ((0.34, 0.335, 0.285), (0.035, 0.04, 0.04), 18),
    "DeckCoolant": ((0.125, 0.29, 0.30), (0.035, 0.06, 0.065), 24),
    "DeckInterlock": ((0.235, 0.26, 0.335), (0.04, 0.05, 0.07), 22),
}


def mat(name):
    return "Crossfire/" + name


def box(m, size, center, bevel, material):
    m.box(size, center, bevel, mat(material))


def transform(p, center, axis="y", angle=0.0):
    # Profile primitives use Y as their default longitudinal axis.
    x, y, z = p
    c, s = math.cos(angle), math.sin(angle)
    x, z = x*c + z*s, -x*s + z*c
    if axis == "z":
        x, y, z = x, -z, y
    elif axis == "x":
        x, y, z = y, z, x
    return (x+center[0], y+center[1], z+center[2])


def polygon_shell(m, profile, levels, center, material, axis="y", angle=0.0,
                  cap_material=None):
    """A convex hard-surface hull, with shaped lower/upper bevel profiles.

    profile contains CCW (x,z) points; levels contain (axis position, scale).
    Duplicate coincident ring positions are prohibited, keeping normals finite.
    """
    rings = [[transform((x*scale, y, z*scale), center, axis, angle)
              for x, z in profile] for y, scale in levels]
    cap_material = cap_material or material
    for a, b in zip(rings, rings[1:]):
        for i in range(len(profile)):
            j = (i+1) % len(profile)
            points = [a[i], a[j], b[j], b[i]]
            n = cross(tuple(b[i][k]-a[i][k] for k in range(3)),
                      tuple(a[j][k]-a[i][k] for k in range(3)))
            m.face(points, n, mat(material), 0.5)
    for ring, sign in ((rings[0], -1), (rings[-1], 1)):
        base = transform((0, sign, 0), (0, 0, 0), axis, angle)
        m.face(ring, base, mat(cap_material), 0.5)


def lathe(m, rings, center, material, segments=20, axis="y", angle=0.0,
          caps=True, cap_material=None):
    """Faceted rotational metal forms; nonzero radii, planar hard normals."""
    p = [(math.cos(2*math.pi*i/segments), math.sin(2*math.pi*i/segments))
         for i in range(segments)]
    rs = [[transform((x*r, h, z*r), center, axis, angle) for x, z in p]
          for h, r in rings]
    for a, b in zip(rs, rs[1:]):
        for i in range(segments):
            j = (i+1) % segments
            n = cross(tuple(b[i][k]-a[i][k] for k in range(3)),
                      tuple(a[j][k]-a[i][k] for k in range(3)))
            m.face([a[i], a[j], b[j], b[i]], n, mat(material), 0.5)
    if caps:
        for r, sign in ((rs[0], -1), (rs[-1], 1)):
            m.face(r, transform((0, sign, 0), (0, 0, 0), axis, angle),
                   mat(cap_material or material), 0.5)


def ring(m, outer, inner, height, center, material, segments=24, axis="y"):
    # Closed ring section avoids a dark disk pretending to be a hollow duct.
    bevel = min(0.02, height*0.25, (outer-inner)*0.25)
    profile = [(-height/2, outer-bevel), (-height/2+bevel, outer),
               (height/2-bevel, outer), (height/2, outer-bevel),
               (height/2, inner), (-height/2, inner),
               (-height/2, outer-bevel)]
    lathe(m, profile, center, material, segments, axis, caps=False)


def beam(m, a, b, width, depth, material):
    direction = unit(tuple(b[i]-a[i] for i in range(3)))
    seed = (0, 0, 1) if abs(direction[2]) < 0.9 else (0, 1, 0)
    u = unit(cross(direction, seed))
    v = cross(direction, u)
    corners = [[tuple(p[i]+su*u[i]*width/2+sv*v[i]*depth/2 for i in range(3))
                for su, sv in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
               for p in (a, b)]
    for i in range(4):
        j = (i+1) % 4
        n = cross(tuple(corners[0][j][k]-corners[0][i][k] for k in range(3)),
                  tuple(b[k]-a[k] for k in range(3)))
        m.face([corners[0][i], corners[0][j], corners[1][j], corners[1][i]],
               n, mat(material), 0.5)
    m.face(corners[0], tuple(-d for d in direction), mat(material), 0.5)
    m.face(corners[1], direction, mat(material), 0.5)


def octagon(w, d, cut=0.18):
    x, z = w/2, d/2
    c = min(cut, x*0.7, z*0.7)
    return [(-x+c, -z), (x-c, -z), (x, -z+c), (x, z-c),
            (x-c, z), (-x+c, z), (-x, z-c), (-x, -z+c)]


def hull(m, w, d, h, center, bevel, material, cut=0.18):
    polygon_shell(m, octagon(w, d, cut),
                  [(-h/2, 1-bevel), (-h/2+h*0.18, 1),
                   (h/2-h*0.22, 1), (h/2, 1-bevel)], center, material)


def drone(identity=1):
    m = Mesh()
    # Armoured carapace narrows towards the sensor nose; a separate dark belly
    # exposes the two real duct rings and their motor hubs from the game camera.
    shape = ([(-0.27, -0.56), (0.27, -0.56), (0.38, -0.31),
              (0.31, 0.34), (0.17, 0.60), (-0.17, 0.60),
              (-0.31, 0.34), (-0.38, -0.31)] if identity == 1 else
             [(-0.35, -0.56), (0.35, -0.56), (0.42, -0.35),
              (0.42, 0.28), (0.29, 0.60), (-0.29, 0.60),
              (-0.42, 0.28), (-0.42, -0.35)])
    identity_material = "Vega" if identity == 1 else "Rook"
    light_material = "Signal" if identity == 1 else "RookLight"
    polygon_shell(m, shape, [(-0.25, 0.76), (-0.17, 1.02), (0.03, 1.0),
                            (0.13, 0.80)], (0, 0, 0), "Graphite")
    polygon_shell(m, shape, [(0.10, 0.86), (0.18, 1.0), (0.33, 0.82),
                            (0.38, 0.58)], (0, 0, -0.015), identity_material)
    # Rear thermal spine and front polarised sensor strip.
    box(m, (0.19, 0.07, 0.40), (0, 0.354, -0.20), 0.025, "Graphite")
    box(m, (0.12, 0.02, 0.15), (0, 0.396, -0.20), 0.009, light_material)
    box(m, (0.36, 0.10, 0.095), (0, 0.213, 0.565), 0.025, "Rubber")
    box(m, (0.22, 0.026, 0.016), (0, 0.229, 0.619), 0.006, light_material)
    for side in (-1, 1):
        x = side*0.485
        beam(m, (side*0.24, -0.045, -0.14), (x, -0.045, -0.14),
             0.14, 0.14, "Steel")
        ring(m, 0.225, 0.151, 0.235, (x, -0.09, -0.18), "Porcelain" if identity == 1 else "Slate", 24)
        ring(m, 0.190, 0.158, 0.075, (x, -0.24, -0.18), identity_material, 24)
        lathe(m, [(-0.15, 0.09), (-0.11, 0.118), (-0.05, 0.10), (0, 0.052)],
              (x, -0.055, -0.18), "Graphite", 16)
        for angle in (0, math.pi*0.5, math.pi, math.pi*1.5):
            dx, dz = math.cos(angle), math.sin(angle)
            beam(m, (x+dx*0.06, -0.08, -0.18+dz*0.06),
                 (x+dx*0.157, -0.08, -0.18+dz*0.157), 0.025, 0.035, "Steel")
        # Forward shoulder fairings have a directional pentagonal silhouette.
        shoulder = ([(-0.13, -0.23), (0.13, -0.23), (0.12, 0.18),
                     (0, 0.31), (-0.12, 0.18)] if identity == 1 else
                    [(-0.18, -0.18), (0.18, -0.18), (0.18, 0.18),
                     (0.10, 0.28), (-0.10, 0.28), (-0.18, 0.18)])
        polygon_shell(m, shoulder, [(-0.02, 0.76), (0.045, 1), (0.13, 0.75)],
                      (side*0.43, 0, 0.21), "Ivory" if identity == 1 else "Rook")
        box(m, (0.08, 0.028, 0.15), (side*0.43, 0.132, 0.23), 0.008,
            "Vega" if identity == 1 else "Ivory")
        for i in range(3):
            box(m, (0.10, 0.033, 0.033), (side*0.19, 0.274, -0.33+i*0.056),
                0.005, "Graphite")
    # Single forward gun: exposed receiver, collar, recessed bore, upper rail.
    box(m, (0.22, 0.18, 0.34), (0, -0.045, 0.51), 0.035, "Slate")
    lathe(m, [(0.60, 0.085), (0.65, 0.072), (0.81, 0.060)],
          (0, -0.02, 0), "Steel", 12, axis="z")
    ring(m, 0.086, 0.046, 0.13, (0, -0.02, 0.83), "Graphite", 12, axis="z")
    lathe(m, [(0.786, 0.043), (0.794, 0.043)], (0, -0.02, 0), "Rubber", 12, axis="z")
    box(m, (0.043, 0.042, 0.28), (0, 0.077, 0.56), 0.009, "Ivory")
    # Strong monochrome glyphs reinforce body colour: VEGA chevron / ROOK twin
    # bars. All new detail remains inside the previous authored mesh bounds.
    if identity == 1:
        for x in (-0.14, 0.14):
            beam(m, (x, 0.393, 0.00), (0, 0.393, 0.20), 0.014, 0.075, "Ivory")
    else:
        for x in (-0.105, 0.105):
            box(m, (0.075, 0.014, 0.26), (x, 0.391, 0.075), 0.004, "Ivory")
    return m


def sentinel():
    m = Mesh()
    # A central turntable and three planted outriggers make the turret's stable
    # rotation readable while leaving an exposed engine weak point to the rear.
    lathe(m, [(-0.80, 0.52), (-0.73, 0.66), (-0.56, 0.66), (-0.46, 0.45)],
          (0, 0, 0), "Graphite", 16)
    ring(m, 0.52, 0.43, 0.13, (0, -0.43, 0), "Steel", 24)
    hull(m, 1.14, 1.12, 0.76, (0, 0.035, -0.12), 0.13, "Slate", 0.28)
    hull(m, 0.91, 0.83, 0.18, (0, 0.48, -0.10), 0.12, "Porcelain", 0.25)
    for a in (0, 2*math.pi/3, 4*math.pi/3):
        dx, dz = math.sin(a), math.cos(a)
        beam(m, (dx*0.36, -0.55, dz*0.36), (dx*0.74, -0.73, dz*0.74),
             0.22, 0.16, "Steel")
        lathe(m, [(-0.80, 0.16), (-0.73, 0.21), (-0.67, 0.17)],
              (dx*0.73, 0, dz*0.73), "Graphite", 8)
    # A split shield exposes a central gun channel. Plates have tapered six-edge
    # outlines, bevelled thickness, bright face inserts and mechanical brackets.
    plate = [(-0.27, -0.58), (0.22, -0.55), (0.30, -0.32),
             (0.24, 0.55), (-0.15, 0.69), (-0.29, 0.41)]
    for side in (-1, 1):
        p = [(x*side, y) for x, y in plate]
        if side == -1:
            p.reverse()
        # In a Z-axis primitive, profile z becomes -Y.
        polygon_shell(m, p, [(-0.11, 0.85), (-0.065, 1.0),
                             (0.075, 1.0), (0.115, 0.90)],
                      (side*0.58, 0.055, 0.42), "Graphite", axis="z")
        polygon_shell(m, p, [(0.117, 0.84), (0.148, 0.78)],
                      (side*0.58, 0.055, 0.42), "Enemy", axis="z")
        box(m, (0.11, 0.69, 0.035), (side*0.62, 0.025, 0.586), 0.022, "EnemyLight")
        beam(m, (side*0.32, 0, 0.17), (side*0.66, 0, 0.37), 0.16, 0.16, "Steel")
    box(m, (0.36, 0.26, 0.52), (0, 0.20, 0.42), 0.045, "Porcelain")
    lathe(m, [(0.62, 0.13), (0.70, 0.11), (0.95, 0.11)],
          (0, 0.20, 0), "Steel", 12, axis="z")
    ring(m, 0.155, 0.075, 0.22, (0, 0.20, 1.00), "Graphite", 12, axis="z")
    lathe(m, [(0.884, 0.07), (0.895, 0.07)], (0, 0.20, 0), "Rubber", 12, axis="z")
    box(m, (0.35, 0.075, 0.18), (0, 0.573, 0.14), 0.015, "Rubber")
    box(m, (0.24, 0.028, 0.022), (0, 0.595, 0.24), 0.007, "EnemyLight")
    # A broad top-plane arrow makes shield direction legible from the game camera.
    polygon_shell(m, [(-0.25, -0.29), (0.25, -0.29), (0, 0.025)],
                  [(0.578, 1), (0.623, 1)], (0, 0, 0), "EnemyLight")
    ring(m, 0.28, 0.21, 0.07, (0, 0.02, -0.702), "Steel", 20, axis="z")
    lathe(m, [(-0.742, 0.195), (-0.711, 0.195)], (0, 0.02, 0), "EnemyLight", 20, axis="z")
    for x in (-0.11, 0, 0.11):
        box(m, (0.032, 0.28, 0.025), (x, 0.02, -0.764), 0.006, "Graphite")
    return m


def tile():
    m = Mesh()
    box(m, (4, 0.165, 4), (0, -0.1575, 0), 0.035, "Graphite")
    # Four inset slabs sit above the structural base, retaining narrow service
    # seams. The original base at Y=0 hid panels only 0.5 mm below it, causing
    # depth fighting at the courtyard camera distance; keep 75 mm separation.
    for x in (-0.982, 0.982):
        for z in (-0.982, 0.982):
            box(m, (1.935, 0.085, 1.935), (x, -0.0425, z), 0.02, "FloorPanel")
    return m


def tile_variant(material):
    # Material grouping alone changes: positions/indices remain identical to the
    # established tile, so Bullet hulls and Recast input are exactly preserved.
    m = tile()
    m.groups[mat(material)] = m.groups.pop(mat("FloorPanel"))
    return m


def wall():
    m = Mesh()
    box(m, (4, 0.32, 0.80), (0, 0.16, 0), 0.055, "Graphite")
    box(m, (3.88, 1.75, 0.53), (0, 1.16, 0), 0.075, "Porcelain")
    box(m, (4, 0.27, 0.72), (0, 2.255, 0), 0.055, "Ivory")
    box(m, (3.70, 0.70, 0.024), (0, 1.13, 0.28), 0.025, "Slate")
    for x in (-1.65, 0, 1.65):
        box(m, (0.14, 1.73, 0.13), (x, 1.19, 0.30), 0.023, "Steel")
    box(m, (1.1, 0.05, 0.035), (0, 1.84, 0.28), 0.01, "Signal")
    for x in (-1.70, 1.70):
        box(m, (0.20, 0.06, 0.03), (x, 0.43, 0.28), 0.008, "Amber")
    return m


def cover():
    m = Mesh()
    hull(m, 2.80, 0.98, 0.23, (0, 0.115, 0), 0.08, "Graphite", 0.18)
    hull(m, 2.65, 0.87, 0.70, (0, 0.55, 0), 0.12, "Porcelain", 0.19)
    box(m, (2.59, 0.12, 0.90), (0, 0.89, 0), 0.025, "Ivory")
    for z in (-0.428, 0.428):
        box(m, (1.85, 0.28, 0.025), (0, 0.52, z), 0.022, "Slate")
        for x in (-1.06, 1.06):
            box(m, (0.12, 0.47, 0.06), (x, 0.56, z), 0.015, "Steel")
        box(m, (0.44, 0.038, 0.032), (0, 0.58, z*1.04), 0.006, "Mark")
    return m


def core():
    m = Mesh()
    hull(m, 3.4, 3.4, 0.30, (0, 0.15, 0), 0.07, "Graphite", 0.50)
    hull(m, 2.94, 2.94, 0.20, (0, 0.38, 0), 0.05, "Steel", 0.54)
    lathe(m, [(0.48, 1.10), (0.70, 1.29), (1.10, 1.29), (1.22, 1.06)],
          (0, 0, 0), "Slate", 12)
    ring(m, 1.24, 1.04, 0.14, (0, 1.15, 0), "Porcelain", 24)
    lathe(m, [(1.15, 0.85), (1.42, 0.71), (2.80, 0.71), (3.03, 0.91)],
          (0, 0, 0), "Graphite", 12)
    # Six luminous conduits live inside the structural ribs; dark wells keep the
    # signal sparse and legible even under broad overhead light.
    for i in range(6):
        a = i*math.pi/3
        dx, dz = math.sin(a), math.cos(a)
        beam(m, (dx*0.80, 1.31, dz*0.80), (dx*0.80, 2.91, dz*0.80),
             0.17, 0.23, "Porcelain")
        sx, sz = math.sin(a+math.pi/6), math.cos(a+math.pi/6)
        beam(m, (sx*0.73, 1.49, sz*0.73), (sx*0.73, 2.74, sz*0.73),
             0.065, 0.065, "Signal")
    ring(m, 1.05, 0.71, 0.25, (0, 2.98, 0), "Porcelain", 24)
    ring(m, 0.92, 0.68, 0.08, (0, 3.145, 0), "Teal", 24)
    lathe(m, [(3.17, 0.68), (3.27, 0.53), (3.31, 0.53)],
          (0, 0, 0), "Slate", 16)
    lathe(m, [(3.29, 0.20), (3.63, 0.20), (3.70, 0.12)],
          (0, 0, 0), "Steel", 12)
    for a in (0, math.pi/2, math.pi, 3*math.pi/2):
        dx, dz = math.sin(a), math.cos(a)
        beam(m, (dx*1.02, 0.84, dz*1.02), (dx*1.43, 0.32, dz*1.43),
             0.22, 0.20, "Teal")
        lathe(m, [(0.33, 0.13), (0.44, 0.13)],
              (dx*1.43, 0, dz*1.43), "Steel", 12)
    return m


def gate():
    m = Mesh()
    for x in (-2.22, 2.22):
        box(m, (1.06, 0.34, 1.5), (x, 0.17, 0), 0.08, "Graphite")
        box(m, (0.86, 3.25, 1.12), (x, 1.81, 0), 0.095, "Porcelain")
        box(m, (0.50, 1.72, 0.08), (x, 1.34, 0.59), 0.035, "Slate")
        box(m, (0.045, 1.9, 0.048), (x-(0.32 if x>0 else -0.32), 1.55, 0.61),
            0.012, "Signal")
        for h in (0.67, 0.91, 1.15):
            box(m, (0.29, 0.055, 0.045), (x, h, 0.66), 0.009, "Steel")
    box(m, (5.5, 0.62, 1.42), (0, 3.39, 0), 0.12, "Ivory")
    box(m, (3.5, 0.30, 0.08), (0, 3.39, 0.75), 0.025, "Graphite")
    for x in (-0.25, 0, 0.25):
        box(m, (0.12, 0.095, 0.035), (x, 3.42, 0.801), 0.016, "Signal")
    box(m, (3.35, 0.14, 0.92), (0, 3.02, 0), 0.04, "Slate")
    return m


def pipe():
    m = Mesh()
    for z in (-0.16, 0.16):
        lathe(m, [(-2, 0.11), (2, 0.11)], (0, 0.22, z), "Teal", 12, axis="x")
        for x in (-1.75, 0, 1.75):
            ring(m, 0.15, 0.111, 0.10, (x, 0.22, z), "Steel", 12, axis="x")
    for x in (-1.75, 1.75):
        box(m, (0.23, 0.12, 0.64), (x, 0.06, 0), 0.03, "Graphite")
    return m


def edge():
    m = Mesh()
    box(m, (4.0, 0.55, 0.70), (0, 0.275, 0), 0.055, "Slate")
    box(m, (4.0, 0.10, 0.58), (0, 0.60, 0), 0.025, "Porcelain")
    for x in (-1.5, -0.5, 0.5, 1.5):
        box(m, (0.38, 0.10, 0.035), (x, 0.32, 0.37), 0.008, "Graphite")
        box(m, (0.20, 0.03, 0.02), (x, 0.32, 0.396), 0.005, "Signal")
    return m


def console():
    m = Mesh()
    hull(m, 1.20, 0.90, 0.18, (0, 0.09, 0), 0.10, "Graphite", 0.14)
    hull(m, 1.0, 0.69, 0.95, (0, 0.625, 0), 0.10, "Porcelain", 0.16)
    box(m, (0.84, 0.22, 0.73), (0, 1.17, 0), 0.055, "Slate")
    box(m, (0.60, 0.018, 0.43), (0, 1.288, 0), 0.018, "Rubber")
    box(m, (0.40, 0.008, 0.21), (-0.035, 1.301, 0), 0.013, "Teal")
    for i in range(3):
        box(m, (0.25-i*0.055, 0.008, 0.024), (-0.065, 1.31, -0.065+i*0.065),
            0.004, "Signal")
    box(m, (0.67, 0.48, 0.03), (0, 0.63, 0.361), 0.022, "Slate")
    for h in (0.46, 0.56, 0.66, 0.76):
        box(m, (0.48, 0.033, 0.026), (0, h, 0.386), 0.007, "Graphite")
    return m


def dock():
    m = Mesh()
    hull(m, 2.4, 2.4, 0.19, (0, 0.095, 0), 0.08, "Graphite", 0.40)
    hull(m, 2.12, 2.12, 0.08, (0, 0.225, 0), 0.04, "Slate", 0.38)
    ring(m, 0.83, 0.79, 0.026, (0, 0.275, 0), "Teal", 32)
    for x in (-0.91, 0.91):
        box(m, (0.12, 0.11, 1.16), (x, 0.295, 0), 0.018, "Porcelain")
        box(m, (0.036, 0.01, 0.51), (x, 0.357, 0), 0.004, "Signal")
    for z in (-0.87, 0.87):
        box(m, (0.42, 0.016, 0.10), (0, 0.276, z), 0.007, "Amber")
    return m


def vent():
    m = Mesh()
    hull(m, 1.62, 1.60, 0.26, (0, 0.13, 0), 0.08, "Graphite", 0.18)
    hull(m, 1.49, 1.47, 0.42, (0, 0.47, 0), 0.12, "Porcelain", 0.17)
    ring(m, 0.61, 0.48, 0.08, (0, 0.717, 0), "Steel", 24)
    lathe(m, [(0.683, 0.48), (0.690, 0.48)], (0, 0, 0), "Rubber", 24)
    for x in (-0.32, -0.16, 0, 0.16, 0.32):
        length = 2*math.sqrt(0.455**2-x*x)
        box(m, (0.037, 0.035, length), (x, 0.745, 0), 0.005, "Slate")
    return m


def deck_marks(sector=7):
    m = Mesh()
    # Stencil geometry only: empty space between the shapes has no backplate,
    # alpha texture or hidden floor quad. Keep all surfaces at 0..6 mm.
    def stencil(points, material):
        polygon_shell(m, points, [(0, 1), (0.006, 1)], (0, 0, 0), material)

    def stripe(x, z, width, length, material):
        stencil([(x-width/2, z-length/2), (x+width/2, z-length/2),
                 (x+width/2, z+length/2), (x-width/2, z+length/2)], material)

    # A broken guide bracket and a segmented forward arrow support route reading.
    for x in (-0.775, 0.775):
        stripe(x, -0.32, 0.05, 2.36, "Teal")
        stripe(x, 1.25, 0.05, 0.50, "Ivory")
        stripe(x*0.79, -1.47, 0.26, 0.06, "Ivory")
    stencil([(-0.48, 0.65), (0.48, 0.65), (0, 1.26)], "Ivory")
    stripe(0, 0.42, 0.20, 0.27, "Ivory")
    stripe(0, 0.03, 0.20, 0.24, "Ivory")
    stripe(0, -0.26, 0.20, 0.13, "Amber")
    # Real seven-segment sector stencils supply scale without font/texture assets.
    segments = {
        "a": (0, 0.24, 0.29, 0.055), "g": (0, 0, 0.29, 0.055),
        "d": (0, -0.24, 0.29, 0.055), "f": (-0.145, 0.12, 0.055, 0.19),
        "b": (0.145, 0.12, 0.055, 0.19), "e": (-0.145, -0.12, 0.055, 0.19),
        "c": (0.145, -0.12, 0.055, 0.19),
    }
    for x, active in ((-0.235, "abcdef"), (0.235, {7: "abc", 8: "abcdefg", 9: "abcdfg"}[sector])):
        for segment in active:
            sx, sz, w, d = segments[segment]
            stripe(-x-sx, -0.91+sz, w, d, "Ivory")
    for x in (-0.54, 0.54):
        stripe(x, -0.92, 0.045, 0.39, "Amber")
    return m


def base():
    m = Mesh()
    # One dark plinth closes the diorama below the floor. Top -0.24 matches tile
    # bottoms exactly; the complete bounds are 20.4 x 0.55 x 24.4 metres.
    box(m, (20.32, 0.55, 24.32), (0, -0.515, 0), 0.10, "Graphite")
    for x in (-10.16, 10.16):
        box(m, (0.08, 0.10, 24.14), (x, -0.415, 0), 0.012, "Slate")
    for z in (-12.16, 12.16):
        box(m, (20.14, 0.10, 0.08), (0, -0.415, z), 0.012, "Slate")
        # Sparse recessed equipment indicators, well below the walking surface.
        for x in (-8.6, -2.9, 2.9, 8.6):
            box(m, (0.55, 0.10, 0.045), (x, -0.58, z+math.copysign(0.012, z)),
                0.01, "Slate")
            box(m, (0.11, 0.026, 0.014), (x-0.12, -0.58, z+math.copysign(0.031, z)),
                0.004, "Signal")
    return m


def relay_tower():
    m = Mesh()
    hull(m, 3.2, 2.8, 0.34, (0, 0.17, 0), 0.08, "Graphite", 0.42)
    hull(m, 2.60, 2.25, 0.78, (0, 0.66, 0), 0.10, "Porcelain", 0.34)
    hull(m, 1.38, 1.35, 2.79, (0, 2.42, 0.10), 0.08, "Slate", 0.28)
    for x in (-0.50, 0.50):
        beam(m, (x, 1.05, 0.25), (x, 3.86, 0.25), 0.15, 0.28, "Ivory")
        beam(m, (x*2.0, 0.87, 0.68), (x, 2.27, 0.35), 0.19, 0.22, "Steel")
    for h in (1.37, 1.67, 1.97, 2.27, 2.57, 2.87, 3.17, 3.47):
        box(m, (0.68, 0.065, 0.065), (0, h, -0.613), 0.012, "Graphite")
    box(m, (0.075, 1.48, 0.035), (0, 2.19, -0.652), 0.008, "Signal")
    lathe(m, [(3.72, 0.49), (3.89, 0.59), (4.10, 0.59)], (0, 0, 0.10), "Steel", 16)
    # The reflector is a closed, concave radial section, open toward local -Z.
    # Its thick outer rim, feed struts and hub create a readable non-box skyline.
    bowl = [(-0.43, 1.27), (-0.35, 1.01), (-0.165, 0.69), (-0.015, 0.37),
            (0.065, 0.13), (0, 0.13), (-0.08, 0.34), (-0.23, 0.66),
            (-0.42, 0.96), (-0.52, 1.22), (-0.43, 1.27)]
    lathe(m, bowl, (0, 4.68, -0.08), "Porcelain", 32, axis="z", caps=False)
    ring(m, 1.285, 1.19, 0.075, (0, 4.68, -0.575), "Steel", 32, axis="z")
    for a in (math.pi/2, math.pi*7/6, math.pi*11/6):
        x,y=math.cos(a),math.sin(a)
        beam(m, (x*1.14, 4.68+y*1.14, -0.56), (0, 4.68, -1.04),
             0.055, 0.055, "Slate")
    lathe(m, [(-1.14, 0.13), (-0.96, 0.13), (-0.89, 0.07)],
          (0, 4.68, 0), "Teal", 12, axis="z")
    box(m, (0.42, 0.55, 0.07), (-0.79, 0.66, -1.143), 0.025, "Slate")
    box(m, (0.24, 0.10, 0.022), (-0.79, 0.75, -1.19), 0.009, "Signal")
    for x in (0.29, 0.48, 0.67, 0.86):
        box(m, (0.08, 0.36, 0.055), (x, 0.63, -1.15), 0.015, "Graphite")
    return m


def cooling_stack():
    m = Mesh()
    hull(m, 2.50, 2.50, 0.27, (0, 0.135, 0), 0.08, "Graphite", 0.33)
    lathe(m, [(0.27, 0.88), (0.49, 1.02), (0.70, 1.02), (0.82, 0.85)],
          (0,0,0), "Slate", 24)
    lathe(m, [(0.76, 0.82), (1.02, 0.82), (2.30, 0.68), (2.55, 0.92)],
          (0,0,0), "Porcelain", 24)
    for a in [i*math.pi/4 for i in range(8)]:
        dx,dz=math.sin(a),math.cos(a)
        beam(m, (dx*0.86, 0.91, dz*0.86), (dx*0.73, 2.35, dz*0.73),
             0.11, 0.13, "Slate")
    for h,r in ((0.90,0.89),(1.48,0.84),(2.10,0.79)):
        ring(m, r, r-0.065, 0.075, (0,h,0), "Steel", 24)
    ring(m, 1.07, 0.75, 0.34, (0,2.66,0), "Ivory", 24)
    lathe(m, [(2.65,0.73),(2.72,0.73)], (0,0,0), "Rubber", 24)
    lathe(m, [(2.73,0.18),(2.89,0.18),(2.93,0.10)], (0,0,0), "Steel", 16)
    for a in [i*math.pi/3 for i in range(6)]:
        dx,dz=math.sin(a),math.cos(a)
        beam(m, (dx*0.17,2.80,dz*0.17),(dx*0.70,2.78,dz*0.70),0.15,0.045,"Slate")
    for x in (-1.06,1.06):
        lathe(m, [(0.25,0.11),(1.05,0.11)], (x,0,0), "Teal", 12)
        ring(m,0.16,0.112,0.09,(x,0.83,0),"Steel",12)
    box(m,(0.56,0.19,0.09),(0,0.52,-1.02),0.025,"Graphite")
    box(m,(0.32,0.055,0.025),(0,0.53,-1.075),0.008,"Teal")
    return m


def service_trench():
    m=Mesh()
    # A shallow service channel sits on the established floor, below step height.
    # The slat gaps contain no fake transparent plane; the lower dark bed is real.
    box(m,(0.65,0.024,6.0),(0,-0.007,0),0.003,"Graphite")
    for x in (-0.30,0.30):
        box(m,(0.048,0.018,5.94),(x,0.011,0),0.003,"Steel")
    for i in range(24):
        box(m,(0.51,0.014,0.09),(0,0.012,-2.875+i*0.25),0.003,"Slate")
    for z in (-2.70,2.70):
        box(m,(0.14,0.010,0.08),(0,0.025,z),0.002,"Amber")
    return m


def service_elbow():
    m=Mesh()
    # Two continuous curved pipes connect perpendicular service runs.
    for radius in (0.56,0.88):
        positions=[]
        for i in range(13):
            a=i*math.pi/24
            positions.append((math.cos(a)*radius,0.22,math.sin(a)*radius))
        for i in range(12):
            # Six-sided closed link segments overlap at their joins only; their
            # actual swept geometry remains faithful to the visible bend.
            a,b=positions[i:i+2]
            beam(m,a,b,0.18,0.18,"Teal")
        for a in (0,math.pi/2):
            x,z=math.cos(a)*radius,math.sin(a)*radius
            ring(m,0.14,0.095,0.10,(x,0.22,z),"Steel",12,axis="z" if a==0 else "x")
    box(m,(1.18,0.12,1.18),(0.55,0.06,0.55),0.04,"Graphite")
    return m


def sector_sign(sector):
    m=Mesh()
    box(m,(2.15,0.92,0.12),(0,0.46,0),0.045,"Graphite")
    box(m,(0.045,0.70,0.030),(-0.94,0.46,-0.072),0.006,"Teal")
    segments={
        "a":(0,0.25,0.32,0.06),"g":(0,0,0.32,0.06),"d":(0,-0.25,0.32,0.06),
        "f":(-0.16,0.125,0.06,0.185),"b":(0.16,0.125,0.06,0.185),
        "e":(-0.16,-0.125,0.06,0.185),"c":(0.16,-0.125,0.06,0.185)}
    for x,active in ((-0.39,"abcdef"),(0.21,{7:"abc",8:"abcdefg",9:"abcdfg"}[sector])):
        for key in active:
            sx,sy,w,h=segments[key]
            box(m,(w,h,0.026),(-x-sx,0.46+sy,-0.076),0.005,"Ivory")
    for h in (0.33,0.43,0.53,0.63):
        box(m,(0.20,0.035,0.025),(0.73,h,-0.076),0.005,"Amber" if h==0.63 else "Slate")
    return m


def rear_plinth():
    m=Mesh()
    # The rear service apron joins the board at Z=12. It is behind the solid wall,
    # providing a physical base for background facilities without a giant floor.
    box(m,(20.4,0.65,3.8),(0,-0.465,0),0.10,"Graphite")
    box(m,(20.0,0.12,3.50),(0,-0.20,0),0.035,"Slate")
    for x in (-8,-4,0,4,8):
        box(m,(0.40,0.05,0.035),(x,-0.49,1.92),0.008,"Teal")
    return m


def court_margin(kind):
    m = Mesh()
    if kind == "side":
        width, depth = 1.0, 24.0
        box(m, (width, 0.024, depth), (0, 0.012, 0), 0.002, "ServiceBand")
        box(m, (0.09, 0.016, depth), (-0.455, 0.034, 0), 0.002, "Mark")
        for z in [i*1.5 for i in range(-7,8)]:
            profile=[(-0.20,z-0.16),(-0.08,z-0.16),(0.25,z+0.16),(0.13,z+0.16)]
            polygon_shell(m,profile,[(0.028,1),(0.042,1)],(0,0,0),"Amber")
    else:
        width, depth = 18.0, (1.0 if kind == "front" else 1.5)
        box(m, (width, 0.024, depth), (0, 0.012, 0), 0.002, "ServiceBand")
        inner_z = depth/2-0.045 if kind == "front" else -depth/2+0.045
        box(m, (width, 0.016, 0.09), (0, 0.034, inner_z), 0.002, "Mark")
        for x in [i*1.5 for i in range(-5,6)]:
            profile=[(x-0.16,-0.20),(x-0.16,-0.08),(x+0.16,0.25),(x+0.16,0.13)]
            polygon_shell(m,profile,[(0.028,1),(0.042,1)],(0,0,0),"Amber")
    return m


BUILDERS = [
    ("drone.mesh", drone, {"pivot": "physics centre", "forward": "+Z",
                           "muzzle": [0, -0.02, 0.90]}),
    ("drone_b.mesh", lambda: drone(2), {"pivot": "physics centre", "forward": "+Z",
                                      "muzzle": [0, -0.02, 0.90]}),
    ("sentinel.mesh", sentinel, {"pivot": "physics centre", "forward": "+Z",
                                 "muzzle": [0, 0.20, 1.12]}),
    ("tile.mesh", tile, {"pivot": "top surface", "top_y": 0}),
    ("tile_service.mesh", lambda: tile_variant("DeckService"), {"pivot": "top surface", "top_y": 0, "geometry_source": "tile.mesh"}),
    ("tile_coolant.mesh", lambda: tile_variant("DeckCoolant"), {"pivot": "top surface", "top_y": 0, "geometry_source": "tile.mesh"}),
    ("tile_interlock.mesh", lambda: tile_variant("DeckInterlock"), {"pivot": "top surface", "top_y": 0, "geometry_source": "tile.mesh"}),
    ("wall.mesh", wall, {"pivot": "ground", "front": "+Z"}),
    ("cover.mesh", cover, {"pivot": "ground"}),
    ("core.mesh", core, {"pivot": "ground"}),
    ("gate.mesh", gate, {"pivot": "ground", "front": "+Z",
                         "collision_note": "Open passage; do not use one convex hull as a traversable gate."}),
    ("pipe.mesh", pipe, {"pivot": "ground", "length_axis": "X"}),
    ("edge.mesh", edge, {"pivot": "ground", "front": "+Z"}),
    ("console.mesh", console, {"pivot": "ground", "front": "+Z"}),
    ("dock.mesh", dock, {"pivot": "ground"}),
    ("vent.mesh", vent, {"pivot": "ground"}),
    ("deck_marks.mesh", deck_marks, {"pivot": "ground", "forward": "+Z",
                                     "collision_note": "Stencil only, with empty space; prefer a visual module and disable shadow casting."}),
    ("base.mesh", base, {"pivot": "tile origin", "top_y": -0.24}),
    ("deck_marks_08.mesh", lambda: deck_marks(8), {"pivot": "ground", "forward": "+Z"}),
    ("deck_marks_09.mesh", lambda: deck_marks(9), {"pivot": "ground", "forward": "+Z"}),
    ("relay_tower.mesh", relay_tower, {"pivot": "ground", "front": "-Z",
                                      "collision_note": "Place behind the court wall; dish convex hull fills the bowl."}),
    ("cooling_stack.mesh", cooling_stack, {"pivot": "ground"}),
    ("service_trench.mesh", service_trench, {"pivot": "floor", "length_axis": "Z"}),
    ("service_elbow.mesh", service_elbow, {"pivot": "ground", "bend": "+X to +Z"}),
    ("sector_07.mesh", lambda: sector_sign(7), {"pivot": "sign bottom", "front": "-Z"}),
    ("sector_08.mesh", lambda: sector_sign(8), {"pivot": "sign bottom", "front": "-Z"}),
    ("sector_09.mesh", lambda: sector_sign(9), {"pivot": "sign bottom", "front": "-Z"}),
    ("rear_plinth.mesh", rear_plinth, {"pivot": "court origin", "top_y": -0.14}),
    ("court_margin_side.mesh", lambda: court_margin("side"), {"pivot": "ground", "inner_edge_x": -0.5}),
    ("court_margin_front.mesh", lambda: court_margin("front"), {"pivot": "ground", "inner_edge_z": 0.5}),
    ("court_margin_rear.mesh", lambda: court_margin("rear"), {"pivot": "ground", "inner_edge_z": -0.75}),
]


def validate(m, name):
    assert m.vertices and m.groups, name
    for v in m.vertices:
        assert len(v) == 11 and all(math.isfinite(c) for c in v), name
        assert abs(dot(v[3:6], v[3:6])-1) < 1e-5, name
        assert abs(dot(v[8:11], v[8:11])-1) < 1e-5, name
        assert abs(dot(v[3:6], v[8:11])) < 1e-5, name
    for material, ids in m.groups.items():
        assert material.removeprefix("Crossfire/") in PALETTE, material
        for offset in range(0, len(ids), 3):
            a, b, c = [m.vertices[ids[offset+i]] for i in range(3)]
            face = cross(tuple(b[i]-a[i] for i in range(3)),
                         tuple(c[i]-a[i] for i in range(3)))
            assert dot(face, face) > 1e-14, (name, material, offset, "degenerate")
            assert dot(face, a[3:6]) > 0, (name, material, offset, "winding")


class Surface:
    """Small deterministic RGBA raster for technical paint/stencil source art."""
    def __init__(self, width, height, colour=(0, 0, 0, 0)):
        self.width, self.height = width, height
        self.pixels = bytearray(colour) * (width * height)

    def pixel(self, x, y, colour):
        if 0 <= x < self.width and 0 <= y < self.height:
            index = (y * self.width + x) * 4
            self.pixels[index:index+4] = bytes(colour)

    def rect(self, left, top, right, bottom, colour):
        row = bytes(colour) * max(0, min(self.width, right)-max(0, left))
        for y in range(max(0, top), min(self.height, bottom)):
            start = (y*self.width+max(0, left))*4
            self.pixels[start:start+len(row)] = row

    def line(self, a, b, width, colour):
        ax, ay = a
        bx, by = b
        length2 = (bx-ax)**2 + (by-ay)**2
        for y in range(max(0, int(min(ay, by)-width)), min(self.height, int(max(ay, by)+width+1))):
            for x in range(max(0, int(min(ax, bx)-width)), min(self.width, int(max(ax, bx)+width+1))):
                t = max(0, min(1, ((x-ax)*(bx-ax)+(y-ay)*(by-ay))/max(1, length2)))
                if (x-ax-t*(bx-ax))**2+(y-ay-t*(by-ay))**2 <= width*width/4:
                    self.pixel(x, y, colour)

    def ring(self, cx, cy, radius, width, colour, segments=0):
        for y in range(max(0, int(cy-radius-width)), min(self.height, int(cy+radius+width+1))):
            for x in range(max(0, int(cx-radius-width)), min(self.width, int(cx+radius+width+1))):
                r = math.hypot(x-cx, y-cy)
                if abs(r-radius) <= width/2:
                    angle = (math.atan2(y-cy, x-cx)+math.pi)/(2*math.pi)
                    if not segments or (angle*segments) % 1 < 0.79:
                        self.pixel(x, y, colour)

    def png(self):
        def chunk(tag, data):
            return struct.pack('>I', len(data))+tag+data+struct.pack('>I', zlib.crc32(tag+data) & 0xffffffff)
        stride = self.width*4
        raw = b''.join(b'\0'+self.pixels[y*stride:(y+1)*stride] for y in range(self.height))
        return (b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR', struct.pack('>IIBBBBB', self.width, self.height, 8, 6, 0, 0, 0))
                +chunk(b'IDAT', zlib.compress(raw, 9))+chunk(b'IEND', b''))


def surface_maps():
    # The floor is painted metal, not simulated PBR: broad polish/wear fields and
    # inset service-panel paint sit in albedo; existing geometry supplies depth.
    # No random noise or fine scratches: marks must survive the fixed 720p view.
    panel = Surface(512, 512)
    for y in range(512):
        for x in range(512):
            wave = 4*math.cos((x-y*.21)*math.pi/256)
            polish = 18*math.exp(-((x-167)/180)**2-((y-296)/225)**2)
            inset = 7 if min(x, y, 511-x, 511-y) < 20 else 0
            value = int(max(0, min(255, 226+wave+polish-inset)))
            panel.pixel(x, y, (value, value, value, 255))
    # Offset broad transfer/scuff strokes, visible as quiet value changes.
    for a,b,w in (((85,344),(223,318),18),((263,114),(362,101),12),((285,415),(393,395),10)):
        panel.line(a,b,w,(214,214,214,255))
    for x,y in ((40,40),(472,472)):
        panel.ring(x,y,7,4,(181,181,181,255))
    panel.rect(27,81,32,214,(248,248,248,255))
    panel.rect(82,477,209,482,(208,208,208,255))

    coat = Surface(256, 256)
    for y in range(256):
        for x in range(256):
            value = round(246+5*math.sin((x+y*.27)*math.pi/128))
            coat.pixel(x,y,(value,value,value,255))

    service = Surface(512, 512)
    ink=(176,173,141,145)
    for x in (32,480):
        service.line((x,38),(x,138),12,ink)
        service.line((x,374),(x,474),12,ink)
    for y in (38,474):
        service.line((32,y),(140,y),12,ink)
        service.line((372,y),(480,y),12,ink)
    # A bolted repair patch and two broad tyre traces give the bay a purpose.
    service.rect(134,146,358,342,(128,132,119,72))
    for x in (151,341):
        for y in (163,325): service.ring(x,y,7,4,(167,167,146,140))
    service.line((174,79),(198,434),27,(16,25,29,67))
    service.line((324,65),(348,421),27,(16,25,29,67))
    for i in range(3): service.rect(210+i*33,50,230+i*33,72,(194,172,104,167))

    coolant = Surface(512, 512)
    coolant.ring(256,256,210,13,(91,143,143,140))
    coolant.ring(256,256,187,5,(92,121,124,110),12)
    for x in (222,290): coolant.line((x,24),(x,110),13,(120,167,163,170))
    for y in (416,445,474): coolant.rect(231,y,281,y+10,(122,160,151,150))
    for x in (46,446):
        coolant.line((x,194),(x,318),10,(177,188,168,145))

    interlock = Surface(768, 256)
    ink=(155,169,187,145)
    for y in (87,169):
        interlock.line((30,y),(291,y),10,ink)
        interlock.line((477,y),(738,y),10,ink)
    for x in (290,478):
        interlock.line((x,62),(x,194),12,ink)
    for x in (326,384,442): interlock.rect(x-12,102,x+12,154,(137,155,177,148))
    for x in (65,705): interlock.ring(x,128,47,8,(162,172,177,120))

    strip = Surface(512,64)
    # A dark housing is deliberately visible in every state; the four broad
    # segments change colour without adding a fake light pool on the floor.
    strip.rect(6,5,506,59,(13,24,28,230))
    for x in (17,140,263,386): strip.rect(x,14,x+109,50,(255,255,255,255))
    column = Surface(64,512)
    for y in range(512):
        for x in range(64):
            start=(x*512+y)*4
            column.pixel(x,y,strip.pixels[start:start+4])
    ring_map=Surface(256,256)
    ring_map.ring(128,128,112,20,(255,255,255,255),12)
    return {'deck_panel.png':panel, 'coating.png':coat, 'service_pad.png':service,
            'coolant_pad.png':coolant, 'interlock_bus.png':interlock,
            'power_strip.png':strip, 'power_column.png':column, 'power_ring.png':ring_map}


OVERLAYS = {
    'ServicePad': ('service_pad.png', (1,1,1,1)),
    'CoolantPad': ('coolant_pad.png', (1,1,1,1)),
    'InterlockBus': ('interlock_bus.png', (1,1,1,1)),
    'PowerIdle': ('power_strip.png', (.47,.34,.16,1)),
    'PowerOnline': ('power_strip.png', (.18,.92,.73,1)),
    'PowerOffline': ('power_strip.png', (.12,.16,.18,1)),
    'ColumnIdle': ('power_column.png', (.47,.34,.16,1)),
    'ColumnOnline': ('power_column.png', (.18,.92,.73,1)),
    'ColumnOffline': ('power_column.png', (.12,.16,.18,1)),
    'CoreIdle': ('power_ring.png', (.43,.32,.15,1)),
    'CoreOnline': ('power_ring.png', (.19,.89,.72,1)),
    'CoreOffline': ('power_ring.png', (.10,.14,.16,1)),
}


def material_text():
    lines = ["// Original Crossfire palette and deterministic project-authored surface maps.",
             "// Lit albedo uses base_material; visual-only stencils/status use relay_overlay.",
             'import base_material from "materials/base.material"', ""]
    for name, (colour, specular, shininess) in PALETTE.items():
        lines += ["material " + mat(name) + " : base_material", "{",
                  '\tset $diffuseCol "' + " ".join(str(x) for x in colour) + ' 1"',
                  '\tset $specularCol "' + " ".join(str(x) for x in specular) + '"',
                  '\tset $shininess "' + str(shininess) + '"']
        if name in ("FloorPanel", "DeckService", "DeckCoolant", "DeckInterlock"):
            lines += ['\tset_texture_alias diffuseMap textures/crossfire/deck_panel.png']
        elif name in ("Porcelain", "Ivory"):
            lines += ['\tset_texture_alias diffuseMap textures/crossfire/coating.png']
        lines += ["}", ""]
    for name, (texture, tint) in OVERLAYS.items():
        lines += ["material " + mat(name), "{", "\treceive_shadows off", "\ttechnique", "\t{", "\t\tpass", "\t\t{",
                  "\t\t\tlighting off", "\t\t\tcull_hardware none", "\t\t\tscene_blend alpha_blend",
                  "\t\t\tdepth_check on", "\t\t\tdepth_write off", "\t\t\tvertex_program_ref relay_overlay_vs {}",
                  "\t\t\tfragment_program_ref relay_overlay_fs", "\t\t\t{",
                  "\t\t\t\tparam_named tint float4 " + " ".join(str(x) for x in tint), "\t\t\t}",
                  "\t\t\ttexture_unit overlayMap", "\t\t\t{", "\t\t\t\ttexture textures/crossfire/"+texture,
                  "\t\t\t\ttex_address_mode clamp", "\t\t\t\tfiltering trilinear", "\t\t\t}", "\t\t}", "\t}", "}", ""]
    return "\n".join(lines)


def write_text(path, text):
    path.write_bytes(text.replace("\r\n", "\n").replace("\n", "\r\n").encode("utf-8"))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="Validate geometry and compare deterministic committed outputs without changing them.")
    args = parser.parse_args()
    import tempfile
    with tempfile.TemporaryDirectory(prefix="crossfire-assets-") as folder:
        serializer.OUT = Path(folder) if args.check else OUT
        serializer.OUT.mkdir(parents=True, exist_ok=True)
        manifest = []
        for filename, builder, metadata in BUILDERS:
            mesh = builder()
            validate(mesh, filename)
            info = mesh.write(filename)
            info.update(metadata)
            info["size"] = [round(info["bounds_max"][i]-info["bounds_min"][i], 6) for i in range(3)]
            info["bounds_min"] = [round(x, 6) for x in info["bounds_min"]]
            info["bounds_max"] = [round(x, 6) for x in info["bounds_max"]]
            info["sha256"] = hashlib.sha256((serializer.OUT / filename).read_bytes()).hexdigest()
            manifest.append(info)
            if args.check:
                assert (OUT / filename).read_bytes() == (serializer.OUT / filename).read_bytes(), filename
            print(f"{filename}: {info['size']} m, {info['triangles']} triangles")
        textures = []
        if not args.check:
            TEXTURES.mkdir(parents=True, exist_ok=True)
        for filename, surface in surface_maps().items():
            data = surface.png()
            if args.check:
                assert (TEXTURES/filename).read_bytes() == data, filename
            else:
                (TEXTURES/filename).write_bytes(data)
            textures.append({"file": "textures/crossfire/"+filename, "width": surface.width,
                             "height": surface.height, "sha256": hashlib.sha256(data).hexdigest()})
        # Verify material-only floor variants cannot alter the collision source.
        original = tile()
        for name in ("DeckService", "DeckCoolant", "DeckInterlock"):
            variant = tile_variant(name)
            assert variant.vertices == original.vertices
            assert list(variant.groups.values()) == list(original.groups.values())
        manifest_text = json.dumps({"generator": "tools/generate_crossfire_assets.py", "units": "metres",
                                    "assets": manifest, "surfaces": textures}, indent=2) + "\n"
        if args.check:
            assert (OUT / "manifest.json").read_text() == manifest_text, "manifest.json"
            assert (ROOT / "media/materials/crossfire.material").read_text() == material_text(), "crossfire.material"
            print("PASS: geometry, winding, material references, unchanged tile collision geometry and deterministic meshes/surface PNGs.")
        else:
            write_text(OUT / "manifest.json", manifest_text)
            write_text(ROOT / "media/materials/crossfire.material", material_text())


if __name__ == "__main__":
    main()
