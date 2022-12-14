
function(GetAbslutePaths AbslutePaths RelativePaths BaseDir)
    if(MSVC)
		set(NewAbslutePaths)
		foreach(RelativePath IN LISTS RelativePaths)
			string(COMPARE EQUAL ${RelativePath} "" bEqual)
			if(${bEqual} EQUAL 0)
				get_filename_component(AbsPath "${RelativePath}" REALPATH BASE_DIR "${BaseDir}")
				list(APPEND NewAbslutePaths ${AbsPath})
			endif()
        endforeach()
		set(${AbslutePaths} ${NewAbslutePaths} PARENT_SCOPE)
    endif(MSVC)
endfunction(GetAbslutePaths)

#按目录结构加入文件
function(AddFiltersForVS FullPathFileNames CurDir)
    if(MSVC)
        foreach(FullPathFileName IN LISTS FullPathFileNames)
            # 求出相对路径
            string(REPLACE "${CurDir}/" "" RelativePathFileName "${FullPathFileName}")
			get_filename_component(RelativePath "${RelativePathFileName}" DIRECTORY)
            source_group("${RelativePath}" FILES "${FullPathFileName}")
        endforeach(FullPathFileName)
    endif(MSVC)
endfunction(AddFiltersForVS)

function(GenIncludeABSDir IncludeAbsDirs IncludeDirs CurDir)
	set(IncludeAbsDirsTemp)
    foreach(IncludeDir IN LISTS IncludeDirs)
		string(COMPARE EQUAL "${IncludeDir}" "" bEqual)
		if(bEqual)
			continue()
		endif()
		STRING(REGEX REPLACE "^[.]($|/)" "${CurDir}/" IncludeDir1 "${IncludeDir}")
		STRING(REGEX REPLACE "^[.][.][/]?" "${CurDir}/.." IncludeDir2 "${IncludeDir1}")
		string(SUBSTRING "${IncludeDir2}" 1 1 char)
		string(COMPARE EQUAL "${char}" ":" bEqual)
		if(bEqual)
			list(APPEND IncludeAbsDirsTemp "${IncludeDir2}")
			continue()
		else()
			STRING(REGEX MATCH "^[a-zA-Z0-9_]+" CurLibName "${IncludeDir2}")
			foreach(libName Path IN ZIP_LISTS LibNames LibPaths)
				string(COMPARE EQUAL "${libName}" "${CurLibName}" bEqual)
				if(bEqual)
					STRING(REGEX REPLACE "^[a-zA-Z0-9_]+/?" "" tailPath "${IncludeDir2}")
					list(APPEND IncludeAbsDirsTemp "${Path}/${tailPath}")
					break()
				endif()
			endforeach()
		endif()
    endforeach()
	set(${IncludeAbsDirs} ${IncludeAbsDirsTemp} PARENT_SCOPE)
	#message("IncludeAbsDirs:${IncludeAbsDirsTemp}")
endfunction(GenIncludeABSDir)

function(GenVSFolder ExeOrExeName LibOrExeAbsDir bLib)
    #找到上一层目录的绝对路径
	get_filename_component(LibOrExeUpLevelAbsDir "${LibOrExeAbsDir}/.." ABSOLUTE)
	#message("GenVSFolder LibOrExeUpLevelDir:${LibOrExeUpLevelDir} LibOrExeAbsDir:${LibOrExeAbsDir}")
	#获取相对于source的目录
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" LibOrExeUpLevelRelativeDir "${LibOrExeUpLevelAbsDir}")
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" LibOrExeUpLevelRelativeDir "${LibOrExeUpLevelRelativeDir}")
    string(REPLACE "/" ";" LibOrExeUpLevelRelativeDirList "${LibOrExeUpLevelRelativeDir}")

	list(LENGTH LibOrExeUpLevelRelativeDirList Len)
	if(${Len} EQUAL 0)
		#message("GenVSFolder ExeOrExeName:${ExeOrExeName} Len 0")
		if(${bLib})
			set_property(TARGET "${ExeOrExeName}" PROPERTY FOLDER "Libs")
		else()
			set_property(TARGET "${ExeOrExeName}" PROPERTY FOLDER "Executables")
		endif()
	else()
		#foreach(LibOrExeUpLevelRelativeDirItemName IN LISTS LibOrExeUpLevelRelativeDirList)
			#message("GenVSFolder LibOrExeUpLevelRelativeDirItemName:${LibOrExeUpLevelRelativeDirItemName}")
		#endforeach(LibOrExeUpLevelRelativeDirItemName)
		set_property(TARGET "${ExeOrExeName}" PROPERTY FOLDER "${LibOrExeUpLevelRelativeDir}")
	endif()
