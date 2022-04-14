#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

namespace uts {
    enum class SCALE
    {
        SCALE_FACTOR                = 4,
        ZOOMED_AREA_WIDTH           = 100,
        ZOOMED_AREA_HEIGHT          = 100,
        ZOOMED_AREA_CURSOR_OFFSET   = 50
    };

    constexpr int CURSOR_MOVE_FACTOR{1};

    /** Struct is used for storing original and scaled dimensions of each connected screen */
    typedef struct Dimensions{
        Dimensions(uint32_t origWidth, uint32_t origHeight, uint32_t scalWidth, uint32_t scalHeight)
            : originalWidth(origWidth)
            , originalHeight(origHeight)
            , scaledWidth(scalWidth)
            , scaledHeight(scalHeight)
        {
        };

        uint32_t originalWidth;
        uint32_t originalHeight;
        uint32_t scaledWidth;
        uint32_t scaledHeight;

    private:
        Dimensions() = default;
    } Dimensions;

    /** All modifiers supported by Win32 API */
    enum class MODIFIERS
    {
        ALT         = 0x0001,
        CONTROL     = 0x0002,
        NOREPEAT    = 0x4000,
        SHIFT       = 0x0004,
        WIN         = 0x0008
    };

    inline MODIFIERS operator|(MODIFIERS mod1, MODIFIERS mod2)
    {
        return static_cast<MODIFIERS>(static_cast<int>(mod1) | static_cast<int>(mod2));
    }

    /** Subset of Virtual Key codes supported by Win32 API */
    enum class KEYCODES
    {
        KEY_T           = 0x54,
        KEY_P           = 0x50,
        KEY_PRINTSCR    = 0x2C
    };
}

#endif // UTILITIES_H
