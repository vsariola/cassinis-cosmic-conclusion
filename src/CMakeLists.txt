option(MINIFY_SHADERS "Use shader_minifier6shader.frag instead of the hand-written shader.inl" OFF)
option(CMAIN "Use the C-written development main instead of inline asm" OFF)
option(_DEBUG "Define the _DEBUG macro" OFF)
option(SAVE_MUSIC "Save music to disk" OFF)
option(FULLSCREEN "Maximize window in debug mode" OFF)
set(COMPRESS_LEVEL LIGHT CACHE STRING "Crinkler compression level: off, light, medium, heavy")
set_property(CACHE COMPRESS_LEVEL PROPERTY STRINGS OFF LIGHT MEDIUM HEAVY)

message(STATUS "Configuration options (configure with cmake -D...):")
message(STATUS "  MINIFY_SHADERS = ${MINIFY_SHADERS}")
message(STATUS "  CMAIN = ${CMAIN}")
message(STATUS "  SAVE_MUSIC = ${SAVE_MUSIC}")
message(STATUS "  FULLSCREEN = ${FULLSCREEN}")
message(STATUS "  COMPRESS_LEVEL = ${COMPRESS_LEVEL}")

set(exename "intro")

if (MINIFY_SHADERS)
    find_program(SHADER_MINIFIER NAMES shader_minifier)
    if (NOT SHADER_MINIFIER)
        message(WARNING "Shader_minifier not found. Cannot minify shaders; using always the hand inlined versions (*.inl). Get shader_minifier from: https://www.ctrl-alt-test.fr/glsl-minifier/")
        set(MINIFY_SHADERS OFF)
    endif()
endif()

if (MINIFY_SHADERS)
    message(STATUS "Shader_minifier found at: ${SHADER_MINIFIER}")
    # Generate the minified inlined shaders
    set(SHADER_FRAG shader.frag)
    set(SHADER_CONFIGURED_FRAG configured/shader.frag)
    set(SHADER_MINIFIED_INL minified/shader.inl)
    configure_file(
        ${SHADER_FRAG}
        ${SHADER_CONFIGURED_FRAG}
    )
    add_custom_command(
        OUTPUT ${SHADER_MINIFIED_INL}
        COMMAND ${SHADER_MINIFIER} -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_MINIFIED_INL} ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_CONFIGURED_FRAG}
        DEPENDS ${SHADER_FRAG}
    )
    set(SHADERS ${SHADER_MINIFIED_INL})
    set(SHADER_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/minified/)
    set(exename "${exename}-toolmin")
else()
    set(SHADERS shader.inl)
    set(SHADER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_executable(intro WIN32 "main.cpp" ${SHADERS})
target_link_libraries(intro opengl32.lib winmm.lib)
target_include_directories(intro PUBLIC ${SHADER_INCLUDE_DIR}) # for the shaders

if (CMAIN)
    set(exename "${exename}-cmain")
    target_compile_definitions(intro PUBLIC CMAIN)
    if (FULLSCREEN)
        set(exename "${exename}-fullscreen")
        target_compile_definitions(intro PUBLIC FULLSCREEN)
    endif()
endif()

if (_DEBUG)
    set(exename "${exename}-debug")
    target_compile_definitions(intro PUBLIC _DEBUG)    
endif()

if (SAVE_MUSIC)
    target_compile_definitions(intro PUBLIC SAVE_MUSIC)
endif()

if(NOT COMPRESS_LEVEL STREQUAL OFF)
    set_property(TARGET intro APPEND PROPERTY COMPILE_FLAGS " /Os /O1 /Ob1 /Oi /Oy")
    find_program(CRINKLER NAMES Crinkler)
    if (NOT CRINKLER)
        message(WARNING "Crinkler not found. Cannot compress executable; using default linker. Get Crinkler from https://github.com/runestubbe/Crinkler & put it in path (as Crinkler.exe)")
        set(COMPRESS_LEVEL OFF)
    endif()
endif()

if (NOT COMPRESS_LEVEL STREQUAL OFF)
    message(STATUS "Crinkler found at: ${CRINKLER}")
    set(CRINKLER_FLAGS "/ENTRY:entrypoint /OVERRIDEALIGNMENTS:24 /RANGE:opengl32 /RANGE:winmm /PROGRESSGUI /UNSAFEIMPORT /UNALIGNCODE /TINYHEADER /TINYIMPORT /NOINITIALIZERS /HASHSIZE:1000 /REPORT:crinkler_report.html")
	target_compile_definitions(intro PUBLIC USE_CRINKLER)
    # TBD: do we add /SATURATE
    if (COMPRESS_LEVEL STREQUAL LIGHT)
        set(CRINKLER_FLAGS "${CRINKLER_FLAGS} /HASHTRIES:100 /COMPMODE:INSTANT /ORDERTRIES:2000")        
    elseif (COMPRESS_LEVEL STREQUAL HEAVY)
        set(CRINKLER_FLAGS "${CRINKLER_FLAGS} /HASHTRIES:1000 /COMPMODE:VERYSLOW /ORDERTRIES:30000")
        set(exename "${exename}-heavy")
    else()
        set(CRINKLER_FLAGS "${CRINKLER_FLAGS} /HASHTRIES:300 /COMPMODE:SLOW /ORDERTRIES:9000")
        set(exename "${exename}-medium")
    endif()

    # we drop the whole manifest creation from the front; did not find a way to disable it from CMake otherwise
    set (CMAKE_CXX_LINK_EXECUTABLE "${CRINKLER} <OBJECTS> /out:<TARGET> ${CRINKLER_FLAGS} <LINK_LIBRARIES>")
else()
    set(exename "${exename}-uncompressed")
    set_property(TARGET intro APPEND PROPERTY LINK_FLAGS " /ENTRY:mainCRTStartup")
endif()

add_custom_command(TARGET intro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:intro> ${PROJECT_SOURCE_DIR}/dist/${exename}.exe
)

if (MINIFY_SHADERS) # if we are using shader.frag, we can also try validating it, if we find glslangValidator
    find_program(GLSLANGVALIDATOR NAMES glslangValidator)
    if (NOT GLSLANGVALIDATOR)
        message(WARNING "glslangValidator not found. Cannot validate shaders. Get glslangValidator from: https://github.com/KhronosGroup/glslang")        
    else()
        add_custom_command(TARGET intro POST_BUILD
        COMMAND ${GLSLANGVALIDATOR} ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_CONFIGURED_FRAG})
    endif()
endif()
