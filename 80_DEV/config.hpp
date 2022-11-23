/*****************************************
|      ____    _  _   _   _   ____       |
|     |  _ \  | || | | \ | | |  __|      |
|     | | \ \ | || | |  \| | | |_        |
|     | | | | | || | |     | |  _|       |
|     | |_/ / | || | | |\  | | |__       |
|     |____/  \____/ |_| \_| |____|      |
|                                        |
|   DUNE - Sandbox Depth Visualizer      |
|  A project by                          |
|  Ludwig Zeller and David Schoosleitner |
|                                        |
|****************************************|
|                                        |
| @file main.cpp                         |
| @author Ludwig Zeller                  |
|                                        |
| This file is responsible for:          |
|  - Configuration for misc. files       |
| This file depends on:                  |
|  -                                     |
|                                        |
|****************************************|
|                                        |
| USED LIBRARIES ARE PROPERTY OF THEIR   |
| CORRESPECTIVE OWNERS.                  |
|                                        |
| SEE COPYRIGHT.md IN PROJECT'S ROOT     |
| DIRECTORY, AS WELL AS                  |
| INDIVIDUAL LICENSES AND TOS.           |
|                                        |
*****************************************/

#pragma once


/****************************************\
|              DEFINITIONS               |
\****************************************/

#define FPS                             (15.0f)         //< Frames per second constant, used by pipeline
#define CPS                             (15u)           //< Captures per second constant, used by rs Grabber
#define FULLSCREEN                      (true)          //< Fullscreen constant for window
#define CYCLE_COUNTER_AVERAGE           (10)            //< Number of ms/frame to be logged
#define CALIBRATION_LOOP_THRESHOLD      (10)            //< Number of frames allowed to elapse until calibration cycle
#define DEBUG_ON_SCREEN                 (true)          //< Do display debug information onscreen
#define VERSION                         ("V1.1 R6")     //< Version string literal
#define DEFAULT_DEVICE_NAME             ("HDMI-3")      //< Preferred displaying device