// Ario Amin - MineSweeper Goedesic Test

/**
 * @todo MineSweeper
 * 1. Make a game-manager, it will keep track of the current playing field, it should bind buttons to board-coords
 *			- Game manager should be it's own class which handles saving persistent data and secret count, as-well as the game-rules
 * 2. When a button is clicked with lmb, use it's coordinate data and check in the game manager if that box is a mine or not.
 *			- if it is a mine, display LOST message (Add a message: "you lost!" or something), then display all blocks which were mines.
 *
 *			- if it is not a mine, set button to un-clickable and change its color; -> Then do the same check recursively up/down/right/left
 *				- If next to one or multiple mines, display number of mines directly next to it (Octal spread)
 *
 * 3. When a button is clicked with rmb, toggle between displaying [flag, question-mark, nothing]
 * 
 **/
#ifndef MINESWEEPER_EP_H
#define MINESWEEPER_EP_H

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <array>
#include <vector>


class FToolBarBuilder;
class FMenuBuilder;

using uint8_t = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;
using uint64_t = unsigned long long int;
using Coords = struct {
    uint16_t X;
    uint16_t Y;
};

/** Forward decls. to be used in MineSweeper function prototypes */
class FGameManager;
struct FSlateImageBrush;

class FMineSweeper_EPModule : public IModuleInterface {
public:
    /** Public member variables */
    uint16_t X_INT; /** this value is displayed in SNumericEntryBox X */
    uint16_t Y_INT; /** this value is displayed in SNumericEntryBox Y */

    TSharedPtr<FGameManager>     GameManager_;
    TSharedPtr<FSlateImageBrush> FlagBrush;
    TSharedPtr<FSlateImageBrush> QuestionBrush;

    /** Public member functions */
    FMineSweeper_EPModule();

    /** IModuleInterface overrides */
    virtual void
    StartupModule() override;
    virtual void
    ShutdownModule() override;

    /** This function will be bound to Command (by default it will bring up plugin window) */
    void
    TabBtnClicked() const;
    void
    BuildBtnSBrush();
    void
    SetNewBtnSBrush(FSlateBrush BtnBrush);

    uint16_t
    GetX() const; // display this value
    uint16_t
    GetY() const; // display this value
    void
    CommittedX(const FText & NewText, ETextCommit::Type CommitType);
    // set value when keyboard input
    void
    CommittedY(const FText & NewText, ETextCommit::Type CommitType);
    // set value when keyboard input

private:
    /** Private member variables */
    TSharedPtr<class FUICommandList> PluginCmds;

    /** Private member functions */
    void
    RegisterMenus();
    TSharedRef<class SUniformGridPanel>
    GenerateGrid(uint8_t XIn, uint8_t YIn) const;
    TSharedRef<class SDockTab>
    OnSpawnTab(const class FSpawnTabArgs & SpawnTabArgs) const;
};


class FGameManager {
public:
    /** Public member enums */
    enum EGameDifficulty : uint8_t {
        Easy = 0x0,
        Normal = 0x1,
        Hard = 0x2,
        Insane = 0x3
    };

    enum EGameState : uint8_t {
        Loss = 0x0,
        Win = 0x1,
        Pass = 0x2
    };

    enum EBitField : uint8_t {
        IsMine = 0x0,
        IsClicked = 0x1,
        HasFlag = 0x2,
        HasQuestion = 0x3,
        NeighbourMines = 0x4,
    };

    /** Public member variables */
    static constexpr uint16_t Gmax_Size = 0xf;
    TSharedPtr<SDockTab>      GameWindow;
    uint16_t                  NumMines = 0x0,    FreeTilesCount = 0x0, ClickedTiles = 0x0;
    uint16_t                  CurrRowSize = 0x8, CurrColSize = 0x8;
    uint16_t                  Wins = 0x0,        Losses = 0x0;

    std::array<std::array<uint8_t, Gmax_Size>, Gmax_Size> GridData = {0};
    /* 256^2 bytes = 8kb, on a mcu it would be unacceptable, on a pc cpu with megabytes of cache it's negligible
                                            Per element:	bit[0] = isMine?
                                                            bit[1] = Clicked?,
                                                            bit[2] = HasFlag?
                                                            bit[3] = HasQuestionMark?
                                                            bits[7,4] = Neighbour Mines Count,
                                                            */

    /** Public member functions */
    FGameManager()
    {
    }

    template<EGameDifficulty Difficulty>
    void
    SetDifficulty();

    void
    PlaceMines();
    void
    ReplaceMine(Coords Tile);
    EGameState
    ClickTile(uint8_t XCoord, uint8_t YCoord);

    /** Templated, inlined, attributes getter/setter functions */
    template<EBitField BitField>
    uint8_t
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
    SetAttributes(Coords Tile, uint8_t Fieldval)
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

    // number = (number & ~(1UL << n)) | (x << n);
    // TileData = TileData & ~(1UL << BitField)) | (Fieldval << BitField)

private:
    /** Private member variables */
    std::vector<int> NeighbourCheck = {0, -1, +1};
    uint8_t          SecretCount_ = 0;
    bool             bConsecutiveWin = false;

    /** Private member functions */
    void
    GenerateBoard();
    void
    CheckNeighbours(const Coords TileCoords);
    void
    SpreadStep(Coords Tile);
    void
    EndGame();
    void
    SaveState();
    void
    LoadState();
};

#endif // MINESWEEPER_EP_H_
