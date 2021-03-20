#include "SweeperTileStyle.h"
#include "GUIStyle.h"

FSweeperTileWidgetStyle::FSweeperTileWidgetStyle()
{
    KeyBindingStyle.SetFont(TEXT("../../../Engine/Content/Slate/Fonts/Roboto-Regular.ttf"), 10);
    KeyBindingStyle.SetColorAndOpacity(FLinearColor::White);
}

FSweeperTileWidgetStyle::~FSweeperTileWidgetStyle()
{
}

const FName FSweeperTileWidgetStyle::TypeName(TEXT("FSweeperTileWidgetStyle"));

const FSweeperTileWidgetStyle &
FSweeperTileWidgetStyle::GetDefault()
{
    static FSweeperTileWidgetStyle Default;
    return Default;
}

FSweeperTileWidgetStyle &
FSweeperTileWidgetStyle::SetTileHeight(const float & InMinSlotHeight);
{
    TileHeightOverride = InMinSlotHeight;
    return *this;
}

FSweeperTileWidgetStyle &
FSweeperTileWidgetStyle::SetTileWidth(const float & InMinSlotWidth);
{
    TileWidthOverride = InMinSlotWidth;
    return *this;
}