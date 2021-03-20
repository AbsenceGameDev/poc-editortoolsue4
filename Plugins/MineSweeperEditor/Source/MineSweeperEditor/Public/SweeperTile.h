// Ario Amin - MineSweeper Goedesic Test

#ifndef SWEEPER_TILE_HH
#define SWEEPER_TILE_HH

#include "SweeperTileStyle.h"
#include "Slate.h"
#include "GUIStyle.h"

/**
 * @brief SSweeperTile is a button that most likely will be put inside SSweeperPanel.
 * Default style should be placed under Content/UI and named SweeperTile_Default.
 **/
class SSweeperTile : public SButton {
public:
SLATE_BEGIN_ARGS(SSweeperTile)
            : _SweeperTileStyle(
                &FGUIStyle::Get().GetWidgetStyle<FSweeperTileWidgetStyle>("SweeperTile_Default"))
        {
        }

        /** @brief Style for this tile button. */
        SLATE_STYLE_ARGUMENT(FSweeperTileWidgetStyle, SweeperTileStyle)

        /** @brief Command list is required to execute mapped commands. */
        SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandList)

        /** @brief Command that will be executed if set. */
        SLATE_ARGUMENT(TSharedPtr<FUICommandInfo>, CommandInfo)

        /**
         * @brief Action used to execute bound action and determine if tile
         * should be disabled (using TileAction.CanExecute).
         **/
        SLATE_ARGUMENT(FUIAction, TileAction)

        /** @brief Simple delegate that will be executed on click. */
        SLATE_EVENT(FSimpleDelegate, OnTileClick)
    SLATE_END_ARGS()

    /** Move logic into real constructor */
    void
    Construct(const FArguments & InArgs);

protected:
    /**
     * @brief Use TileAction to see if tile is clicked.
     * @return true/false 
     */
    bool
    IsTileClicked() const;

    /**
     * @brief Does the magic.
     * @return FReply::Handled
     */
    FReply
    OnClick() const;

    /** @brief Command list is required to execute mapped commands. */
    TSharedPtr<FUICommandList> CommandList;
    /** @brief Command that will be executed on click if set. */
    TSharedPtr<FUICommandInfo> CommandInfo;
    /** @brief Tile action. */
    FUIAction TileAction;
    /** @brief Simple callback for tile click. */
    FSimpleDelegate OnTileClick;
};
#endif // SWEEPER_TILE_HH
