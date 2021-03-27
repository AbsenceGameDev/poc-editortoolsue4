/**
* @file  MineSweeperEditor.cpp
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
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
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"


static const FName GMineSweeperEditorTabName("MineSweeperEditor");

#define LOCTEXT_NAMESPACE "MineSweeperEditorModule"

/*
 * Impl. of FSysManager::GetPrivateMemberRef
 * Implemented here as it is used in this file and it has auto return-type,
 *  this will make sure it is linked and compiles properly
 */
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
    if constexpr (PrivateMember == EPrivateMember::STextEndMsgRef) {
        return OptEndMsgRef;
    }
    if constexpr (PrivateMember == EPrivateMember::STextStatsRef) {
        return OptStatsRef;
    }
    if constexpr (PrivateMember == EPrivateMember::STextScoreRef) {
        return OptScoreRef;
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
    SysManager->UpdateScoreWidget();
    // FCurveHandle ZoomCurve = Sequence.AddCurve( 0, 0.15f );
    // FCurveHandle FadeCurve = Sequence.AddCurve( 0.15f, 0.1f);
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
    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->SC() = 0b0;
    PluginCmds->MapAction(
        FMineSweeperEditorCommands::Get().WindowContext,
        FExecuteAction::CreateRaw(this, &FMineSweeperEditorModule::TabBtnClicked),
        FCanExecuteAction()
        );

    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->BW();
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSweeperEditorModule::RegisterMenus));
    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->BC();
    SysManager->GetPrivateMemberRef<FSysManager::FObfsctr>()->CB();
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
    FTileBinder::NewGameBind(this, SysManager);
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
    FCoords TileCoords;
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
    FCoords TileCoords;
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
    TSharedRef<SGridPanel> MainField = SNew(SGridPanel);
    const FText            WelcomeText = LOCTEXT("MineSweeperPrompt0",
                                                 "Welcome to MineSweeper. \n Win 8 matches during one session to uncover a secret! \n"
                                                 "(Win/Loss is saved, but Secret Count resets when closing the editor. \nSecret count is only applicable on hard or above. \n"
                                                 "Please, no cheating by looking at source code. \nIn case you do, I have tried obfuscating it alot haha.)");
    MainField->AddSlot(0x0, 0x0)
    [
        SNew(STextBlock).Text(WelcomeText)
    ];

    // List difficulty options, bind selected/clicked item to 
    // fField->AddSlot(0x2, 0x0)
    // [
    //     SNew(SListView<TSharedPtr<FString>>)
    //         .ItemHeight(24)
    //         .ListItemsSource(&SysManager->DifficultyList)
    //     // .OnGenerateRow(SListView<TSharedPtr<FString>>::Widget)
    //     // .OnGenerateRow(SListView<TSharedPtr<FString>>::MakeOnGenerateWidget(this, &FMineSweeperEditorModule::OnGenerateRowForList))
    // ];

    // Create/Restart Game Button
    MainField->AddSlot(0x1, 0x0)
    [
        // Create New Game Button
        SNew(SWrapBox)
        + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
        [
            SNew(SButton).OnClicked_Static(&FTileBinder::NewGameBind, this, SysManager)
            [
                SNew(STextBlock)
                MAKETEXT("Create New Game")
                MAKEROBOTO(24)
            ]
        ]

        // Restart Game/Board Button 
        + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
        [
            SNew(SButton).OnClicked_Static(&FTileBinder::RestartGameBind, this, SysManager)
            [
                SNew(STextBlock)
                MAKETEXT("Restart this Board! (Once per board)")
                MAKEROBOTO(18)
                //.Text(FText::FromString("Restart this Board! (Once per board)"))
            ]
        ]
    ];

    // Grid/Row input
    MainField->AddSlot(0x1, 0x1)
    [
        SNew(SWrapBox)

        // Grid Row size numeric box 
        + SWrapBox::Slot().Padding(10).HAlign(HAlign_Center)
        [
            SNew(SWrapBox).PreferredWidth(300)
            + SWrapBox::Slot().Padding(2)
            [
                SNew(STextBlock)
                MAKETEXT("Row Size")
            ]
            + SWrapBox::Slot().Padding(2)
            [
                SNew(SNumericEntryBox<uint16>).Value_Raw(
                    SysManager.Get(),
                    &FSysManager::DisplayRowSize)
            ]

            // Row Slider
            + SWrapBox::Slot().Padding(5)
            [
                SNew(SBox)
                        .HeightOverride(20)
                        .WidthOverride(300)
                [
                    SNew(SSlider)
                            .StepSize(0x1)
                            .MinValue(0x5)
                            .MaxValue(SysManager->Gmax_Size)
                            .Orientation(EOrientation::Orient_Horizontal)
                            .Visibility(EVisibility::Visible)
                            .Locked(false)
                            .OnValueChanged_Raw(SysManager.Get(), &FSysManager::RowSizeCommitted)
                ]
            ]
        ]

        // Grid Column size numeric box
        + SWrapBox::Slot().Padding(10).HAlign(HAlign_Center)
        [
            SNew(SWrapBox).PreferredWidth(300)
            + SWrapBox::Slot().Padding(2)
            [
                SNew(STextBlock)
                MAKETEXT("Colum Size")
            ]

            + SWrapBox::Slot().Padding(2)
            [
                SNew(SNumericEntryBox<uint16>).Value_Raw(
                    SysManager.Get(),
                    &FSysManager::DisplayColSize)
            ]

            // Col Slider
            + SWrapBox::Slot().Padding(5)
            [
                SNew(SBox)
                        .HeightOverride(20)
                        .WidthOverride(300)
                [
                    SNew(SSlider)
                            .StepSize(0x1)
                            .MinValue(0x5)
                            .MaxValue(SysManager->Gmax_Size)
                            .Orientation(EOrientation::Orient_Horizontal)
                            .Visibility(EVisibility::Visible)
                            .Locked(false)
                            .OnValueChanged_Raw(SysManager.Get(), &FSysManager::ColSizeCommitted)
                ]
            ]
        ]

    ];

    // Win/Loss Message
    // replace .Text with .Text_Raw and bind it to a function which reflects changes on SContainer or R00Container
    MainField->AddSlot(0x0, 0x1)
    [
        *SysManager->GetPrivateMemberRef<FSysManager::STextEndMsgRef>() =
        SNew(STextBlock)
        .Text_Raw(SysManager.Get(), &FSysManager::DisplayEndMsg)
        MAKEROBOTO(38)
    ];

    // TotalScore
    // replace .Text with .Text_Raw and bind it to a function which reflects changes on SContainer or R00Container
    MainField->AddSlot(0x0, 0x1)
    [
        *SysManager->GetPrivateMemberRef<FSysManager::STextEndMsgRef>() =
        SNew(STextBlock)
        .Text_Raw(SysManager.Get(), &FSysManager::DisplayEndMsg)
        MAKEROBOTO(38)
    ];

    // Show GameData == Free tiles left to click
    // replace .Text with .Text_Raw and bind it to a function which reflects changes on SContainer or R00Container
    MainField->AddSlot(0x2, 0x0)
    [
        SNew(SWrapBox)
        + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
        [
            *SysManager->GetPrivateMemberRef<FSysManager::STextScoreRef>()
        ]

        + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
        [
            *SysManager->GetPrivateMemberRef<FSysManager::STextEndMsgRef>() =
            SNew(STextBlock)
            MAKETEXT("Free Tiles:")
            MAKEROBOTO(16)
        ]

        + SWrapBox::Slot().Padding(5).VAlign(VAlign_Center)
        [
            *SysManager->GetPrivateMemberRef<FSysManager::STextStatsRef>()
        ]
    ];

    // Game-board / Gridpanel
    MainField->AddSlot(0x2, 0x1).VAlign(VAlign_Fill)
    [
        // TSharedPtr<SUniformGridPanel>::
        *(SysManager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>())
        = GenerateGrid(SysManager->CurrRowSize, SysManager->CurrColSize)
    ];

    // Quick implementation of scrolling to be able to show a max-size playingboard,
    // does cause issues in order of priority,but it is not very problematic in this case
    return
        SNew(SDockTab).TabRole(ETabRole::NomadTab)
        [
            SNew(SScrollBox).Orientation(EOrientation::Orient_Horizontal)
            + SScrollBox::Slot()
            [
                SNew(SScrollBox).Orientation(EOrientation::Orient_Vertical)
                + SScrollBox::Slot()
                [
                    MainField
                ]
            ]
        ];
}

