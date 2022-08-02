function(copy_files)
    foreach (ARG ${ARGV})
        copy_file(${ARG})
    endforeach ()
endfunction()

function(copy_file FILE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FILE}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endfunction()