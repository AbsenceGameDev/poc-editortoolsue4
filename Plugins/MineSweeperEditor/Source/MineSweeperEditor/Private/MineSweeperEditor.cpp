/** Ario Amin - MineSweeper Geodesic Test */

#include "MineSweeperEditor.h"
#include "EditorReimportHandler.h"
#include "MineSweeperEditorStyle.h"
#include "MineSweeperEditorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Views/SListView.h"
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

template<FSysManager::EPrivateMember PrivateMember>
auto
FSysManager::GetPrivateMemberRef() -> auto&
{
    if constexpr (PrivateMember == EPrivateMember::BoolPlayAgain) {
        return bPlaySameAgain;
    }
    if constexpr (PrivateMember == EPrivateMember::VectorSlateGrid) {
        return SlateGrid;
    }
    if constexpr (PrivateMember == EPrivateMember::VectorTileDisplayGrid) {
        return TileDisplayGrid;
    }
    if constexpr (PrivateMember == EPrivateMember::TOptGridWidgetRef) {
        return OptGridWidgetRef;
    }
    if constexpr (PrivateMember == EPrivateMember::FObfsctr) {
        return Obfsctr;
    }
}

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
    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->DW();
    PluginCmds->MapAction(
        FMineSweeperEditorCommands::Get().WindowContext,
        FExecuteAction::CreateRaw(this, &FMineSweeperEditorModule::TabBtnClicked),
        FCanExecuteAction()
        );

    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->BW();
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSweeperEditorModule::RegisterMenus));
    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->BC();
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
    FTileBinder::ResetGameBind(this, SysManager);
}


/*
 * Set Value
 */
void
FMineSweeperEditorModule::CommittedX(const uint8       NewInt,
                                     ETextCommit::Type CommitType) const
{
    SysManager->CurrRowSize =
        // Bounds checking with default size of 0x5
        (NewInt * (NewInt > 0x0 && NewInt < 0x40)) | (0x5 * (NewInt < 0x1 || NewInt > 0x3f));
}

/*
 * Set Value
 */
void
FMineSweeperEditorModule::CommittedY(const uint8       NewInt,
                                     ETextCommit::Type CommitType) const
{
    SysManager->CurrColSize =
        // Bounds checking with default size of 0x5
        (NewInt * (NewInt > 0x0 && NewInt < 0x40)) | (0x5 * (NewInt < 0x1 || NewInt > 0x3f));
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
    /** Actual function body in GenerateGrid */
    TSharedRef<SUniformGridPanel> IdxField = SNew(SUniformGridPanel);
    if (XIn < 0x1 || YIn < 0x1 || XIn > 0x40 || YIn > 0x40) {
        return IdxField; // Return empty grid, should make as an assertion, but this works temporarily
    }
    uint16 Row = 0, Col = 0;
    Coords TileCoords;
    for (Row = 0; Row < YIn;) {
        TileCoords.Y = Row;
        for (Col = 0; Col < XIn;) {
            TileCoords.X = Col;
            IdxField->AddSlot(Col, Row)[FTileBinder::MakeTile(TileCoords, SysManager)];
            Col++;
        }
        Row++;
    }
    return IdxField;
}

/*
 * Regenerate Slate Grid 
 */
void
FMineSweeperEditorModule::RegenerateGrid(uint8 XIn, uint8 YIn, TSharedRef<SUniformGridPanel> IdxField) const
{
    /** Actual function body in GenerateGrid */
    uint16 Row = 0, Col = 0;
    Coords TileCoords;
    for (Row = 0; Row < YIn;) {
        TileCoords.Y = Row;
        for (Col = 0; Col < XIn;) {
            TileCoords.X = Col;
            IdxField->AddSlot(Col, Row)[FTileBinder::MakeTile(TileCoords, SysManager)];
            Col++;
        }
        Row++;
    }
}

/*
 * Call when spawning window to spawn internal tab/page.
 */
