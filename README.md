# Description

This repository contains utility functions for embedded projects.

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `EMBEDDED_UTILS_ERROR_STACK_DEPTH` | `<error_stack_depth>` | Maximum number of errors stored in stack. |
| `EMBEDDED_UTILS_ERROR_STACK_SUCCESS_VALUE` | `<success_value>` | Default value to store in stack when there is no error. |
| `EMBEDDED_UTILS_ERROR_STACK_SIGFOX` | `undefined` / `defined` | Enable specific function to import Sigfox EP library errors in stack. |
| `EMBEDDED_UTILS_MATH_PRECISION` | `0` / `1` / `2` | Math compulation mode: 0 = using integer, 1 = using float, 2 = using double. |
| `EMBEDDED_UTILS_MATH_COS_TABLE` | `undefined` / `defined` | Enable cosine table declaration. |
| `EMBEDDED_UTILS_MATH_SIN_TABLE` | `undefined` / `defined` | Enable sine table declaration. |
