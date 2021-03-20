#include "TilePanel.h"

DEFINE_LOG_CATEGORY_STATIC(LogTilePanel, Log, All);

void
STilePanel::Construct(const FArguments & InArgs)
{
    if (!InArgs._TilePanelStyle) {
        SetVisibility(EVisibility::Collapsed);
        UE_LOG(LogTilePanel, Warning, TEXT("No style detected for STilePanel!"));
        return;
    }
    SlotCounter = 0;
    CommandList = InArgs._CommandList;
    Orientation = InArgs._Orientation;
    SelfStyle = InArgs._TilePanelStyle;
    TileStyle = &SelfStyle->TileStyle;

    // Note that tiles will be added to SUniformGridPanel.
    ChildSlot
    [
        SNew(SBorder)
			.BorderImage(&SelfStyle->BackgroundBrush)
			.Padding(SelfStyle->ContentPadding)
			.HAlign(HAlign_Center)
        [
            SAssignNew(TileContainer, SUniformGridPanel)
				.MinDesiredSlotHeight(SelfStyle->MinSlotHeight)
				.MinDesiredSlotWidth(SelfStyle->MinSlotWidth)
				.SlotPadding(SelfStyle->SlotPadding)
        ]
    ];
}

void
STilePanel::AddSweeperTile(TSharedRef<FUICommandInfo> InCommand)
{
    AddTile(
        SNew(SSweeperTile)
		.CommandList(CommandList)
		.CommandInfo(InCommand)
		.SweeperTileStyle(GetTileStyle())
        );
}

void
STilePanel::AddSweeperTile(TSharedRef<FUICommandInfo>      InCommand,
                           const FSweeperTileWidgetStyle * InTileStyle)
{
    AddTile(
        SNew(SSweeperTile)
		.CommandList(CommandList)
		.CommandInfo(InCommand)
		.SweeperTileStyle(InTileStyle)
        );
}

void
STilePanel::AddSweeperTile(FSimpleDelegate OnTileClick)
{
    AddTile(
        SNew(SSweeperTile)
		.OnTileClick(OnTileClick)
		.SweeperTileStyle(GetTileStyle())
        );
}

void
STilePanel::AddSweeperTile(FSimpleDelegate OnTileClick, FString KeyText)
{
    AddTile(
        SNew(SSweeperTile)
		.OnTileClick(OnTileClick)
		.SweeperTileStyle(GetTileStyle())
        );
}

void
STilePanel::AddSweeperTile(FUIAction InAction, FString KeyText)
{
    AddTile(
        SNew(SSweeperTile)
		.TileAction(InAction)
		.SweeperTileStyle(GetTileStyle())
        );
}

void
STilePanel::AddSweeperTile(FSimpleDelegate OnTileClick, const FSweeperTileWidgetStyle * InTileStyle)
{
    AddTile(
        SNew(SSweeperTile)
		.SweeperTileStyle(InTileStyle)
		.OnTileClick(OnTileClick)
        );
}

void
STilePanel::AddTile(TSharedRef<SSweeperTile> InTile)
{
    if (TileContainer.IsValid()) {
        ActiveTiles.Add(
            InTile
            );

        // Wrap Tile into customizable border
        const TSharedRef<SWidget> NewWidget = SNew(SBorder)
			.Padding(SelfStyle->TileContentPadding)
			.BorderImage(&SelfStyle->TileBorderBrush)
                                              [
                                                  ActiveTiles.Last().ToSharedRef()
                                              ];

        if (Orientation == EOrientation::Orient_Horizontal) {
            TileContainer->AddSlot(SlotCounter, 0)
            [
                NewWidget
            ];
        } else {
            TileContainer->AddSlot(0, SlotCounter)
            [
                NewWidget
            ];
        }

        SlotCounter++;
    }
}

const FSweeperTileWidgetStyle *
STilePanel::GetTileStyle() const
{
    if (SelfStyle && SelfStyle->SweeperTileStyles.Num() > SlotCounter && SlotCounter >= 0) {
        return SelfStyle->SweeperTileStyles[SlotCounter]->GetStyle<FSweeperTileWidgetStyle>();
    }

    return TileStyle;
}
