/**
* @file  MineSweeperEditor.h
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#ifndef MINESWEEPER_EDITOR_H
#define MINESWEEPER_EDITOR_H

#include "CoreMinimal.h"
#include "FSysManager.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "Modules/ModuleManager.h"

/**
 * @class FMineSweeperEditorModule
 * @brief  Minesweeper Editor-module
 * @details This is the editor module which is responsible for
 * initializing slate elements. (That regards the related editor window and it's elements)
 * 
 **/
class FMineSweeperEditorModule : public IModuleInterface {
public:

    /**
     * @name SysManager
     * @typedef TSharedPtr<FSysManager>
     * @brief Public member variables
     * @details A shared pointer to an FSysManager,
     * which manages much of the game and the system functionality. \n
     * In contrast to FMineSweeperEditorModule,
     * which mainly is used for initializing the visual elements,
     * as-well as binding them through FTileBinder.
     **/
    TSharedPtr<FSysManager> SysManager;

    /**
     * @brief Public member functions
     * 
     **/

    /**
     * @brief FMineSweeperEditorModule constructor
     * @details Creates a FSysManager and loads total saved score 
     **/
    FMineSweeperEditorModule();

    /**
    * @brief StatupModule - IModuleInterface override
    * @details Initializes the style-set and command,
    * then registers callback and the new tab in the editor menu.
    **/
    virtual void
    StartupModule() override;

    /**
    * @brief Shutdown - IModuleInterface override
    * @details De-initializes the style-set and command,
    * as-well as triggers SaveState in FSysManager, then de-registers
    * callback and the new tab in the editor menu.
    **/
    virtual void
    ShutdownModule() override;

    /**
     * @brief Brings up main plugin window
     * @details Tries to invoke tab through FGlobalTabmanager::
     * and then Triggers the New Game event
     * @note This function will be bound to Command
     **/
    void
    TabBtnClicked() const;

    /**
     * @brief Commit value from slider
     * @details Commits value to this->SysManager->NextRowSize,
     * is bound to an SSlider which is defined in OnSpawnTab
     * @param NewInt New display value
     * @param CommitType
     * @note set value when keyboard input
     **/
    void
    CommittedX(const uint8 NewInt, ETextCommit::Type CommitType) const;

    /**
     * @brief Commit text from text-box 1 to value
     * @param NewInt New display value
     * @param CommitType
     * @note set value when keyboard input
     **/
    void
    CommittedY(const uint8 NewInt, ETextCommit::Type CommitType) const;

    /**
     * @brief Regenerates an already existing Slate Grid 
     * @param Xin Maximum size X
     * @param Yin Maximum size Y
     * @note FSLocal - Local container to create and bind function \n
     * FSLocal::OnTileClick(const Coords, TSharedPtr<FSysManager>) \n
     * FSLocal::MakeTile(const Coords, TSharedPtr<FSysManager>)
     * @return Shared reference of Grid panel, type: TSharedRef<SUniformGridPanel>
     **/
    void
    RegenerateGrid(uint8 Xin, uint8 Yin, TSharedRef<class SUniformGridPanel>) const;
private:

    /**
     * @brief Private member variables
     * 
     **/
    TSharedPtr<class FUICommandList> PluginCmds;
    FCurveSequence                   Sequence;
    FCurveHandle                     ZoomCurve;
    FCurveHandle                     FadeCurve;

    /**
     * @brief Private member functions
     * 
     **/

    /**
     * @brief Register level editor menu
     **/
    void
    RegisterMenus();

    /**
     * @brief Generate Slate Grid 
     * @param Xin Maximum size X
     * @param Yin Maximum size Y 
     * @note FSLocal - Local container to create and bind function \n
     * FSLocal::OnTileClick(const Coords, TSharedPtr<FSysManager>) \n
     * FSLocal::MakeTile(const Coords, TSharedPtr<FSysManager>)
     * @return Shared reference of Grid panel, type: TSharedRef<SUniformGridPanel>
     **/
    TSharedRef<class SUniformGridPanel>
    GenerateGrid(uint8 Xin, uint8 Yin) const;


    /**
     * @brief Call when spawning window to spawn internal tab/page.
     * @details Implements the initial SUniFormGrid, the New Game & Restart Game SAButton, 
     * the SSlider's, the welcome message and the win/loss SPopup 
     * @param SpawnTabArgs
     * @return The tab-page in the module window, through type: TSharedRef<class SDockTab>  
     **/
    TSharedRef<class SDockTab>
    OnSpawnTab(const class FSpawnTabArgs & SpawnTabArgs) const;

    // EActiveTimerReturnType TriggerTextAnim(double InCurrentTime, float InDeltaTime);
}; /** End of FMineSweeperEditorModule class */

/**
 * @struct FTileBinder
 * @brief  Function binder
 * @details Implements functions which are then used to bind to delegates in FMineSweeperEditorModule
 **/
struct FTileBinder {
    /**
     * @brief New Game event
     * @details Starts a new game, uses the slider values to generate new board-dimensions. 
     * @param Owner 
     * @param Manager 
     * @retun returns an FReply::Handled() when finished.
     **/
    static FReply
    NewGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager);

    /**
     * @brief Restart Board event
     * @details Restart the current game. can one be used once per board you're playing. 
     * @param Owner 
     * @param Manager 
     * @retun returns an FReply::Handled() when finished.
     **/
    static FReply
    RestartGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager);

    /**
    * @brief OnTileClick, function to bind to game-board tiles
    * @details
    * @param Difficulty Requested Difficulty Setting
    * @param ManagerShared An FSysManager to trigger it's ClickTile() function
    * @retun returns an FReply::Handled() when finished.
    */
    static FReply
    OnDifficultyClick(FSysManager::EGameDifficulty Difficulty,
                      TSharedPtr<FSysManager>      ManagerShared);

    /**
    * @brief OnTileClick, function to bind to game-board tiles
    * @details
    * @param TileCoords Coordinates of the tile which has been clicked
    * @param ManagerShared An FSysManager to trigger it's ClickTile() function
    * @retun returns an FReply::Handled() when finished.
    */
    static FReply
    OnTileClick(FCoords                 TileCoords,
                TSharedPtr<FSysManager> ManagerShared);

    /**
     * @brief Make Tile and bind OnClick to it
     * @details
     * @param TileCoords Given coordinates at which I tile will be created 
     * @param ManagerShared FSysManager pointer to pass through to internal function call
     * @return TSharedRef<SWidget>, a shared reference to the tile (SAButton widget)
     */
    static TSharedRef<SWidget>
    MakeTile(const FCoords TileCoords,
             TSharedPtr<FSysManager>
             ManagerShared);
};

#endif // MINESWEEPER_EDITOR_H
