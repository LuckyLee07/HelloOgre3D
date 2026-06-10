import argparse
import json
import math
from pathlib import Path

try:
    import numpy as np
    from PIL import Image, ImageChops, ImageDraw
except ImportError as exc:
    raise SystemExit("missing image analysis dependency: {}".format(exc))


TRACE_PREFIX = "[ParityTrace] "


def parse_capture_times(value):
    if not value:
        return []
    result = []
    for item in value:
        for token in str(item).replace(",", " ").split():
            if token:
                result.append(int(token))
    return sorted(set(result))


def discover_capture_times(capture_dir):
    old_dir = capture_dir / "old"
    result = []
    for path in old_dir.glob("old_*ms.png"):
        name = path.stem
        try:
            result.append(int(name[len("old_"):-len("ms")]))
        except ValueError:
            pass
    return sorted(set(result))


def load_rgb(path):
    return np.asarray(Image.open(path).convert("RGB")).astype(np.int16)


def build_world_mask(shape):
    height, width = shape[:2]
    mask = np.zeros((height, width), dtype=bool)
    # Exclude the right-side help panel and the bottom status text. This keeps
    # the metric focused on terrain, agents, and influence-map visuals.
    mask[0:int(height * 0.94), 0:int(width * 0.73)] = True
    return mask


def count_debug_colors(image, mask):
    red = image[:, :, 0]
    green = image[:, :, 1]
    blue = image[:, :, 2]
    red_count = ((red > 120) & (red > green * 1.35) & (red > blue * 1.35) & mask).sum()
    blue_count = ((blue > 100) & (blue > red * 1.25) & (blue > green * 1.25) & mask).sum()
    return int(red_count), int(blue_count)


def collect_visual_stats(capture_dir, capture_times):
    stats = []
    for capture_time in capture_times:
        old_path = capture_dir / "old" / "old_{:05d}ms.png".format(capture_time)
        new_path = capture_dir / "new" / "new_{:05d}ms.png".format(capture_time)
        old_image = load_rgb(old_path)
        new_image = load_rgb(new_path)
        world_mask = build_world_mask(old_image.shape)
        diff = np.abs(old_image - new_image)
        changed = diff.max(axis=2) > 16
        old_red, old_blue = count_debug_colors(old_image, world_mask)
        new_red, new_blue = count_debug_colors(new_image, world_mask)
        stats.append({
            "time_ms": capture_time,
            "changed_pixels_pct": round(float(changed.mean() * 100.0), 2),
            "changed_pixels_world_pct": round(float(changed[world_mask].mean() * 100.0), 2),
            "mean_abs_rgb": round(float(diff.mean()), 2),
            "mean_abs_rgb_world": round(float(diff[world_mask].mean()), 2),
            "old_red_world": old_red,
            "new_red_world": new_red,
            "old_blue_world": old_blue,
            "new_blue_world": new_blue,
            "red_delta_pct_new_vs_old": round((new_red - old_red) * 100.0 / max(old_red, 1), 2),
            "blue_delta_pct_new_vs_old": round((new_blue - old_blue) * 100.0 / max(old_blue, 1), 2),
        })
    return stats


def read_trace(path):
    records = []
    if not path.exists():
        return records
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = line.strip()
        if not line:
            continue
        if line.startswith(TRACE_PREFIX):
            line = line[len(TRACE_PREFIX):]
        try:
            records.append(json.loads(line))
        except ValueError:
            pass
    return records


def get_agent_position(agent):
    value = agent.get("position") or agent.get("pos")
    if isinstance(value, dict):
        return float(value.get("x", 0.0)), float(value.get("y", 0.0)), float(value.get("z", 0.0))
    if isinstance(value, list) and len(value) >= 3:
        return float(value[0]), float(value[1]), float(value[2])
    return 0.0, 0.0, 0.0


