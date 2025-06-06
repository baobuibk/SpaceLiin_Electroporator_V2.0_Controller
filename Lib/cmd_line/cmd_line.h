//*****************************************************************************
//
// cmdline.h - Prototypes for command line processing functions.
//
// Copyright (c) 2007-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the Tiva Utility Library.
//
//*****************************************************************************

#ifndef CMD_LINE_H_
#define CMD_LINE_H_
#include <stdint.h>
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup cmdline_api
//! @{
//
//*****************************************************************************
#define CMDLINE_OK        (0)
//*****************************************************************************
//
//! Defines the value that is returned if the command is not found.
//
//*****************************************************************************
#define CMDLINE_BAD_CMD         (1)

//*****************************************************************************
//
//! Defines the value that is returned if there are too many arguments.
//
//*****************************************************************************
#define CMDLINE_TOO_MANY_ARGS   (2)

//*****************************************************************************
//
//! Defines the value that is returned if there are too few arguments.
//
//*****************************************************************************
#define CMDLINE_TOO_FEW_ARGS   (3)

//*****************************************************************************
//
//! Defines the value that is returned if an argument is invalid.
//
//*****************************************************************************
#define CMDLINE_INVALID_ARG   (4)

//*****************************************************************************
//
//! Defines the value that is returned if an command is invalid.
//
//*****************************************************************************
#define CMDLINE_INVALID_CMD   (5)

//*****************************************************************************
//
//! Defines the value that is returned if an command is invalid.
//
//*****************************************************************************
#define CMDLINE_CALIB_IS_RUNNING   (6)

//*****************************************************************************
//
//! Defines the value that is returned if an command is invalid.
//
//*****************************************************************************
#define CMDLINE_IS_PROCESSING (7)

//*****************************************************************************
//
#define CMDLINE_NO_RESPONSE  (8)

// Command line function callback type.
//
//*****************************************************************************
typedef int (*pfnCmdLine)(int argc, char *argv[]);

//*****************************************************************************
//
//! Structure for an entry in the command list table.
//
//*****************************************************************************
typedef struct
{
    //
    //! A pointer to a string containing the name of the command.
    //
    const char *pcCmd;

    //
    //! A function pointer to the implementation of the command.
    //
    pfnCmdLine pfnCmd;

    //
    //! A pointer to a string of brief help text for the command.
    //
    const char *pcHelp;
}
tCmdLineEntry;

//*****************************************************************************
//
//! This is the command table that must be provided by the application.  The
//! last element of the array must be a structure whose pcCmd field contains
//! a NULL pointer.
//
//*****************************************************************************
extern tCmdLineEntry g_psCmdTable[];

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern uint8_t CmdLineProcess(char *pcCmdLine);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // CMD_LINE_H_
