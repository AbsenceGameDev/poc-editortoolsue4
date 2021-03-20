// Ario Amin - MineSweeper Goedesic Test

#ifndef TILE_PANEL_STYLE_HH
#define TILE_PANEL_STYLE_HH

#include "Slate/SlateGameResources.h"
#include "SweeperTileStyle.h"


/**
* Represents the appearance of an SShooterScoreboardWidget
*/
struct FTilePanelWidgetStyle : public FSlateWidgetStyle {
    FTilePanelWidgetStyle();
    virtual ~FTilePanelWidgetStyle();

    // FSlateWidgetStyle
    virtual void
    GetResources(TArray<const FSlateBrush *> & OutBrushes) const override;
    static const FName TypeName;

    virtual const FName
    GetTypeName() const override { return TypeName; };
    static const FTilePanelWidgetStyle &
    GetDefault();

    /** Background brush of this Tile Panel. */
    FSlateBrush BackgroundBrush;

    FTilePanelWidgetStyle &
    SetBackgroundBrush(const FSlateBrush & InItemBorderBrush)
    {
        BackgroundBrush = InItemBorderBrush;
        return *this;
    }

    /** General padding from Panel border to tiles list. */
    FMargin ContentPadding;

    FTilePanelWidgetStyle &
    SetContentPadding(const FMargin & InContentPadding)
    {
        ContentPadding = InContentPadding;
        return *this;
    }

    /** Padding between tiles. */
    FMargin SlotPadding;

    FTilePanelWidgetStyle &
    SetSlotPadding(const FMargin & InSlotPadding)
    {
        SlotPadding = InSlotPadding;
        return *this;
    }

    /** Minimal height of one slot. */
    float MinSlotHeight;

    FTilePanelWidgetStyle &
    SetMinSlotHeight(const float & InMinSlotHeight)
    {
        MinSlotHeight = InMinSlotHeight;
        return *this;
    }

    /** Minimal width of one slot. */
    float MinSlotWidth;

    FTilePanelWidgetStyle &
    SetMinSlotWidth(const float & InMinSlotWidth)
    {
        MinSlotWidth = InMinSlotWidth;
        return *this;
    }

    /** Border brush of the tile. Use it if you want all tiles to have same border. */
    FSlateBrush TileBorderBrush;

    FTilePanelWidgetStyle &
    SetTileBorderBrush(const FSlateBrush & InTileBorderBrush)
    {
        TileBorderBrush = InTileBorderBrush;
        return *this;
    }

    /** Tiles styles that will be added through code. If left empty, tiles will use style defined by TileStyle below.
    * Example: if you will add 5 Tiles (tile_1, tile_2, ...) to this panel,
    * Styles from this array will be applied to corresponding Tiles like this:
    * tile_1.Style = SweeperTileStyles[0];
    * tile_2.Style = SweeperTileStyles[1];
    * tile_3.Style = SweeperTileStyles[2];
    * And so on... */
    TArray<USlateWidgetStyleAsset *> SweeperTileStyles;

    /** Default Tile style that will be used if no style was found in SweeperTileStyles. */
    FSweeperTileWidgetStyle TileStyle;

    /** Content padding of this tile. Used only if content is set (when creating tile from command info). */
    FMargin TileContentPadding;
};

#endif // TILE_PANEL_STYLE_HH