def compare_traces(capture_dir):
    legacy_records = read_trace(capture_dir / "legacy_trace.jsonl")
    modern_records = read_trace(capture_dir / "modern_trace.jsonl")
    legacy_samples = [record for record in legacy_records if record.get("type") == "sample"]
    modern_samples = [record for record in modern_records if record.get("type") == "sample"]
    samples = []

    for sample_index, pair in enumerate(zip(legacy_samples, modern_samples), 1):
        legacy, modern = pair
        legacy_agents = {
            agent.get("index"): agent
            for agent in legacy.get("agents", [])
            if isinstance(agent, dict)
        }
        modern_agents = {
            agent.get("index"): agent
            for agent in modern.get("agents", [])
            if isinstance(agent, dict)
        }
        agent_deltas = []
        max_distance = 0.0
        distance_sum = 0.0
        distance_count = 0
        for index, legacy_agent in legacy_agents.items():
            modern_agent = modern_agents.get(index)
            if modern_agent is None:
                continue
            old_x, old_y, old_z = get_agent_position(legacy_agent)
            new_x, new_y, new_z = get_agent_position(modern_agent)
            xz_distance = math.hypot(old_x - new_x, old_z - new_z)
            max_distance = max(max_distance, xz_distance)
            distance_sum += xz_distance
            distance_count += 1
            agent_deltas.append({
                "index": index,
                "posDelta": [
                    round(new_x - old_x, 3),
                    round(new_y - old_y, 3),
                    round(new_z - old_z, 3),
                ],
                "xzDistance": round(xz_distance, 3),
                "oldSpeed": legacy_agent.get("speed"),
                "newSpeed": modern_agent.get("speed"),
                "newAction": modern_agent.get("legacyAction") or (modern_agent.get("extra") or {}).get("legacyAction"),
            })

        legacy_tactics = legacy.get("tactics") or {}
        modern_tactics = modern.get("tactics") or {}
        samples.append({
            "sampleIndex": sample_index,
            "old_tMs": legacy.get("tMs"),
            "new_tMs": modern.get("tMs"),
            "frameDelta": (modern.get("frame") or 0) - (legacy.get("frame") or 0),
            "maxAgentXzDistance": round(max_distance, 3),
            "avgAgentXzDistance": round(distance_sum / max(distance_count, 1), 3),
            "agentDeltas": agent_deltas,
            "oldEvents": legacy_tactics.get("events"),
            "newEvents": modern_tactics.get("events"),
            "newDangerCells": modern_tactics.get("dangerCells"),
            "newTeamCells": modern_tactics.get("teamCells"),
        })

    return {
        "legacy_records": len(legacy_records),
        "modern_records": len(modern_records),
        "legacy_samples": len(legacy_samples),
        "modern_samples": len(modern_samples),
        "legacy_last_type": legacy_records[-1].get("type") if legacy_records else None,
        "modern_last_type": modern_records[-1].get("type") if modern_records else None,
        "last_legacy_tMs": legacy_samples[-1].get("tMs") if legacy_samples else None,
        "last_modern_tMs": modern_samples[-1].get("tMs") if modern_samples else None,
        "samples": samples,
    }


def write_compare_images(capture_dir, capture_times):
    analysis_dir = capture_dir / "analysis"
    analysis_dir.mkdir(parents=True, exist_ok=True)
    cell_width = 360
    cell_height = 230
    sheet = Image.new("RGB", (cell_width * 3, (cell_height + 28) * len(capture_times)), "white")
    draw = ImageDraw.Draw(sheet)

    for row, capture_time in enumerate(capture_times):
        y = row * (cell_height + 28)
        old_image = Image.open(capture_dir / "old" / "old_{:05d}ms.png".format(capture_time)).convert("RGB")
        new_image = Image.open(capture_dir / "new" / "new_{:05d}ms.png".format(capture_time)).convert("RGB")
        diff_image = ImageChops.difference(old_image, new_image)

        for column, image, label in ((0, old_image, "old"), (1, new_image, "new"), (2, diff_image, "diff")):
            sheet.paste(image.resize((cell_width, cell_height)), (column * cell_width, y + 28))
            draw.text((column * cell_width + 6, y + 7), "{}ms {}".format(capture_time, label), fill=(0, 0, 0))

        strip = Image.new("RGB", (old_image.width * 3, old_image.height), "white")
        strip.paste(old_image, (0, 0))
        strip.paste(new_image, (old_image.width, 0))
        strip.paste(diff_image, (old_image.width * 2, 0))
        strip.save(analysis_dir / "compare_{:05d}ms.png".format(capture_time))

    sheet.save(analysis_dir / "chapter9_visual_compare_sheet.png")


def write_json(path, value):
    path.write_text(json.dumps(value, indent=2, ensure_ascii=False), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--capture-dir", required=True)
    parser.add_argument("--capture-ms", nargs="*", default=[])
    args = parser.parse_args()

    capture_dir = Path(args.capture_dir)
    capture_times = parse_capture_times(args.capture_ms)
    if not capture_times:
        capture_times = discover_capture_times(capture_dir)
    if not capture_times:
        raise SystemExit("no capture images found in {}".format(capture_dir))

    analysis_dir = capture_dir / "analysis"
    analysis_dir.mkdir(parents=True, exist_ok=True)
    visual_stats = collect_visual_stats(capture_dir, capture_times)
    trace_stats = compare_traces(capture_dir)
    write_compare_images(capture_dir, capture_times)
    write_json(analysis_dir / "visual_stats.json", visual_stats)
    write_json(analysis_dir / "trace_compare.json", trace_stats)
    print("[CH9_ANALYZE] analysisDir={}".format(analysis_dir))
    print("[CH9_ANALYZE] visualStats={}".format(analysis_dir / "visual_stats.json"))
    print("[CH9_ANALYZE] traceCompare={}".format(analysis_dir / "trace_compare.json"))
    print("[CH9_ANALYZE] compareSheet={}".format(analysis_dir / "chapter9_visual_compare_sheet.png"))


if __name__ == "__main__":
    main()
