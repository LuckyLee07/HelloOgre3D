# Crossfire mechanical feedback

Nine original synthesized short sounds, with deterministic source in
`tools/generate_crossfire_audio.py`. No source recordings, music samples or
third-party assets are used. These project-authored assets may be reused with
this project. Format: 44.1 kHz mono PCM16 WAV; 0.19–1.52 seconds per sound.

| File | Audible construction | Suggested priority |
|---|---|---:|
| select.wav | Small relay latch and metallic tail | 70 |
| order.wav | Two mechanical latches with a rising resonant confirmation | 75 |
| shot.wav | Pressure crack, low body, delayed mechanism/casing tail; one round | 20 |
| shield.wav | Dispersed glass/field resonance with a high-frequency decay | 30 |
| hit.wav | Hard plate impact, low body and loose metal | 30 |
| destroy.wav | Mechanical rupture, falling motor and staggered debris | 50 |
| start.wav | Power-up motor, latches and resonant readiness sequence | 90 |
| win.wav | Warm resonant completion chord and mechanical release | 100 |
| lose.wav | Motor spin-down and closing latch | 100 |

The assets are intentionally distinct in attack, resonance and decay. They are
an authored first pass; audible quality and game mix still require listening in
the final scene. They do not supply a continuous ambience or music track.

## Runtime use

```lua
SandboxAudio:SetVolume(0.65)
SandboxAudio:PlayLayer("res/audio/crossfire/shot.wav", 0.65, -0.3, 20)
SandboxAudio:PlayLayer("res/audio/crossfire/shield.wav", 0.60, 0.3, 30)
SandboxAudio:PlayLayer("res/audio/crossfire/order.wav", 0.70, 0, 75)
```

`PlayLayer(path, gain, pan, priority)` uses `gain` 0–1, `pan` -1 (left) through
0 (centre) to +1 (right), and integer `priority` 0–100. Compute distance gain in
the sample and pass it here. Pan uses stereo balance; this is not positional 3D
audio. There are at most eight voices. At capacity, the oldest voice at the
lowest priority is replaced only if the new sound has at least that priority.
A false result can mean normal priority rejection as well as an invalid WAV or
backend failure; do not disable the whole audio service on one rejected layer.

`Play(path)` retains its legacy exclusive behavior, replacing preceding sounds.
`StopAll()` stops every voice. `SetVolume` changes active voices, and zero stops
all of them; silent calls succeed without opening an audio device. No native
handles or callbacks are owned by Lua. Calls are made on the game/main thread.

macOS uses one NSSound per voice; its owner releases the sound on completion.
Windows uses dynamically loaded waveOut functions with separate device handles,
no audio callback into game state and no new link dependency. Completed voice
slots/PCM are reclaimed on the next play, volume change or stop; storage is
bounded by eight slots. The master volume uses instance handles, not the system
volume. On a Windows device lacking per-instance volume support, playback fails
rather than bypassing the master setting. No Windows runtime claim is implied
by the macOS checks.

Use `python3 tools/generate_crossfire_audio.py --check` to verify reproducibility.
The individual peaks leave headroom but there is no software bus limiter; keep
combat gains conservative and evaluate simultaneous fire during final mixing.
