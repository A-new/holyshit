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

by lynnux, 始于2013年5月2日
