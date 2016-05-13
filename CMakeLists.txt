################################################################################
# CMakeLists.txt file for Motor Test Stand PCB (Atmega16u4) Code
# 
# Instructions:
# Modify the options listed below to add the source files, specify the final 
# compiled elf hex file, and specify the fuses that need to be set upon the 
# first flashing to the MCU's bootloader.
#
# The following commands are to be executed within the build folder. Root access
# may be required. If it is, prepend "sudo " to each command below (note the 
# space).
#
# To build the code, type "make".
#
# To check if avrdude and the programer are connected and working, type 
# "make avrdude_test".
#
# To set the fuses, type "make fuses".
#
# To upload the program to the MCU (DOES NOT BUILD IT), type "make upload".
#
# To remove the build files, type "make clean".
#
# Typical use is "make" followed by "make upload".
################################################################################

cmake_minimum_required(VERSION 2.8)

# LIST SOURCES HERE
set(SOURCES main.c)

# NAME TARGET HEX FILE HERE
set(ELF main)

# FUSES (specified by the MCU Specifications/Manual)
set(HFUSE "0xF3")
set(EFUSE "0x99")
set(LFUSE "0x5E")

# Specify avrdude programmer flags
#set(PROGRAMMER avrispmkII)
set(PROGRAMMER avr109)

#set(PORT usb)
set(PORT /dev/ttyACM0)

set(BAUD 57600)
set(CONF /etc/avrdude.conf)

set(AVRDUDE_FLAGS
#    -F 
#    -C ${CONF} 
    -c ${PROGRAMMER} 
    -p ${MCU} 
    -P ${PORT} 
#    -b ${BAUD} 
    -v
)

################################################################################
# SYSTEM-SPECIFIC CMAKE OPTIONS 
################################################################################

# Set cross compilation information
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# Find AVR build utils
find_program(AVR_CC 		avr-gcc)
find_program(AVR_CXX	 	avr-g++)
find_program(AVR_ASM 		avr-as)
find_program(AVR_AR			avr-ar)
find_program(AVR_OBJCOPY 	avr-objcopy)
find_program(AVR_OBJDUMP 	avr-objdump)
find_program(AVR_SIZE_TOOL 	avr-size)

# Find flash utility
find_program(FLASH_UTIL 	avrdude)

# Set cmake compiler
set(CMAKE_C_COMPILER 	${AVR_CC})
set(CMAKE_CXX_COMPILER 	${AVR_CXX})
set(CMAKE_ASM_COMPILER 	${AVR_ASM})
set(CMAKE_AR 			${AVR_AR})
set(CMAKE_OBJCOPY 		${AVR_OBJCOPY})
set(CMAKE_OBJDUMP 		${AVR_OBJDUMP})
enable_language(ASM)

message(STATUS "")
message(STATUS "Using compilers: ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER}")
message(STATUS "")

# Set compiler flags
#set(MCU "atmega16u4")
set(MCU atmega32u4)
set(COMMON_FLAGS "-g -Os -Wall -mmcu=${MCU} -mcall-prologues -std=c99")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS) # clear out unnecessary linker flags
set(CMAKE_EXE_LINKER_FLAGS)

# set bin folder for convenience
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# shorter variable name for even more convenience
set(ODIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# main object build target
add_executable(${ELF}.obj ${SOURCES})

add_custom_target("hex" DEPENDS ${ODIR}/${ELF}.hex
	COMMAND ${CMAKE_OBJCOPY} -R .eeprom -O ihex ${ODIR}/${ELF}.obj ${ODIR}/${ELF}.hex
)


# target for setting fuses for first-time bootloader use
add_custom_target("fuses" 
	COMMAND ${FLASH_UTIL} ${AVRDUDE_FLAGS} -U lfuse:w:${LFUSE}:m
	COMMAND ${FLASH_UTIL} ${AVRDUDE_FLAGS} -U hfuse:w:${HFUSE}:m
	COMMAND ${FLASH_UTIL} ${AVRDUDE_FLAGS} -U efuse:w:${EFUSE}:m
)

# target for testing if flash utility is working
add_custom_target("avrdude_test" 
	COMMAND ${FLASH_UTIL} ${AVRDUDE_FLAGS}
)

add_custom_target("upload" DEPENDS ${ODIR}/${ELF}.hex
# "upload" DEPENDS ${ODIR}/${ELF}.obj
#	COMMAND ${CMAKE_OBJCOPY} -R .eeprom -O ihex ${ODIR}/${ELF}.obj ${ODIR}/${ELF}.hex
	COMMAND ${FLASH_UTIL} ${AVRDUDE_FLAGS} -D -U flash:w:${ELF}.hex:i
)
