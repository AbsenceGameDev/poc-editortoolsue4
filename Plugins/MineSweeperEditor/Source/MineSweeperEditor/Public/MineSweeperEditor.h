// Ario Amin - MineSweeper Goedesic Test

/**
 * @brief MineSweeper module
 *
 * At the end of minesweeper class, below it,
 * I have included a minified and optimized version of cppcodec library (https://github.com/tplgy/cppcodec),
 * only kept the decoder. I know I should maybe have not tacked it on at the end of this header,
 * and maybe should've kept a separate header for it, but here we are now haha
 * 
 **/
#ifndef MINESWEEPER_EDITOR_H
#define MINESWEEPER_EDITOR_H

#include "CoreMinimal.h"
#include "FSysManager.h"
#include "Modules/ModuleManager.h"
#include <array>
#include <vector>

/**
* @brief  Game module
*
**/
class FMineSweeperEditorModule : public IModuleInterface {
public:

    /**
    * @brief Public member variables
    * 
    **/
    TSharedPtr<FSysManager>             SysManager;
    // TSharedPtr<SNumericEntryBox<uint8>> NumbericBoxX{};
    // TSharedPtr<SNumericEntryBox<uint8>> NumbericBoxY{};

    /**
    * @brief Public member functions
    * 
    **/
    FMineSweeperEditorModule();

    /** IModuleInterface overrides */
    virtual void
    StartupModule() override;
    virtual void
    ShutdownModule() override;

    /**
     * @brief Brings up main plugin window
     * @note This function will be bound to Command
     */
    void
    TabBtnClicked() const;

    /**
    * @brief Commit text from text-box 1 to value
    * @param NewInt New display value
    * @param CommitType
    * @note set value when keyboard input
    */
    void
    CommittedX(const uint8 NewInt, ETextCommit::Type CommitType) const;

    /**
    * @brief Commit text from text-box 1 to value
    * @param NewInt New display value
    * @param CommitType
    * @note set value when keyboard input
    */
    void
    CommittedY(const uint8 NewInt, ETextCommit::Type CommitType) const;

    void
    RegenerateGrid(uint8 XIn, uint8 YIn, TSharedRef<class SUniformGridPanel>) const;
private:

    /**
    * @brief Private member variables
    * 
    **/
    TSharedPtr<class FUICommandList> PluginCmds;

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
    * @brief Bound to reset button
    **/
    // FReply
    // ResetGameBind();

    /**
    * @brief Generate Slate Grid 
    * @param XIn Grid Row Size
    * @param YIn Grid Column Size
    * @note FSLocal - Local container to create and bind function \n
    * FSLocal::OnTileClick(const Coords, TSharedPtr<FSysManager>) \n
    * FSLocal::MakeTile(const Coords, TSharedPtr<FSysManager>)
    * @return Shared reference of Grid panel, type: TSharedRef<SUniformGridPanel>
    **/
    TSharedRef<class SUniformGridPanel>
    GenerateGrid(uint8 XIn, uint8 YIn) const;


    /**
    * @brief Call when spawning window to spawn internal tab/page.
    * @param SpawnTabArgs
    **/
    TSharedRef<class SDockTab>
    OnSpawnTab(const class FSpawnTabArgs & SpawnTabArgs) const;
}; /** End of FMineSweeperEditorModule class */


/**
* @brief  public delegate container
*
**/
struct FTileBinder {
    /** Reset Game event */
    static FReply
    ResetGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager);

    /** Restart Board event */
    static FReply
    RestartGameBind(const FMineSweeperEditorModule * Owner, TSharedPtr<FSysManager> Manager);

    /** On Tile Click event */
    static FReply
    OnTileClick(Coords                  TileCoords,
                TSharedPtr<FSysManager> ManagerShared);

    /** Make Tile and bind OnClick to it */
    static TSharedRef<SWidget>
    MakeTile(const Coords TileCoords,
             TSharedPtr<FSysManager>
             ManagerShared);
};

#endif // MINESWEEPER_EDITOR_H
