# Only works with relative path
function(copy_files)
    foreach (ARG ${ARGV})
        copy_file(${ARG})
    endforeach ()
endfunction()

# Only works with relative path
function(copy_file FILE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
            COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
            "${PROJECT_SOURCE_DIR}/${FILE}"      # <--this is in-file
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path
endfunction()