endfunction(GenVSFolder)


#执行一些公共操作
function(DoCommonActions ProjectName InDefineList)
	#设置通用宏定义
	foreach(DefineItem IN LISTS GlobalDefineList)
		add_definitions(-D"${DefineItem}")
	endforeach()

	foreach(LocalDefineItem IN LISTS InDefineList)
		add_definitions(-D"${LocalDefineItem}")
	endforeach()


endfunction(DoCommonActions)

#生成lib库
function(GenLib LibName CurDir IncludeDirs LibDirs OutputDir InDefineList)
	#message("Begin Generate lib:${LibName} CurDir:${CurDir}")
	GenIncludeABSDir(IncludeAbsDirs "${IncludeDirs}" "${CurDir}" )
	GetAbslutePaths(LibAbsDirs "${LibDirs}" "${CurDir}")
	GetAbslutePaths(OutputAbsDir "${OutputDir}" "${CurDir}")

	#所有文件保存在一个变量中		
	file(GLOB_RECURSE all_files *.h *.cpp *.c *.cc *.inl *.hpp glm.natvis)

	#生成vs中对应的文件夹
	AddFiltersForVS("${all_files}" "${CurDir}")
	
	#********这种方法设置后, Output目录后边不会添加Release/debug***********
	#把这种方法命名为 OutputNoDebugOrRelase
	#如果启用了该方法, 则方法OutputWithDebugOrRelase永远不会生效
	# Properties->General->Output Directory
	if(MSVC)
		#开启这几行, 可以去除output尾部的debug, 但是指定目录就会失效
		#https://stackoverflow.com/questions/543203/cmake-runtime-output-directory-on-windows
		#官方链接https://cmake.org/cmake/help/latest/prop_tgt/LIBRARY_OUTPUT_DIRECTORY_CONFIG.html
		#官方链接https://cmake.org/cmake/help/latest/variable/CMAKE_LIBRARY_OUTPUT_DIRECTORY_CONFIG.html#variable:CMAKE_LIBRARY_OUTPUT_DIRECTORY_%3CCONFIG%3E
		SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OutputAbsDir}/${ArchitectureDirName}")
		SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OutputAbsDir}/${ArchitectureDirName}")
		SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OutputAbsDir}/${ArchitectureDirName}")
		SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OutputAbsDir}/${ArchitectureDirName}")
		SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OutputAbsDir}/${ArchitectureDirName}")
		SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OutputAbsDir}/${ArchitectureDirName}")
	endif(MSVC)
	
	add_library("${LibName}" "${all_files}")
	message(STATUS "Begin Generate lib:${LibName}")
	
	# Properties->C/C++->General->Additional Include Directories
	if(NOT ("${IncludeAbsDirs} " STREQUAL " "))
		include_directories("${IncludeAbsDirs}")
	endif()
	
	
	#静态链接库无法添加依赖的链接 即xxx.lib
	#target_link_libraries("${LibName}" "${LibItems}")
	
	#静态连接库无法设置 连接器->常规->附加库目录
	#target_link_directories("${LibName}" PRIVATE "${LibAbsDirs}")
	#target_link_directories("${LibName}" PUBLIC "${LibAbsDirs}")
	
	# Creates a folder "libraries" and adds target this project under it
	#set_property(TARGET "${LibName}" PROPERTY FOLDER "Libraries")
	GenVSFolder("${LibName}" "${CurDir}" true)


	#执行一些公共操作
	DoCommonActions("${LibName}" "${InDefineList}")
endfunction(GenLib)


