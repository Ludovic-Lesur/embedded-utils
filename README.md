# Description

This repository contains **utility functions** for embedded projects.

# Dependencies

The driver relies on:

* An external `types.h` header file defining the **standard C types** of the targeted MCU.

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `EMBEDDED_UTILS_DISABLE_FLAGS_FILE` | `defined` / `undefined` | Disable the `embedded_utils_flags.h` header file inclusion when compilation flags are given in the project settings or by command line. |
| `EMBEDDED_UTILS_HW_INTERFACE_ERROR_BASE_LAST` | `<value>` | Last error base of the low level terminal interface driver. |
| `EMBEDDED_UTILS_AT_DRIVER_DISABLE` | `defined` / `undefined` | Disable the AT driver. |
| `EMBEDDED_UTILS_AT_BAUD_RATE` | `<value>` | Use a fixed baud rate if defined, otherwise the value is dynamically given. |
| `EMBEDDED_UTILS_AT_REPLY_END` | `<string>` | Reply ending string. |
| `EMBEDDED_UTILS_AT_FORCE_OK` | `undefined` / `defined` | Force status printing after command success. |
| `EMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE` | `undefined` / `defined` | Enable AT basic commands (ping, echo, etc.). |
| `EMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE` | `<value>` | Maximum number of commands that can be registered. |
| `EMBEDDED_UTILS_AT_BUFFER_SIZE` | `<value>` | Internal RX buffer size of the AT driver. |
| `EMBEDDED_UTILS_AT_BOARD_NAME` | `<value>` | Board name. |
| `EMBEDDED_UTILS_AT_HW_VERSION_MAJOR` | `<value>` | Hardware major version. |
| `EMBEDDED_UTILS_AT_HW_VERSION_MINOR` | `<value>` | Hardware minor version. |
| `EMBEDDED_UTILS_AT_SW_VERSION_MAJOR` | `<value>` | Software major version. |
| `EMBEDDED_UTILS_AT_SW_VERSION_MINOR` | `<value>` | Software minor version. |
| `EMBEDDED_UTILS_AT_SW_VERSION_INDEX` | `<value>` | Software version index. |
| `EMBEDDED_UTILS_AT_SW_VERSION_DIRTY_FLAG` | `0` / `1` | Software version dirty flag. |
| `EMBEDDED_UTILS_AT_SW_VERSION_ID` | `<value>` | Software version ID. |
| `EMBEDDED_UTILS_ERROR_STACK_DEPTH` | `<value>` | Maximum number of errors stored in stack. |
| `EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE` | `<value>` | Default value to store in stack when there is no error. |
| `EMBEDDED_UTILS_ERROR_STACK_SIGFOX` | `undefined` / `defined` | Enable specific function to import Sigfox EP library errors in stack. | 
| `EMBEDDED_UTILS_MATH_DRIVER_DISABLE` | `defined` / `undefined` | Disable the MATH driver. |
| `EMBEDDED_UTILS_MATH_PRECISION` | `0` / `1` / `2` | Math computation mode: `0` = using integer `1` = using float `2` = using double. |
| `EMBEDDED_UTILS_MATH_COS_TABLE` | `undefined` / `defined` | Enable cosine table declaration. |
| `EMBEDDED_UTILS_MATH_SIN_TABLE` | `undefined` / `defined` | Enable sine table declaration. |
| `EMBEDDED_UTILS_MATH_ATAN2` | `undefined` / `defined` | Enable atan2 function. |
| `EMBEDDED_UTILS_PARSER_DRIVER_DISABLE` | `defined` / `undefined` | Disable the PARSER driver. |
| `EMBEDDED_UTILS_STRING_DRIVER_DISABLE` | `defined` / `undefined` | Disable the STRING driver. |
| `EMBEDDED_UTILS_STRING_HEXADECIMAL_UPPER_CASE` | `undefined` / `defined` | Select format when converting hexadecimal numbers into string. |
| `EMBEDDED_UTILS_SWREG_DRIVER_DISABLE` | `defined` / `undefined` | Disable the SWREG driver. |
| `EMBEDDED_UTILS_TERMINAL_DRIVER_DISABLE` | `defined` / `undefined` | Disable the TERMINAL driver. |
| `EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER` | `<value>` | Number of terminals to use. |
| `EMBEDDED_UTILS_TERMINAL_BUFFER_SIZE` | `<value>` | Internal TX buffer size of the terminal driver. |
| `EMBEDDED_UTILS_TERMINAL_MODE_BUS` | `defined` / `undefined` | Enable destination address setting in terminal driver. |

# Build

A static library can be compiled by command line with `cmake`.

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="<toolchain_file_path>" \
      -DTOOLCHAIN_PATH="<arm-none-eabi-gcc_path>" \
      -DTYPES_PATH="<types_file_path>" \
      -DEMBEDDED_UTILS_HW_INTERFACE_ERROR_BASE_LAST=0 \
      -DEMBEDDED_UTILS_AT_BAUD_RATE=OFF \
      -DEMBEDDED_UTILS_AT_REPLY_END=\"\\r\\n\" \
      -DEMBEDDED_UTILS_AT_FORCE_OK=ON
      -DEMBEDDED_UTILS_AT_INTERNAL_COMMANDS_ENABLE=ON \
      -DEMBEDDED_UTILS_AT_COMMANDS_LIST_SIZE=32 \
      -DEMBEDDED_UTILS_AT_BUFFER_SIZE=64 \
      -DEMBEDDED_UTILS_AT_BOARD_NAME=\"name\" \
      -DEMBEDDED_UTILS_AT_HW_VERSION_MAJOR=0 \
      -DEMBEDDED_UTILS_AT_HW_VERSION_MINOR=0 \
      -DEMBEDDED_UTILS_AT_SW_VERSION_MAJOR=0 \
      -DEMBEDDED_UTILS_AT_SW_VERSION_MINOR=0 \
      -DEMBEDDED_UTILS_AT_SW_VERSION_INDEX=0 \
      -DEMBEDDED_UTILS_AT_SW_VERSION_DIRTY_FLAG=0 \
      -DEMBEDDED_UTILS_AT_SW_VERSION_ID=0 \
      -DEMBEDDED_UTILS_ERROR_STACK_DEPTH=32 \
      -DEMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE=0 \
      -DEMBEDDED_UTILS_ERROR_STACK_SIGFOX=OFF \
      -DEMBEDDED_UTILS_MATH_PRECISION=0 \
      -DEMBEDDED_UTILS_MATH_COS_TABLE=ON \
      -DEMBEDDED_UTILS_MATH_SIN_TABLE=ON \
      -DEMBEDDED_UTILS_MATH_ATAN2=ON \
      -DEMBEDDED_UTILS_STRING_HEXADECIMAL_UPPER_CASE=OFF \
      -DEMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER=1 \
      -DEMBEDDED_UTILS_TERMINAL_BUFFER_SIZE=64 \
      -DEMBEDDED_UTILS_TERMINAL_MODE_BUS=OFF \
      -G "Unix Makefiles" ..
make all
```
