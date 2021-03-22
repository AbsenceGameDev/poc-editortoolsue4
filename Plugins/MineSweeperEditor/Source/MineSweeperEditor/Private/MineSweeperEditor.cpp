/** Ario Amin - MineSweeper Geodesic Test */

#include "MineSweeperEditor.h"
#include "MineSweeperEditorStyle.h"
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
* @brief FMineSweeperEditorModule::  Public member functions
* @function FMineSweeperEditorModule()
* @function void StartupModule(), ShutdownModule(), TabBtnClicked() const
* @function uint16 GetX() const, GetY() const
* @function void CommittedX(const FText& NewText, ETextCommit::Type CommitType)
* @function void CommittedY(const FText& NewText, ETextCommit::Type CommitType)
* 
*/

FMineSweeperEditorModule::FMineSweeperEditorModule()
{
    X_INT = Y_INT = 0;
    SysManager = MakeShared<FSysManager>();
    SysManager->LoadState();
}

/*
* Call when loading module.
*/
void
FMineSweeperEditorModule::StartupModule()
{
    /** This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module */
    FMineSweeperEditorStyle::Init();
    FMineSweeperEditorStyle::ReloadTextures();
    FMineSweeperEditorCommands::Register();

    PluginCmds = MakeShareable(new FUICommandList);
    SysManager->Obfsctr->DW();
    PluginCmds->MapAction(
        FMineSweeperEditorCommands::Get().WindowContext,
        FExecuteAction::CreateRaw(this, &FMineSweeperEditorModule::TabBtnClicked),
        FCanExecuteAction()
        );

    SysManager->Obfsctr->BW();
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSweeperEditorModule::RegisterMenus));
    SysManager->Obfsctr->BC();
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GMineSweeperEditorTabName,
                                                      FOnSpawnTab::CreateRaw(
                                                          this,
                                                          &FMineSweeperEditorModule::OnSpawnTab))
                            .SetDisplayName(LOCTEXT(
                                "FMineSweeperEditorTabTitle",
                                "MineSweeperEditor"))
                            .SetMenuType(ETabSpawnerMenuType::Hidden);
}

/*
 * Call during shutdown or unload to clean up the module.
 */
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

/*
 *  Brings up main plugin window
 */
void
FMineSweeperEditorModule::TabBtnClicked() const
{
    FGlobalTabmanager::Get()->TryInvokeTab(GMineSweeperEditorTabName);
}


/*
 * get value to display in SNumericEntryBox
 */
uint16
FMineSweeperEditorModule::GetX() const
{
    return X_INT;
}

/*
 * get value to display in SNumericEntryBox
 */
uint16
FMineSweeperEditorModule::GetY() const
{
    return Y_INT;
}

/*
 * Set Value
 */
void
FMineSweeperEditorModule::CommittedX(const FText &     NewText,
                                     ETextCommit::Type CommitType)
{
    X_INT = FCString::Atoi(*NewText.ToString());
}

/*
 * Set Value
 */
void
FMineSweeperEditorModule::CommittedY(const FText &     NewText,
                                     ETextCommit::Type CommitType)
{
    Y_INT = FCString::Atoi(*NewText.ToString());
}

/**
 *
 * @brief FMineSweeperEditorModule::  Private member functions 
 * @function void RegisterMenus() 
 * @function TSharedRef<SUniformGridPanel> GenerateGrid(uint8, uint8) const 
 * @function TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs&) const
 *
 **/

/*
 * Register level editor menu
 */
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

/*
 * Generate Slate Grid 
 */
