#pragma once

namespace MatIOType
{
    struct MatIOType_t
    {
    public:
        const uint8_t index;
        const std::string name;
    }
        const static NONE{0, "None"},
                     SHORT_16{1, "Short (16-bit int)"},
                     DOUBLE_64{2, "Double (64-bit floating point)"},
                     FLOAT_32{3, "Float (32-bit floating point)"},
                     INT_32{4, "Int (32-bit int)"},
                     CHAR_8{5, "Char (8-bit int)"},
                     LONG_64{6, "Long (64-bit int)"},
                     VEC_3_CHAR_8{7, "Char-3-Vector (3x 8-bit int)"};
};
