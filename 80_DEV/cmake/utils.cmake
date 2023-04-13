# Only works with relative path
function(copy_files)
    foreach (ARG ${ARGV})
        copy_file(${ARG})
    endforeach ()
endfunction()

# Only works with absolute path
function(copy_files_absolute)
    foreach(ARG ${ARGV})
        copy_file_absolute(${ARG})
    endforeach()
endfunction()


# Only works with relative path
function(copy_file FILE)
    message(STATUS "${FILE} -> $<TARGET_FILE_DIR:${PROJECT_NAME}>")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
            COMMAND ${CMAKE_COMMAND} -E copy_if_different       # which executes "cmake - E copy_if_different..."
            "${PROJECT_SOURCE_DIR}/${FILE}"                     # <--this is in-file
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path
endfunction()

# Only works with absolute path
function(copy_file_absolute FILE)
    message(STATUS "${DIR} -> $<TARGET_FILE_DIR:${PROJECT_NAME}>")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
            COMMAND ${CMAKE_COMMAND} -E copy_if_different       # which executes "cmake - E copy_if_different..."
            "${FILE}"                                           # <--this is in-file
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path
endfunction()

# Only works with relative path
function(copy_directory DIR)
    message(STATUS "${DIR} -> $<TARGET_FILE_DIR:${PROJECT_NAME}>${DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}${DIR}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>${DIR}")
endfunction()

# Only works with relative path
function(copy_directory_dll DIR)
    message(STATUS "${DIR} -> $<TARGET_FILE_DIR:${PROJECT_NAME}>")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DIR}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endfunction()

# Asset generator script
function(makeasset DEP NAM PNGNAM)
    add_custom_target(${DEP}
        COMMAND make clean run name=${NAM}_asmak outputname=${NAM}_asmak targetpng=${PNGNAM}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/assets/AssetMaker
    )
    add_dependencies(${PROJECT_NAME} ${DEP})
endfunction()
