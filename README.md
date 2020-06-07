# Pack
基于C的命令行打包

1.需要打包的目录是 \tmp，打包后生成的文件xy.z存放于\out，则可以用：pack  \tmp  \out\xy.z
  考虑到方便性可以使用通配符选择特定的文件打包，比如：pack  \tmp\*.c  \out\xy.z

2.能够列出打包文件中的文件。pack  –l  xy.z，默认 –l 列出包中所有文件
  如果包中文件比较多，一屏显示不下，可以分次显示，比如pack  –l25  xy.z，表示每次列出包中的25个文件

3.能够解包中的文件。pack  –u  xy.z  \my\，表示解出包中所有的文件到\my\目录中，当然xy.z可以带路径
  pack  –u6  xy.z  \my\，表示解包中第六个文件到\my\目录中
