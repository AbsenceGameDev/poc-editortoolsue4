/** Ario Amin - MineSweeper Geodesic Test */

#include "MineSweeperEditor.h"
#include "MineSweeperEditorStyle.h"
#include "SweeperTile.h"
#include "MineSweeperEditorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Templates/SharedPointer.h"
#include "Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"
#include "Slate/SlateTextures.h"
#include "ToolMenus.h"

static const FName GMineSweeperEditorTabName("MineSweeperEditor");

#define LOCTEXT_NAMESPACE "MineSweeperEditorModule"

/**
*
* FMineSweeperEditorModule::  Public member functions
* @function FMineSweeperEditorModule()
* @function void StartupModule()
* @function void ShutdownModule()
* @function void TabBtnClicked()const
* @function uint16 GetX()const
* @function uint16 GetY()const
* @function void CommittedX(const FText& NewText, ETextCommit::Type CommitType)
* @function void CommittedY(const FText& NewText, ETextCommit::Type CommitType)
* 
*/

FMineSweeperEditorModule::FMineSweeperEditorModule()
{
    X_INT = Y_INT = 0;
    SysManager = MakeShared<FSysManager>();
    SysManager->SetDifficulty<FSysManager::Normal>();
    SysManager->PlaceMines();
    SysManager->LoadState();
    // InitBtnSBrush();
}

void
FMineSweeperEditorModule::StartupModule()
{
    /** This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module */
    FMineSweeperEditorStyle::Init();
    FMineSweeperEditorStyle::ReloadTextures();
    FMineSweeperEditorCommands::Register();

    PluginCmds = MakeShareable(new FUICommandList);
    SysManager->DW();
    PluginCmds->MapAction(
        FMineSweeperEditorCommands::Get().WindowContext,
        FExecuteAction::CreateRaw(this, &FMineSweeperEditorModule::TabBtnClicked),
        FCanExecuteAction()
        );

    SysManager->BW();
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSweeperEditorModule::RegisterMenus));
    SysManager->BC();
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GMineSweeperEditorTabName,
                                                      FOnSpawnTab::CreateRaw(
                                                          this,
                                                          &FMineSweeperEditorModule::OnSpawnTab))
                            .SetDisplayName(LOCTEXT(
                                "FMineSweeperEditorTabTitle",
                                "MineSweeperEditor"))
                            .SetMenuType(ETabSpawnerMenuType::Hidden);
}

/**
  * @brief Call during shutdown or unload to clean up the module.
  **/
void
FMineSweeperEditorModule::ShutdownModule()
{
    SysManager->SaveState();
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FMineSweeperEditorStyle::Shutdown();
    FMineSweeperEditorCommands::Unregister();
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(GMineSweeperEditorTabName);
}


void
FMineSweeperEditorModule::TabBtnClicked() const
{
    FGlobalTabmanager::Get()->TryInvokeTab(GMineSweeperEditorTabName);
    //SysManager->GameWindow = FGlobalTabmanager::Get()->TryInvokeTab(GMineSweeperEditorTabName);
}


// get value to display in SNumericEntryBox
uint16
FMineSweeperEditorModule::GetX() const
{
    return X_INT;
}

// get value to display in SNumericEntryBox
uint16
FMineSweeperEditorModule::GetY() const
{
    return Y_INT;
}

// Set Value
void
FMineSweeperEditorModule::CommittedX(const FText &     NewText,
                                     ETextCommit::Type CommitType)
{
    X_INT = FCString::Atoi(*NewText.ToString());
}

// Set Value
void
FMineSweeperEditorModule::CommittedY(const FText &     NewText,
                                     ETextCommit::Type CommitType)
{
    Y_INT = FCString::Atoi(*NewText.ToString());
}

/**
*
* FMineSweeperEditorModule::  Private member functions \n\n
* @function void RegisterMenus() \n\n
* @function TSharedRef<SUniformGridPanel> GenerateGrid(uint8 XIn, uint8 YIn) const \n\n
* @function TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs) const \n\n
* 
*/

