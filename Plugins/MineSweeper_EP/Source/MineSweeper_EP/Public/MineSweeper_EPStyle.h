// Ario Amin - MineSweeper Goedesic Test

#ifndef MINESWEEPER_EPSTYLE_H
#define MINESWEEPER_EPSTYLE_H

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/** MineSweeper  Slate Style  */
class FMineSweeper_EPStyle
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
};
#endif // MINESWEEPER_EPSTYLE_H