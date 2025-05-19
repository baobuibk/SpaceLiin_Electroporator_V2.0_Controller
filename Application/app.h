#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MY_CORE_H_
#include "stm32f4_header.h"

// SYSTEM DRIVER //
#include "board.h"

// USER DRIVER //
#include "scheduler.h"
#include "uart.h"

// INCLUDE TASK //
// #include "adc_task.h"
#include "cap_controller_task.h"
#include "pid_task.h"
#include "cap_calib_task.h"
#include "cmd_line_task.h"
#include "fsp_line_task.h"
// #include "impedance_task.h"

#include "command.h"

void App_Main(void);

#endif /* APP_H_ */