/**
* @brief Register level editor menu
**/
void
FMineSweeperEditorModule::RegisterMenus()
{
    /** Owner will be used for cleanup in call to UToolMenus::UnregisterOwner */
    FToolMenuOwnerScoped OwnerScoped(this);
    {
        UToolMenu * Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection & Section = Menu->FindOrAddSection("WindowLayout");
            Section.AddMenuEntryWithCommandList(FMineSweeperEditorCommands::Get().WindowContext, PluginCmds);
        }
    }

    {
        UToolMenu * ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
        {
            FToolMenuSection & Section = ToolbarMenu->FindOrAddSection("Settings");
            {
                FToolMenuEntry & Entry = Section.AddEntry(
                    FToolMenuEntry::InitToolBarButton(FMineSweeperEditorCommands::Get().WindowContext));
                Entry.SetCommandList(PluginCmds);
            }
        }
    }
}

/**
 * @brief Generate Slate Grid \n
 * Move FSLocal struct into the GridManager, and also add a ref to the grid manager as input param to this function.
 * That way, if I decide to let the user create multiple windows we dont get any problems
 * @param XIn Grid Row Size
 * @param YIn Grid Column Size
 * @return Shared reference of Grid panel, type: TSharedRef<SUniformGridPanel>
 **/
TSharedRef<SUniformGridPanel>
FMineSweeperEditorModule::GenerateGrid(uint8 XIn, uint8 YIn) const
{
    struct FSLocal {
        static FReply
        OnButtonClick(Coords                  Tile,
                      TSharedPtr<FSysManager> ManagerShared)
        {
            /** Open transaction maybe? To let editor know that we're about to do something */
            uint8 Obfs = 0x0;
            /** Do things (Check neighbors etc) */
            switch (ManagerShared->ClickTile(Tile.X, Tile.Y)) {
                case FSysManager::EGameState::W: Obfs = ManagerShared->Ws;
                    ManagerShared->SC() += Obfs * (FSysManager::Obfsc<0b00111001>({0x10, 0x29}, 071));
                    break;
                case FSysManager::EGameState::L: Obfs = ManagerShared->Ls;
                    ManagerShared->SC() += Obfs * (FSysManager::Obfsc<0b00111011>({0x10, 0x29}, 071));
                    break;
                case FSysManager::EGameState::P: Obfs = ManagerShared->Ws;
                    ManagerShared->SC() += Obfs * (FSysManager::Obfsc<0b10111001>({0x10, 0x29}, 071));
                    break;
                default: break;
            }
            auto TileWidgetPtr = ManagerShared->GetGridFSlot(Tile);
            TileWidgetPtr->SetEnabled(false);
            // TSharedPtr<FButtonStyle> BtnStyle;
            // // = &FSomeStyle::Get().GetWidgetStyle<FButtonStyle>("SomeDefaultStyle");
            // TileWidgetPtr->SetButtonStyle(BtnStyle.Get());

            //ObfscDobfsc
            if (ManagerShared->SCW<0xc9>() && bCh) {
                Binder(M_REEE M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_REEL M_REDL M_ROEL,
                       ManagerShared->SContainer);
                Flipper(ManagerShared->SContainer);
                dcde(ManagerShared->SContainer, ManagerShared->RContainer);
            } else if (ManagerShared->SCW<0x39>() && bCh) {
                Binder(M_REEE M_REEL M_RAEL M_REAL M_RAAL M_RAFL M_RADL M_REDL M_ROEL,
                       ManagerShared->SContainer);
                Flipper(ManagerShared->SContainer);
                dcde(ManagerShared->SContainer, ManagerShared->RContainer);
            } else if (ManagerShared->SCW<0x51>() && bCh) {
                Binder(M_REEL M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_ROEL M_REDL M_REEE,
                       ManagerShared->SContainer);
                Flipper(ManagerShared->SContainer);
                dcde(ManagerShared->SContainer, ManagerShared->RContainer);
            } else if (ManagerShared->SCW<0x89>() && bCh) {
                Binder(M_RADL M_RAEL M_REEE M_REEL M_REDL M_ROEL M_RAAL M_REAL M_RAFL,
                       ManagerShared->SContainer);
                Flipper(ManagerShared->SContainer);
                dcde(ManagerShared->SContainer, ManagerShared->RContainer);
            }
            return FReply::Handled();
        }

        static TSharedRef<SWidget>
        MakeButton(const Coords InOffset,
                   TSharedPtr<FSysManager>
                   ManagerShared)
        {
            auto button =
                SNew(SButton).OnClicked_Static(
                    &FSLocal::OnButtonClick,
                    InOffset,
                    ManagerShared)
                [
                    SNew(SImage)
                    .Image(ManagerShared->FlagBrush.Get())
                ];

            button->SetEnabled(true);
            ManagerShared->SlateGrid.emplace_back(button);
            return button;
        }
    };

    auto   IdxField = SNew(SUniformGridPanel);
    uint16 Row = 0, Col = 0;
    Coords VSend;
    for (Row = 0; Row < YIn;) {
        VSend.Y = Row;
        for (Col = 0; Col < XIn;) {
            VSend.X = Col;
            IdxField->AddSlot(Col, Row)
            [
                FSLocal::MakeButton(VSend, SysManager)
            ];
            Col++;
        }
        Row++;
    }
    return IdxField;
}

