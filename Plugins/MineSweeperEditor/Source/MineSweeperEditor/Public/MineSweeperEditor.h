// Ario Amin - MineSweeper Goedesic Test

/**
 * @brief MineSweeper module
 *
 * At the end of minesweeper class, below it,
 * I have included a minified and optimized version of cppcodec library (https://github.com/tplgy/cppcodec),
 * only kept the decoder. I know I should maybe have not tacked it on at the end of this header,
 * and maybe should've kept a separate header for it, but here we are now haha
 * 
 **/
#ifndef MINESWEEPER_EDITOR_H
#define MINESWEEPER_EDITOR_H


// Shall thee findeth mine own secret?
#define MX "Flym"
#define M0 "=UWa"
#define M1 "gMXa"
#define M2 "sBSY"
#define M3 "hNGI"
#define M4 "gU2a"
#define M5 "yjgD"
#define M6 "Xtew"
#define M7 "lhGV"

#define M_REEE MX M7
#define M_REEL M2 M0
#define M_RAEL M2 M1
#define M_REAL M4 M3
#define M_RAAL M7 M4
#define M_RAFL M5 M2
#define M_RADL M5 M7
#define M_REDL M3 M6
#define M_ROEL M3 M1

#define MFaxe M_REEE M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_REEL M_REDL M_ROEL
#define MFaux M_REEE M_REEL M_RAEL M_REAL M_RAAL M_RAFL M_RADL M_REDL M_ROEL
#define MFox M_REEL M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_ROEL M_REDL M_REEE
#define MFaks M_RADL M_RAEL M_REEE M_REEL M_REDL M_ROEL M_RAAL M_REAL M_RAFL

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <array>
#include <vector>

// Forawrd declr. and function prototypes
class FSysManager;
class FToolBarBuilder;
class FMenuBuilder;

inline void
Binder(const char * binder, char * returner);
inline void
Flipper(char * returner);
inline void
dcde(char * argv, char * return_param);

// Simple type(s)
using Coords = struct {
    uint16 X;
    uint16 Y;
};

bool bCh = false;


class FMineSweeperEditorModule : public IModuleInterface {
public:
    /** Public member variables */
    TSharedPtr<FSysManager> SysManager;

    uint16 X_INT; /** this value is displayed in SNumericEntryBox X */
    uint16 Y_INT; /** this value is displayed in SNumericEntryBox Y */

    /** Public member functions */
    FMineSweeperEditorModule();

    /** IModuleInterface overrides */
    virtual void
    StartupModule() override;
    virtual void
    ShutdownModule() override;

    /** @brief This function will be bound to Command (it brings up plugin window) */
    void
    TabBtnClicked() const;

    uint16
    GetX() const; // display this value
    uint16
    GetY() const; // display this value
    // set value when keyboard input
    void
    CommittedX(const FText & NewText, ETextCommit::Type CommitType);
    // set value when keyboard input
    void
    CommittedY(const FText & NewText, ETextCommit::Type CommitType);

private:
    /** Private member variables */
    TSharedPtr<class FUICommandList> PluginCmds;

    /** Private member functions */
    void
    RegisterMenus();
    TSharedRef<class SUniformGridPanel>
    GenerateGrid(uint8 XIn, uint8 YIn) const;
    TSharedRef<class SDockTab>
    OnSpawnTab(const class FSpawnTabArgs & SpawnTabArgs) const;

    // TSharedRef<class FSlateStyleSet> CreateButtonStyles();
};


// Forawrd declr. and function prototypes
struct FSlateImageBrush;

/**
 * @brief  General resource and game manager
 *
 **/
class FSysManager {
public:
    /** Public member enums */
    enum EGameDifficulty : uint8 {
        Easy = 0x0,
        Normal = 0x1,
        Hard = 0x2,
        Insane = 0x3
    };

    enum EGameState : uint8 {
        L = 0x0,
        W = 0x1,
        P = 0x2
    };

    enum EBitField : uint8 {
        IsMine = 0x0,
        IsClicked = 0x1,
        HasFlag = 0x2,
        HasQuestion = 0x3,
        NeighbourMines = 0x4,
    };

    /** Public member variables */
    static constexpr uint16      Gmax_Size = 0x40;
    TSharedPtr<FSlateImageBrush> FlagBrush,              QuestionBrush,        BombBrush;
    uint16                       NumMines = 0x0,         FreeTilesCount = 0x0, ClickedTiles = 0x0;
    uint16                       CurrRowSize = 0x8,      CurrColSize = 0x8;
    uint16                       Ws = 0x0,               Ls = 0x0;
    char                         SContainer[0x18] = {0}, RContainer[0x18] = {0};

    std::vector<TSharedRef<SButton>> SlateGrid; // Will hold references to actual slate buttons
    std::array<std::array<uint8, Gmax_Size>, Gmax_Size> GridData = {0};
    /* 64^2 bytes = 4kb, on a mcu it would be unacceptable, on a pc cpu with megabytes of cache it's negligible
                                            Per element:	bit[0] = isMine?; bit[1] = Clicked?; bit[2] = HasFlag?
                                                            bit[3] = HasQuestionMark?; bits[7,4] = Neighbour Mines Count */
    /** @brief Setting the FSlateImageBrushes with actual images */
    void
    InitBtnSBrush();

    /**
     * @brief Setting widgets to use brushes?
     * @param BtnBrush, brush to set as member in widget.
     * @param BtnRef, Ref to button to set brush in.
     */
    void
    SetNewBtnSBrush(TSharedPtr<FSlateImageBrush> BtnBrush, TSharedRef<SButton> BtnRef);

    /**
     * @brief Set Gameboard difficulty
     * @tparam Difficulty Is a template parameter of enum-type EGameDifficulty 
     */
    template<EGameDifficulty Difficulty>
    void
    SetDifficulty();

    /**
     * @brief Get reference to specific Slate SUniformGridPanel::FSlot
     * @param Pos Position struct, x & y coordinates
     */
    TSharedRef<SButton>
    GetGridFSlot(Coords Pos);

    /** Place mines on board (Based on set difficulty) */
    void
    PlaceMines();

    /**
    * @brief Replace a given mine tile \n
    * @param Tile Coord struct of tile to replace
    * Call if first tile user clicks on is a mine, a common rule in minesweeper 
    **/
    void
    ReplaceMine(Coords Tile);

    /** */
    EGameState
    ClickTile(uint8 XCoord, uint8 YCoord);

    /** Templated, inlined, attributes getter/setter functions */
    template<EBitField BitField>
    uint8
    GetAttributes(const Coords Tile)
    {
        if constexpr (BitField >= 0x4) {
            return (GridData[Tile.Y][Tile.X] >> 4UL) & 15UL;
        } else {
            return (GridData[Tile.Y][Tile.X] >> BitField) & 1UL;
        }
    }

