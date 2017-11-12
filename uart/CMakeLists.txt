cmake_minimum_required(VERSION 2.8.9)
project(template_project C)
set(NAME "firmware")
set(HEX_NAME ${NAME}.hex)
set(BIN_NAME ${NAME}.bin)
set(ST_FLASH "st-flash")
set(FLASH_ADDRESS "0x8000000")

set(TOOLCHAIN_BASE /opt/gcc-arm-none-eabi-6-2017-q1-update/bin)
set(CUBE_FW_PATH /opt/STM32Cube_FW_F4_V1.15.0)

enable_language(C ASM)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER ${TOOLCHAIN_BASE}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BASE}/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BASE}/arm-none-eabi-gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_BASE}/arm-none-eabi-objcopy)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld)

# Includes
include_directories(include)
include_directories(${CUBE_FW_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(${CUBE_FW_PATH}/Drivers/CMSIS/Include)
include_directories(${CUBE_FW_PATH}/Projects/STM32F4-Discovery/Templates/Inc)
include_directories(${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(${CUBE_FW_PATH}/Drivers/BSP/STM32F4-Discovery)

# Sources
file(GLOB SOURCES
  src/*.c
  startup_stm32f407xx.s
  system_stm32f4xx.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
  ${CUBE_FW_PATH}/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery.c
  ${CUBE_FW_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
  )

add_definitions(-DSTM32F407xx)
add_definitions(-mcpu=cortex-m4 -mthumb -mthumb-interwork -Os)
add_definitions(-DHAL_UART_MODULE_ENABLED)
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m4 -T${LINKER_SCRIPT}")

add_executable(${NAME} ${SOURCES})

# Add post-build action to form hex and binary files
add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${NAME} ${HEX_NAME}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${NAME} ${BIN_NAME}
        )

add_custom_target(flash
    COMMAND ${ST_FLASH} write ${CMAKE_BINARY_DIR}/${BIN_NAME} ${FLASH_ADDRESS}
    DEPENDS ${CMAKE_BINARY_DIR}/${BIN_NAME}
    )