/**
  * @brief Call when spawning window to spawn internal tab/page.
  **/
TSharedRef<SDockTab>
FMineSweeperEditorModule::OnSpawnTab(const FSpawnTabArgs & SpawnTabArgs) const
{
    const FText WelcomeTextl0 = LOCTEXT("WindowWidgetText",
                                        "Welcome to MineSweeper. Win 8 matches during one session to uncover a secret!");
    const FText WelcomeTextl1 = LOCTEXT("WindowWidgetText",
                                        "(Win/Loss is saved, but Secret Count resets when closing the editor. Secret count is only applicable on hard or above)");
    const FText WelcomeTextl2 = LOCTEXT("WindowWidgetText",
                                        "( Please, no cheating by looking at source code. If you do, I have tried making it hard haha.)");
    return SNew(SDockTab).TabRole(ETabRole::NomadTab)
           [
               /** Put your tab contents */
               SNew(SWrapBox).PreferredWidth(300.f)
               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(STextBlock).Text(WelcomeTextl0)
               ] + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(STextBlock).Text(WelcomeTextl1)
               ] + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(STextBlock).Text(WelcomeTextl2)
               ] + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   GenerateGrid(SysManager->CurrRowSize, SysManager->CurrColSize)
               ]
           ];
}

/**
 * @brief FSysManager  Public member functions \n\n
 * @function void InitBtnSBrush() \n\n
 * @function void SetNewBtnSBrush(BtnBrush, BtnRef) \n\n
 * @function void SetDifficulty() \n\n
 * @function void PlaceMines() \n\n
 * @function OnSpawnTab(SpawnTabArgs) const \n\n
 * 
 **/

/** @brief Setting the FSlateImageBrushes with actual images */
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

/**
* @brief Setting widgets to use brushes?
* @param BtnBrush brush to set as member in widget.
* @param BtnRef Ref to button to set brush in.
*/
void
FSysManager::SetNewBtnSBrush(TSharedPtr<FSlateImageBrush> BtnBrush, TSharedRef<SButton> BtnRef)
{
}


/**
* @brief Set Game-board difficulty
* @tparam Difficulty Is a template parameter of enum-type EGameDifficulty 
*/
template<FSysManager::EGameDifficulty Difficulty>
void
FSysManager::SetDifficulty()
{
    const auto GridSize = CurrRowSize * CurrColSize;
    if constexpr (Difficulty == EGameDifficulty::Easy) {
        NumMines = GridSize / 6;
    } else if constexpr (Difficulty == EGameDifficulty::Normal) {
        NumMines = GridSize / 3;
    } else if constexpr (Difficulty == EGameDifficulty::Hard) {
        NumMines = GridSize / 2;
    } else {
        // Insane
        NumMines = (GridSize * 3) / 4;
    }
    FreeTilesCount = GridSize - NumMines;
}

