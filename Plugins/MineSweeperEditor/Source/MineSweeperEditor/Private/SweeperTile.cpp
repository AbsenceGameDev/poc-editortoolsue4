#include "SweeperTile.h"
#include "GUIStyle.h"
#include "SweeperTileStyle.h"

DEFINE_LOG_CATEGORY_STATIC(LogSweeperTile, Log, All);

// Move into real constructor/initializer, similar how I do it in MineSweeperEditor
void
SSweeperTile::Construct(const FArguments & InArgs)
{
    if (!InArgs._SweeperTileStyle) {
        SetVisibility(EVisibility::Collapsed);
        UE_LOG(LogSweeperTile, Warning, TEXT("No style deteced for SSweeperTile!"));
        return;
    }

    CommandList = InArgs._CommandList;
    CommandInfo = InArgs._CommandInfo;
    OnTileClick = InArgs._OnTileClick;
    TileAction = InArgs._TileAction;

    FSweeperTileWidgetStyle * SelfStyle = new FSweeperTileWidgetStyle(*InArgs._SweeperTileStyle);

    TSharedPtr<SWidget> ImageWidget = SNullWidget::NullWidget;

    if (CommandInfo.IsValid() && CommandInfo->GetIcon().IsSet()) {
        const FSlateBrush * IconBrush = CommandInfo->GetIcon().GetIcon();
        if (IconBrush->GetResourceName() != NAME_None) {
            ImageWidget = SNew(SImage)
                          .Image(IconBrush);
        }
    }

    // TODO: move to seperate func.
    TSharedRef<SOverlay> ButtonOverlay =
        SNew(SOverlay)
        + SOverlay::Slot()
        [
            ImageWidget.ToSharedRef()
        ];

    // TODO: Handle this more smoothly somehow
    TSharedPtr<SWidget> Box = SNew(SButton)
		.ButtonStyle(&SelfStyle->TileStyle)
		.OnClicked(this, &SSweeperTile::OnClick)
		.IsEnabled(this, &SSweeperTile::IsTileClicked)
		.ContentPadding(0)
		.Content()
                              [
                                  ButtonOverlay
                              ];

    if (SelfStyle->TileWidthOverride > 0 || SelfStyle->TileHeightOverride > 0) {
        Box = SNew(SBox)
			.HeightOverride(SelfStyle->TileHeightOverride)
			.WidthOverride(SelfStyle->TileWidthOverride)
              [
                  SNew(SButton)
				.ButtonStyle(&SelfStyle->TileStyle)
				.OnClicked(this, &SSweeperTile::OnClick)
				.IsEnabled(this, &SSweeperTile::IsTileClicked)
				.ContentPadding(0)
				.Content()
                  [
                      ButtonOverlay
                  ]
              ];
    }
}

bool
SSweeperTile::IsTileClicked() const
{
    if (TileAction.IsBound()) {
        return TileAction.CanExecute();
    }

    return true;
}

FReply
SSweeperTile::OnClick() const
{
    if (CommandInfo.IsValid() && CommandList.IsValid()) {
        CommandList->TryExecuteAction(CommandInfo.ToSharedRef());
    } else if (TileAction.IsBound() && TileAction.CanExecute()) {
        TileAction.Execute();
    } else {
        OnTileClick.ExecuteIfBound();
    }

    return FReply::Handled();
}
