
########################################################################################
#################### !!! this is slution makefile !!! ##################################
########################################################################################

#指定cmake的最小版本
cmake_minimum_required(VERSION 3.18)


#设置项目名称
set(SlnName "HappyEveryDay")


MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

#在顶层引入通用函数模块
INCLUDE("${CMAKE_BINARY_DIR}/AutoGenHeader.cmake")  
INCLUDE("${CMAKE_SOURCE_DIR}/../CMake/Utility.cmake")  

#和Generator.bat同级目录
set(RootDir "${CMAKE_SOURCE_DIR}/..")
set(SlnAutoGenLibsDir "${CMAKE_BINARY_DIR}/Libs")


#开启项目文件夹
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	
project("${SlnName}")
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

set(DefineList)

#添加c++代码中宏定义
if(PLATFORM_WINDOWS)
    message("PLATFORM_WINDOWS")
    list(APPEND DefineList "PLATFORM_WINDOWS")
elseif(PLATFORM_ANDROID)
    message("PLATFORM_ANDROID")
    list(APPEND DefineList "PLATFORM_ANDROID")
elseif(PLATFORM_IOS)
    message("PLATFORM_IOS")
    list(APPEND DefineList "PLATFORM_IOS")
endif()

AddSlnSubDir("${CMAKE_SOURCE_DIR}")


# won't work before project()!   
#https://stackoverflow.com/questions/4845198/fatal-error-no-target-architecture-in-visual-studio 
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message("64 bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message("32 bit")
endif()