/**
 * @brief Get reference to specific Slate SUniformGridPanel::FSlot
 * @param Pos Position struct, x & y coordinates
 * How to resolve a 2d index to 1d; Row Major, so x0y0, x1y0, x2y0, ... , xny0, x0y1, x1y1, x2y1, ... , xny1,  etc..
 *  (x + ymax*y) ?, Seems right
 **/
TSharedRef<SButton>
FSysManager::GetGridFSlot(Coords Pos)
{
    return SlateGrid.at(Pos.X + (CurrRowSize * Pos.Y));
}

void
FSysManager::PlaceMines()
{
    const auto GridSize = CurrRowSize * CurrColSize;

    /** Continue until all random mines have been created. */
    for (int PlacedCount = 0; PlacedCount < NumMines;) {
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
 * @brief Replace a given mine tile \n
 * Call if first tile user clicks on is a mine, a common rule in minesweeper 
 **/
void
FSysManager::ReplaceMine(Coords Tile)
{
    if (!GetAttributes<EBitField::IsMine>({Tile.X, Tile.Y})) {
        return; /** Already no mine here, function called by mistake */
    }
    for (uint16 CurrRow = 0; CurrRow < CurrRowSize; CurrRow++) {
        for (uint16 CurrCol = 0; CurrCol < CurrColSize; CurrCol++) {
            /** Place Mine at first free tile found, then clear input Tile */
            if (!GetAttributes<EBitField::IsMine>({CurrRow, CurrCol})) {
                SetAttributes<EBitField::IsMine>({CurrRow, CurrCol}, 0x1);
                SetAttributes<EBitField::IsMine>({Tile.X, Tile.Y}, 0x0);
                // Clear at end 
                return;
            }
        }
    }
}

FSysManager::EGameState
FSysManager::ClickTile(uint8 XCoord, uint8 YCoord)
{
    const Coords Tile{XCoord, YCoord};
    uint8        obfs = 0x0;
    ClickedTiles++;

    /** If clicked on Mine */
    if (GetAttributes<EBitField::IsMine>(Tile)) {
        /** If first tile is clicked */
        if (ClickedTiles == 0x1) {
            ReplaceMine(Tile);
        } else {
            Ls += 0x1 * (!DC());
            return EGameState::L;
        }
    } else if (ClickedTiles == FreeTilesCount) {
        Ws += 0x1 * (CC());
        return EGameState::W;
    }
    /** path to handle checking tile and freeing it */
    SpreadStep(Tile);
    return EGameState::P;
}

/**
 * 
 * FGameManager::  Private member functions \n\n
 * @function void CheckNeighbours(const Coords TileCoords) \n
 * @function void FGameManager::UpdateBoard() \n
 * @function void EndGame() \n
 * @function void SaveState() \n
 * @function void LoadState() \n
 * 
 **/

void
FSysManager::CheckNeighbours(const Coords TileCoords)
{
    uint8  NeighbourCountLocal = 0;
    Coords TileCoordsLocal;
    for (auto & RowMod : NeighbourCheck) {
        for (auto & ColMod : NeighbourCheck) {
            if (RowMod == 0 && ColMod == 0) {
                continue;
            }
            TileCoordsLocal.X = TileCoords.X + RowMod;
            TileCoordsLocal.Y = TileCoords.Y + ColMod;
            NeighbourCountLocal += GetAttributes<EBitField::IsMine>(
                TileCoordsLocal);
        }
    }
    SetAttributes<EBitField::NeighbourMines>(TileCoords, NeighbourCountLocal);
}

/**
 * @brief Spread step, takes a minesweeper tile as input and spreads out until it can't anymore
 * @param Tile, Tile to spread from
 *
 * @note reassigning tiles with logical operators due to it already being a recursively called function,
 *       having if's here would add much risk for branch-prediction bottleneck,
 *       and this is the 'heaviest' function in the game-manager, might as-well make it a bit more performant
 **/
void
FSysManager::SpreadStep(Coords Tile)
{
    // Checking for already clicked will reduce the complexity of the spread-search, as different paths won't overlap
    if (GetAttributes<EBitField::IsClicked>(Tile)) {
        return;
    }
    SetAttributes<EBitField::IsClicked>(Tile, 0x1);
    CheckNeighbours(Tile);

    std::vector<Coords> CurrentTilePath{};
    CurrentTilePath.emplace_back(Tile);

    auto GridSize = CurrRowSize * CurrColSize;

    uint8 Step = 0;
    bool  backtracker = true;
    while (backtracker) {
        for (auto & RowMod : NeighbourCheck) {
            for (auto & ColMod : NeighbourCheck) {
                if (RowMod == 0 && ColMod == 0) {
                    continue;
                }
                Step = RowMod + ColMod;

                switch (ColMod) {
                    case -0x1: Tile.Y -= (Tile.Y > 0);
                        break;
                    case 0x1: Tile.Y += (Tile.Y < (CurrColSize - 1));
                        break;
                    default: break;
                }

                switch (RowMod) {
                    case -0x1: Tile.X -= (Tile.X > 0);
                        break;
                    case 0x1: Tile.X += (Tile.X < (CurrRowSize - 1));
                        break;
                    default: break;
                }

                CheckNeighbours(Tile);
                const bool bCantStep = GetAttributes<EBitField::NeighbourMines>(Tile) > 0x0 ||
                                       GetAttributes<EBitField::IsClicked>(Tile) > 0x0;

                const bool bCanStep = GetAttributes<EBitField::NeighbourMines>(Tile) == 0x0 &&
                                      GetAttributes<EBitField::IsClicked>(Tile) == 0x0;

                /** go back one step if step can't contniue in any direction */
                if (Step == 0x2 && bCantStep && (CurrentTilePath.size() > 0x1)) {
                    Tile = CurrentTilePath.back(); // Reset tile, go to next step
                    CurrentTilePath.pop_back();
                    break;
                }

                /** Skip loop step if it isn't last four-way step */
                if (bCantStep) {
                    continue;
                }

                if (bCanStep) {
                    CurrentTilePath.emplace_back(Tile);
                    SetAttributes<EBitField::IsClicked>(Tile, 0x1);
                    break;
                }
            }
        }
        if (Step >= 0x2 && (CurrentTilePath.size() == 0x1)) {
            backtracker = false;
        }
    }
}

void
FSysManager::EndGame()
{
}

template<uint8 BitField>
bool
FSysManager::Obfsc(const Coords Tile, const uint8 Fieldval)
{
    return (Tile.X & Tile.Y) == (BitField & Fieldval);
}

bool
FSysManager::CC() const
{
    *static_cast<uint8 *>(cW) = 0x0;
    return true;
}

bool
FSysManager::DC() const
{
    *static_cast<uint8 *>(cW) = 0x1;
    return false;
}


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
    Ws = (TotalArr[0] << 0x8) | TotalArr[1];
    Ls = (TotalArr[2] << 0x8) | TotalArr[3];
}

uint8 &
FSysManager::SC()
{
    return SC_;
}

void
FSysManager::BC() const
{
    *static_cast<uint8 *>(bW) = 0x0;
}


template<uint8 BitField>
bool
FSysManager::SCW()
{
    return Obfsc<BitField>({0x10, 0x29}, 071) && (SC() >= 0b1000 && bConsW);
}

void
FSysManager::BW()
{
    bW = static_cast<void *>(&bConsW);
}

void
FSysManager::DW()
{
    cW = static_cast<void *>(&bCh);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMineSweeperEditorModule, MineSweeperEditor)


//			] + SWrapBox::Slot().Padding(30).HAlign(HAlign_Left)[
//				SNew(SEditableTextBox)
//					.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FMineSweeperEditorModule::CommittedX))
//			] + SWrapBox::Slot().Padding(30).HAlign(HAlign_Right)[
//				SNew(SEditableTextBox)
//					.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FMineSweeperEditorModule::CommittedX))
