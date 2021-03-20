// Ario Amin - MineSweeper Goedesic Test

#ifndef TILE_PANEL_HH
#define TILE_PANEL_HH

#include "Slate.h"
#include "TilePanelStyle.h"
#include "SweeperTile.h"
#include "GUIStyle.h"

/** STilePanel is a container that holds SSweeperTiles. Use one of the AddTile function to do it.
*	Default style should be placed under Content/UI and named TilePanel_Default.*/
class STilePanel : public SCompoundWidget {
public:
SLATE_BEGIN_ARGS(STilePanel)
            : _TilePanelStyle(&FGUIStyle::Get().GetWidgetStyle<FTilePanelWidgetStyle>("TilePanel_Default")),
              _Orientation(EOrientation::Orient_Horizontal),
              _TileHeight(50),
              _TileWidth(50),
              _TilesPadding(FMargin(4))
        {
        }

        /* Style of this widget. */
        SLATE_STYLE_ARGUMENT(FTilePanelWidgetStyle, TilePanelStyle)
        /* List needed to execute commands mapped to Tiles  */
        SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandList)
        /* Orientation of this panel. Default: Horizontal */
        SLATE_ARGUMENT(EOrientation, Orientation)
        /* Minimum Tile height. Default: 50 */
        SLATE_ARGUMENT(float, TileHeight)
        /* Minimum Tile width. Default: 50 */
        SLATE_ARGUMENT(float, TileWidth)
        /* Tiles Padding. Default: 4 */
        SLATE_ARGUMENT(FMargin, TilesPadding)
    SLATE_END_ARGS()

    // Move into real constructor
    void
    Construct(const FArguments & InArgs);

    /**
     *	@brief Add Tile to panel.
     *	@param InCommand Command that should be executed when Tile is pressed.
     *	@note Tile will display SlateIcon bound to InCommand on top of everything else.
     *	Also, if there is InputGesture bound to command it will be used as Hotkey text.
     **/
    void
    AddSweeperTile(TSharedRef<FUICommandInfo> InCommand);

    /**
     *	@brief Add Tile to panel.
     *	@param InCommand Command that should be executed when Tile is pressed.
     *	@param InTileStyle Style that should be applied to this Tile.
     *	NOTE: Tile will display SlateIcon bound to InCommand on top of everything else.
     *	Also, if there is InputGesture bound to command it will be used as Hotkey text.
     **/
    void
    AddSweeperTile(TSharedRef<FUICommandInfo> InCommand, const FSweeperTileWidgetStyle * InTileStyle);

    /**
     *	@brief Add Tile to panel.
     *	@param OnTileClick Simple delegate that will be executed when Tile is pressed.
     **/
    void
    AddSweeperTile(FSimpleDelegate OnTileClick);

    /**
     *	@brief Add Tile to panel.
     *	@param OnTileClick Simple delegate that will be executed when Tile is pressed.
     *	@param KeyText Text that should be displayed as "Hotkey" under the Tile.
     **/
    void
    AddSweeperTile(FSimpleDelegate OnTileClick, FString KeyText);

    /**
     *	@brief Add tile to panel.
     *	@param InAction Action that will be executed when Tile is pressed. Tile will also use 'CanExecute' to determine if it should be disabled or not.
     *	@param KeyText Text that should be displayed as "Hotkey" under the Tile.
     **/
    void
    AddSweeperTile(FUIAction InAction, FString KeyText = FString(""));

    /**
     *	@brief Add tile to panel.
     *	@param OnTileClick Simple delegate that will be executed when tile is pressed.
     *	@param InTileStyle Style that should be applied to this tile.
     **/
    void
    AddSweeperTile(FSimpleDelegate OnTileClick, const FSweeperTileWidgetStyle * InTileStyle);

protected:

    /**
     *	@brief Add tile widget to display list.
     *	@param InTile Tile to add.
     **/
    void
    AddTile(TSharedRef<SSweeperTile> InTile);

    /**
     *	@brief Get tile style.
     *	@return Return value from Styles array or default TileStyle.
     **/
    const FSweeperTileWidgetStyle *
    GetTileStyle() const;

    /** @brief Container for tiles. */
    TSharedPtr<SUniformGridPanel> TileContainer;

    /** @brief Tiles counter. */
    int32 SlotCounter = 0x0;

    /** @brief Array of active tiles. */
    TArray<TSharedPtr<SSweeperTile>> ActiveTiles;

    /** @brief List needed to execute commands mapped to tiles. */
    TSharedPtr<FUICommandList> CommandList;

    /** @brief Orientation. Horizontal by default. */
    EOrientation Orientation = EOrientation::Orient_Horizontal;

    /**
     * @brief Default tile style used for tiles added to this panel.
     * Note that it wont be used if there are styles defined in Style->SweeperTileStyles.
     **/
    const FSweeperTileWidgetStyle * TileStyle;

    /** @brief Style of this panel. */
    const FTilePanelWidgetStyle * SelfStyle;
};

#endif // TILE_PANEL_HH
