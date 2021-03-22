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
    TSharedPtr<FSysManager> SysManager;
    uint16                  X_INT; /** this value is displayed in SNumericEntryBox X */
    uint16                  Y_INT; /** this value is displayed in SNumericEntryBox Y */

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
    * @brief Get inputted X-value
    */
    uint16
    GetX() const;

    /**
    * @brief Get inputted Y-value
    */
    uint16
    GetY() const;

    /**
    * @brief Commit text from text-box 1 to value
    * @param NewText New display value
    * @param CommitType
    * @note set value when keyboard input
    */
    void
    CommittedX(const FText & NewText, ETextCommit::Type CommitType);

    /**
    * @brief Commit text from text-box 1 to value
    * @param NewText New display value
    * @param CommitType
    * @note set value when keyboard input
    */
    void
    CommittedY(const FText & NewText, ETextCommit::Type CommitType);

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
    static constexpr uint16      Gmax_Size = 0x40;
    TSharedPtr<FSlateImageBrush> FlagBrush,              QuestionBrush,        BombBrush;
    uint16                       NumMines = 0x0,         FreeTilesCount = 0x0, ClickedTiles = 0x0;
    uint16                       CurrRowSize = 0x8,      CurrColSize = 0x8;
    uint16                       Ws = 0x0,               Ls = 0x0;
    char                         SContainer[0x18] = {0}, RContainer[0x18] = {0};
    TSharedPtr<FObfuscator>      Obfsctr;

    std::vector<TSharedRef<SButton>> SlateGrid; /** Holds references to actual slate buttons */
    std::vector<TSharedRef<STextBlock>> TileDisplayGrid; /** Holds textblock refs for neighbour-count */
    std::array<std::array<uint8, Gmax_Size>, Gmax_Size> GridData = {0};
    /* 64^2 bytes = 4kb, on a mcu it would be unacceptable, on a pc cpu with megabytes of cache it's negligible
                                            Per element:	bit[0] = isMine?; bit[1] = Clicked?; bit[2] = HasFlag?
                                                            bit[3] = HasQuestionMark?; bits[7,4] = Neighbour Mines Count */

    // Inside Construct
    // SNew(STextBlock).Text(this, &SMyWidget::GetText)
 
    // Inside your widget class
    FText GetText(Coords TileCoords) const
    {
        FText Text = FText::FromString(FString::FromInt(GetAttributes<EBitField::NeighbourMines>(TileCoords)));
        return Text;
    }


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
     * @brief  Spread from clicked tile
     * @param TileCoords Tile to spread from
     * @note This takes input a simple Coords type and does a recursive
     * backtracking until it can't reveal more tiles
     **/
    void
    SpreadStep(Coords TileCoords);

    /**
     * @brief  Reset game / end game
     * Pretty self-explanatory
     **/
    void
    ResetGame();
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

    // Decode
    inline
    void
    dcde(char * argv, char * return_param)
    {
        size_t        arglen = strlen(argv);
        char *        dcded_param = "temp string"; // = base64::decode<std::string>(argv,arglen).c_str();
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

private:
    uint8  SC_ = 0x0;
    bool   bConsW = false;
    void * bW = nullptr;
    void * cW = nullptr;


}; /** End of FObfuscator class */


#endif // MINESWEEPER_EDITOR_H
