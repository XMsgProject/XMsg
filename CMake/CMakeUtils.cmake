function(xplatform_add_dependence target dependence)

add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${dependence}>
                $<TARGET_FILE_DIR:${target}>)

add_dependencies(${target} ${dependence})
endfunction(xplatform_add_dependence)