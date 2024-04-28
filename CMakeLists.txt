cmake_minimum_required(VERSION 3.8)

project(OmLang VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(LLVM_DIR "D:/ProgramFiles/LLVM/lib/cmake/llvm")

set(LLVM_ROOT_DIR "D:/ProgramFiles/LLVM")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINATIONS})

# Macros =============================================
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")
if(WIN32)
  add_compile_definitions(O_EXT="exe")
else()
  add_compile_definitions(O_EXT="out")
endif(WIN32)
# Main Application /===================================
add_executable(om src/main.cpp src/lexer.cpp src/driver.cpp src/parser.cpp src/utils.cpp src/AST.cpp src/Env.cpp)

target_include_directories(om PRIVATE include ${LLVM_INCLUDE_DIRS})

set(llvm_libs 
    LLVMX86TargetMCA
    LLVMMCA 
    LLVMX86Disassembler 
    LLVMX86AsmParser 
    LLVMX86CodeGen 
    LLVMX86Desc 
    LLVMX86Info 
    LLVMMCDisassembler 
    LLVMInstrumentation 
    LLVMGlobalISel 
    LLVMSelectionDAG 
    LLVMCFGuard 
    LLVMAsmPrinter 
    LLVMCodeGen 
    LLVMTarget 
    LLVMScalarOpts 
    LLVMInstCombine 
    LLVMAggressiveInstCombine 
    LLVMObjCARCOpts 
    LLVMTransformUtils 
    LLVMCodeGenTypes 
    LLVMBitWriter 
    LLVMAnalysis 
    LLVMProfileData 
    LLVMSymbolize 
    LLVMDebugInfoBTF 
    LLVMDebugInfoPDB 
    LLVMDebugInfoMSF 
    LLVMDebugInfoDWARF 
    LLVMObject 
    LLVMTextAPI 
    LLVMMCParser 
    LLVMMC 
    LLVMDebugInfoCodeView 
    LLVMIRReader 
    LLVMBitReader 
    LLVMAsmParser 
    LLVMCore 
    LLVMRemarks 
    LLVMBitstreamReader 
    LLVMBinaryFormat 
    LLVMTargetParser 
    LLVMSupport 
    LLVMDemangle
)

FUNCTION(PREPEND_LLVM var)
    set(listVar "")
    FOREACH(f ${ARGN})
        list(APPEND listVar "${LLVM_ROOT_DIR}/lib/lib${f}.a")
    ENDFOREACH(f)
    # message(STATUS "${files_to_install}")
    set(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND_LLVM)


target_link_libraries(om PRIVATE ${llvm_libs} )
if(WIN32)
target_link_libraries(om PRIVATE pthread Shell32 ole32)
endif(WIN32)


# LibOm ============================================
add_library(stdom libom/src/libom.c)
# list(APPEND files_libom "libom/src/std.om")

# SystemLibs ===================================
if(MINGW)
  get_filename_component(Mingw_Path ${CMAKE_CXX_COMPILER} PATH)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${Mingw_Path}/libwinpthread-1.dll ${Mingw_Path}/libgcc_s_seh-1.dll ${Mingw_Path}/libstdc++-6.dll)
endif(MINGW)
include(InstallRequiredSystemLibraries)

# Install ======================================
install(TARGETS om 
RUNTIME DESTINATION bin 
LIBRARY DESTINATION lib 
ARCHIVE DESTINATION lib
)

# if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
# install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT System)
# endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)

# only required if making a installer
# PREPEND_LLVM(files_to_install ${llvm_libs})
# install(FILES ${files_to_install} DESTINATION bin)
install(DIRECTORY libom/include/ DESTINATION include)
install(DIRECTORY libom/lib/ DESTINATION lib)
install(TARGETS stdom LIBRARY DESTINATION libs)

# CPack ========================================

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

# get_target_property(OUT om LINK_LIBRARIES)
# message(STATUS ${OUT})
