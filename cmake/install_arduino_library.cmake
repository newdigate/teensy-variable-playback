message("installing ardiuno library")
set(ARDUINO_LIBRARY_TARGET "~/Documents/Arduino/Libraries/teensy_variable_playback")

get_filename_component(DIR_ONE_ABOVE ../../ ABSOLUTE)
message(STATUS ${DIR_ONE_ABOVE})
set(ARDUINO_LIBRARY_SOURCE ${DIR_ONE_ABOVE}/src/)
set(INSTALL_ARDUINO_CMD "ln -s ${ARDUINO_LIBRARY_SOURCE} ${ARDUINO_LIBRARY_TARGET}")
set(UNINSTALL_ARDUINO_CMD "unlink ${ARDUINO_LIBRARY_TARGET}")

message(${UNINSTALL_ARDUINO_CMD})
exec_program(
        ${UNINSTALL_ARDUINO_CMD}
        OUTPUT_VARIABLE stdout
        RETURN_VALUE result
)
message(STATUS ${stdout})
message(STATUS ${result})

message(${INSTALL_ARDUINO_CMD})
exec_program(
        ${INSTALL_ARDUINO_CMD}
        OUTPUT_VARIABLE stdout
        RETURN_VALUE result
)
message(STATUS ${stdout})
message(STATUS ${result})
if (${result} STREQUAL 0)
    message("Success...")
else()
    message(FATAL_ERROR "Unable to create symbolic link in ${ARDUINO_LIBRARY_TARGET}")
endif()