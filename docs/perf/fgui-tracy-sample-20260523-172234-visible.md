# FGUI Tracy Sample 20260523-172234 Visible

- Mode: Pressure
- Count: 2
- ListCount: 20
- Seconds: 45
- Run mode: visible window
- Note: hidden/nested PowerShell launches intermittently failed in this desktop session with D3D9 `Cannot create device`; visible selftest completed and is used as the sampling record.

## Extract

```text
[FGUI] pressure self test begin: 1,2 list= 20
[FGUI] pressure stats: open popup= 1 list= 20 openUI= 2 hiddenUI= 0 binding= 8 timer= 0 objectHandle= 2 childCache= 200 packageRef= 1 warnings= 0 command= 0 triangle= 0 draw= 0/0 switch= 0/0 clip= 0/0 cpuClip= 0/0/0 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 0/0 material= 1 texture= 1 materialAlias= 1 textureAlias= 1 openPerf= 3 closePerf= 1
[FGUI] pressure stats: rendered popup= 1 list= 20 openUI= 2 hiddenUI= 0 binding= 8 timer= 0 objectHandle= 2 childCache= 200 packageRef= 1 warnings= 0 command= 106 triangle= 212 draw= 105/215 switch= 1/1 clip= 101/2 cpuClip= 202/205/200 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 3/9 material= 3 texture= 3 materialAlias= 3 textureAlias= 3 openPerf= 3 closePerf= 1
[FGUI] pressure case: 1 PASS openUI=0 hiddenUI=0 binding=0 timer=0 objectHandle=0 childCache=0 view=0 controller=0 packageRef=0 warnings=0
[FGUI] pressure stats: open popup= 2 list= 20 openUI= 3 hiddenUI= 0 binding= 9 timer= 0 objectHandle= 3 childCache= 200 packageRef= 1 warnings= 0 command= 0 triangle= 0 draw= 0/0 switch= 0/0 clip= 0/0 cpuClip= 0/0/0 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 0/0 material= 3 texture= 3 materialAlias= 3 textureAlias= 3 openPerf= 4 closePerf= 3
[FGUI] pressure stats: rendered popup= 2 list= 20 openUI= 3 hiddenUI= 0 binding= 9 timer= 0 objectHandle= 3 childCache= 200 packageRef= 1 warnings= 0 command= 107 triangle= 214 draw= 106/217 switch= 1/1 clip= 101/2 cpuClip= 202/205/200 stencil= 0/0 stencilClip= 0/0 backend= shapeCpu maxBatch= 3/9 material= 3 texture= 3 materialAlias= 3 textureAlias= 3 openPerf= 4 closePerf= 3
[FGUI] pressure case: 2 PASS openUI=0 hiddenUI=0 binding=0 timer=0 objectHandle=0 childCache=0 view=0 controller=0 packageRef=0 warnings=0
[FGUI] pressure self test end: 2 / 2 finalClean= true openUI=0 hiddenUI=0 binding=0 timer=0 objectHandle=0 childCache=0 view=0 controller=0 packageRef=0 warnings=0
[FGUI] pressure self test result: true
[FGUI] PerfStat open count= 4 success= 4 fail= 0 avgMs= 20.500 maxMs= 32.000 lastMs= 14.000 last= Act38Test max= Act37TestMvc
[FGUI] PerfStat close count= 6 success= 6 fail= 0 avgMs= 9.000 maxMs= 22.000 lastMs= 0.000 last= __PressurePopup1 max= __PressureAct38
[FGUI] PerfStat loadPackage count= 2 success= 2 fail= 0 avgMs= 2.000 maxMs= 2.000 lastMs= 2.000 last= res/fuires/act_38_test max= res/fuires/act_37_test
[FGUI] PerfStat createObject count= 3 success= 3 fail= 0 avgMs= 7.333 maxMs= 17.000 lastMs= 2.000 last= act_42_qingluanact/QingLuanActMain max= act_37_Dress_up_collection/main_pifushoumai
[FGUI] PerfStat service count= 3 success= 3 fail= 0 avgMs= 0.667 maxMs= 1.000 lastMs= 0.000 last= PressurePopup max= PressurePopup
```