#生成Application项目
function(GenExecutable ExeName CurDir IncludeDirs LibDirs OutputDir NeedLibItems bApplication InDefineList)
	#message("Begin Generate EXE:${ExeName}")
	GenIncludeABSDir(IncludeAbsDirs "${IncludeDirs}" "${CurDir}")
	GetAbslutePaths(LibAbsDirs "${LibDirs}" "${CurDir}")
	GetAbslutePaths(OutputAbsDir "${OutputDir}" "${CurDir}")

	#所有文件保存在一个变量中		
	file(GLOB_RECURSE all_files *.h *.cpp *.c *.cc *.inl *.hpp)
	#file(GLOB_RECURSE cpp_files *.cpp *.c *.cc)
	AddFiltersForVS("${all_files}" "${CurDir}")

	add_executable("${ExeName}" "${all_files}")
	message(STATUS "Begin Generate EXE:${ExeName}")

	# Properties->C/C++->General->Additional Include Directories
	if(NOT ("${IncludeAbsDirs} " STREQUAL " "))
		include_directories("${IncludeAbsDirs}")
	endif()
	

	#添加静态链接库, 即xxx.lib
	target_link_libraries("${ExeName}" "${NeedLibItems}")
	
	#静态连接库,  连接器->常规->附加库目录
	foreach(LibAbsDirItem IN LISTS LibAbsDirs)
		target_link_directories("${ExeName}" PRIVATE "${LibAbsDirItem}/${ArchitectureDirName}")
		target_link_directories("${ExeName}" PUBLIC "${LibAbsDirItem}/${ArchitectureDirName}")
	endforeach()

	set_target_properties("${ExeName}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
	
	#Creates a folder "libraries" and adds target project (math.vcproj) under it
	#set_property(TARGET "${ExeName}" PROPERTY FOLDER "Executables")
	GenVSFolder("${ExeName}" "${CurDir}" false)

	#Properties->General->Output Directory
	set_target_properties("${ExeName}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OutputAbsDir}/${ArchitectureDirName}")
	
	#if(bApplication)
	#	#message("GenExecutable EXE:${ExeName} use /SUBSYSTEM:WINDOWS")
	#	#ref:https://developercommunity.visualstudio.com/t/cmake-set-subsystem-to-console/46678
	#	#set_target_properties("${ExeName}" PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	#	#set_target_properties("${ExeName}" PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	#	#set_target_properties("${ExeName}" PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	#	#set_target_properties("${ExeName}" PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	#	#注意Release和Debug都要设置
	#	set_target_properties("${ExeName}" PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	#	set_target_properties("${ExeName}" PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
	#	set_target_properties("${ExeName}" PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	#endif()

	#执行一些公共操作
	DoCommonActions("${ExeName}" "${InDefineList}")

	#message("Succeed Generate EXE:${ExeName}")
endfunction(GenExecutable)

#生成exe项目
function(GenEXE ExeName CurDir IncludeDirs LibDirs OutputDir NeedLibItems InDefineList)
	GenExecutable("${ExeName}" "${CurDir}" "${IncludeDirs}" "${LibDirs}" "${OutputDir}" "${NeedLibItems}" false "${InDefineList}")
endfunction(GenEXE)


#生成Application项目
function(GenApplication ExeName CurDir IncludeDirs LibDirs OutputDir NeedLibItems InDefineList)
	GenExecutable("${ExeName}" "${CurDir}" "${IncludeDirs}" "${LibDirs}" "${OutputDir}" "${NeedLibItems}" true "${InDefineList}")
endfunction(GenApplication)


#function(AddSlnSubDir CurDir)
#	#遍历sln文件夹,获取文件夹下所有含有CMakeLists.txt的项目
#	file(GLOB ChildDirs RELATIVE "${CurDir}" "${CurDir}/*")
#	foreach(child IN LISTS ChildDirs)
#		if(IS_DIRECTORY "${CurDir}/${child}")
#			if(EXISTS "${CurDir}/${child}/CMakeLists.txt")
#				add_subdirectory("${child}")
#			endif()
#		endif()
#	endforeach()
#endfunction(AddSlnSubDir)


function(AddSlnSubDir CurDir)
	#遍历sln文件夹,获取文件夹下所有含有CMakeLists.txt的项目
	#file(GLOB_RECURSE CMakeFiles RELATIVE "${CurDir}" CMakeLists.txt)
	file(GLOB_RECURSE CMakeFiles CMakeLists.txt)
	foreach(CMakeFile IN LISTS CMakeFiles)
		get_filename_component(FileDir "${CMakeFile}" DIRECTORY)
		string(COMPARE EQUAL "${FileDir}" "${CurDir}" bEqual)
		if(NOT bEqual)
			add_subdirectory("${FileDir}")
		endif(NOT bEqual)
	endforeach()
endfunction(AddSlnSubDir)