TSharedRef<SUniformGridPanel>
FMineSweeperEditorModule::GenerateGrid(uint8 XIn, uint8 YIn) const
{
    struct FSLocal {
        /**
         * @brief On Tile Click event
         **/
        static FReply
        OnTileClick(Coords                  TileCoords,
                    TSharedPtr<FSysManager> ManagerShared)
        {
            /** Open transaction maybe? To let editor know that we're about to do something */
            uint8 Obfs = 0x0;
            /** Do things (Check neighbors etc) */
            auto ObfscDobfsc = ManagerShared->Obfsctr;
            switch (ManagerShared->ClickTile(TileCoords.X, TileCoords.Y)) {
                case FSysManager::EGameState::W: Obfs = ManagerShared->Ws;
                    ObfscDobfsc->SC() += Obfs * (FObfuscator::Obfsc<0b00111001>({0x10, 0x29}, 071));
                    break;
                case FSysManager::EGameState::L: Obfs = ManagerShared->Ls;
                    ObfscDobfsc->SC() += Obfs * (FObfuscator::Obfsc<0b00111011>({0x10, 0x29}, 071));
                    break;
                case FSysManager::EGameState::P: Obfs = ManagerShared->Ws;
                    ObfscDobfsc->SC() += Obfs * (FObfuscator::Obfsc<0b10111001>({0x10, 0x29}, 071));
                    break;
                default: break;
            }
            auto TileWidgetPtr = ManagerShared->GetGridFSlot(TileCoords);
            TileWidgetPtr->SetEnabled(false);
            // TSharedPtr<FButtonStyle> BtnStyle;
            // // = &FSomeStyle::Get().GetWidgetStyle<FButtonStyle>("SomeDefaultStyle");
            // TileWidgetPtr->SetButtonStyle(BtnStyle.Get());

            //ObfscDobfsc
            if (ObfscDobfsc->SCW<0xc9>() && bCh) {
                ObfscDobfsc->Binder(M_REEE M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_REEL M_REDL M_ROEL,
                                    ManagerShared->SContainer);
            } else if (ObfscDobfsc->SCW<0x39>() && bCh) {
                ObfscDobfsc->Binder(M_REEE M_REEL M_RAEL M_REAL M_RAAL M_RAFL M_RADL M_REDL M_ROEL,
                                    ManagerShared->SContainer);
            } else if (ObfscDobfsc->SCW<0x51>() && bCh) {
                ObfscDobfsc->Binder(M_REEL M_RAEL M_RAFL M_RADL M_REAL M_RAAL M_ROEL M_REDL M_REEE,
                                    ManagerShared->SContainer);
            } else if (ObfscDobfsc->SCW<0x89>() && bCh) {
                ObfscDobfsc->Binder(M_RADL M_RAEL M_REEE M_REEL M_REDL M_ROEL M_RAAL M_REAL M_RAFL,
                                    ManagerShared->SContainer);
            }
            ObfscDobfsc->Flipper(ManagerShared->SContainer);
            ObfscDobfsc->dcde(ManagerShared->SContainer, ManagerShared->RContainer);

            return FReply::Handled();
        }

        /**
         * @brief Make Tile
         * Make Tile and bind OnClick to it
         **/
        static TSharedRef<SWidget>
        MakeTile(const Coords TileCoords,
                 TSharedPtr<FSysManager>
                 ManagerShared)
        {

            auto Btn =
                SNew(SButton).OnClicked_Static(
                    &FSLocal::OnTileClick,
                    TileCoords,
                    ManagerShared).ForegroundColor(FSlateColor::UseForeground())
                [
                    SNew(SImage).Image(ManagerShared->BombBrush.Get())
                    // SNew(SBorder)
                    // .Padding(FMargin(3))
                    // [
                    //     SNew(STextBlock)
                    //     // Text(TEXT("0"))
                    //     //.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),16))
                    //     //.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
                    // ]
                ];

            Btn->SetEnabled(true);
            ManagerShared->SlateGrid.emplace_back(Btn);
            return Btn;
        }
    };

    auto   IdxField = SNew(SUniformGridPanel);
    uint16 Row = 0, Col = 0;
    Coords TileCoords;
    for (Row = 0; Row < YIn;) {
        TileCoords.Y = Row;
        for (Col = 0; Col < XIn;) {
            TileCoords.X = Col;
            IdxField->AddSlot(Col, Row)
            [
                FSLocal::MakeTile(TileCoords, SysManager)
            ];
            Col++;
        }
        Row++;
    }
    return IdxField;
}