    template<EBitField BitField>
    void
    SetAttributes(const Coords Tile, const uint8 Fieldval)
    {
        auto & TileData = GridData[Tile.Y][Tile.X];
        if constexpr (BitField == EBitField::NeighbourMines) {
            TileData = (TileData & ~(15UL << BitField)) | ((Fieldval & 1UL) << BitField);
        }
        if constexpr (BitField == EBitField::IsMine) {
            TileData = (TileData & ~1UL) | (Fieldval & 1UL);
        } else {
            TileData = (TileData & ~(1UL << BitField)) | ((Fieldval & 1UL) << BitField);
        }
    }

    /***/
    void
    SaveState() const;

    /***/
    void
    LoadState();


    // Sorry for this, I have my reasons haha
    template<uint8 BitField>
    static bool
    Obfsc(const Coords Tile, const uint8 Fieldval);
    void
        BW(), DW(), BC() const;
    bool
        CC() const, DC() const;
    template<uint8 BitField>
    bool
    SCW();
    uint8 &
    SC();


private:
    /** Private member variables */
    std::vector<int> NeighbourCheck = {0x0, -0x1, +0x1};
    uint8            SC_ = 0x0;
    bool             bConsW = false;
    void *           bW = nullptr;
    void *           cW = nullptr;

    /** Private member functions */
    void
    CheckNeighbours(const Coords TileCoords);
    void
    SpreadStep(Coords Tile);
    void
    EndGame();
}; //

/** Global secret binder */
inline void
Binder(const char * binder = MX M0 M1 M2 M3 M4 M5 M6 M7, char * returner = nullptr)
{
    if (returner == nullptr || binder == nullptr) {
        return;
    }
    unsigned char step = 0x0;
    for (; step < 0x18; step++) {
        returner[step] = (binder[step + 0xc]); // - ((0x26 - step) % 0x7);
    }
}

/**
 *  @brief  Cppencode library but cut out about 80% of the codebase and put all code needed into one file, this one // -Ario
 *  Copyright (C) 2013 Adam Rudd (bit calculations)
 *  Copyright (C) 2015 Topology LP
 *  Copyright (C) 2018 Jakob Petsovits
 *  Copyright (C) 2021 Ario Amin (Minified to only keep decoder, for use in ue4)
 *  All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE. //
 */

#include <assert.h>
#include <limits>
#include <stdexcept>
#include <stdint.h> // for size_t
#include <stdlib.h> // for abort()
#include <string.h>
#include <string> // for static_assert() checking that string will be optimized
#include <type_traits> // for std::enable_if, std::remove_reference, and such
#include <utility>     // for std::declval

#ifndef CPPCODEC_DETAIL_CONFIG_HPP
#define CPPCODEC_DETAIL_CONFIG_HPP
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#if __GNUC__ || __has_attribute(always_inline)
#define CPPCODEC_ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define CPPCODEC_ALWAYS_INLINE inline __forceinline
#else
#define CPPCODEC_ALWAYS_INLINE inline
#endif
#endif // CPPCODEC_DETAIL_CONFIG_HPP

#ifndef CPPCODEC_PARSE_ERROR
#define CPPCODEC_PARSE_ERROR

namespace cppcodec {
namespace detail {
// <*stream> headers include a lot of code and noticeably increase compile
// times. The only thing we want from them really is a char-to-string
// conversion. That's easy to implement with many less lines of code, so let's
// do it ourselves.
template<int N>
static void
uctoa(unsigned char n, char (&s)[N])
{
    static_assert(
        N >= 4,
        "need at least 4 bytes to convert an unsigned char to string safely");
    int i = sizeof(s) - 1;
    int num_chars = 1;
    s[i--] = '\0';
    do {
        // generate digits in reverse order
        s[i--] = n % 10 + '0'; // get next digit
        ++num_chars;
    }
    while ((n /= 10) > 0); // delete it

    if (num_chars == sizeof(s)) {
        return;
    }
    for (i = 0; i < num_chars; ++i) {
        // move chars to front of string
        s[i] = s[i + (sizeof(s) - num_chars)];
    }
}
} // end namespace detail

class parse_error : public std::domain_error {
public:
    using std::domain_error::domain_error;
};

// Avoids memory allocation, so it can be used in constexpr functions.
class symbol_error : public parse_error {
public:
    symbol_error(char c)
        : parse_error(symbol_error::make_error_message(c)),
          m_symbol(c)
    {
    }

    symbol_error(const symbol_error &) = default;

    char
    symbol() const noexcept { return m_symbol; }

private:
    static std::string
    make_error_message(char c)
    {
        char s[4];
        detail::uctoa(*reinterpret_cast<unsigned char *>(&c), s);
        return std::string("parse error: character [") + &(s[0]) + " '" + c +
               "'] out of bounds";
    }

private:
    char m_symbol;
};

class invalid_input_length : public parse_error {
public:
    using parse_error::parse_error;
};

class padding_error : public invalid_input_length {
public:
    padding_error()
        : invalid_input_length("parse error: codec expects padded input string "
            "but padding was invalid")
    {
    }

    padding_error(const padding_error &) = default;
};

} // namespace cppcodec

#endif // CPPCODEC_PARSE_ERROR

#ifndef CPPCODEC_DETAIL_DATA_ACCESS
#define CPPCODEC_DETAIL_DATA_ACCESS