TSharedRef<SDockTab>
FMineSweeperEditorModule::OnSpawnTab(const FSpawnTabArgs & SpawnTabArgs) const
{
    // TSharedRef<SButton> TempRestartBtn = RestartGameBtn;
    // TSharedRef<SButton> TempCreateBtn = CreateGameBtn;

    const FText WelcomeTextl0 = LOCTEXT("MineSweeperPrompt0",
                                        "Welcome to MineSweeper. Win 8 matches during one session to uncover a secret!");
    const FText WelcomeTextl1 = LOCTEXT("MineSweeperPrompt1",
                                        "(Win/Loss is saved, but Secret Count resets when closing the editor. Secret count is only applicable on hard or above)");
    const FText WelcomeTextl2 = LOCTEXT("MineSweeperPromp2",
                                        "( Please, no cheating by looking at source code. In case you do, I have tried obfuscating it alot haha.)");
    return SNew(SDockTab).TabRole(ETabRole::NomadTab)
           [
               SNew(SWrapBox).PreferredWidth(300.f)
               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(STextBlock).Text(WelcomeTextl0)
               ]

               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(STextBlock).Text(WelcomeTextl1)
               ]

               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(STextBlock).Text(WelcomeTextl2)
               ] + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   // List difficulty options, bind selected/clicked item to 
                   SNew(SListView<TSharedPtr<FString>>)
                       .ItemHeight(24)
                       .ListItemsSource(&SysManager->DifficultyList)
                   // .OnGenerateRow(SListView<TSharedPtr<FString>>::Widget)
                   // .OnGenerateRow(SListView<TSharedPtr<FString>>::MakeOnGenerateWidget(this, &FMineSweeperEditorModule::OnGenerateRowForList))
               ]

               // Create New Game/Board Button 
               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(SWrapBox)
                   + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
                   [
                       SNew(SButton).OnClicked_Static(&FTileBinder::ResetGameBind, this, SysManager)
                       [
                           SNew(STextBlock).Text(FText::FromString("Create New Game")).Font(
                               FSlateFontInfo(
                                   FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),
                                   24))
                       ]
                   ]

                   // Restart Game/Board Button 
                   + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
                   [
                       SNew(SButton).OnClicked_Static(&FTileBinder::RestartGameBind, this, SysManager)
                       [
                           SNew(STextBlock).Text(FText::FromString("Restart this Board! (Once per board)"))
                                           .Font(FSlateFontInfo(FPaths::EngineContentDir()
                                                                / TEXT("Slate/Fonts/Roboto-Bold.ttf"),
                                                                18))
                       ]
                   ]
               ]

               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   SNew(SWrapBox)

                   // Grid Row size numeric box 
                   + SWrapBox::Slot().Padding(5).HAlign(HAlign_Left)[
                       SNew(SNumericEntryBox<uint8>).Value(SysManager->GetRowSize()).OnValueCommitted_Raw(
                           this,
                           &FMineSweeperEditorModule::CommittedX)
                   ]

                   // Grid Column size numeric box
                   + SWrapBox::Slot().Padding(5).HAlign(HAlign_Right)[
                       SNew(SNumericEntryBox<uint8>).Value(SysManager->GetColSize()).OnValueCommitted_Raw(
                           this,
                           &FMineSweeperEditorModule::CommittedY)
                   ]
               ]
               + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)[
                   // TSharedPtr<SUniformGridPanel>::
                   *(SysManager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()) = GenerateGrid(
                       SysManager->CurrRowSize,
                       SysManager->CurrColSize)
               ]
           ];
}

// FRegenLocal

/** On Tile Click event */
FReply
FTileBinder::ResetGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager)
{
    /** Reset gamein SysManager, generate new slategrid, etc*/
    Manager->ResetGame();
    Manager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()->Get().ClearChildren();
    Owner->RegenerateGrid(Manager->CurrRowSize,
                          Manager->CurrColSize,
                          *(Manager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()));
    Manager->GetPrivateMemberRef<FSysManager::BoolPlayAgain>() = true;
    return FReply::Handled();
}

/** On Tile Click event */
FReply
FTileBinder::RestartGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager)
{
    /** Reset gamein SysManager, generate new slategrid, etc*/
    if (!Manager->GetPrivateMemberRef<FSysManager::BoolPlayAgain>()) {
        return FReply::Unhandled();
    }
    Manager->RestartGame();
    Manager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()->Get().ClearChildren();
    Owner->RegenerateGrid(Manager->CurrRowSize,
                          Manager->CurrColSize,
                          *(Manager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()));
    Manager->GetPrivateMemberRef<FSysManager::BoolPlayAgain>() = false;
    return FReply::Handled();
}

/** On Tile Click event */
FReply
FTileBinder::OnTileClick(Coords TileCoords, TSharedPtr<FSysManager> ManagerShared)
{
    auto ObfsPtr = ManagerShared->GetPrivateMemberRef<FSysManager::FObfsctr>();
    ObfsPtr->DobfscObfsc(ManagerShared, ManagerShared->ClickTile(TileCoords.X, TileCoords.Y));
    ObfsPtr->ObfscDobfsc(ManagerShared);

    return FReply::Handled();
}

/** Make Tile and bind OnClick to it */
TSharedRef<SWidget>
FTileBinder::MakeTile(const Coords TileCoords,
                      TSharedPtr<FSysManager>
                      ManagerShared)
{
    TSharedPtr<uint8> TileDataPtr =
        MakeShared<uint8>(ManagerShared->GridData[TileCoords.Y][TileCoords.X]);

    TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
                                           .Text(FText::FromString("  "))
                                           .ColorAndOpacity(FLinearColor(0, 0, 0, 1));
    TSharedRef<SButton> Btn =
        SNew(SButton)
            .OnClicked_Static(&FTileBinder::OnTileClick, TileCoords, ManagerShared)
            .ForegroundColor(FSlateColor::UseForeground())
        [
            SNew(SBorder).BorderImage(ManagerShared->BombBrush.Get())
            [
                TextBlock
            ]
        ];

    ManagerShared->GetPrivateMemberRef<FSysManager::VectorSlateGrid>().emplace_back(Btn);
    ManagerShared->GetPrivateMemberRef<FSysManager::VectorTileDisplayGrid>().emplace_back(TextBlock);
    return Btn;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMineSweeperEditorModule, MineSweeperEditor)


//			] + SWrapBox::Slot().Padding(30).HAlign(HAlign_Left)[
//				SNew(SEditableTextBox)
//					.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FMineSweeperEditorModule::CommittedX))
//			] + SWrapBox::Slot().Padding(30).HAlign(HAlign_Right)[
//				SNew(SEditableTextBox)
//					.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FMineSweeperEditorModule::CommittedX))