/*
 * Call when spawning window to spawn internal tab/page.
 */
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
 *
 * @brief FSysManager::  Public member functions 
 * @function void FSysManager(), InitBtnSBrush() 
 * @function TSharedRef<SButton> GetGridFSlot(Coords)
 * @function FSysManager::EGameState ClickTile(uint8, uint8)
 * @function void SaveState(), LoadState()
 * @function uint8 GetAttributes<FSysManager::EBitField>(const Coords);
 * @function void SetAttributes<FSysManager::EBitField>(const Coords, const uint8);
 * 
 **/

FSysManager::FSysManager()
{
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
 * Get reference to specific Slate SUniformGridPanel::FSlot
 * How to resolve a 2d index to 1d;
 * Row Major, so x0y0, x1y0, x2y0, ... , xny0, x0y1, x1y1, x2y1, ... , xny1,  etc..
 * (x + xmax*y) ? Seems right
 */
TSharedRef<SButton>
FSysManager::GetGridFSlot(Coords TileCoords)
{
    return SlateGrid.at(TileCoords.X + (CurrRowSize * TileCoords.Y));
}

FSysManager::EGameState
FSysManager::ClickTile(uint8 XCoord, uint8 YCoord)
{
    const Coords TileCoords{XCoord, YCoord};
    uint8        obfs = 0x0;
    ClickedTiles++;

    /** If clicked on Mine */
    if (GetAttributes<EBitField::IsMine>(TileCoords)) {
        /** If first tile is clicked */
        if (ClickedTiles == 0x1) {
            ReplaceMine(TileCoords);
        } else {
            Ls += 0x1 * (!Obfsctr->DC());
            for (auto & TileWidget : SlateGrid) {
                TileWidget.Get().SetEnabled(false);
            }
            return EGameState::L;
        }
    } else if (ClickedTiles == FreeTilesCount) {
        Ws += 0x1 * (Obfsctr->CC());
        return EGameState::W;
    }
    /** path to handle checking tile and freeing it */
    SpreadStep(TileCoords);
    return EGameState::P;
}

/*
 * Get FSysManager Attributes
 */
template<FSysManager::EBitField BitField>
uint8
FSysManager::GetAttributes(const Coords TileCoords)
{
    if constexpr (BitField >= 0x4) {
        return (GridData[TileCoords.Y][TileCoords.X] >> 4UL) & 15UL;
    } else {
        return (GridData[TileCoords.Y][TileCoords.X] >> BitField) & 1UL;
    }
}

/*
 * Set FSysManager Attributes
 */
template<FSysManager::EBitField BitField>
void
FSysManager::SetAttributes(const Coords TileCoords, const uint8 Fieldval)
{
    auto & TileData = GridData[TileCoords.Y][TileCoords.X];
    if constexpr (BitField == EBitField::NeighbourMines) {
        TileData = (TileData & ~(15UL << BitField)) | ((Fieldval & 1UL) << BitField);
    }
    if constexpr (BitField == EBitField::IsMine) {
        TileData = (TileData & ~1UL) | (Fieldval & 1UL);
    } else {
        TileData = (TileData & ~(1UL << BitField)) | ((Fieldval & 1UL) << BitField);
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
    Ws = (TotalArr[0] << 0x8) | TotalArr[1];
    Ls = (TotalArr[2] << 0x8) | TotalArr[3];
}

/**
 * 
 * FSysManager::  Private member functions 
 * @function void SetDifficulty<FSysManager::EGameDifficulty>() 
 * @function void PlaceMines()
 * @function void ReplaceMine(Coords)
 * @function void CheckNeighbours(const Coords) 
 * @function void SpreadStep(Coords) 
 * @function void ResetGame() 
 *
 **/

/*
 * Set Game-board difficulty
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

/*
 * Place mines on board
 */
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
 * Replace a given mine tile 
 */
void
FSysManager::ReplaceMine(Coords TileCoords)
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
 * Check Neighbouring tiles for bomb-tiles
 */
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

/*
 * Spread step, expands from give tile coords
 * @note reassigning tiles with logical operators due to it already being a recursively called function,
 *       having if's here would add much risk for branch-prediction bottleneck,
 *       and this is the 'heaviest' function in the game-manager, might as-well make it a bit more performant
 */
void
FSysManager::SpreadStep(Coords TileCoords)
{
    // Checking for already clicked will reduce the complexity of the spread-search, as different paths won't overlap
    if (GetAttributes<EBitField::IsClicked>(TileCoords)) {
        return;
    }
    SetAttributes<EBitField::IsClicked>(TileCoords, 0x1);
    CheckNeighbours(TileCoords);

    std::vector<Coords> CurrentTilePath{};
    CurrentTilePath.emplace_back(TileCoords);

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
                    case -0x1: TileCoords.Y -= (TileCoords.Y > 0);
                        break;
                    case 0x1: TileCoords.Y += (TileCoords.Y < (CurrColSize - 1));
                        break;
                    default: break;
                }

                switch (RowMod) {
                    case -0x1: TileCoords.X -= (TileCoords.X > 0);
                        break;
                    case 0x1: TileCoords.X += (TileCoords.X < (CurrRowSize - 1));
                        break;
                    default: break;
                }

                CheckNeighbours(TileCoords);
                const bool bCantStep = GetAttributes<EBitField::NeighbourMines>(TileCoords) > 0x0 ||
                                       GetAttributes<EBitField::IsClicked>(TileCoords) > 0x0;

                const bool bCanStep = GetAttributes<EBitField::NeighbourMines>(TileCoords) == 0x0 &&
                                      GetAttributes<EBitField::IsClicked>(TileCoords) == 0x0;

                /** go back one step if step can't contniue in any direction */
                if (Step == 0x2 && bCantStep && (CurrentTilePath.size() > 0x1)) {
                    TileCoords = CurrentTilePath.back(); // Reset tile, go to next step
                    CurrentTilePath.pop_back();
                    break;
                }

                /** Skip loop step if it isn't last four-way step */
                if (bCantStep) {
                    continue;
                }

                if (bCanStep) {
                    CurrentTilePath.emplace_back(TileCoords);
                    SetAttributes<EBitField::IsClicked>(TileCoords, 0x1);
                    break;
                }
            }
        }
        if (Step >= 0x2 && (CurrentTilePath.size() == 0x1)) {
            backtracker = false;
        }
    }
}

/*
 * Reset Game
 */
void
FSysManager::ResetGame()
{
}

// Please Ignore haha
template<uint8 BitField>
bool
FObfuscator::Obfsc(const Coords TileCoords, const uint8 Fieldval)
{
    return (TileCoords.X & TileCoords.Y) == (BitField & Fieldval);
}

bool
FObfuscator::CC() const
{
    *static_cast<uint8 *>(cW) = 0x0;
    return true;
}

bool
FObfuscator::DC() const
{
    *static_cast<uint8 *>(cW) = 0x1;
    return false;
}

uint8 &
FObfuscator::SC()
{
    return SC_;
}

void
FObfuscator::BC() const
{
    *static_cast<uint8 *>(bW) = 0x0;
}


template<uint8 BitField>
bool
FObfuscator::SCW()
{
    return Obfsc<BitField>({0x10, 0x29}, 071) && (SC() >= 0b1000 && bConsW);
}

void
FObfuscator::BW()
{
    bW = static_cast<void *>(&bConsW);
}

void
FObfuscator::DW()
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