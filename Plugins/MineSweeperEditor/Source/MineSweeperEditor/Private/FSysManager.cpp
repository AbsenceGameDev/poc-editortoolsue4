/**
* @file  FSysManager.cpp
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#include "FSysManager.h"
#include "EditorReimportHandler.h"
#include "Misc/FileHelper.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Interfaces/IPluginManager.h"
#include "Brushes/SlateImageBrush.h"

#define LOCTEXT_NAMESPACE "MineSweeperEditorModule"

/*
 *
 * FSysManager::  Public member functions 
 * @function void FSysManager(), InitBtnSBrush() 
 * @function TSharedRef<SButton> GetGridFSlot(Coords)
 * @function FSysManager::EGameState ClickTile(uint8, uint8)
 * @function void SaveState(), LoadState()
 * @function uint8 GetAttributes<FSysManager::EBitField>(const Coords);
 * @function void SetAttributes<FSysManager::EBitField>(const Coords, const uint8);
 * @function void ResetGame()
 * 
 */

/*
 * Setting the FSlateImageBrushes with actual images
 */
FSysManager::FSysManager()
{
    OptGridWidgetRef = MakeShared<SUniformGridPanel>();
    OptStatsRef = SNew(STextBlock) MAKEROBOTO(18);
    OptScoreRef = SNew(STextBlock) MAKEROBOTO(18);
    InitBtnSBrush();
    ClearGridData();
    SetDifficulty();
    PlaceMines();
}

/*
 * Setting the FSlateImageBrushes with actual images
 */
void
FSysManager::InitBtnSBrush()
{
    const FString AppendDataDir = TEXT("/Resources/");
    const FString FilePath = IPluginManager::Get().FindPlugin("MineSweeperEditor")->GetBaseDir() +
                             AppendDataDir;
    FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*FilePath);

    FlagBrush = MakeShared<FSlateImageBrush>(FilePath + TEXT("flag16x16.png"), FVector2D(32, 32));
    QuestionBrush = MakeShared<FSlateImageBrush>(FilePath + TEXT("q16x16.png"), FVector2D(32, 32));
    BombBrush = MakeShared<FSlateImageBrush>(FilePath + TEXT("bomb16x16.png"), FVector2D(32, 32));
}

/*
 * Updating the STextBlock score-widget after a finished match
 */
void
FSysManager::UpdateScoreWidget()
{
    (*OptScoreRef)->SetText(
        FText::FromString("Wins: " + FString::FromInt(Wins) + "\n"
                          + "Losses:" + FString::FromInt(Losses)));
}

/*
 * Get value to display in SNumericEntryBox
 */
TOptional<uint16>
FSysManager::DisplayColSize() const
{
    return NextColSize;
}

/*
 * Get value to display in SNumericEntryBox
 */
TOptional<uint16>
FSysManager::DisplayRowSize() const
{
    return NextRowSize;
}

/*
 * Set value for NextRowSize, bound to SSlider
 */
void
FSysManager::RowSizeCommitted(float NewRowSize)
{
    NextRowSize = NewRowSize;
}

/*
 * Set value for NextColSize, bound to SSlider
 */
void
FSysManager::ColSizeCommitted(float NewColSize)
{
    NextColSize = NewColSize;
}

/*
 * Updates CurrRowSize & CurrColSize
 */
void
FSysManager::UpdateGridSize()
{
    CurrColSize = NextColSize;
    CurrRowSize = NextRowSize;
}

/*
 * Get reference to specific Slate SUniformGridPanel::FSlot
 * Resolve a 2d index to 1d, Row-Major: (x + xmax*y) 
 */
TSharedRef<SButton>
FSysManager::GetGridFSlot(FCoords TileCoords)
{
    return SlateGrid.at(TileCoords.X + (CurrRowSize * TileCoords.Y));
}

/*
 * Get reference to specific Slate SUniformGridPanel::FSlot
 * Resolve a 2d index to 1d, Row-Major: (x + xmax*y) 
 */
