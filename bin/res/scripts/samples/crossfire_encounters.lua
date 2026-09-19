-- Values only: authored deployments shared by scene, director and mission UI.
return {
 {id="07",name="维修庭院",subtitle="寻找侧翼",lesson="两机分路，绕过橙色护盾正面。",
  description="一座哨卫，两条路线。绕到侧面进攻。",
  allies={{x=-4,z=-8},{x=4,z=-8}},enemies={{x=0,z=6,hp=180,fx=0,fz=-1}},
  routes={{x=-5,z=5},{x=5,z=5}},parMs=22000},
 {id="08",name="冷却区",subtitle="绕开遮挡",lesson="借冷却设备掩护绕行，抵达后寻找畅通射界。",
  description="设备会挡住弹丸，先绕过设备再形成夹击。",
  allies={{x=-4,z=-8},{x=4,z=-8}},enemies={{x=3,z=6,hp=180,fx=0,fz=-1}},
  routes={{x=-5,z=6},{x=8,z=4.5}},parMs=27000},
 {id="09",name="联锁区",subtitle="交叉攻击",lesson="两机拉开护盾朝向，再分别点对侧哨卫。",
  description="两座哨卫，交叉选敌；红色射线说明中途有阻挡。",
  allies={{x=-4,z=-8},{x=4,z=-8}},
  enemies={{x=-3,z=5,hp=120,fx=0,fz=-1,chargeMs=1300,cooldownMs=1700},
           {x=3,z=7,hp=120,fx=0,fz=-1,chargeMs=1300,cooldownMs=1700}},
  routes={{x=-7,z=6},{x=7,z=4}},parMs=36000},
}
