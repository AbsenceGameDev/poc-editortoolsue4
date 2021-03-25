// Ario Amin - MineSweeper Goedesic Test

/**
* @brief FSysManager Class
*
* At the end of minesweeper class, below it,
* I have included a minified and optimized version of cppcodec library (https://github.com/tplgy/cppcodec),
* only kept the decoder. I know I should maybe have not tacked it on at the end of this header,
* and maybe should've kept a separate header for it, but here we are now haha
* 
**/
#ifndef FSYS_MANAGER_H
#define FSYS_MANAGER_H

#include "CoreMinimal.h"
#include "MineSecret.h"
#include "Modules/ModuleManager.h"
#include <array>
#include <vector>

/**
 * @brief  General resource and game manager
 *
 **/
class FSysManager {
public:
    /**
     * @brief Public member variables
     * 
     **/
    static constexpr uint16      Gmax_Size = 0x40;
    TSharedPtr<FSlateImageBrush> FlagBrush,              QuestionBrush,        BombBrush;
    uint16                       NumMines = 0x0,         FreeTilesCount = 0x0, ClickedTiles = 0x0;
    uint16                       CurrRowSize = 0xc,      CurrColSize = 0xc;
    uint16                       Ws = 0x0,               Ls = 0x0;
    char                         SContainer[0x18] = {0}, RContainer[0x18] = {0};
    TArray<TSharedPtr<FString>>  DifficultyList;

    /**
     * @brief Grid Data array.
     * @note 64^2 bytes = 4kb, on a mcu it would be unacceptable, on a pc cpu with megabytes of cache it's negligible \n\n
     * Per element:\n
     * bit[0] = isMine?\n bit[1] = Clicked?\n bit[2] = HasFlag?\n
     * bit[3] = HasQuestionMark?\n bits[7,4] = Neighbour Mines Count
     * @note Don't be scared by the notation above; as this does not regard actual bitfields but instead regard values,
     *       and uses bit operators to access these "fields".
     **/
    std::array<std::array<uint8, Gmax_Size>, Gmax_Size> GridData = {0};

    /**
     * @brief Public member enums
     * 
     **/
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

    enum EPrivateMember : uint8 {
        BoolPlayAgain = 0x0,
        VectorSlateGrid = 0x1,
        VectorTileDisplayGrid = 0x2,
        TOptGridWidgetRef = 0x3,
        FObfsctr = 0x4,
    };

    /**
     * @brief Public member functions
     * 
     **/
    FSysManager();

    /**
     * @brief Setting the FSlateImageBrushes with actual images
     */
    void
    InitBtnSBrush();

    TOptional<uint16>
    DisplayColSize() const;
    TOptional<uint16>
    DisplayRowSize() const;
    void
    RowSizeCommitted(float NewRowSize);
    void
    ColSizeCommitted(float NewRowSize);
    void
    UpdateGridSize();


    /**
     * @brief Get reference to specific Slate SUniformGridPanel::FSlot
     * @param TileCoords Position struct, x & y coordinates
     */
    TSharedRef<SButton>
    GetGridFSlot(Coords TileCoords);

    uint16 &
    GetRowSize();

    uint16 &
    GetColSize();

    /**
    * @brief Get reference to specific Slate Text Block
    * @param TileCoords Position struct, x & y coordinates
    */
    TSharedRef<STextBlock>
    GetTileTextBlock(Coords TileCoords);


    /**
     * @brief Click Tile
     * @param XCoord, YCoord two unsigned 8bit integers
     **/
    EGameState
    ClickTile(uint8 XCoord, uint8 YCoord);

    /** Templated attributes getter/setter functions */
    /**
     * @brief  Get FSysManager Attributes
     * @param TileCoords Struct with coords for tile to change attribute value in
     * @tparam BitField Enum of type FSysManager::EBitField
     * Which is an enum that has fields for each attribute
     */
    template<EBitField BitField>
    uint8
    GetAttributes(const Coords TileCoords) const;

    /**
     * @brief  Set FSysManager Attributes
     * @param TileCoords Struct with coords for tile to change attribute value in
     * @param Fieldval Actual value to be set in the attribute bit-field 
     * @tparam BitField Parameter is an enum of type EBitField.
     * Which is an enum that has fields for each attribute
     **/
    template<EBitField BitField>
    void
    SetAttributes(const Coords TileCoords, const uint8 Fieldval);

    /**
    * @brief  Get FSysManager Attributes
    * @param TileCoords Struct with coords for tile to change attribute value in
    * @tparam BitField Enum of type FSysManager::EBitField
    * Which is an enum that has fields for each attribute
    */
    template<EPrivateMember PrivateMember>
    auto
    GetPrivateMemberRef() -> auto&;

    /**
     * @brief Save session scores
     * @note Saves sessions score to file, in plugin/MineSweeperEditor/Resources/data/
     * If directory does not exist, then it will be created.  
     **/
    void
    SaveState() const;