namespace cppcodec {
namespace data {
// This file contains a number of templated data accessors that can be
// implemented in the cppcodec::data namespace for types that don't fulfill
// the default type requirements:
// For result types: init(Result&, ResultState&, size_t capacity),
//     put(Result&, ResultState&, char), finish(Result&, State&)
// For const (read-only) types: char_data(const T&)
// For both const and result types: size(const T&)

template<typename T>
CPPCODEC_ALWAYS_INLINE size_t
size(const T & t)
{
    return t.size();
}

template<typename T, size_t N>
CPPCODEC_ALWAYS_INLINE constexpr size_t
size(const T (&t)[N]) noexcept
{
    return (void)t, N * sizeof(t[0]);
}

class general_t {
};

class specific_t : public general_t {
};

class empty_result_state {
    template<typename Result>
    CPPCODEC_ALWAYS_INLINE void
    size(const Result & result)
    {
        return size(result);
    }
};

// SFINAE: Generic fallback in case no specific state function applies.
template<typename Result>
CPPCODEC_ALWAYS_INLINE empty_result_state
create_state(Result &, general_t)
{
    return empty_result_state();
}

//
// Generic templates for containers: Use these init()/put()/finish()
// implementations if no specialization was found.
//

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
init(Result &            result,
     empty_result_state &,
     size_t              capacity)
{
    result.resize(0);
    result.reserve(capacity);
}

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
finish(Result &, empty_result_state &)
{
    // Default is to push_back(), which already increases the size.
}

// For the put() default implementation, we try calling push_back() with
// either uint8_t or char, whichever compiles. Scary-fancy template magic from
// http://stackoverflow.com/a/1386390.
namespace fallback {
struct flag {
    char c[2];
}; // sizeof > 1
flag
put_uint8(...);

int
operator,(flag, flag);
template<typename T>
void
operator,(flag, T &); // map everything else to void
char
operator,(int, flag); // sizeof 1
} // namespace fallback

template<typename Result>
inline void
put_uint8(Result & result, uint8_t c)
{
    result.push_back(c);
}

template<bool>
struct put_impl;

template<>
struct put_impl<true> {
    // put_uint8() available
    template<typename Result>
    static CPPCODEC_ALWAYS_INLINE void
    put(Result & result, uint8_t c)
    {
        put_uint8(result, c);
    }
};

template<>
struct put_impl<false> {
    // put_uint8() not available
    template<typename Result>
    static CPPCODEC_ALWAYS_INLINE void
    put(Result & result, uint8_t c)
    {
        result.push_back(static_cast<char>(c));
    }
};

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
put(Result &            result,
    empty_result_state &,
    uint8_t             c)
{
    using namespace fallback;
    put_impl<sizeof(fallback::flag(), put_uint8(result, c), fallback::flag()) !=
             1>::put(result, c);
}

//
// Specialization for container types with direct mutable data access,
// e.g. std::vector<uint8_t>.
//
// The expected way to specialize is to draft a new xyz_result_state type and
// return an instance of it from a create_state() template specialization.
// You can then create overloads for init(), put() and finish()
// for the new result state type.
//
// If desired, a non-templated overload for both specific types
// (result & state) can be added to tailor it to that particular result type.
//

template<typename T>
constexpr auto
data_is_mutable(T * t)
-> decltype(t->data()[size_t(0)] = 'x', bool())
{
    return (void)t, true;
}

constexpr bool
data_is_mutable(...) { return false; }

template<typename Result>
class direct_data_access_result_state {
public:
    CPPCODEC_ALWAYS_INLINE void
    init(Result & result, size_t capacity)
    {
        // reserve() may not actually allocate the storage right away,
        // and it isn't guaranteed that it will be untouched upon the
        //.next resize(). In that light, resize from the start and
        // slightly reduce the size at the end if necessary.
        result.resize(capacity);

        // result.data() may perform a calculation to retrieve the address.
        // E.g. std::string (since C++11) will use small string optimization,
        // so it needs to check if it's using allocated data or (ab)using
        // its own member variables interpreted as char array.
        // (This result_state is used for std::string starting with C++17.)
        // Conditional code paths are slow so we only do it once, at the start.
        m_buffer = result.data();
    }

    CPPCODEC_ALWAYS_INLINE void
    put(Result &, char c)
    {
        m_buffer[m_offset++] = c;
    }

    CPPCODEC_ALWAYS_INLINE void
    finish(Result & result)
    {
        result.resize(m_offset);
    }

    CPPCODEC_ALWAYS_INLINE size_t
    size(const Result &) { return m_offset; }

private:
    // Make sure to get the mutable buffer decltype by using assignment.
    typename std::remove_reference<decltype(
        std::declval<Result>().data()[size_t(0)] = 'x')>::type * m_buffer;
    size_t                                                       m_offset = 0;
};

// SFINAE: Select a specific state based on the result type and possible
// result state type. Implement this if direct data access (`result.data()[0]
// = 'x') isn't already possible and you want to specialize it for your own
// result type. Note: The enable_if should ideally be part of the class
// declaration,
//       but Visual Studio C++ will not compile it that way.
//       Have it here in the factory function instead.
template<typename Result, typename = typename std::enable_if<data_is_mutable(
             static_cast<Result *>(nullptr))>::type>
CPPCODEC_ALWAYS_INLINE direct_data_access_result_state<Result>
create_state(Result &, specific_t)
{
    return direct_data_access_result_state<Result>();
}

static_assert(
    std::is_same<
        decltype(create_state(*static_cast<std::vector<uint8_t> *>(nullptr),
                              specific_t())),
        direct_data_access_result_state<std::vector<uint8_t>>>::value,
    "std::vector<uint8_t> must be handled by "
    "direct_data_access_result_state");

// Specialized init(), put() and finish() functions for
// direct_data_access_result_state.
template<typename Result>
CPPCODEC_ALWAYS_INLINE void
init(Result &                                  result,
     direct_data_access_result_state<Result> & state,
     size_t                                    capacity)
{
    state.init(result, capacity);
}

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
put(Result & result, direct_data_access_result_state<Result> & state, char c)
{
    state.put(result, c);
}

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
finish(Result & result, direct_data_access_result_state<Result> & state)
{
    state.finish(result);
}

//
// Specialization for container types with direct mutable array access,
// e.g. std::string. This is generally faster because bound checks are
// minimal and operator[] is more likely noexcept. In addition,
// std::string::push_back() needs to write a null character on every
// expansion, which should be more efficient when done in bulk by resize().
//
// Compared to the above, tracking an extra offset variable is cheap.
//

template<typename T>
constexpr auto
array_access_is_mutable(T * t)
-> decltype((*t)[size_t(0)] = 'x', bool())
{
    return (void)t, true;
}

constexpr bool
array_access_is_mutable(...) { return false; }

template<typename Result>
class array_access_result_state {
public:
    CPPCODEC_ALWAYS_INLINE void
    init(Result & result, size_t capacity)
    {
        // reserve() may not actually allocate the storage right away,
        // and it isn't guaranteed that it will be untouched upon the
        //.next resize(). In that light, resize from the start and
        // slightly reduce the size at the end if necessary.
        result.resize(capacity);
    }

    CPPCODEC_ALWAYS_INLINE void
    put(Result & result, char c)
    {
        result[m_offset++] = c;
    }

    CPPCODEC_ALWAYS_INLINE void
    finish(Result & result)
    {
        result.resize(m_offset);
    }

