## HelloOgre3D ##

目标：通过重构<Lua游戏AI开发指南>一书中的案例，将游戏开发中的一些经验沉淀下来


1、剥离出最基础的ogre3d的依赖库，挪到src/external目录下

2、新增HelloOgre3D项目，用纯C++实现一些ogre3d相关的内容

3、新增lua/tolua库，编写基础C++类并进行tolua

4、以tolua的形式实现原书chapter_1_introduction的内容
(0)搭建新框架
(1)tolua场景创建相关的对象及接口
(2)加入bullet物理世界
(3)加入UI显示系统
(4)加入输入响应系统

5、完成原书chapter_2_introduction的内容

6、内存泄露问题查询 -- 5/22 内存问题早查明是由于tolua对象没释放导致，使用gc释放即可
但由于最近在忙暑期版本天天加班太累，所以停了段时间没更新

7、新增个lua线程缓存池，以备需要之时

8、加入luasocket库及LuaPanda插件实现vscode中的lua断点调试功能

2024-08-10
7月末公司项目发布暑期版本，这段忙碌的时间终于过去，后续会继续补上新的内容


2025-03-01
8、新增LuaPluginMgr用来将C++对象与指定lua文件绑定
实现要点及总结：
(1)lua_pcall时narg参数要算好，否则会导致call错，特别是pcall带表函数时
(2)lua调用C++函数传递table时，要手动算好参数个数，直接用tolua++不行
(3)lua_pushvalue(m_pState, -2);  // 将 table 复制一份压栈（self）
   lua_setfenv(m_pState, -2);    // 不需要self即不带冒号调用时 设置函数环境为 table


8、使用C++实现有限状态机 -- 即第3章角色动画的内容


