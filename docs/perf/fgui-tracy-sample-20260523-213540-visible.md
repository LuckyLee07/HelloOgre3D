# FGUI Tracy Sample 20260523-213540 Visible

- Mode: Pressure
- Count: 2
- ListCount: 20
- Seconds: 45
- Run mode: visible window
- Result: PASS
- Command: `tools\run_fgui_tracy_sample.ps1 -Count 2 -ListCount 20 -Seconds 45 -Tail 140 -Visible`
- Tracy viewer: tools\tracy-viewer

## Extract

```text
21:35:05: Lua:[fgui_init.lua:1883][FGUI] pressure stats: open popup= 1 list= 20 openUI= 2 hiddenUI= 0 binding= 8 timer= 0 objectHandle= 2 childCache= 200 packageRef= 1 warnings= 0 command= 0 triangle= 0 draw= 0/0 switch= 0/0 clip= 0/0 cpuClip= 0/0/0 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 0/0 material= 1 texture= 1 materialAlias= 1 textureAlias= 1 openPerf= 3 closePerf= 1 act_42_qingluanact:ref=1,open=1|<dynamic>:ref=0,open=1
21:35:06: Lua:[fgui_init.lua:1883][FGUI] pressure stats: rendered popup= 1 list= 20 openUI= 2 hiddenUI= 0 binding= 8 timer= 0 objectHandle= 2 childCache= 200 packageRef= 1 warnings= 0 command= 106 triangle= 212 draw= 105/215 switch= 1/1 clip= 101/2 cpuClip= 202/205/200 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 3/9 material= 3 texture= 3 materialAlias= 3 textureAlias= 3 openPerf= 3 closePerf= 1 act_42_qingluanact:ref=1,open=1|<dynamic>:ref=0,open=1
21:35:06: Lua:[fgui_init.lua:2424][FGUI] pressure case: 1 PASS openUI=0 hiddenUI=0 binding=0 timer=0 objectHandle=0 childCache=0 view=0 controller=0 packageRef=0 warnings=0
21:35:07: Lua:[fgui_init.lua:1883][FGUI] pressure stats: open popup= 2 list= 20 openUI= 3 hiddenUI= 0 binding= 9 timer= 0 objectHandle= 3 childCache= 200 packageRef= 1 warnings= 0 command= 0 triangle= 0 draw= 0/0 switch= 0/0 clip= 0/0 cpuClip= 0/0/0 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 0/0 material= 3 texture= 3 materialAlias= 3 textureAlias= 3 openPerf= 4 closePerf= 3 act_42_qingluanact:ref=1,open=1|<dynamic>:ref=0,open=2
21:35:09: Lua:[fgui_init.lua:1883][FGUI] pressure stats: rendered popup= 2 list= 20 openUI= 3 hiddenUI= 0 binding= 9 timer= 0 objectHandle= 3 childCache= 200 packageRef= 1 warnings= 0 command= 107 triangle= 214 draw= 106/217 switch= 1/1 clip= 101/2 cpuClip= 202/205/200 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 3/9 material= 3 texture= 3 materialAlias= 3 textureAlias= 3 openPerf= 4 closePerf= 3 act_42_qingluanact:ref=1,open=1|<dynamic>:ref=0,open=2
21:35:09: Lua:[fgui_init.lua:2424][FGUI] pressure case: 2 PASS openUI=0 hiddenUI=0 binding=0 timer=0 objectHandle=0 childCache=0 view=0 controller=0 packageRef=0 warnings=0
21:35:09: Lua:[fgui_init.lua:2482][FGUI] pressure self test end: 2 / 2 finalClean= true openUI=0 hiddenUI=0 binding=0 timer=0 objectHandle=0 childCache=0 view=0 controller=0 packageRef=0 warnings=0
21:35:09: Lua:[fgui_init.lua:2483][FGUI] pressure self test result: true
21:35:09: Lua:[FairyGuiProfiler.lua:430][FGUI] PerfStat open count= 4 success= 4 fail= 0 avgMs= 29.500 maxMs= 57.000 lastMs= 15.000 last= Act38Test max= Act38Test
21:35:09: Lua:[FairyGuiProfiler.lua:430][FGUI] PerfStat close count= 6 success= 6 fail= 0 avgMs= 10.167 maxMs= 24.000 lastMs= 2.000 last= __PressurePopup1 max= __PressureAct38
21:35:09: Lua:[FairyGuiProfiler.lua:430][FGUI] PerfStat event count= 0 success= 0 fail= 0 avgMs= 0.000 maxMs= 0.000 lastMs= 0.000 last=  max=
21:35:09: Lua:[FairyGuiProfiler.lua:430][FGUI] PerfStat loadPackage count= 2 success= 2 fail= 0 avgMs= 5.500 maxMs= 6.000 lastMs= 6.000 last= res/fuires/act_38_test max= res/fuires/act_38_test
21:35:09: Lua:[FairyGuiProfiler.lua:430][FGUI] PerfStat createObject count= 3 success= 3 fail= 0 avgMs= 9.333 maxMs= 13.000 lastMs= 2.000 last= act_42_qingluanact/QingLuanActMain max= act_37_Dress_up_collection/main_pifushoumai
21:35:09: Lua:[FairyGuiProfiler.lua:430][FGUI] PerfStat service count= 3 success= 3 fail= 0 avgMs= 1.000 maxMs= 2.000 lastMs= 0.000 last= PressurePopup max= PressurePopup
```
