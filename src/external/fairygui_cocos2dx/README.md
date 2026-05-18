# FairyGUI-cocos2dx Vendor

This directory contains the upstream `fairygui/FairyGUI-cocos2dx` source used by
the HelloOgre3D FairyGUI integration.

The integration plan intentionally keeps upstream FairyGUI code separate from the
HelloOgre3D runtime adapter:

- upstream source: `src/external/fairygui_cocos2dx`
- Ogre/cocos-lite adapter: `src/HelloOgre3D/runtime/ui/fairygui`

Do not copy env1 MiniUI source here. MiniUI is only used as an implementation
reference for renderer, input, and resource bridge behavior.

Vendored upstream content:

- `libfairygui`: official FairyGUI Cocos2d-x SDK source.
- `LICENSE`: upstream MIT license.
- `UPSTREAM_README.md`: upstream README snapshot.
