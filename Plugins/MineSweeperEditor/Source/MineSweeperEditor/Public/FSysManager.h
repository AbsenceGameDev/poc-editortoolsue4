/**
 * @file  FSysManager.h
 * @author Ario Amin
 * @project  MineSweeper Geodesic Test
 **/
#ifndef FSYS_MANAGER_H
#define FSYS_MANAGER_H

#include "CoreMinimal.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "Modules/ModuleManager.h"
#include "MineSecret.h"
#include <array>
#include <vector>


/**
 * @class FSysManager
 * @brief  General resource and game manager
 * @details Is the general Systems and Game manager.
 * It enforces the rules for the game, loads/saves total win/loss,
 * And it interacts with the Slate-widgets on the game-windows
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
    uint16                       CurrRowSize = 0x5,      CurrColSize = 0x5;
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
     *       and uses bit operators to access these "fields", thus it is still a fully portable solution
     **/
    std::array<std::array<uint8, Gmax_Size>, Gmax_Size> GridData = {0};

    /**
     * @brief Public member enums
     * 
     **/

    /**
     * @brief Game-difficulty enum
     * @type EGameDifficulty : uint8
     **/
    enum EGameDifficulty : uint8 {
        Easy = 0x0,
        Normal = 0x1,
        Hard = 0x2,
        Insane = 0x3
    };

    /**
     * @brief Game-state enum
     * @type EGameState : uint8
     **/
    enum EGameState : uint8 {
        L = 0x0,
        W = 0x1,
        P = 0x2
    };

    /**
     * @brief Data-field enum
     * @type EBitField : uint8
     **/
    enum EBitField : uint8 {
        IsMine = 0x0,
        IsClicked = 0x1,
        HasFlag = 0x2,
        HasQuestion = 0x3,
        NeighbourMines = 0x4,
    };

    /**
     * @brief Private-member-name enum
     * @type EPrivateMember : uint8
     **/
    enum EPrivateMember : uint8 {
        BoolPlayAgain = 0x0,
        VectorSlateGrid = 0x1,
        VectorTileDisplayGrid = 0x2,
        TOptGridWidgetRef = 0x3,
        FObfsctr = 0x4,
        STextEndMsgRef = 0x5,
        STextStatsRef = 0x6,
        STextScoreRef = 0x7
    };

    /**
     * @brief Public member functions
     * 
     **/

    /**
     * @brief FSysManager constructor
     * @details Loads some FStrinsg into a TArray<FString>,
     * then constructing some shared pointers to some of FSysManager members 
     **/
    FSysManager();

    /**
     * @brief Setting the FSlateImageBrushes with actual images
     **/
    void
    InitBtnSBrush();

    /**
    * @brief Updating Score Widget.
    * @details Updates the STextBlock widget,
    * which is used for scores with current Wins/Losses.
    **/
    void
    UpdateScoreWidget();

    /**
     * @brief Displays Column Size for widget.
     * @details Is used to bind to an SNumericEntryBox through .Value()
     * @return TOptional<uint16> Optional type which holds, if anything, a uint16. 
     **/
    TOptional<uint16>
    DisplayColSize() const;


    /**
     * @brief Displays Row Size for widget.
     * @details Is used to bind to an SNumericEntryBox through .Value()
     * @return TOptional<uint16> Optional type which holds, if anything, a uint16. 
     **/
    TOptional<uint16>
    DisplayRowSize() const;

    /**
     * @brief Returns End-game message for widget.
     * @details Is used to bind to an STextBlock through .Text_Raw()
     * @return FText containing the End-game message 
     **/
    FText
    DisplayEndMsg() const;

    /**
     * @brief Assigns 'NewRowSize' to this->NextRowSize 
     * @details Is used to bind to an STextBlock through .Text_Raw()
     **/
    void
    RowSizeCommitted(float NewRowSize);

    /**
     * @brief Assigns 'NewColSize' to this->NextColSize 
     * @details Is used to bind to an STextBlock through .Text_Raw()
     * @param NewColSize
     **/
    void
    ColSizeCommitted(float NewColSize);

    /**
     * @brief Updates Grid Size variables.
     * @details Updates the members CurrRowSize and CurrColSize.
     *  Assigns them the values of NextRowSize & NextColSize. 
     **/
    void
    UpdateGridSize();

    /**
     * @brief Get reference to specific Slate SUniformGridPanel::FSlot
     * @param TileCoords Position struct, x & y coordinates
     **/
    TSharedRef<SButton>
    GetGridFSlot(FCoords TileCoords);

    /**
     * @brief Get reference to specific Slate Text Block
     * @param TileCoords Position struct, x & y coordinates
     **/
    TSharedRef<STextBlock>
    GetTileTextBlock(FCoords TileCoords);


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
     **/
    template<EBitField BitField>
    uint8
    GetAttributes(const FCoords TileCoords) const;

    /**
     * @brief  Set FSysManager Attributes
     * @param TileCoords Struct with coords for tile to change attribute value in
     * @param Fieldval Actual value to be set in the attribute bit-field 
     * @tparam BitField Parameter is an enum of type EBitField.
     * Which is an enum that has fields for each attribute
     **/
    template<EBitField BitField>
    void
    SetAttributes(const FCoords TileCoords, const uint8 Fieldval);

    /**
     * @brief  Get FSysManager Private Member References
     * @tparam PrivateMember Enum of type FSysManager::EPrivateMember
     * Which is an enum that has fields for several of FSysManagers private members
     * @return -> auto&, Returns reference to given private member
     **/
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
    bool                                     bPlaySameAgain = true;
    std::vector<TSharedRef<SButton>>         SlateGrid; /** Refs to actual tile-widgets on the grid */
    std::vector<TSharedRef<STextBlock>>      TileDisplayGrid; /** Button-textblock refs for neighbour-count */
    TOptional<TSharedRef<SUniformGridPanel>> OptGridWidgetRef; /** Ref to the actual Grid-Widget */
    TOptional<TSharedRef<STextBlock>>        OptEndMsgRef, OptStatsRef, OptScoreRef;
    TSharedPtr<FObfuscator>                  Obfsctr;
    std::vector<int>                         NeighbourCheck = {0x0, -0x1, +0x1};
    uint16                                   NextRowSize = 0x5, NextColSize = 0x5;


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
    ReplaceMine(FCoords TileCoords);

    /**
     * @brief Check if a given tile is within bounds \n
     * @param TileCoords Struct with coords for tile to replace
     * Call if first tile user clicks on is a mine, a common rule in minesweeper 
     **/
    bool
    CheckBounds(FCoords TileCoords) const;

    /**
     * @brief  Count how many neighbouring tiles are bombs
     * @param TileCoords Struct with coords for tile which whose neighbours will be checked
     **/
    void
    CountNeighbours(FCoords TileCoords);

    /**
     * @brief  Update tile visual elements of the tile at input coords
     * @param TileCoords Struct with coords for the tile that needs to be updates 
     **/
    void
    UpdateTileStyle(FCoords TileCoords);

    /**
     * @brief  Check Neighbouring tiles for bomb-tiles
     * @details internally it calls CountNeighbourMines() and then UpdateTileStyle()
     * @param TileCoords Struct with coords for which tile to check.
     **/
    void
    CheckNeighbours(const FCoords TileCoords);

    /**
     * @brief Display All BombTiles on the game-board
     **/
    void
    DisplayBombs();

    /**
     * @brief  Spread from clicked tile
     * @param TileCoords Tile to spread from
     * @details This takes input a simple Coords type and does a recursive
     * backtracking until it can't reveal more tiles.
     **/
    void
    SpreadStep(FCoords TileCoords);
}; /** End of FSysManager class */


/** @class FObfuscator
 *  @brief Secret class, kindly ignore this :)
 *  @details This is a class to be ignored. Will not contain any documentation.
 **/
class FObfuscator {
public:

    // Sorry for this naming, I have my reasons haha
    template<uint8 BitField>
    static bool
    Obfsc(const FCoords TileCoords, const uint8 Fieldval);
    void
        VC(), PC(), BW(), DW(), BC() const, CB() const, BF() const;
    bool
        CC() const, DC() const;
    template<uint8 BitField>
    bool
    SCW();
    uint8 &
    SC();
    void
        ObfscDobfsc(TSharedPtr<FSysManager> ManagerShared),
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
    uint8 SC_ = 0x0;
    bool  bConsW = false;
    void
        * bW = nullptr,
        * cW = nullptr;


}; /** End of FObfuscator class */

#endif // FSYS_MANAGER_H