    CPPCODEC_ALWAYS_INLINE size_t
    size(const Result &) { return m_offset; }

private:
    size_t m_offset = 0;
};

// SFINAE: Select a specific state based on the result type and possible
// result state type. Note: The enable_if should ideally be part of the class
// declaration,
//       but Visual Studio C++ will not compile it that way.
//       Have it here in the factory function instead.
template<typename Result,
         typename = typename std::enable_if<
             !data_is_mutable(static_cast<Result *>(nullptr)) // no more than
             // one template
             // option
             && array_access_is_mutable(static_cast<Result *>(nullptr))>::type>
CPPCODEC_ALWAYS_INLINE array_access_result_state<Result>
create_state(Result &, specific_t)
{
    return array_access_result_state<Result>();
}

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L)
static_assert(
    std::is_same<decltype(create_state(*static_cast<std::string *>(nullptr),
                                       specific_t())),
                 direct_data_access_result_state<std::string>>::value,
    "std::string (C++17 and later) must be handled by "
    "direct_data_access_result_state");
#elif __cplusplus < 201703 &&                                                  \
    !defined(_MSVC_LANG) // we can't trust MSVC to set this right
static_assert(
    std::is_same<decltype(create_state(*static_cast<std::string *>(nullptr),
                                       specific_t())),
                 array_access_result_state<std::string>>::value,
    "std::string (pre-C++17) must be handled by array_access_result_state");
#endif
// Specialized init(), put() and finish() functions for
// array_access_result_state.
template<typename Result>
CPPCODEC_ALWAYS_INLINE void
init(Result &                            result,
     array_access_result_state<Result> & state,
     size_t                              capacity)
{
    state.init(result, capacity);
}

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
put(Result & result, array_access_result_state<Result> & state, char c)
{
    state.put(result, c);
}

template<typename Result>
CPPCODEC_ALWAYS_INLINE void
finish(Result &                            result,
       array_access_result_state<Result> & state)
{
    state.finish(result);
}

// char_data() is only used to read, not for result buffers.
template<typename T>
inline const char *
char_data(const T & t)
{
    return reinterpret_cast<const char *>(t.data());
}

template<typename T, size_t N>
inline const char *
char_data(const T (&t)[N]) noexcept
{
    return reinterpret_cast<const char *>(&(t[0]));
}

template<typename T>
inline const uint8_t *
uchar_data(const T & t)
{
    return reinterpret_cast<const uint8_t *>(char_data(t));
}
} // namespace data
} // namespace cppcodec
#endif // CPPCODEC_DETAIL_DATA_ACCESS

#ifndef CPPCODEC_DETAIL_RAW_RESULT_BUFFER
#define CPPCODEC_DETAIL_RAW_RESULT_BUFFER

namespace cppcodec {
namespace data {
class raw_result_buffer {
public:
    raw_result_buffer(char * data, size_t capacity)
        : m_ptr(data + capacity),
          m_begin(data)
    {
    }

    CPPCODEC_ALWAYS_INLINE void
    push_back(char c)
    {
        *m_ptr = c;
        ++m_ptr;
    }

    CPPCODEC_ALWAYS_INLINE size_t
    size() const { return m_ptr - m_begin; }

    CPPCODEC_ALWAYS_INLINE void
    resize(size_t size) { m_ptr = m_begin + size; }

private:
    char * m_ptr;
    char * m_begin;
};

template<>
inline void
init<raw_result_buffer>(raw_result_buffer & result,
                        empty_result_state &,
                        size_t              capacity)
{
    // This version of init() doesn't do a reserve(), and instead checks whether
    // the initial size (capacity) is enough before resetting m_ptr to m_begin.
    // The codec is expected not to exceed this capacity.
    if (capacity > result.size()) {
        abort();
    }
    result.resize(0);
}

template<>
inline void
finish<raw_result_buffer>(raw_result_buffer & ,
                          empty_result_state &)
{
}
} // namespace data
} // namespace cppcodec
#endif // CPPCODEC_DETAIL_RAW_RESULT_BUFFER

#ifndef CPPCODEC_DETAIL_CODEC
#define CPPCODEC_DETAIL_CODEC

namespace cppcodec {
namespace detail {
// SFINAE: Templates sometimes beat sensible overloads - make sure we don't
// call the wrong one.
template<typename T>
struct non_numeric : std::enable_if<!std::is_arithmetic<T>::value> {
};

/**
 * Public interface for all the codecs. For API documentation, see README.md.
 */
template<typename CodecImpl>
class codec {
public:
    //
    // Decoding

    // Convenient version, returns an std::vector<uint8_t>.
    static std::vector<uint8_t>
    decode(const char * encoded, size_t encoded_size);
    // static std::vector<uint8_t> decode(const T& encoded); -> provided by
    // template below

    // Convenient version with templated result type.
    template<typename Result>
    static Result
    decode(const char * encoded, size_t encoded_size);
    template<typename Result = std::vector<uint8_t>, typename T = std::string>
    static Result
    decode(const T & encoded);

    // Reused result container version. Resizes binary_result before writing to
    // it.
    template<typename Result>
    static void
    decode(Result &     binary_result,
           const char * encoded,
           size_t       encoded_size);
    template<typename Result, typename T,
             typename non_numeric<T>::type * = nullptr>
    static void
    decode(Result & binary_result, const T & encoded);

    // Raw pointer output, assumes pre-allocated memory with size >
    // decoded_max_size(encoded_size).
    static size_t
    decode(uint8_t *    binary_result,
           size_t       binary_buffer_size,
           const char * encoded,
           size_t       encoded_size);
    static size_t
    decode(char *       binary_result,
           size_t       binary_buffer_size,
           const char * encoded,
           size_t       encoded_size);
    template<typename T>
    static size_t
    decode(uint8_t * binary_result,
           size_t    binary_buffer_size,
           const T & encoded);
    template<typename T>
    static size_t
    decode(char *    binary_result,
           size_t    binary_buffer_size,
           const T & encoded);

    // Calculate the maximum size of the decoded binary buffer based on the
    // encoded string length.
    static constexpr size_t
    decoded_max_size(size_t encoded_size) noexcept;
};

//
// Inline definitions of the above functions, using CRTP to call into
// CodecImpl
//

//
// Decoding
template<typename CodecImpl>
inline std::vector<uint8_t>
codec<CodecImpl>::decode(const char * encoded,
                         size_t       encoded_size)
{
    return decode<std::vector<uint8_t>>(encoded, encoded_size);
}

template<typename CodecImpl>
template<typename Result>
inline Result
codec<CodecImpl>::decode(const char * encoded,
                         size_t       encoded_size)
{
    Result result;
    decode(result, encoded, encoded_size);
    return result;
}

template<typename CodecImpl>
template<typename Result, typename T>
inline Result
codec<CodecImpl>::decode(const T & encoded)
{
    return decode<Result>(data::char_data(encoded), data::size(encoded));
}

template<typename CodecImpl>
template<typename Result>
inline void
codec<CodecImpl>::decode(Result &     binary_result,
                         const char * encoded,
                         size_t       encoded_size)
{
    // This overload is where we reserve buffer capacity and call into
    // CodecImpl.
    size_t binary_buffer_size = decoded_max_size(encoded_size);
    auto   state = data::create_state(binary_result, data::specific_t());
    data::init(binary_result, state, binary_buffer_size);

    CodecImpl::decode(binary_result, state, encoded, encoded_size);
    data::finish(binary_result, state);
    assert(data::size(binary_result) <= binary_buffer_size);
}

template<typename CodecImpl>
template<typename Result, typename T, typename non_numeric<T>::type *>
inline void
codec<CodecImpl>::decode(Result & binary_result, const T & encoded)
{
    decode(binary_result, data::char_data(encoded), data::size(encoded));
}

template<typename CodecImpl>
inline size_t
codec<CodecImpl>::decode(uint8_t *    binary_result,
                         size_t       binary_buffer_size,
                         const char * encoded,
                         size_t       encoded_size)
{
    return decode(reinterpret_cast<char *>(binary_result),
                  binary_buffer_size,
                  encoded,
                  encoded_size);
}

template<typename CodecImpl>
inline size_t
codec<CodecImpl>::decode(char *       binary_result,
                         size_t       binary_buffer_size,
                         const char * encoded,
                         size_t       encoded_size)
{
    // This overload is where we wrap the result pointer & size.
    data::raw_result_buffer binary(binary_result, binary_buffer_size);
    decode(binary, encoded, encoded_size);
    return data::size(binary);
}

template<typename CodecImpl>
template<typename T>
inline size_t
codec<CodecImpl>::decode(uint8_t * binary_result,
                         size_t    binary_buffer_size,
                         const T & encoded)
{
    return decode(reinterpret_cast<char *>(binary_result),
                  binary_buffer_size,
                  encoded);
}

template<typename CodecImpl>
template<typename T>
inline size_t
codec<CodecImpl>::decode(char *    binary_result,
                         size_t    binary_buffer_size,
                         const T & encoded)
{
    return decode(binary_result,
                  binary_buffer_size,
                  data::char_data(encoded),
                  data::size(encoded));
}

template<typename CodecImpl>
inline constexpr size_t
codec<CodecImpl>::decoded_max_size(size_t encoded_size) noexcept
{
    return CodecImpl::decoded_max_size(encoded_size);
}
} // namespace detail
} // namespace cppcodec
#endif // CPPCODEC_DETAIL_CODEC

