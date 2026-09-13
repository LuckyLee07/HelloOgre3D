# Sandbox19 自制实体资产

日期：2026-09-12；2026-09-13 追加短掩体。生成器：[generate_relay_meshes.py](../../../tools/generate_relay_meshes.py)。

四张网格均由本项目脚本直接构造顶点、法线、切线、UV 和三角形，未使用外部模型或品牌。运行 `python3 tools/generate_relay_meshes.py` 可重建；`manifest.json` 记录三角形数、材质和实际边界。Ogre v1.100 二进制格式依据仓库 vendored `OgreMeshFileFormat.h`，无运行时生成依赖。

- `relay_barrier.mesh`：4.20×1.25×1.154m；混凝土主体倒角 8.5cm，细小拉结孔为独立材质。使用现有项目混凝土 diffuse/normal，来源见 [表面资源](../../textures/sandbox19/SOURCE.md)。
- `relay_barrier_short.mesh`：2.40×1.25×1.154m；同一生成器的短掩体变体，放在院区敌人后侧，保留中央直达路线。
- `relay_supply_crate.mesh`：1.65×1.05×1.104m。
- `relay_supply_crate_tall.mesh`：1.65×1.65×1.104m。两种箱体均有倒角主体、盖、密封条、加强筋、脚垫、锁扣、把手与简化 ID 条。涂层使用项目生成 `supply_case_wear_v1.png`。

Y 向上，米制，中心 pivot，静态实体。Lua 保留多 submesh 材质；既有 `CreateBlockObject` 从同一组网格顶点生成 Bullet 简化凸包，作为封闭箱体/掩体的碰撞体；细小凹缝不具有独立可穿透语义。场景与 navmesh 按真实变换读取，ObjectManager 负责销毁。初版运行验收见 [场景资产计划](../../../docs/dev-design/plans/2026-09-12-sandbox19-scene-assets.md)，短掩体与院区布局验收见 [构图续办](../../../docs/dev-design/plans/2026-09-13-sandbox19-visual-composition.md)。
