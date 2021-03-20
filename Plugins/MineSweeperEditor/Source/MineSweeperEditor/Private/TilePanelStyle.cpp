#include "TilePanelStyle.h"

FTilePanelWidgetStyle::FTilePanelWidgetStyle()
{
    MinSlotHeight = 32;
    MinSlotWidth = 32;
    SlotPadding = 2;
}

FTilePanelWidgetStyle::~FTilePanelWidgetStyle()
{
}

const FName FTilePanelWidgetStyle::TypeName(TEXT("FTilePanelWidgetStyle"));

const FTilePanelWidgetStyle &
FTilePanelWidgetStyle::GetDefault()
{
    static FTilePanelWidgetStyle Default;
    return Default;
}

void
FTilePanelWidgetStyle::GetResources(TArray<const FSlateBrush *> & OutBrushes) const
{
    OutBrushes.Add(&BackgroundBrush);
    OutBrushes.Add(&TileBorderBrush);
}
