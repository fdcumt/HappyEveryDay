#file(GLOB ROOT_SOURCE *.cpp)
#file(GLOB ROOT_INLINE *.inl)
#file(GLOB ROOT_HEADER *.hpp)
#file(GLOB ROOT_TEXT ../*.txt)
#file(GLOB ROOT_MD ../*.md)
#file(GLOB ROOT_NAT ../util/glm.natvis)
#
#file(GLOB_RECURSE CORE_SOURCE ./detail/*.cpp)
#file(GLOB_RECURSE CORE_INLINE ./detail/*.inl)
#file(GLOB_RECURSE CORE_HEADER ./detail/*.hpp)
#
#file(GLOB_RECURSE EXT_SOURCE ./ext/*.cpp)
#file(GLOB_RECURSE EXT_INLINE ./ext/*.inl)
#file(GLOB_RECURSE EXT_HEADER ./ext/*.hpp)
#
#file(GLOB_RECURSE GTC_SOURCE ./gtc/*.cpp)
#file(GLOB_RECURSE GTC_INLINE ./gtc/*.inl)
#file(GLOB_RECURSE GTC_HEADER ./gtc/*.hpp)
#
#file(GLOB_RECURSE GTX_SOURCE ./gtx/*.cpp)
#file(GLOB_RECURSE GTX_INLINE ./gtx/*.inl)
#file(GLOB_RECURSE GTX_HEADER ./gtx/*.hpp)
#
#file(GLOB_RECURSE SIMD_SOURCE ./simd/*.cpp)
#file(GLOB_RECURSE SIMD_INLINE ./simd/*.inl)
#file(GLOB_RECURSE SIMD_HEADER ./simd/*.h)
#
#source_group("Text Files" FILES ${ROOT_TEXT} ${ROOT_MD})
#source_group("Core Files" FILES ${CORE_SOURCE})
#source_group("Core Files" FILES ${CORE_INLINE})
#source_group("Core Files" FILES ${CORE_HEADER})
#source_group("EXT Files" FILES ${EXT_SOURCE})
#source_group("EXT Files" FILES ${EXT_INLINE})
#source_group("EXT Files" FILES ${EXT_HEADER})
#source_group("GTC Files" FILES ${GTC_SOURCE})
#source_group("GTC Files" FILES ${GTC_INLINE})
#source_group("GTC Files" FILES ${GTC_HEADER})
#source_group("GTX Files" FILES ${GTX_SOURCE})
#source_group("GTX Files" FILES ${GTX_INLINE})
#source_group("GTX Files" FILES ${GTX_HEADER})
#source_group("SIMD Files" FILES ${SIMD_SOURCE})
#source_group("SIMD Files" FILES ${SIMD_INLINE})
#source_group("SIMD Files" FILES ${SIMD_HEADER})
#
#add_library(glm INTERFACE)
#target_include_directories(glm INTERFACE ../)
#
#if(BUILD_STATIC_LIBS)
#add_library(glm_static STATIC ${ROOT_TEXT} ${ROOT_MD} ${ROOT_NAT}
#	${ROOT_SOURCE}    ${ROOT_INLINE}    ${ROOT_HEADER}
#	${CORE_SOURCE}    ${CORE_INLINE}    ${CORE_HEADER}
#	${EXT_SOURCE}     ${EXT_INLINE}     ${EXT_HEADER}
#	${GTC_SOURCE}     ${GTC_INLINE}     ${GTC_HEADER}
#	${GTX_SOURCE}     ${GTX_INLINE}     ${GTX_HEADER}
#	${SIMD_SOURCE}    ${SIMD_INLINE}    ${SIMD_HEADER})
#	target_link_libraries(glm_static PUBLIC glm)
#	add_library(glm::glm_static ALIAS glm_static)
#endif()
#
#if(BUILD_SHARED_LIBS)
#add_library(glm_shared SHARED ${ROOT_TEXT} ${ROOT_MD} ${ROOT_NAT}
#	${ROOT_SOURCE}    ${ROOT_INLINE}    ${ROOT_HEADER}
#	${CORE_SOURCE}    ${CORE_INLINE}    ${CORE_HEADER}
#	${EXT_SOURCE}     ${EXT_INLINE}     ${EXT_HEADER}
#	${GTC_SOURCE}     ${GTC_INLINE}     ${GTC_HEADER}
#	${GTX_SOURCE}     ${GTX_INLINE}     ${GTX_HEADER}
#	${SIMD_SOURCE}    ${SIMD_INLINE}    ${SIMD_HEADER})
#	target_link_libraries(glm_shared PUBLIC glm)
#	add_library(glm::glm_shared ALIAS glm_shared)
#endif()



########################################################################################
#################### !!! this is lib makefile !!! ##################################
########################################################################################

#设置项目名称
set(LibName "glm")

#静态连接库, 连接器->输入->附加依赖项
#set(LibItems )

#针对该exe的宏定义
set(LocalDefineList
)

################这里面所有路径都要填相对路径, 相对于当前目录所在的路径 begin ##########################


#设置Include目录, Properties->C/C++->General->Additional Include Directories
set(IncludeDirs
	"."
	)

#静态连接库,  连接器->常规->附加库目录
set(LibDirs
	)

#设置Output目录, Properties->General->Output Directory
#静态文件一般都输出到和sln同级的Libs目录中.
set(OutputDir "${SlnAutoGenLibsDir}")


GenLib( 
		"${LibName}" 
		"${CMAKE_CURRENT_SOURCE_DIR}" 
		"${IncludeDirs}" 
		"${LibDirs}" 
		"${OutputDir}" 
		"${LibItems}" 
		"${LocalDefineList}"
		)
