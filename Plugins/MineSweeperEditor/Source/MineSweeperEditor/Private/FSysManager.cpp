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

/**
 *
 * @brief FSysManager::  Public member functions 
 * @function void FSysManager(), InitBtnSBrush() 
 * @function TSharedRef<SButton> GetGridFSlot(Coords)
 * @function FSysManager::EGameState ClickTile(uint8, uint8)
 * @function void SaveState(), LoadState()
 * @function uint8 GetAttributes<FSysManager::EBitField>(const Coords);
 * @function void SetAttributes<FSysManager::EBitField>(const Coords, const uint8);
 * @function void ResetGame()
 * 
 **/
FSysManager::FSysManager()
{
    DifficultyList.Emplace(MakeShared<FString>("Easy :/"));
    DifficultyList.Emplace(MakeShared<FString>("Normal :)"));
    DifficultyList.Emplace(MakeShared<FString>("Hard :D"));
    DifficultyList.Emplace(MakeShared<FString>("Insane :("));
    OptGridWidgetRef = MakeShared<SUniformGridPanel>();
    OptEndMsgRef = SNew(STextBlock); // MakeShared<STextBlock>();
    OptStatsRef = SNew(STextBlock) MAKEROBOTO(18);
    OptScoreRef = SNew(STextBlock) MAKEROBOTO(18);
    Obfsctr = MakeShared<FObfuscator>();
    InitBtnSBrush();
    SetDifficulty<FSysManager::Normal>();
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
        FText::FromString("Wins: " + FString::FromInt(Ws) + "\n"
                          + "Losses:" + FString::FromInt(Ls)));
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
 * Get End message Ftext to display in OptEndMsgRef
 */