TSharedRef<STextBlock>
FSysManager::GetTileTextBlock(FCoords TileCoords)
{
    return TileDisplayGrid.at(TileCoords.X + (CurrRowSize * TileCoords.Y));
}

/*
 * Clicks tile and calls game-logic
 */
FSysManager::EGameState
FSysManager::ClickTile(const FCoords TileCoords)
{
    ClickedTiles++;
    // If clicked on Mine
    if (GetAttributes<EBitField::IsMine>(TileCoords)) {
        // If first tile is clicked
        if (ClickedTiles == 0x1) {
            ReplaceMine(TileCoords);
        } else {
            Losses += 0x1;
            DisplayBombs();
            SetEnableSlateGrid(false);
            ClickedTiles--;
            (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
            return EGameState::L;
        }
    } else if (ClickedTiles >= FreeTilesCount) {
        Wins += 0x1;
        SetAttributes<EBitField::IsClicked>(TileCoords, 0x1);
        CheckNeighbours(TileCoords);
        DisplayBombs();
        SetEnableSlateGrid(false);
        (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
        return EGameState::W;
    }
    (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
    // Path to handle checking tile and freeing it
    SpreadStep(TileCoords);
    if (ClickedTiles < FreeTilesCount) {
        return EGameState::P;
    }
    Wins += 0x1;
    SetEnableSlateGrid(false);
    return EGameState::W;
}

/*
 * Get FSysManager Attributes
 */
template<FSysManager::EBitField BitField>
uint8
FSysManager::GetAttributes(const FCoords TileCoords) const
{
    if constexpr (BitField == EBitField::NeighbourMines) {
        return (GridData[TileCoords.Y][TileCoords.X] >> 0x4) & 0xf;
    } else {
        return (GridData[TileCoords.Y][TileCoords.X] >> BitField) & 0x1;
    }
}

/*
 * Set FSysManager Attributes
 */
template<FSysManager::EBitField BitField>
void
FSysManager::SetAttributes(const FCoords TileCoords, const uint8 Fieldval)
{
    auto & TileData = GridData[TileCoords.Y][TileCoords.X];
    if constexpr (BitField == EBitField::NeighbourMines) {
        TileData = (TileData & 0xf) | ((Fieldval & 0xf) << BitField);
    }
    if constexpr (BitField == EBitField::IsMine) {
        TileData = (TileData & ~0x1) | (Fieldval & 0x1);
    } else {
        TileData = (TileData & ~(0x1 << BitField)) | ((Fieldval & 0x1) << BitField);
    }
}

/*
 * Save session scores
 */
void
FSysManager::SaveState() const
{
    // Works for now, but doesn't look purdy
    const FString AppendDataDir = TEXT("/Resources/data/");
    FString FilePath = IPluginManager::Get().FindPlugin("MineSweeperEditor")->GetBaseDir() + AppendDataDir;
    FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*FilePath);
    FilePath += TEXT("Sweeper.dat");
    uint8 TotalArr[4] = {0};
    TotalArr[0] = Wins & 0xff;
    TotalArr[1] = Wins >> 0x8;
    TotalArr[2] = Losses & 0xff;
    TotalArr[3] = Losses >> 0x8;
    const TArrayView<const uint8> DatView = TotalArr;
    FFileHelper::SaveArrayToFile(DatView, *FilePath, &IFileManager::Get(), FILEWRITE_NoFail);
}

/*
 * Loads saved score
 */
void
FSysManager::LoadState()
{
    const FString Filename = TEXT("SweeperData.dat");
    IFileHandle * FilePtr = FPlatformFileManager::Get().GetPlatformFile().OpenRead(*Filename);
    if (FilePtr == nullptr) {
        // Should create file if it does not already exist
        return;
    }
    uint8 TotalArr[4] = {0};
    FilePtr->Read(&TotalArr[0x0], 0x4);
    Wins = (static_cast<uint16>(TotalArr[0]) << 0x8) | static_cast<uint16>(TotalArr[1]);
    Losses = (static_cast<uint16>(TotalArr[2]) << 0x8) | static_cast<uint16>(TotalArr[3]);
}

/*
 * Reset Game-board (Generating new match)
 */
void
FSysManager::ResetGame()
{
    UpdateGridSize();
    ClickedTiles = 0;
    SetEnableSlateGrid(true);
    for (auto & TextBlock : TileDisplayGrid) {
        TextBlock->SetText(TEXTARG(""));
    }
    ClearGridData();
    SlateGrid.clear();
    TileDisplayGrid.clear();
    SetDifficulty();
    PlaceMines();
    (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
}

/*
 * Restart same Game
 */
void
FSysManager::RestartGame()
{
    SetEnableSlateGrid(true);
    for (auto & TextBlock : TileDisplayGrid) {
        TextBlock->SetText(TEXTARG(""));
    }
    for (uint16 ColMod = 0x0; ColMod < CurrColSize;) {
        for (uint16 RowMod = 0x0; RowMod < CurrRowSize;) {
            SetAttributes<EBitField::IsClicked>({RowMod, ColMod}, 0x0);
            SetAttributes<EBitField::HasFlag>({RowMod, ColMod}, 0x0);
            SetAttributes<EBitField::HasQuestion>({RowMod, ColMod}, 0x0);
            RowMod++;
        }
        ColMod++;
    }
    ClickedTiles = 0;
    (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
    SlateGrid.clear();
    TileDisplayGrid.clear();
}

/*
 * Sets value to NextDifficulty, next game this new value will be loaded-
 */
void
FSysManager::FSetNextDiff(FSysManager::EGameDifficulty NextDiff)
{
    NextDifficulty = NextDiff;
}


/*
 * 
 * FSysManager::  Private member functions
 * @function void ClearGridData()
 * @function void DisableSlateGrid()
 * @function void SetDifficulty<FSysManager::EGameDifficulty>() 
 * @function void PlaceMines()
 * @function void ReplaceMine(Coords)
 * @function void CheckNeighbours(const Coords) 
 * @function void SpreadStep(Coords) 
 *
 */

/*
 * Clear GridData
 */
void
FSysManager::ClearGridData()
{
    for (auto & TileRow : GridData) {
        for (auto & TileData : TileRow) {
            TileData = 0x0;
        }
    }
}

/*
 * Set enable FSysManager->SlateGrid 
 */
void
FSysManager::SetEnableSlateGrid(bool bShouldEnable)
{
    for (auto & TileWidget : SlateGrid) {
        TileWidget.Get().SetEnabled(bShouldEnable);
    }
}

/*
 * Place mines on board
 */
void
FSysManager::PlaceMines()
{
    const auto GridSize = CurrRowSize * CurrColSize;

    // Continue until all random mines have been created.
    for (int PlacedCount = 0; PlacedCount < NumMines;) {
        const auto   Random = FMath::RandRange(0x0, GridSize);
        const uint16 X = Random / CurrRowSize;
        const uint16 Y = Random % CurrColSize;

        // Add the mine if no mine is placed at this position on the board
        if (!GetAttributes<EBitField::IsMine>({X, Y})) {
            SetAttributes<EBitField::IsMine>({X, Y}, 0x1);
            PlacedCount++;
        }
    }
}

/**
 * Replace a given mine tile 
 */
void
FSysManager::ReplaceMine(FCoords TileCoords)
{
    if (!GetAttributes<EBitField::IsMine>({TileCoords.X, TileCoords.Y})) {
        return; // Already no mine here, function called by mistake
    }
    for (uint16 CurrRow = 0; CurrRow < CurrRowSize; CurrRow++) {
        for (uint16 CurrCol = 0; CurrCol < CurrColSize; CurrCol++) {
            // Place Mine at first free tile found, then clear input Tile
            if (!GetAttributes<EBitField::IsMine>({CurrRow, CurrCol})) {
                SetAttributes<EBitField::IsMine>({CurrRow, CurrCol}, 0x1);
                SetAttributes<EBitField::IsMine>({TileCoords.X, TileCoords.Y}, 0x0);
                // Clear at end 
                return;
            }
        }
    }
}

/*
 * Check if a given tile is within bounds
 * Using 
 */
bool
FSysManager::CheckBounds(FCoords TileCoords) const
{
    return TileCoords >= FCoords{0, 0} &&
           TileCoords < FCoords{CurrRowSize, CurrColSize};
}

/*
 * Counts how many neighbours are mines
 */
void
FSysManager::CountNeighbours(const FCoords TileCoords)
{
    uint8   NeighbourCountLocal = 0;
    FCoords TileCoordsLocal;
    for (auto & RowMod : NeighbourCheck) {
        TileCoordsLocal.X = TileCoords.X + RowMod;
        for (auto & ColMod : NeighbourCheck) {
            if (RowMod == 0 && ColMod == 0) { continue; }
            TileCoordsLocal.Y = TileCoords.Y + ColMod;

            if (!CheckBounds(TileCoordsLocal)) { continue; }
            NeighbourCountLocal += GetAttributes<EBitField::IsMine>(
                TileCoordsLocal);
        }
    }
    SetAttributes<EBitField::NeighbourMines>(TileCoords, NeighbourCountLocal);
    if (!GetAttributes<EBitField::IsClicked>(TileCoords) && NeighbourCountLocal > 0x0) {
        SetAttributes<EBitField::IsClicked>(TileCoords, 0x1);
        ClickedTiles++;
    }
}

/*
 * Update text and enabled status of tile
 */
void
FSysManager::UpdateTileStyle(const FCoords TileCoords)
{
    GetTileTextBlock(TileCoords)
        ->SetText(NUMTEXTARG(GetAttributes<EBitField::NeighbourMines>(TileCoords)));
    GetGridFSlot(TileCoords)->SetEnabled(false);
}

/*
 * Check Neighbouring tiles for bomb-tiles
 */
void
FSysManager::CheckNeighbours(const FCoords TileCoords)
{
    if (GetAttributes<EBitField::IsMine>(TileCoords)) {
        SetAttributes<EBitField::NeighbourMines>(TileCoords, 0xf);
        return;
    }
    CountNeighbours(TileCoords);
    UpdateTileStyle(TileCoords);
}

/*
 * Display BombTiles
 */
void
FSysManager::DisplayBombs()
{
    FCoords TileCoords;
    for (auto ColMod = 0x0; ColMod < CurrColSize; ColMod++) {
        TileCoords.Y = ColMod;
        for (auto RowMod = 0x0; RowMod < CurrRowSize; RowMod++) {
            TileCoords.X = RowMod;
            if (GetAttributes<EBitField::IsMine>(TileCoords)) {
                FString ToString = "X";
                GetTileTextBlock(TileCoords)->SetText(TEXTARG(ToString));
            }
        }
    }
}

/*
 * Spread step, expands from give tile coords
 * @note Reassigning tiles with logical operators instead of if-statements.
 *       Having if's here would add risk for branch-prediction bottlenecks on larger boards,
 *       (We have a max time-complexity of O(n^k) for the actual search,
 *        therefore we should try to lessen other possible bottlenecks, like branchprediction)
 */
void
FSysManager::SpreadStep(FCoords TileCoords)
{
    // Checking for already clicked will reduce the complexity of the spread-search, as different paths won't overlap
    if (GetAttributes<EBitField::IsClicked>(TileCoords)) { return; }
    SetAttributes<EBitField::IsClicked>(TileCoords, 0x1);

    CheckNeighbours(TileCoords);
    if (GetAttributes<EBitField::NeighbourMines>(TileCoords) > 0x0) { return; }

    std::vector<FCoords> CurrentTilePath{};
    CurrentTilePath.emplace_back(TileCoords);

    uint8 Step = 0;
    bool  bBacktracker = true;
    while (bBacktracker) {
        for (auto & RowMod : NeighbourCheck) {
            for (auto & ColMod : NeighbourCheck) {
                if (RowMod == 0 && ColMod == 0) {
                    continue;
                }
                Step = RowMod + ColMod;

                const FCoords LastTile = TileCoords;

                /* Branchless switch, not really needed here but I thought I could use it to showcase
                 * how I would optimize a piece of code with alot of logic such as: if(cond) -> do arithmetic operation
                 * This omits 4 conditional if-checks from this nected loop, halving the amount
                 *  of times we run into risk of failed branch-prediction
                 *  
                 * Definitive reasons one would use branchless logic like this:
                 * 1. Compilers are very fast at doing arihtemtic operations.
                 * 2. Compilers do not optimize if's into arihtemtic operations, even with optimization turned on
                 * 3. if statements, on assembly-level, loads a group of instructions into memory, and then checks if the supplied condition is met.
                 *    In-case the condition is not met during evaluation, then it throws away the loaded instructions and then wastes time loading the other instruction, during runtime, before executing.
                 */ 
                TileCoords.Y +=
                    ((TileCoords.Y < (CurrColSize - 1)) * (ColMod == 0x1)) -
                    ((TileCoords.Y > 0) * (ColMod == -0x1));

                TileCoords.X +=
                    ((TileCoords.X < (CurrRowSize - 1)) * (RowMod == 0x1)) -
                    ((TileCoords.X > 0) * (RowMod == -0x1));

                CheckNeighbours(TileCoords);
                const bool bCantStep = GetAttributes<EBitField::NeighbourMines>(TileCoords) > 0x0 ||
                                       GetAttributes<EBitField::IsClicked>(TileCoords) > 0x0;

                const bool bCanStep = GetAttributes<EBitField::NeighbourMines>(TileCoords) == 0x0 &&
                                      GetAttributes<EBitField::IsClicked>(TileCoords) == 0x0;
                const bool bPathEmpty = CurrentTilePath.empty();
                const bool bLastStep = Step == 0x2;

                // Go back one step if we can't contniue in any direction
                if (bLastStep && bCantStep && !bPathEmpty) {
                    TileCoords = CurrentTilePath.back(); // Reset tile, go to next step
                    CurrentTilePath.pop_back();
                    break;
                }
                if (bLastStep && bCantStep && bPathEmpty) {
                    TileCoords = LastTile;
                    CurrentTilePath.emplace_back(TileCoords); // Reset tile, go to next step
                    break;
                }

                // Skip loop step if it isn't last four-way step
                if (bCantStep) {
                    TileCoords = LastTile;
                    continue;
                }

                if (bCanStep) {
                    CurrentTilePath.emplace_back(TileCoords);
                    SetAttributes<EBitField::IsClicked>(TileCoords, 0x1);
                    ClickedTiles++;
                    break;
                }
            }
        }
        bBacktracker = !(Step >= 0x2 && (CurrentTilePath.size() == 0x1 || CurrentTilePath.empty()));
    }
    (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
}


/*
 * Set Game-board difficulty/
 */
void
FSysManager::SetDifficulty()
{
    const auto GridSize = CurrRowSize * CurrColSize;
    if (NextDifficulty == EGameDifficulty::Easy) {
        NumMines = GridSize / 6; // A 6th of the board-tiles are mines
    } else if (NextDifficulty == EGameDifficulty::Normal) {
        NumMines = GridSize / 3; // A 3rd of the board-tiles are mines
    } else if (NextDifficulty == EGameDifficulty::Hard) {
        NumMines = GridSize / 2; // Half the board-tiles are mines
    } else { // Insane
        NumMines = (GridSize * 3) / 4; // Three quarters of the board-tiles are mines
    }
    FreeTilesCount = GridSize - NumMines;
}
#undef LOCTEXT_NAMESPACE