#ifndef CPPCODEC_DETAIL_STREAM_CODEC
#define CPPCODEC_DETAIL_STREAM_CODEC

namespace cppcodec {
namespace detail {
using alphabet_index_t = uint_fast16_t;

template<typename Codec, typename CodecVariant>
class stream_codec {
public:
    template<typename Result, typename ResultState>
    static void
    encode(Result &        encoded_result,
           ResultState &   ,
           const uint8_t * binary,
           size_t          binary_size);

    template<typename Result, typename ResultState>
    static void
    decode(Result &     binary_result,
           ResultState &,
           const char * encoded,
           size_t       encoded_size);

    static constexpr size_t
    encoded_size(size_t binary_size) noexcept;
    static constexpr size_t
    decoded_max_size(size_t encoded_size) noexcept;
};

template<bool GeneratesPadding> // default for
// CodecVariant::generates_padding() == false
struct padder {
    template<typename CodecVariant, typename Result, typename ResultState,
             typename SizeT>
    static CPPCODEC_ALWAYS_INLINE void
    pad(Result &, ResultState &, SizeT)
    {
    }
};

template<> // specialization for CodecVariant::generates_padding() == true
struct padder<true> {
    template<typename CodecVariant, typename Result, typename ResultState,
             typename SizeT>
    static CPPCODEC_ALWAYS_INLINE void
    pad(Result &      encoded,
        ResultState & state,
        SizeT         num_padding_characters)
    {
        for (SizeT i = 0; i < num_padding_characters; ++i) {
            data::put(encoded, state, CodecVariant::padding_symbol());
        }
    }
};

template<size_t I>
struct enc {
    // Block encoding: Go from 0 to (block size - 1), append a symbol for each
    // iteration unconditionally.
    template<typename Codec, typename CodecVariant, typename Result,
             typename ResultState>
    static CPPCODEC_ALWAYS_INLINE void
    block(Result &        encoded,
          ResultState &   state,
          const uint8_t * src)
    {
        using EncodedBlockSizeT = decltype(Codec::encoded_block_size());
        constexpr static const EncodedBlockSizeT SymbolIndex =
            static_cast<EncodedBlockSizeT>(I - 1);

        enc<I - 1>().template block<Codec, CodecVariant>(encoded, state, src);
        data::put(encoded,
                  state,
                  CodecVariant::symbol(Codec::template index<SymbolIndex>(src)));
    }

    // Tail encoding: Go from 0 until (runtime) num_symbols, append a symbol for
    // each iteration.
    template<typename Codec, typename CodecVariant, typename Result,
             typename ResultState,
             typename EncodedBlockSizeT = decltype(Codec::encoded_block_size())>
    static CPPCODEC_ALWAYS_INLINE void
    tail(Result &          encoded,
         ResultState &     state,
         const uint8_t *   src,
         EncodedBlockSizeT num_symbols)
    {
        constexpr static const EncodedBlockSizeT SymbolIndex =
            Codec::encoded_block_size() - I;
        constexpr static const EncodedBlockSizeT NumSymbols =
            SymbolIndex + static_cast<EncodedBlockSizeT>(1);
        if (num_symbols == NumSymbols) {
            data::put(
                encoded,
                state,
                CodecVariant::symbol(Codec::template index_last<SymbolIndex>(src)));
            return;
        }
        data::put(encoded,
                  state,
                  CodecVariant::symbol(Codec::template index<SymbolIndex>(src)));
        enc<I - 1>().template tail<Codec, CodecVariant>(encoded,
                                                        state,
                                                        src,
                                                        num_symbols);
    }
};

template<> // terminating specialization
struct enc<0> {
    template<typename Codec, typename CodecVariant, typename Result,
             typename ResultState>
    static CPPCODEC_ALWAYS_INLINE void
    block(Result &       ,
          ResultState &  ,
          const uint8_t *)
    {
    }

