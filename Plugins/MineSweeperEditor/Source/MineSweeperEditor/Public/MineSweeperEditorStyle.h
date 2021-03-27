/**
* @file  MineSweeperEditorStyle.h
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#ifndef MINESWEEPER_EDITOR_STYLE_H
#define MINESWEEPER_EDITOR_STYLE_H

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"


enum class EBtnStyles: uint8 {
    BtnFlag,
    BtnQ,
    BtnBomb,
    BtnInt
};
/** MineSweeper button Slate Style  */
class FMineSweeperEditorStyle
{
public:
	static void Init();
	static void Shutdown();
	static void ReloadTextures(); /** reloads textures used by slate renderer */
	static const ISlateStyle& Get(); /** @return The Slate style set*/
	static FName GetName();

private:
	static TSharedPtr<class FSlateStyleSet> StyleContext;
	static TSharedRef<class FSlateStyleSet> Create();
    template<EBtnStyles BtnStyle>
    static TSharedRef<class FSlateStyleSet> CreateBtn();
};


#endif // MINESWEEPER_EDITOR_STYLE_H
