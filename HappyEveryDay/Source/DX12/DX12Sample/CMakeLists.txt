
########################################################################################
#################### !!! this is exe makefile !!! ######################################
########################################################################################


#设置项目名称
set (ExeName "DX12Sample")

#静态连接库, 连接器->输入->附加依赖项
set(LibItems
		"d3d12.lib"
		"dxgi.lib"
		"d3dcompiler.lib"
	)

#针对该exe的宏定义
set(LocalDefineList
		"UNICODE"
		"_UNICODE"
	)
################这里面所有路径都要填相对路径, 相对于当前目录所在的路径 begin ##########################

#设置Include目录, Properties->C/C++->General->Additional Include Directories
set(IncludeDirs 
		"."
	)

#静态连接库,  连接器->常规->附加库目录
set(LibDirs 
	"${RootDir}/Libs"
	"${SlnAutoGenLibsDir}"
	)

#设置Output目录, Properties->General->Output Directory
#静态文件一般都输出到和sln同级的Libs目录中.
set(OutputDir "${RootDir}/Binaries")

################这里面所有路径都要填相对路径, 相对于当前目录所在的路径  end  ##########################


GenEXE(
		"${ExeName}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${IncludeDirs}"
		"${LibDirs}"
		"${OutputDir}"
		"${LibItems}"
		"${LocalDefineList}"
	  )

#使用Unicode
add_definitions(-DUNICODE -D_UNICODE)