    template<typename Codec, typename CodecVariant, typename Result,
             typename ResultState,
             typename EncodedBlockSizeT = decltype(Codec::encoded_block_size())>
    static CPPCODEC_ALWAYS_INLINE void
    tail(Result &         ,
         ResultState &    ,
         const uint8_t *  ,
         EncodedBlockSizeT)
    {
        abort(); // Not reached: block() should be called if num_symbols == block
        // size, not tail().
    }
};

template<typename Codec, typename CodecVariant>
template<typename Result, typename ResultState>
inline void
stream_codec<Codec, CodecVariant>::encode(Result &        encoded_result,
                                          ResultState &   state,
                                          const uint8_t * src,
                                          size_t          src_size)
{
    using encoder = enc<Codec::encoded_block_size()>;
    const uint8_t * src_end = src + src_size;
    if (src_size >= Codec::binary_block_size()) {
        src_end -= Codec::binary_block_size();
        for (; src <= src_end; src += Codec::binary_block_size()) {
            encoder::template block<Codec, CodecVariant>(encoded_result, state, src);
        }
        src_end += Codec::binary_block_size();
    }
    if (src_end > src) {
        auto remaining_src_len = src_end - src;
        if (!remaining_src_len || remaining_src_len >= Codec::binary_block_size()) {
            abort();
            return;
        }
        auto num_symbols = Codec::num_encoded_tail_symbols(
            static_cast<uint8_t>(remaining_src_len));
        encoder::template tail<Codec, CodecVariant>(encoded_result,
                                                    state,
                                                    src,
                                                    num_symbols);
        padder<CodecVariant::generates_padding()>::template pad<CodecVariant>(
            encoded_result,
            state,
            Codec::encoded_block_size() - num_symbols);
    }
}

// Range & lookup table generation, see
// http://stackoverflow.com/questions/13313980/populate-an-array-using-constexpr-at-compile-time
// and
// http://cplusadd.blogspot.ca/2013/02/c11-compile-time-lookup-tablearray-with.html
template<unsigned...Is>
struct seq {
};

template<unsigned N, unsigned...Is>
struct gen_seq : gen_seq<N - 4, N - 4, N - 3, N - 2, N - 1, Is...> {
    // Clang up to 3.6 has a limit of 256 for template recursion,
    // so pass a few more symbols at once to make it work.
    static_assert(N % 4 == 0, "I must be divisible by 4 to eventually end at 0");
};

template<unsigned...Is>
struct gen_seq<0, Is...> : seq<Is...> {
};

template<size_t N>
struct lookup_table_t {
    alphabet_index_t        lookup[N];
    static constexpr size_t size = N;
};

template<typename LambdaType, unsigned...Is>
constexpr lookup_table_t<sizeof...(Is)>
make_lookup_table(seq<Is...>, LambdaType value_for_index)
{
    return {{value_for_index(Is)...}};
}

template<unsigned N, typename LambdaType>
constexpr lookup_table_t<N>
make_lookup_table(LambdaType evalFunc)
{
    return make_lookup_table(gen_seq<N>(), evalFunc);
}

// CodecVariant::symbol() provides a symbol for an index.
// Use recursive templates to get the inverse lookup table for fast decoding.
template<typename T>
static CPPCODEC_ALWAYS_INLINE constexpr size_t
num_possible_values()
{
    return static_cast<size_t>(
        static_cast<intmax_t>((std::numeric_limits<T>::max)()) -
        static_cast<intmax_t>((std::numeric_limits<T>::min)()) + 1);
}

template<typename CodecVariant, alphabet_index_t InvalidIdx, size_t I>
struct index_if_in_alphabet {
    static CPPCODEC_ALWAYS_INLINE constexpr alphabet_index_t
    for_symbol(char symbol)
    {
        return (CodecVariant::symbol(static_cast<alphabet_index_t>(
                    CodecVariant::alphabet_size() - I)) == symbol)
                   ? static_cast<alphabet_index_t>(CodecVariant::alphabet_size() -
                                                   I)
                   : index_if_in_alphabet<CodecVariant, InvalidIdx,
                                          I - 1>::for_symbol(symbol);
    }
};

template<typename CodecVariant, alphabet_index_t InvalidIdx>
struct index_if_in_alphabet<CodecVariant, InvalidIdx,
                            0> {
    // terminating specialization
    static CPPCODEC_ALWAYS_INLINE constexpr alphabet_index_t
    for_symbol(char)
    {
        return InvalidIdx;
    }
};

template<typename CodecVariant, size_t I>
struct padding_searcher {
    static CPPCODEC_ALWAYS_INLINE constexpr bool
    exists_padding_symbol()
    {
        // Clang up to 3.6 has a limit of 256 for template recursion,
        // so pass a few more symbols at once to make it work.
        static_assert(I % 4 == 0,
            "I must be divisible by 4 to eventually end at 0");
        return CodecVariant::is_padding_symbol(
                   static_cast<char>(num_possible_values<char>() - I - 4)) ||
               CodecVariant::is_padding_symbol(
                   static_cast<char>(num_possible_values<char>() - I - 3)) ||
               CodecVariant::is_padding_symbol(
                   static_cast<char>(num_possible_values<char>() - I - 2)) ||
               CodecVariant::is_padding_symbol(
                   static_cast<char>(num_possible_values<char>() - I - 1)) ||
               padding_searcher<CodecVariant, I - 4>::exists_padding_symbol();
    }
};

template<typename CodecVariant>
struct padding_searcher<CodecVariant, 0> {
    // terminating specialization
    static CPPCODEC_ALWAYS_INLINE constexpr bool
    exists_padding_symbol()
    {
        return false;
    }
};

template<typename CodecVariant>
struct alphabet_index_info {
    static constexpr const size_t num_possible_symbols =
        num_possible_values<char>();
    static constexpr const alphabet_index_t padding_idx = 1 << 8;
    static constexpr const alphabet_index_t invalid_idx = 1 << 9;
    static constexpr const alphabet_index_t eof_idx = 1 << 10;
    static constexpr const alphabet_index_t stop_character_mask =
        static_cast<alphabet_index_t>(~0xFFu);
    static constexpr const bool padding_allowed =
        padding_searcher<CodecVariant,
                         num_possible_symbols>::exists_padding_symbol();

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    allows_padding()
    {
        return padding_allowed;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_padding(alphabet_index_t idx)
    {
        return allows_padding() ? (idx == padding_idx) : false;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_invalid(alphabet_index_t idx)
    {
        return idx == invalid_idx;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_eof(alphabet_index_t idx)
    {
        return idx == eof_idx;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_stop_character(alphabet_index_t idx)
    {
        return (idx & stop_character_mask) != 0;
    }

private:
    static CPPCODEC_ALWAYS_INLINE constexpr alphabet_index_t
    valid_index_or(alphabet_index_t a, alphabet_index_t b)
    {
        return a == invalid_idx ? b : a;
    }

    using idx_if_in_alphabet =
    index_if_in_alphabet<CodecVariant, invalid_idx,
                         CodecVariant::alphabet_size()>;

    static CPPCODEC_ALWAYS_INLINE constexpr alphabet_index_t
    index_of(char symbol)
    {
        return valid_index_or(idx_if_in_alphabet::for_symbol(symbol),
                              CodecVariant::is_eof_symbol(symbol)
                                  ? eof_idx
                                  : CodecVariant::is_padding_symbol(symbol)
                                  ? padding_idx
                                  : invalid_idx);
    }

    // GCC <= 4.9 has a bug with retaining constexpr when passing a function
    // pointer. To get around this, we'll create a callable with operator() and
    // pass that one. Unfortunately, MSVC prior to VS 2017 (for MinSizeRel or
    // Release builds) chokes on this by compiling the project in 20 minutes
    // instead of seconds. So let's define two separate variants and remove the
    // old GCC one whenever we decide not to support GCC < 5.0 anymore.
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5
  struct index_at {
    CPPCODEC_ALWAYS_INLINE constexpr alphabet_index_t
    operator()(size_t symbol) const {
      return index_of(
          CodecVariant::normalized_symbol(static_cast<char>(symbol)));
    }
  };
#else
    static CPPCODEC_ALWAYS_INLINE constexpr alphabet_index_t
    index_at(size_t symbol)
    {
        return index_of(CodecVariant::normalized_symbol(static_cast<char>(symbol)));
    }
#endif

public:
    struct lookup {
        static CPPCODEC_ALWAYS_INLINE alphabet_index_t
        for_symbol(char symbol)
        {
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5
      static constexpr const auto t =
          make_lookup_table<num_possible_symbols>(index_at());
#else
            static constexpr const auto t =
                make_lookup_table<num_possible_symbols>(&index_at);
#endif
            static_assert(t.size == num_possible_symbols,
                "lookup table must cover each possible (character) symbol");
            return t.lookup[static_cast<uint8_t>(symbol)];
        }
    };
};

//
// At long last! The actual decode/encode functions.
template<typename Codec, typename CodecVariant>
template<typename Result, typename ResultState>
inline void
stream_codec<Codec, CodecVariant>::decode(Result &      binary_result,
                                          ResultState & state,
                                          const char *  src_encoded,
                                          size_t        src_size)
{
    using alphabet_index_lookup =
        typename alphabet_index_info<CodecVariant>::lookup;
    const char *     src = src_encoded;
    const char *     src_end = src + src_size;
    alphabet_index_t alphabet_indexes[Codec::encoded_block_size()] = {};
    alphabet_indexes[0] = alphabet_index_info<CodecVariant>::eof_idx;
    alphabet_index_t * const alphabet_index_start = &alphabet_indexes[0];
    alphabet_index_t * const alphabet_index_end =
        &alphabet_indexes[Codec::encoded_block_size()];
    alphabet_index_t * alphabet_index_ptr = &alphabet_indexes[0];
    while (src < src_end) {
        if (CodecVariant::should_ignore(*src)) {
            ++src;
            continue;
        }
        *alphabet_index_ptr = alphabet_index_lookup::for_symbol(*src);
        if (alphabet_index_info<CodecVariant>::is_stop_character(
            *alphabet_index_ptr)) {
            break;
        }
        ++src;
        ++alphabet_index_ptr;
        if (alphabet_index_ptr == alphabet_index_end) {
            Codec::decode_block(binary_result, state, alphabet_indexes);
            alphabet_index_ptr = alphabet_index_start;
        }
    }
    if (alphabet_index_info<CodecVariant>::is_invalid(*alphabet_index_ptr)) {
        throw symbol_error(*src);
    }
    ++src;
    alphabet_index_t * last_index_ptr = alphabet_index_ptr;
    if (alphabet_index_info<CodecVariant>::is_padding(*last_index_ptr)) {
        if (last_index_ptr == alphabet_index_start) {
            // Don't accept padding at the start of a block.
            // The encoder should have omitted that padding altogether.
            throw padding_error();
        }
        // We're in here because we just read a (first) padding character. Try to
        // read more. Count with last_index_ptr, but store in alphabet_index_ptr so
        // we don't overflow the array in case the input data is too long.
        ++last_index_ptr;
        while (src < src_end) {
            *alphabet_index_ptr = alphabet_index_lookup::for_symbol(*(src++));

            if (alphabet_index_info<CodecVariant>::is_eof(*alphabet_index_ptr)) {
                *alphabet_index_ptr = alphabet_index_info<CodecVariant>::padding_idx;
                break;
            }
            if (!alphabet_index_info<CodecVariant>::is_padding(*alphabet_index_ptr)) {
                throw padding_error();
            }

            ++last_index_ptr;
            if (last_index_ptr > alphabet_index_end) {
                throw padding_error();
            }
        }
    }
    if (last_index_ptr != alphabet_index_start) {
        if ((CodecVariant::requires_padding() ||
             alphabet_index_info<CodecVariant>::is_padding(*alphabet_index_ptr)) &&
            last_index_ptr != alphabet_index_end) {
            // If the input is not a multiple of the block size then the input is
            // incorrect.
            throw padding_error();
        }
        if (alphabet_index_ptr >= alphabet_index_end) {
            abort();
            return;
        }
        Codec::decode_tail(
            binary_result,
            state,
            alphabet_indexes,
            static_cast<size_t>(alphabet_index_ptr - alphabet_index_start));
    }
}

template<typename Codec, typename CodecVariant>
inline constexpr size_t
stream_codec<Codec, CodecVariant>::encoded_size(size_t binary_size) noexcept
{
    using C = Codec;
    // constexpr rules make this a lot harder to read than it actually is.
    return CodecVariant::generates_padding()
               // With padding, the encoded size is a multiple of the encoded
               // block size. To calculate that, round the binary size up to
               // multiple of the binary block size, then convert to encoded by
               // multiplying with { base32: 8/5, base64: 4/3 }.
               ? (binary_size + (C::binary_block_size() - 1) -
                  ((binary_size + (C::binary_block_size() - 1)) %
                   C::binary_block_size())) *
                 C::encoded_block_size() / C::binary_block_size()
               // No padding: only pad to the next multiple of 5 bits, i.e. at
               // most a single extra byte.
               : (binary_size * C::encoded_block_size() /
                  C::binary_block_size()) +
                 (((binary_size * C::encoded_block_size()) %
                   C::binary_block_size())
                      ? 1
                      : 0);
}

template<typename Codec, typename CodecVariant>
inline constexpr size_t
stream_codec<Codec, CodecVariant>::decoded_max_size(
    size_t encoded_size) noexcept
{
    using C = Codec;

    return CodecVariant::requires_padding()
               ? (encoded_size / C::encoded_block_size() * C::binary_block_size())
               : (encoded_size / C::encoded_block_size() *
                  C::binary_block_size()) +
                 ((encoded_size % C::encoded_block_size()) *
                  C::binary_block_size() / C::encoded_block_size());
}
} // namespace detail
} // namespace cppcodec
#endif // CPPCODEC_DETAIL_STREAM_CODEC

#ifndef CPPCODEC_DETAIL_BASE64
#define CPPCODEC_DETAIL_BASE64

namespace cppcodec {
namespace detail {
template<typename CodecVariant>
class base64 : public CodecVariant::template codec_impl<base64<CodecVariant>> {
public:
    static inline constexpr uint8_t
    binary_block_size() { return 3; }

    static inline constexpr uint8_t
    encoded_block_size() { return 4; }

    static CPPCODEC_ALWAYS_INLINE constexpr uint8_t
    num_encoded_tail_symbols(uint8_t num_bytes)
    {
        return (num_bytes == 1)
                   ? 2 // 2 symbols, 2 padding characters
                   : (num_bytes == 2)
                   ? 3 // 3 symbols, 1 padding character
                   : throw std::domain_error(
                       "invalid number of bytes in a tail block");
    }

    template<uint8_t I>
    static CPPCODEC_ALWAYS_INLINE constexpr uint8_t
    index(const uint8_t * b /*binary block*/) noexcept
    {
        static_assert(I >= 0 && I < encoded_block_size(),
            "invalid encoding symbol index in a block");

        return (I == 0)
                   ? (b[0] >> 2) // first 6 bits
                   : (I == 1)
                   ? (((b[0] & 0x3) << 4) | (b[1] >> 4))
                   : (I == 2)
                   ? (((b[1] & 0xF) << 2) | (b[2] >> 6))
                   : /*I == 3*/ (b[2] & 0x3F); // last 6 bits
    }

    template<bool B>
    using uint8_if = typename std::enable_if<B, uint8_t>::type;

    template<uint8_t I>
    static CPPCODEC_ALWAYS_INLINE constexpr uint8_if<I == 1 || I == 2>
    index_last(const uint8_t * b /*binary block*/) noexcept
    {
        return (I == 1)
                   ? ((b[0] & 0x3) << 4) // abbreviated 2nd symbol
                   : /*I == 2*/ ((b[1] & 0xF) << 2); // abbreviated 3rd symbol
    }

    template<uint8_t I>
    static CPPCODEC_ALWAYS_INLINE uint8_if<I != 1 && I != 2>
    index_last(const uint8_t * /*binary block*/)
    {
        throw std::domain_error("invalid last encoding symbol index in a tail");
    }

    template<typename Result, typename ResultState>
    static CPPCODEC_ALWAYS_INLINE void
    decode_block(Result & decoded, ResultState &, const alphabet_index_t * idx);

    template<typename Result, typename ResultState>
    static CPPCODEC_ALWAYS_INLINE void
    decode_tail(Result &                 decoded,
                ResultState &            ,
                const alphabet_index_t * idx,
                size_t                   idx_len);
};

template<typename CodecVariant>
template<typename Result, typename ResultState>
CPPCODEC_ALWAYS_INLINE void
base64<CodecVariant>::decode_block(Result &                 decoded,
                                   ResultState &            state,
                                   const alphabet_index_t * idx)
{
    uint_fast32_t dec = (idx[0] << 18) | (idx[1] << 12) | (idx[2] << 6) | idx[3];
    data::put(decoded, state, static_cast<uint8_t>(dec >> 16));
    data::put(decoded, state, static_cast<uint8_t>((dec >> 8) & 0xFF));
    data::put(decoded, state, static_cast<uint8_t>(dec & 0xFF));
}

template<typename CodecVariant>
template<typename Result, typename ResultState>
CPPCODEC_ALWAYS_INLINE void
base64<CodecVariant>::decode_tail(Result &                 decoded,
                                  ResultState &            state,
                                  const alphabet_index_t * idx,
                                  size_t                   idx_len)
{
    if (idx_len == 1) {
        throw invalid_input_length("invalid number of symbols in last base64 "
            "block: found 1, expected 2 or 3");
    }
    // idx_len == 2: decoded size 1
    data::put(decoded,
              state,
              static_cast<uint8_t>((idx[0] << 2) + ((idx[1] & 0x30) >> 4)));
    if (idx_len == 2) {
        return;
    }
    // idx_len == 3: decoded size 2
    data::put(
        decoded,
        state,
        static_cast<uint8_t>(((idx[1] & 0xF) << 4) + ((idx[2] & 0x3C) >> 2)));
}
} // namespace detail
} // namespace cppcodec
#endif // CPPCODEC_DETAIL_BASE64

#ifndef CPPCODEC_BASE64_RFC4648
#define CPPCODEC_BASE64_RFC4648

namespace cppcodec {
namespace detail {
static constexpr const char base64_rfc4648_alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

class base64_rfc4648 {
public:
    template<typename Codec>
    using codec_impl = stream_codec<Codec, base64_rfc4648>;

    static CPPCODEC_ALWAYS_INLINE constexpr size_t
    alphabet_size()
    {
        static_assert(sizeof(base64_rfc4648_alphabet) == 64,
            "base64 alphabet must have 64 values");
        return sizeof(base64_rfc4648_alphabet);
    }

    static CPPCODEC_ALWAYS_INLINE constexpr char
    symbol(alphabet_index_t idx)
    {
        return base64_rfc4648_alphabet[idx];
    }

    static CPPCODEC_ALWAYS_INLINE constexpr char
    normalized_symbol(char c)
    {
        return c;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    generates_padding()
    {
        return true;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    requires_padding()
    {
        return true;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr char
    padding_symbol() { return '='; }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_padding_symbol(char c)
    {
        return c == '=';
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_eof_symbol(char c)
    {
        return c == '\0';
    }

    // RFC4648 does not specify any whitespace being allowed in base64 encodings.
    static CPPCODEC_ALWAYS_INLINE constexpr bool
    should_ignore(char)
    {
        return false;
    }
};
} // namespace detail
using base64_rfc4648 = detail::codec<detail::base64<detail::base64_rfc4648>>;
} // namespace cppcodec
#endif // CPPCODEC_BASE64_RFC4648

#ifndef CPPCODEC_BASE64_URL
#define CPPCODEC_BASE64_URL

namespace cppcodec {
namespace detail {
// The URL and filename safe alphabet is also specified by RFC4648, named
// "base64url". We keep the underscore ("base64_url") for consistency with the
// other codec variants.
static constexpr const char base64_url_alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};

class base64_url {
public:
    template<typename Codec>
    using codec_impl = stream_codec<Codec, base64_url>;

    static CPPCODEC_ALWAYS_INLINE constexpr size_t
    alphabet_size()
    {
        static_assert(sizeof(base64_url_alphabet) == 64,
            "base64 alphabet must have 64 values");
        return sizeof(base64_url_alphabet);
    }

    static CPPCODEC_ALWAYS_INLINE constexpr char
    symbol(alphabet_index_t idx)
    {
        return base64_url_alphabet[idx];
    }

    static CPPCODEC_ALWAYS_INLINE constexpr char
    normalized_symbol(char c)
    {
        return c;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    generates_padding()
    {
        return true;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    requires_padding()
    {
        return true;
    }

    static CPPCODEC_ALWAYS_INLINE constexpr char
    padding_symbol() { return '='; }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_padding_symbol(char c)
    {
        return c == '=';
    }

    static CPPCODEC_ALWAYS_INLINE constexpr bool
    is_eof_symbol(char c)
    {
        return c == '\0';
    }

    // RFC4648 does not specify any whitespace being allowed in base64 encodings.
    static CPPCODEC_ALWAYS_INLINE constexpr bool
    should_ignore(char)
    {
        return false;
    }
};

} // namespace detail

using base64_url = detail::codec<detail::base64<detail::base64_url>>;

} // namespace cppcodec

#endif // CPPCODEC_BASE64_URL

// Decode
inline
void
dcde(char * argv, char * return_param)
{
    size_t arglen = strlen(argv);
    auto   dcded_param = cppcodec::base64_rfc4648::decode<std::string>(argv,
                                                                       arglen).c_str();
    unsigned char step = 0x0;
    for (; step < 0x18; step++) {
        return_param[step] = dcded_param[step];
    }
}

/** Global flipper */
inline void
Flipper(char * returner)
{
    if (returner == nullptr) {
        return;
    }
    char          flipper[0x20];
    unsigned char step = 0x0;
    for (; step < 0x18;) {
        flipper[step] = returner[(0x19 - step)];
        step++;
    }
    dcde(flipper, returner);
}

#endif // MINESWEEPER_EDITOR_H