// We want to zoom in a widget, and then fade in its contents.
// EActiveTimerReturnType
// FMineSweeperEditorModule::TriggerTextAnim(double InCurrentTime, float InDeltaTime)
// {
// 
//     return EActiveTimerReturnType::Continue;
// }


/** Create New Game event */
FReply
FTileBinder::NewGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager)
{
    /** Reset game in SysManager, generate new slategrid, etc*/
    Manager->ResetGame();
    Manager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()->Get().ClearChildren();
    Owner->RegenerateGrid(Manager->CurrRowSize,
                          Manager->CurrColSize,
                          *(Manager->GetPrivateMemberRef<FSysManager::TOptGridWidgetRef>()));
    Manager->GetPrivateMemberRef<FSysManager::BoolPlayAgain>() = true;
    return FReply::Handled();
}

/** Restart Game event */
FReply
FTileBinder::RestartGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager)
{
    /** Restart last game in SysManager, generate new slategrid, etc*/
    if (!Manager->GetPrivateMemberRef<FSysManager::BoolPlayAgain>() || Manager->ClickedTiles == 0x0) {
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
FTileBinder::OnTileClick(FCoords TileCoords, TSharedPtr<FSysManager> ManagerShared)
{
    auto ObfsPtr = ManagerShared->GetPrivateMemberRef<FSysManager::FObfsctr>();
    ObfsPtr->DobfscObfsc(ManagerShared, ManagerShared->ClickTile(TileCoords.X, TileCoords.Y));
    ManagerShared->UpdateScoreWidget(); // Update just in-case win or loss has occured
    ObfsPtr->ObfscDobfsc(ManagerShared);
    return FReply::Handled();
}

/** Make Tile and bind OnClick to it */
TSharedRef<SWidget>
FTileBinder::MakeTile(const FCoords TileCoords,
                      TSharedPtr<FSysManager>
                      ManagerShared)
{
    TSharedPtr<uint8> TileDataPtr =
        MakeShared<uint8>(ManagerShared->GridData[TileCoords.Y][TileCoords.X]);

    TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
                                       MAKETEXT("_")
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
