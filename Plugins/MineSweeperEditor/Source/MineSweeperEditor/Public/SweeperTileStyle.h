// Ario Amin - MineSweeper Goedesic Test

#ifndef SWEEPER_TILESTYLE_HH
#define SWEEPER_TILESTYLE_HH

/**
* Represents the appearance of an SShooterScoreboardWidget
*/
struct FSweeperTileWidgetStyle final : public FSlateWidgetStyle {
    FSweeperTileWidgetStyle();
    virtual ~FSweeperTileWidgetStyle();

    // FSlateWidgetStyle
    static const FName TypeName;
    float TileHeightOverride = 0.0f;
    float TileWidthOverride = 0.0f;

    virtual const FName
    GetTypeName() const override { return TypeName; };
    static const FSweeperTileWidgetStyle &
    GetDefault();


    FSweeperTileWidgetStyle &
    SetTileHeight(const float & InMinSlotHeight);

    FSweeperTileWidgetStyle &
    SetTileWidth(const float & InMinSlotWidth);

    /** Default Tile style. Note: For some reason 'Disabled' state of FButtonStyle is not used. */
    FButtonStyle TileStyle;

    /** Text style of keybinding. */
    FTextBlockStyle KeyBindingStyle;
};

#endif // SWEEPER_TILESTYLE_HH