FText
FSysManager::DisplayEndMsg() const
{
    return FText::FromString(SContainer);
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
    /** If clicked on Mine */
    if (GetAttributes<EBitField::IsMine>(TileCoords)) {
        /** If first tile is clicked */
        if (ClickedTiles == 0x1) {
            ReplaceMine(TileCoords);
        } else {
            Ls += 0x1 * (Obfsctr->CC());
            DisplayBombs();
            for (auto & TileWidget : SlateGrid) {
                TileWidget.Get().SetEnabled(false);
            }
            ClickedTiles--;
            (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
            return EGameState::L;
        }
    } else if (ClickedTiles > FreeTilesCount) {
        Ws += 0x1 * (!Obfsctr->DC());
        for (auto & TileWidget : SlateGrid) {
            TileWidget.Get().SetEnabled(false);
        }
        return EGameState::W;
    }
    (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
    /** path to handle checking tile and freeing it */
    SpreadStep(TileCoords);
    return EGameState::P;
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
    TotalArr[0] = Ws & 0xff;
    TotalArr[1] = Ws >> 0x8;
    TotalArr[2] = Ls & 0xff;
    TotalArr[3] = Ls >> 0x8;
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
    Ws = (static_cast<uint16>(TotalArr[0]) << 0x8) | static_cast<uint16>(TotalArr[1]);
    Ls = (static_cast<uint16>(TotalArr[2]) << 0x8) | static_cast<uint16>(TotalArr[3]);
}

/*
* Reset Game-board (Generating new match)
*/
void
FSysManager::ResetGame()
{
    UpdateGridSize();
    ClickedTiles = 0;
    for (auto & TileWidget : SlateGrid) {
        TileWidget->SetEnabled(true);
    }
    for (auto & TextBlock : TileDisplayGrid) {
        TextBlock->SetText(FText::FromString("_"));
    }
    for (auto & TileRow : GridData) {
        for (auto & TileData : TileRow) {
            TileData = 0x0;
        }
    }
    SlateGrid.clear();
    TileDisplayGrid.clear();
    SetDifficulty<FSysManager::Normal>(); // Will bind this later
    PlaceMines();
    (*OptStatsRef)->SetText(NUMTEXTARG(FREETILES));
}

/*
* Restart same Game
*/
void
FSysManager::RestartGame()
{
    for (auto & TileWidget : SlateGrid) {
        TileWidget->SetEnabled(true);
    }
    for (auto & TextBlock : TileDisplayGrid) {
        TextBlock->SetText(FText::FromString("_"));
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

/**
 * 
 * FSysManager::  Private member functions 
 * @function void SetDifficulty<FSysManager::EGameDifficulty>() 
 * @function void PlaceMines()
 * @function void ReplaceMine(Coords)
 * @function void CheckNeighbours(const Coords) 
 * @function void SpreadStep(Coords) 
 *
 **/

/*
 * Set Game-board difficulty/
 */
template<FSysManager::EGameDifficulty Difficulty>
void
FSysManager::SetDifficulty()
{
    const auto GridSize = CurrRowSize * CurrColSize;
    if constexpr (Difficulty == EGameDifficulty::Easy) {
        NumMines = GridSize / 6; // A 6th of the board-tiles are mines
    } else if constexpr (Difficulty == EGameDifficulty::Normal) {
        NumMines = GridSize / 3; // A 3rd of the board-tiles are mines
    } else if constexpr (Difficulty == EGameDifficulty::Hard) {
        NumMines = GridSize / 2; // Half the board-tiles are mines
    } else {
        // Insane
        NumMines = (GridSize * 3) / 4; // Three quarters of the board-tiles are mines
    }
    FreeTilesCount = GridSize - NumMines;
}

/*
 * Place mines on board
 */
void
FSysManager::PlaceMines()
{
    const auto GridSize = CurrRowSize * CurrColSize;

    /** Continue until all random mines have been created. */
    for (int PlacedCount = 0; PlacedCount <= NumMines;) {
        const auto   Random = FMath::RandRange(0x0, GridSize);
        const uint16 X = Random / CurrRowSize;
        const uint16 Y = Random % CurrColSize;

        /** Add the mine if no mine is placed at this position on the board */
        if (!GetAttributes<EBitField::IsMine>({X, Y})) {
            SetAttributes<EBitField::IsMine>({X, Y}, 0x1);
            PlacedCount++;
        }
    }
    return; // symbolic
}

/**
 * Replace a given mine tile 
 */
void
FSysManager::ReplaceMine(FCoords TileCoords)
{
    if (!GetAttributes<EBitField::IsMine>({TileCoords.X, TileCoords.Y})) {
        return; /** Already no mine here, function called by mistake */
    }
    for (uint16 CurrRow = 0; CurrRow < CurrRowSize; CurrRow++) {
        for (uint16 CurrCol = 0; CurrCol < CurrColSize; CurrCol++) {
            /** Place Mine at first free tile found, then clear input Tile */
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

void
FSysManager::CountNeighbours(const FCoords TileCoords)
{
    uint8  NeighbourCountLocal = 0;
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

void
FSysManager::UpdateTileStyle(const FCoords TileCoords)
{
    GetTileTextBlock(TileCoords)
        ->SetText(
            FText::FromString(
                FString::FromInt(
                    GetAttributes<EBitField::NeighbourMines>(TileCoords)
                    )
                )
            );
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
                GetTileTextBlock(TileCoords)->SetText(FText::FromString(ToString));
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

                // Branchless switch,
                // because I already have too many if-blocks in this nested loop as it is
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

                /** go back one step if step can't contniue in any direction */
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

                /** Skip loop step if it isn't last four-way step */
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

// Please Ignore haha
template<uint8 BitField>
bool
FObfuscator::Obfsc(const FCoords TileCoords, const uint8 Fieldval)
{return (TileCoords.X & TileCoords.Y) == (BitField & Fieldval);}
void
FObfuscator::PC() { VC(); }
template<uint8 BitField>
bool
FObfuscator::SCW(){return Obfsc<BitField>({0x10, 0x29}, 071) && (SC() >= 0b1000 && bConsW);}
bool
FObfuscator::CC() const{*static_cast<uint8 *>(cW) = 0x0;return true;}
bool
FObfuscator::DC() const{*static_cast<uint8 *>(cW) = 0x1;return false;}
void
FObfuscator::VC() { SC() = 0x0; }
void
FObfuscator::ObfscDobfsc(TSharedPtr<FSysManager> ManagerShared)
{
    //ObfscDobfsc
    if (SCW<0xc9>() && bCh) {
        Binder(M_REEE M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_REEL M_REDL M_ROEL,
               ManagerShared->SContainer);
    } else if (SCW<0x39>() && bCh) {
        Binder(M_REEE M_REEL M_RAEL M_REAL M_RAAL M_RAFL M_RADL M_REDL M_ROEL,
               ManagerShared->SContainer);
    } else if (SCW<0x51>() && bCh) {
        Binder(M_REEL M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_ROEL M_REDL M_REEE,
               ManagerShared->SContainer);
    } else if (SCW<0x89>() && bCh) {
        Binder(M_RADL M_RAEL M_REEE M_REEL M_REDL M_ROEL M_RAAL M_REAL M_RAFL,
               ManagerShared->SContainer);
    }
    Flipper(ManagerShared->SContainer);
    Dcde(ManagerShared->SContainer, ManagerShared->RContainer);
}
void
FObfuscator::DobfscObfsc(TSharedPtr<FSysManager> ManagerShared, FSysManager::EGameState GameState)
{
    switch (GameState) {
        case FSysManager::EGameState::W: SC() += (FObfuscator::Obfsc<0b00111001>({0x10, 0x29}, 071));
            break;
        case FSysManager::EGameState::L: SC() += (FObfuscator::Obfsc<0b00111011>({0x10, 0x29}, 071));
            break;
        case FSysManager::EGameState::P: SC() += (FObfuscator::Obfsc<0b10111001>({0x10, 0x29}, 071));
            break;
        default: break;
    }
}
uint8 &
FObfuscator::SC(){return SC_;}
void
FObfuscator::BC() const{*static_cast<uint8 *>(bW) = 0x0;}
void
FObfuscator::CB() const{*static_cast<uint8 *>(cW) = 0x0;}
void
FObfuscator::BF() const{*static_cast<uint8 *>(bW) = 0xff;}
void
FObfuscator::BW(){SC() <<= 0x8;bW = static_cast<void *>(&bConsW);}
void
FObfuscator::DW(){cW = static_cast<void *>(&bCh);}

#undef LOCTEXT_NAMESPACE
//IMPLEMENT_MODULE(FMineSweeperEditorModule, MineSweeperEditor)
