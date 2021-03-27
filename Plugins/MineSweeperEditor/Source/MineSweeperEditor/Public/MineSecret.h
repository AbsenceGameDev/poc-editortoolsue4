﻿/**
* @file  MineSecret.h
* @brief  #defines and FCoordsT definition
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#ifndef MINESECRET_H
#define MINESECRET_H

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


/** @note  FREETILES is ONLY to be used within member functions of class FSysManager */
#define FREETILES FreeTilesCount - ClickedTiles

/** @note  MAKEROBOTO is ONLY to be used Slate widgets that, when created through SNew(), have the member function .Font(...)  */
#define MAKEROBOTO(FontSize) .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), FontSize))
#define ROBOTOARG(FontSize) (FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), FontSize))
/** @note  MAKETEXT is ONLY to be used Slate widgets that, when created through SNew(), have the member function .Text(...) */
#define MAKETEXT(InString) .Text(FText::FromString( InString ))
#define TEXTARG(InString) (FText::FromString( InString ))
/** @note  INT_TEXT is ONLY to be used Slate widgets that, when created through SNew(),
 *   have the member function .Text(...) as-well as an integer to read as text*/
#define MAKENUMTEXT(InInt) .Text(FText::FromString( FString::FromInt(InInt)))
#define NUMTEXTARG(InInt) (FText::FromString( FString::FromInt(InInt))) 

bool bCh = false;

/** Simple type(s) */
struct FCoordsT {
    uint16 X;
    uint16 Y;

    /**
     * Compiler by some reason (Even tho it was a very simple type)
     * It wasn't doing the logical nor arithmetic operations properly,
     * so I had to manually define overloads for them
     **/

    /** Arithmetic operations */
    FCoordsT
    operator =(const FCoordsT & C2)
    {
        this->X = C2.X;
        this->Y = C2.Y;
        return *this;
    }

    FCoordsT
    operator +(const FCoordsT & C2) const
    {
        return FCoordsT{static_cast<uint16>(this->X + C2.X),
                       static_cast<uint16>(this->Y + C2.Y)};
    }

    FCoordsT
    operator -(const FCoordsT & C2) const
    {
        return FCoordsT{static_cast<uint16>(this->X - C2.X),
                       static_cast<uint16>(this->Y - C2.Y)};
    }

    FCoordsT
    operator *(const FCoordsT & C2) const
    {
        return FCoordsT{static_cast<uint16>(this->X * C2.X),
                       static_cast<uint16>(this->Y * C2.Y)};
    }

    FCoordsT
    operator /(const FCoordsT & C2) const
    {
        return FCoordsT{static_cast<uint16>(this->X / C2.X),
                       static_cast<uint16>(this->Y / C2.Y)};
    }

    FCoordsT
    operator %(const FCoordsT & C2) const
    {
        return FCoordsT{static_cast<uint16>(this->X % C2.X),
                       static_cast<uint16>(this->Y % C2.Y)};
    }

    FCoordsT
    operator +=(const FCoordsT & C2)
    {
        *this = *this + C2;
        return *this;
    }

    FCoordsT
    operator -=(const FCoordsT & C2)
    {
        *this = *this - C2;
        return *this;
    }

    FCoordsT
    operator *=(const FCoordsT & C2)
    {
        *this = *this * C2;
        return *this;
    }

    FCoordsT
    operator /=(const FCoordsT & C2)
    {
        *this = *this / C2;
        return *this;
    }

    FCoordsT
    operator %=(const FCoordsT & C2)
    {
        *this = *this % C2;
        return *this;
    }

    // Conditional operations
    bool
    operator ==(const FCoordsT & C2) const
    {
        return (this->X == C2.X &&
                this->Y == C2.Y);
    }

    bool
    operator !=(const FCoordsT & C2) const
    {
        return !(*this == C2);
    }

    bool
    operator <(const FCoordsT & C2) const
    {
        return (this->X < C2.X &&
                this->Y < C2.Y);
    }

    bool
    operator >(const FCoordsT & C2) const
    {
        return (this->X > C2.X &&
                this->Y > C2.Y);
    }

    bool
    operator <=(const FCoordsT & C2) const
    {
        return !(*this > C2);
    }

    bool
    operator >=(const FCoordsT & C2) const
    {
        return !(*this < C2);
    }

};

using FCoords = FCoordsT;


// Forawrd declrs.
class FSysManager;
class FObfuscator;
class FToolBarBuilder;
class FMenuBuilder;
struct FSlateImageBrush;
class SUniformGridPanel;

#endif // MINESECRET_H
