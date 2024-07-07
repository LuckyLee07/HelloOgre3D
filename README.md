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

8、使用C++实现有限状态机

6、加入luasocket库及LuaPanda插件实现vscode中的lua断点调试功能
