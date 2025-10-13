set(PROJECT_NAME DemoReframer)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "DemoReframer.cpp"
)
source_group("" FILES ${no_group_source_files})

set(Common
    "../DemoCutter/common.cpp"
    "../DemoCutter/files.cpp"
    "../DemoCutter/huffman.cpp"
    "../DemoCutter/msg.cpp"
    "../shared/animMappingTable.cpp"
    "../shared/CModel.cpp"
    "../shared/DemoReader.cpp"
    "../shared/DemoReaderBase.cpp"
    "../shared/pocketlzma/lzma_c.hpp"
)
source_group("Common" FILES ${Common})

set(Common__lzmaSDK
    "../shared/lzmaSDK/7zAlloc.c"
    "../shared/lzmaSDK/7zArcIn.c"
    "../shared/lzmaSDK/7zBuf.c"
    "../shared/lzmaSDK/7zBuf2.c"
    "../shared/lzmaSDK/7zCrc.c"
    "../shared/lzmaSDK/7zCrcOpt.c"
    "../shared/lzmaSDK/7zDec.c"
    "../shared/lzmaSDK/7zFile.c"
    "../shared/lzmaSDK/7zStream.c"
    "../shared/lzmaSDK/Aes.c"
    "../shared/lzmaSDK/AesOpt.c"
    "../shared/lzmaSDK/Alloc.c"
    "../shared/lzmaSDK/Bcj2.c"
    "../shared/lzmaSDK/Bcj2Enc.c"
    "../shared/lzmaSDK/Bra.c"
    "../shared/lzmaSDK/Bra86.c"
    "../shared/lzmaSDK/BraIA64.c"
    "../shared/lzmaSDK/CpuArch.c"
    "../shared/lzmaSDK/Delta.c"
    "../shared/lzmaSDK/DllSecur.c"
    "../shared/lzmaSDK/LzFind.c"
    "../shared/lzmaSDK/LzFindMt.c"
    "../shared/lzmaSDK/LzFindOpt.c"
    "../shared/lzmaSDK/Lzma2Dec.c"
    "../shared/lzmaSDK/Lzma2DecMt.c"
    "../shared/lzmaSDK/Lzma2Enc.c"
    "../shared/lzmaSDK/Lzma86Dec.c"
    "../shared/lzmaSDK/Lzma86Enc.c"
    "../shared/lzmaSDK/LzmaDec.c"
    "../shared/lzmaSDK/LzmaEnc.c"
    "../shared/lzmaSDK/LzmaLib.c"
    "../shared/lzmaSDK/MtCoder.c"
    "../shared/lzmaSDK/MtDec.c"
    "../shared/lzmaSDK/Ppmd7.c"
    "../shared/lzmaSDK/Ppmd7Dec.c"
    "../shared/lzmaSDK/Ppmd7Enc.c"
    "../shared/lzmaSDK/Sha256.c"
    "../shared/lzmaSDK/Sha256Opt.c"
    "../shared/lzmaSDK/Sort.c"
    "../shared/lzmaSDK/Threads.c"
    "../shared/lzmaSDK/Xz.c"
    "../shared/lzmaSDK/XzCrc64.c"
    "../shared/lzmaSDK/XzCrc64Opt.c"
    "../shared/lzmaSDK/XzDec.c"
    "../shared/lzmaSDK/XzEnc.c"
    "../shared/lzmaSDK/XzIn.c"
)
source_group("Common\\lzmaSDK" FILES ${Common__lzmaSDK})

set(Common__zlib
    "../shared/zlib/adler32.c"
    "../shared/zlib/compress.c"
    "../shared/zlib/crc32.c"
    "../shared/zlib/crc32.h"
    "../shared/zlib/deflate.c"
    "../shared/zlib/deflate.h"
    "../shared/zlib/gzclose.c"
    "../shared/zlib/gzguts.h"
    "../shared/zlib/gzlib.c"
    "../shared/zlib/gzread.c"
    "../shared/zlib/gzwrite.c"
    "../shared/zlib/infback.c"
    "../shared/zlib/inffast.c"
    "../shared/zlib/inffast.h"
    "../shared/zlib/inffixed.h"
    "../shared/zlib/inflate.c"
    "../shared/zlib/inflate.h"
    "../shared/zlib/inftrees.c"
    "../shared/zlib/inftrees.h"
    "../shared/zlib/ioapi.c"
    "../shared/zlib/ioapi.h"
    "../shared/zlib/trees.c"
    "../shared/zlib/trees.h"
    "../shared/zlib/uncompr.c"
    "../shared/zlib/zconf.h"
    "../shared/zlib/zlib.h"
    "../shared/zlib/zutil.c"
    "../shared/zlib/zutil.h"
)
source_group("Common\\zlib" FILES ${Common__zlib})

set(Header_Files
    "../DemoCutter/demoCut.h"
    "../shared/CModel.h"
    "../shared/DemoReader.h"
    "../shared/DemoReaderBase.h"
    "../shared/include/jpcre2.hpp"
    "../shared/include/pcre2.h"
)
source_group("Header Files" FILES ${Header_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Common}
    ${Common__lzmaSDK}
    ${Common__zlib}
    ${Header_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE DemoHighlightFinder)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "DEBUG;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CRT_NONSTDC_NO_DEPRECATE;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "DEBUG;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CRT_NONSTDC_NO_DEPRECATE;"
        "_CRT_SECURE_NO_WARNINGS;"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Ob2;
                /Oi;
                /arch:AVX2;
                /Gy;
                /Ot;
                /fp:fast;
                /Zp4;
                /openmp
            >
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Ob2;
                /Oi;
                /arch:AVX2;
                /Gy;
                /Ot;
                /fp:fast;
                /Zp4;
                /openmp
            >
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE;
            /PDBALTPATH:%_PDB%
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE;
            /PDBALTPATH:%_PDB%
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "pcre2-8-staticd;"
            "pcre2-posix-staticd"
        ">"
        "$<$<CONFIG:Release>:"
            "pcre2-8-static;"
            "pcre2-posix-static"
        ">"
        "dbghelp;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "advapi32;"
        "shell32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "pcre2-8-staticd;"
            "pcre2-posix-staticd"
        ">"
        "$<$<CONFIG:Release>:"
            "pcre2-8-static;"
            "pcre2-posix-static"
        ">"
        "dbghelp;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "advapi32;"
        "shell32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

