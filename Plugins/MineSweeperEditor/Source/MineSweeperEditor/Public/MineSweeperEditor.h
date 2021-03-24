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

#include "CoreMinimal.h"
#include "MineSecret.h"
#include "Modules/ModuleManager.h"
#include <array>
#include <vector>

/**
* @brief  Game module
*
**/
class FMineSweeperEditorModule : public IModuleInterface {
public:

    /**
    * @brief Public member variables
    * 
    **/
    TSharedPtr<FSysManager>             SysManager;
    TSharedPtr<SNumericEntryBox<uint8>> NumbericBoxX{};
    TSharedPtr<SNumericEntryBox<uint8>> NumbericBoxY{};

    /**
    * @brief Public member functions
    * 
    **/
    FMineSweeperEditorModule();

    /** IModuleInterface overrides */
    virtual void
    StartupModule() override;
    virtual void
    ShutdownModule() override;

    /**
     * @brief Brings up main plugin window
     * @note This function will be bound to Command
     */
    void
    TabBtnClicked() const;

    /**
    * @brief Commit text from text-box 1 to value
    * @param NewInt New display value
    * @param CommitType
    * @note set value when keyboard input
    */
    void
    CommittedX(const uint8 NewInt, ETextCommit::Type CommitType) const;

    /**
    * @brief Commit text from text-box 1 to value
    * @param NewInt New display value
    * @param CommitType
    * @note set value when keyboard input
    */
    void
    CommittedY(const uint8 NewInt, ETextCommit::Type CommitType) const;

    void 
    RegenerateGrid(uint8 XIn, uint8 YIn, TSharedRef<class SUniformGridPanel>) const;
private:

    /**
    * @brief Private member variables
    * 
    **/
    TSharedPtr<class FUICommandList> PluginCmds;

    /**
    * @brief Private member functions
    * 
    **/

    /**
    * @brief Register level editor menu
    **/
    void
    RegisterMenus();

    /**
    * @brief Bound to reset button
    **/
    // FReply
    // ResetGameBind();

    /**
    * @brief Generate Slate Grid 
    * @param XIn Grid Row Size
    * @param YIn Grid Column Size
    * @note FSLocal - Local container to create and bind function \n
    * FSLocal::OnTileClick(const Coords, TSharedPtr<FSysManager>) \n
    * FSLocal::MakeTile(const Coords, TSharedPtr<FSysManager>)
    * @return Shared reference of Grid panel, type: TSharedRef<SUniformGridPanel>
    **/
    TSharedRef<class SUniformGridPanel>
    GenerateGrid(uint8 XIn, uint8 YIn) const;
    

    /**
    * @brief Call when spawning window to spawn internal tab/page.
    * @param SpawnTabArgs
    **/
    TSharedRef<class SDockTab>
    OnSpawnTab(const class FSpawnTabArgs & SpawnTabArgs) const;
}; /** End of FMineSweeperEditorModule class */


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
    static constexpr uint16                  Gmax_Size = 0x40;
    TSharedPtr<FSlateImageBrush>             FlagBrush,              QuestionBrush,        BombBrush;
    uint16                                   NumMines = 0x0,         FreeTilesCount = 0x0, ClickedTiles = 0x0;
    uint16                                   CurrRowSize = 0xc,      CurrColSize = 0xc;
    uint16                                   Ws = 0x0,               Ls = 0x0;
    char                                     SContainer[0x18] = {0}, RContainer[0x18] = {0};
    TSharedPtr<FObfuscator>                  Obfsctr;
    TOptional<TSharedRef<SUniformGridPanel>> OptGridWidgetRef;
    TArray<TSharedPtr<FString>>              DifficultyList;

    std::vector<TSharedRef<SButton>>    SlateGrid; /** Holds references to actual slate buttons */
    std::vector<TSharedRef<STextBlock>> TileDisplayGrid; /** Holds textblock refs for neighbour-count */

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
private:
    /**
     * @brief Private member variables
     * 
     **/
    std::vector<int> NeighbourCheck = {0x0, -0x1, +0x1};


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


struct FTileBinder {
    /** Reset Game event */
    static FReply
    ResetGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager);
    
    /** On Tile Click event */
    static FReply
    OnTileClick(Coords                  TileCoords,
                TSharedPtr<FSysManager> ManagerShared);

    /** Make Tile and bind OnClick to it */
    static TSharedRef<SWidget>
    MakeTile(const Coords TileCoords,
             TSharedPtr<FSysManager>
             ManagerShared);
};


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

    /**
     * The Base64 decode algorithm converts plain text into original data. Technically, it can be said that it converts six-bit bytes into eight-bit bytes. To understand how the decoding algorithm works, check the example below that describes step by step how to manually decode strings from Base64 (if you are looking for an automatic converter, use the Base64 online decoder).
     * 
     * For example, you have the “QUJD” string and want to decode it from Base64:
     * 
     * First, you need to split the string letter by letter. Thus, you got 4 groups:
     * Q
     * U
     * J
     * D
     * 
     * Each group (letter) is a Base64 character that has its own index, and now your task is to convert groups to indices.
     * To do this, by mapping values from the Base64 Characters Table replace each character by its index (if you cannot find an index for a specific group, just discard it). All in all, you should get the following indices:
     * 16
     * 20
     * 9
     * 3
     * 
     * At this step you should convert each group from decimal to binary. So find corresponding decimal values in the ASCII table and make sure you get the following binary values:
     * 00010000
     * 00010100
     * 00001001
     * 00000011
     * 
     * Now remove the prefix “00” (two zeros) in front of each group:
     * 010000
     * 010100
     * 001001
     * 000011
     * 
     * There you have a simple concatenation of previous groups (that is, glue all the binary values together and get an 24-character string):
     * 010000010100001001000011
     * 
     * Then, divide the resulting string into groups so that each one has 8 characters (if the last group has less than 8 characters, you must discard it). Now you have 3 groups of eight-bit bytes:
     * 01000001
     * 01000010
     * 01000011
     * 
     * Once again using the ASCII table, convert all binary values into their ASCII characters:
     * A
     * B
     * C
     * 
     * The final chord, concatenate all ASCII characters to get the result string:
     * ABC
     **/
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

#endif // MINESWEEPER_EDITOR_H
