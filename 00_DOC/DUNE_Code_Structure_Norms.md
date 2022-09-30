# **Commenting and Structural Coding Norms for Project DUNE**
## **0. Index**
1. [Introduction](#1-introduction) 
2. [Head of File](#2-head-of-file)
3. [Section Division](#3-section-division)
4. [Section Sub-Divisions](#4-section-sub-divisions)
5. [Commenting Declarations and Definitions](#5-commenting-declarations-and-definitions)
    - [5.1. Commenting Downwards](#51-commenting-downwards)
    - [5.2. Commenting to the Left](#52-commenting-to-the-left)
6. [Commenting Spacing](#6-comment-spacing)
7. [Function Headings](#7-function-headings)
    - [7.1. Comments within Functions](#71-comments-within-functions)
8. [Class and Struct Headings](#8-class-and-struct-headings)
9. [Operators and Brackets](#9-operators-and-brackets)
    - [9.1. Spacing for Operators](#91-spacing-for-operators)
10. [Squiggly Brackets](#10-squiggly-brackets)
11. [Implicit Casting](#11-implicit-casting)
12. [Defines with Single Values](#12-defines-with-values)
13. [Norms not mentioned](#13-norms-not-mentioned)

---
## **1. Introduction**

This file includes examples of norms to commenting and describing lines, such as code structure within all code files.

**You may copy any commenting template in this file to your heart's content.**

These norms will **not** influence the compiler in any way.

MISRA is a tool that checks if a project contains code that is not acceptable according to its norm. It is used by Automotive firms and industries. Due to the automotive standards, this is very much applicable for the HTL.

Follow these examples to commenting files and read their description above:

---

## **2. Head of File**
The head of a file is placed in the first lines of the file. Its purpose is to claim this file as part of a project, describe the purpose of said file and its dependencies.

Think of it as a *rough description for humans*:
```
/****************************************\
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
|  - Pipeline and loop                   |
| This file depends on:                  |
|  - GLAD Library                        |
|  - glfw3 Library                       |
|  - OpenCV Library                      |
|  - Intel (R) Real Sense 2 Library      |
|  - Local Utilities                     |
|                                        |
\****************************************/

#ifndef __MAIN_HPP_
#define __MAIN_HPP_


/****************************************\
| ......
```
---
## **3. Section Division**
Almost all files have something to #include or #define; something to typedef and declare or intialize.
It is good practice to divide these into "sections", at least visually.

This can be good for readability for humans.
```
/****************************************\
|               INCLUDES                 |
\****************************************/
#include <iostream>
#include "utils/utils.hpp"


/****************************************\
|              DEFINITIONS               |
\****************************************/
#define UINT_MISCELANEOUS_CONSTANT (10u)


/****************************************\
|          LOCAL DECLARATIONS            |
\****************************************/
volatile uint32_t random_integer;


/****************************************\
|          GLOBAL DECLARATIONS           |
\****************************************/
typedef struct Global_Struct_s {
    ...
} Global_Struct_t;


/****************************************\
|           LOCAL DEFINITIONS            |
\****************************************/
random_integer = INT_MISCELANEOUS_CONSTANT;


/****************************************\
|          GLOBAL DEFINITIONS            |
\****************************************/
...
```

Of course extra sections such as "FUNCTIONS" or "CLASS XYZ" may be added.

---
## **4. Section Sub-Divisions**

This step is not too important but may also boost the readability of your document. The idea is to divide your sections for another layer, or even more layers.

```
/****************************************\
|               INCLUDES                 |
\****************************************/

/*****       LIBRARY INCLUDES       *****/
#include <glfw/glfw3.h>

/*****   LOCAL UTILITIES INCLUDES   *****/

/****     WINDOW UTILS INCLUDES      ****/
#include "utils/Window.h"
#include "utils/WindowUtils.h"

/****     OTHER UTILS INCLUDES       ****/
#include "utils/Utils.h"

/*****        MISCELLANEOUS         *****/
#include ....
```
Reduce the amount of *'s to subdivide those divisions again.

---
## **5. Commenting declarations and definitions**

This is how the MISRA norm checker does it. The direction to which is commented to is the first (to 2nd) character in said comment.

Usually your IDE supports tooltips via comments either way.

---
### **5.1. Commenting downwards**

```
//! XYZ
int something = something_else + yes;
```

### **5.2. Commenting to the left**

```
#define SOMETHING_CONSTANT (10u)        //< XYZ
```
---
## **6. Spacing**

Relevant for readability. Sometimes you are required to write multiple lines of #define to e.g. configurate something (also applies to multiple lines of similar things).

Since defines, declarations and initializations are usually not a set amount of characters long, you will end up with comments and code starting at different columns.
Therefore, reading comments for individual lines can be very tasking for the eyes.

Keep your spacing and tabs at an equal distance for all applicable code in a row. However, keeping the same distance for all defines or initializations in a row is **not** suggested.

```
#define UINT_CFG_FOR_SOMETHING  (10u)                           //< ABC
#define UINT_CFG_FOR_ELSE       (2.2f)                          //< DEF
#define UINT_CFG_FPS            (16u)                           //< GHI
#define UINT_CFG_WHATEVER       (UINT_CFG_FOR_ELSE + 1)         //< JKL


/****************************************\
|          GLOBAL DEFINITIONS            |
\****************************************/
//! Global Runtime Environment; holds global data statically
extern static RTE_t RTE =
{
    .some_value = 1,                        //< ABC
    .something_else = 2,                    //< DEF
    .example = "EXAMPLE",                   //< GHI
    .max = UINT_CFG_MUSTERMANN,             //< JKL
    ...                                     //< ...
};
```
---
## **7. Function Headings**

This is the comment that shows up as a doxygen-markdown tooltip upon hovering on any method.

The comment of a method's **definition** is usually the tooltip that the IDE uses, therefore MISRA norms accept not having to use this type of comment for the **declaration** a second time. 

```
/**
 * @fn uint8_t Some_method(Stuff_class &stuff, const std::string another_param, const unsigned int a_param = 10u)
 * @brief This method is responsible for something.
 * @param stuff Object to be changeds
 * @param another_param Argument string
 * @param a_param Some integer | Default = 10u
 * @returns Error code | 0 = Success
 */
uint8_t Some_method(Stuff_class &stuff, const std::string another_param, const unsigned int a_param = 10u)
{
    ...
}

/**
 * @fn void Another_method(void)
 * @brief Method responsible for something else.
 * @param none
 * @returns none
 */
void Another_method(void)
{
    ...
}
```
---
### **7.1. Comments within Functions**

You are free to subdivide sections of code within functions and freely comment without having to think too much about it. Of course you may still apply the downward and left rules.

---
## **8. Class and Struct Headings**

Like in [Function Headings](#7-function-headings), doxygen-markdown is supported.

Keep in mind that your IDE probably will not show the first three lines.
```
/**********************\
 *   CLASS Test_c     *
 **********************
 * @class Test_c : This
 * @brief Test Test_c is a class for XYZ.
\**********************/
static class Test_c
{
    ...
```
---
## **9. Operators and Brackets**

C++ Operators always follow a [set order](https://en.cppreference.com/w/cpp/language/operator_precedence) according to the common norms of C++.

Because said order can be difficult to remember for more rarely used operators, you are to use () - round brackets to ensure a certain order of operation without any chance to mix it up by accident.
This applies to numerical and logical operators as well.
```
int value_a = (2 * something) - (2 * something_else) + 10 + DEF_ADJUST_INT;

if((value_a == value_b) && (something != (DEF_SOMETHING - 5)))
{
    ...
```
---
### ***9.1. Spacing for Operators***

Always put a space between any numerical and logical operation.
You __may__ put a space after opening and before closing a bracket as well.
```
a + b
(a[0] == ~((*(b[0])) / 2))
( a + b )                   //< You can do this as well.
(a == 1) ? (true) : (false)
```
---
## **10. Squiggly Brackets**

MISRA only accepts to open squiggly brackets on the line after a function definition, an if or a for statement, etc.

```
if(statement == true)
{
    ...
}
```
---
## **11. Implicit Casting**

Implicit casting is to be avoided. This includes casting to a smaller data types (e.g. int32 to int8), vice versa.

This is to avoid casting errors from the compiler or from the user. A classic example of a mistake would be dividing a float by an integer.

C++ provides you with the "auto" keyword. You may use it, but make sure to get the R-value right!

Furthermore, make sure to assign unsigned values to unsigned variables and cast via (signed) or (unsigned) if you have to.

```
bool result = (bool) int_result;
float calc = (float) (a_float / (float) an_int);
auto str = std::to_string(calc);
uint8_t value_ui8 = (uint8_t) (value_ui32 & 0xFFu);
#define DEF_SOMETHING (100u)
unsigned int a = DEF_SOMETHING;
```
---
## **12. Defines with Values**

Defines, includes and pragmas are on the compiler layer and Defines directly convert a text into another text.
A define within a define is able to expand the parameters which can be helpful for compiler-level concatenation.

It is possible to reduce extremely rare errors via defines and value expansion by putting the define value in () - round brackets as well as define parameters if given.

```
#define UINT_DEF_CONST_VALUE_1 (10u) //< Since this value is also unsigned, make sure to give it a u-suffix.
#define UINT_SOME_MACRO(value) (((unsigned int) (value)) * UINT_DEF_CONST_VALUE_1) 
```
---
## **13. Norms not mentioned**

- Single lines of code that explain themselves and do not need more explanation should **not** be commented.
- Preferably use subdividors along with downward comments in switch-cases.
- Preferably use squiggly brackets for switch-cases.
- In-code-comments should always be indented with their code.
- In case of having multiple correct alternatives to comment something, use the most natural and most visually appealing one.
