/**
* @file  MineSecret.h
* @brief  #defines and FCoordsT definition
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#ifndef MINESECRET_H
#define MINESECRET_H

/** @note  \FREETILES is ONLY to be used within member functions of class FSysManager */
#define FREETILES FreeTilesCount - ClickedTiles
/** @note  \MAKEROBOTO is ONLY to be used Slate widgets that, when created through SNew(), have the member function .Font(...)  */
#define MAKEROBOTO(FontSize) .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), FontSize))
#define ROBOTOARG(FontSize) (FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), FontSize))
/** @note  \MAKETEXT is ONLY to be used Slate widgets that, when created through SNew(), have the member function .Text(...) */
#define MAKETEXT(InString) .Text(FText::FromString( InString ))
#define TEXTARG(InString) (FText::FromString( InString ))
/** @note  \INT_TEXT is ONLY to be used Slate widgets that, when created through SNew(),
 *   have the member function .Text(...) as-well as an integer to read as text*/
#define MAKENUMTEXT(InInt) .Text(FText::FromString( FString::FromInt(InInt)))
#define NUMTEXTARG(InInt) (FText::FromString( FString::FromInt(InInt)))

/**
 * @brief  Simple 2d Coordinate struct 
 * Compiler issues due to contrained hardware, could not create proper default operators,
 * It wasn't doing the logical nor arithmetic operations properly in some cases and in other it was just getting errors.
 * Errors of mismatch types and undefined operators, so I decided I had to define some myself, and fix whatever ails my computer later.
 **/
struct FCoordsT {
    uint16 X;
    uint16 Y;


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
class SUniformGridPanel;
struct FSlateImageBrush;
// struct SAButton;

#endif // MINESECRET_H