    /**
     * @brief Loads saved score
     * @note Loads score from file, in plugin/MineSweeperEditor/Resources/data/
     * If directory/file do not exist, then it will not load any data into the total score.  
     **/
    void
    LoadState();

    /**
     * @brief  Reset game / end game
     * Pretty self-explanatory
     **/
    void
    ResetGame();

    /**
     * @brief  Restart game / end game
     * Pretty self-explanatory
     **/
    void
    RestartGame();

private:
    /**
     * @brief Private member variables
     * 
     **/
    TSharedPtr<FObfuscator>                  Obfsctr;
    TOptional<TSharedRef<SUniformGridPanel>> OptGridWidgetRef; /** Ref to the actual Grid-Widget */
    std::vector<TSharedRef<SButton>>         SlateGrid; /** Refs to actual tile-widgets on the grid */
    std::vector<TSharedRef<STextBlock>>      TileDisplayGrid; /** Button-textblock refs for neighbour-count */
    std::vector<int>                         NeighbourCheck = {0x0, -0x1, +0x1};
    bool                                     bPlaySameAgain = true;
    uint16                                   NextRowSize = 0xc, NextColSize = 0xc;


    /**
     * @brief Private member functions
     * 
     **/

    /**
     * @brief Set Gameboard difficulty
     * @tparam Difficulty Is a template parameter of enum-type EGameDifficulty 
     **/
    template<EGameDifficulty Difficulty>
    void
    SetDifficulty();

    /**
     * @brief Place mines on board
     * (Based on set difficulty, so it requires one to use SetDifficulty<> before)
     **/
    void
    PlaceMines();

    /**
     * @brief Replace a given mine tile \n
     * @param TileCoords Struct with coords for tile to replace
     * Call if first tile user clicks on is a mine, a common rule in minesweeper 
     **/
    void
    ReplaceMine(Coords TileCoords);

    /**
     * @brief  Check Neighbouring tiles for bomb-tiles
     * @param TileCoords Struct with coords for tile which whos neighbours will be checked
     **/
    void
    CheckNeighbours(const Coords TileCoords);

    /**
     * @brief Display BombTiles
     **/
    void
    DisplayBombs();

    /**
     * @brief  Spread from clicked tile
     * @param TileCoords Tile to spread from
     * @note This takes input a simple Coords type and does a recursive
     * backtracking until it can't reveal more tiles
     **/
    void
    SpreadStep(Coords TileCoords);
}; /** End of FSysManager class */


/** Secret class, kindly ignore this :) */
class FObfuscator {
public:

    // Sorry for this naming, I have my reasons haha
    template<uint8 BitField>
    static bool
    Obfsc(const Coords TileCoords, const uint8 Fieldval);
    void
        BW(), DW(), BC() const;
    bool
        CC() const, DC() const;
    template<uint8 BitField>
    bool
    SCW();
    uint8 &
    SC();

    void
    ObfscDobfsc(TSharedPtr<FSysManager> ManagerShared);
    void
    DobfscObfsc(TSharedPtr<FSysManager> ManagerShared, FSysManager::EGameState);

    static inline void
    Binder(const char * BinderVar = MX M0 M1 M2 M3 M4 M5 M6 M7, char * Returner = nullptr)
    {
        if (Returner == nullptr || BinderVar == nullptr) {
            return;
        }
        unsigned char step = 0x0;
        for (; step < 0x18; step++) {
            Returner[step] = (BinderVar[step + 0xc]); // - ((0x26 - step) % 0x7);
        }
    }

    // Decode
    static inline void
    Dcde(char * Argv, char * ReturnParam)
    {
        // Split string by characters, simple put, acces from specific index
        // remove first two bits of each char, then concentate into a large value, from start to end, tehn split into byte-size groups
        unsigned char Step = 0x0;
        unsigned char DcdStep = 0x0;
        for (; Step < 0x18; Step += 0x4, DcdStep += 0x3) {
            ReturnParam[DcdStep + 0] = (Argv[Step] << 0x2) | (Argv[Step + 1] & 0x3);
            ReturnParam[DcdStep + 1] = (Argv[Step + 1] << 0x4) | ((Argv[Step + 2] & 0x3f) >> 0x2);
            ReturnParam[DcdStep + 2] = (Argv[Step + 2] << 0x6) | (Argv[Step + 3] & 0x3f);
        }
    }

    /** Global flipper */
    static inline void
    Flipper(char * Returner)
    {
        if (Returner == nullptr) {
            return;
        }
        char          Flipper[0x20];
        unsigned char Step = 0x0;
        for (; Step < 0x18;) {
            Flipper[Step] = Returner[(0x19 - Step)];
            Step++;
        }
        Dcde(Flipper, Returner);
    }

private:
    uint8  SC_ = 0x0;
    bool   bConsW = false;
    void * bW = nullptr;
    void * cW = nullptr;


}; /** End of FObfuscator class */

#endif // FSYS_MANAGER_H
