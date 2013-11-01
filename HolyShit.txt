HolyShit

features 功能:
1.功能同步支持OD1.10和OD2.01h
2.在汇编窗口增加一栏，显示label(包含dll的export信息)
3.使OD可以加载sys和ntoskrnl.exe文件
4.非常灵活toolbar功能

note 说明:
版本对应文件：
OD1.10  -> holyshit110.dll
OD2.01 -> holyshit201.dll
loadsys.exe需要放在ollydbg.exe同级目录，其它放plugins目录

toolbar内置命令：
toolbar默认是调用shell，以call:开头就是调用内置命令
格式：call:内置命令,参数1,参数2,参数3...
当前所有内置命令：
explorer，不需要参数，打开当前调试程序的目录
Pluginmenu，参数1为调用插件的文件名，参数2为MAIN或者DISASM，参数3是点击的哪项从0起

配置：
OD配置文件的[Plugin HolyShit]节里设置：
toolbar_ini     -> toolbar的配置ini文件路径，该ini的内容设置见示例toolbar.ini文件，需重启OD生效
label_enable    -> 是否显示label栏，默认为显示，需重启OD生效
enable_jmp      -> 是否开启esc和`跳转，OD2默认开启，OD1默认不开启

changelog 更新历史:
2013年11月1日 v0.3.3，移植到OD2.01正式版，功能无改变

2013年7月15日 v0.3.2，更改sys加载方式，现在可以直接到达sys的OEP！便于驱动脱壳(暂时只处理了ExAllocatePoolWithTag和ExFreePoolWithTag)

2013年7月5日 v0.3.1，修正toolbar的一个bug，增加toolbar调用内置命令的功能，支持调用OD其他插件的功能—初步只支持
调用menu菜单项，使用范例见toolbar.ini。OD2添加esc和`的跳转(OD1也有但是默认不开启)。

2013年6月25日 v0.3，增加toolbar(不兼容IDAFicator)，label添加export函数名显示，lable显示宽度自动保存。
label支持合并到comment栏里去，但是会遇到某些地方不显示的情况，建议开启label显示，ini里可设置
增加配置。OD201H全功能同步发布。

2013年5月3日 v0.2，label显示为高亮，让OD可以加载驱动！
感谢：疯子 http://bbs.pediy.com/showthread.php?t=171729

2013年5月2日 v0.1, 显示label

by lynnux, 始于2013年5月2日
