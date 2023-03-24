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
#define VERSION                         ("V1.1 R6")     //< Version string literal
#define DEFAULT_DEVICE_PWIDTH           (508)           //< Preferred displaying device
#define DEFAULT_DEVICE_PHEIGHT          (285)           //< Preferred displaying device
#define TOUCH_DEVICE_PWIDTH             (150)
#define TOUCH_DEVICE_PHEIGHT            (100)
#define STREAM_WIDTH                    (1280)          //< Camera provider's output size width
#define STREAM_HEIGHT                   (720)           //< Camera provider's output size height
#define DISCRETE_STEPS                  (16)
#define LINE_MASK                       (0x40)
#define WEB_UI                          (true)
#define DO_CALIB                        (false)
#define TARGET_VOLUME                   (3300000)       //< Discrete sand volume
#define TARGET_VOLUME_TOLERANCE         (0.05)           //< Discrete sand volume +/-value%
#define TARGET_VOLUME_UPPER             (TARGET_VOLUME * (1.0 + TARGET_VOLUME_TOLERANCE))
#define TARGET_VOLUME_LOWER             (TARGET_VOLUME * (1.0 - TARGET_VOLUME_TOLERANCE))

#define TEMPORAL_BUFFER_LENGTH          (12)            //< Size for Temporal buffer
#define PERLIN_LINGER_LENGTH            (30)            //< Amount of time that the perlin image stays on screen

#define CUTOFF_TOP 53
#define CUTOFF_BOT 681
#define CUTOFF_LEFT 250
#define CUTOFF_RIGHT 1100
