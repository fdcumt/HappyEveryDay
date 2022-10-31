
########################################################################################
#################### !!! this is exe makefile !!! ######################################
########################################################################################


#设置项目名称
set (ExeName "LearnOpenGL")

#静态连接库, 连接器->输入->附加依赖项
set(LibItems 
	"Math"
	"Core"
	"ApplicationCore"
	"RenderCore"
	"GeneratedHeaders"
	"glfw3.lib"
	"glm"
	)

#针对该exe的宏定义
set(LocalDefineList
)


################这里面所有路径都要填相对路径, 相对于当前目录所在的路径 begin ##########################

#设置Include目录, Properties->C/C++->General->Additional Include Directories
set(IncludeDirs 
		"."
		"${RootDir}/Include"
		"Math"
		"Utility"
		"Core"
		"ApplicationCore"
		"RenderCore"
		"GeneratedHeaders"
		"glm"
	)

#静态连接库,  连接器->常规->附加库目录
set(LibDirs 
	"${RootDir}/Libs"
	"${RootDir}/Shaders"
	"${SlnAutoGenLibsDir}"
	)

#设置Output目录, Properties->General->Output Directory
#静态文件一般都输出到和sln同级的Libs目录中.
set(OutputDir "${RootDir}/Binaries")

################这里面所有路径都要填相对路径, 相对于当前目录所在的路径  end  ##########################

GenApplication(
		"${ExeName}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${IncludeDirs}"
		"${LibDirs}"
		"${OutputDir}"
		"${LibItems}"
		"${LocalDefineList}"
	)
