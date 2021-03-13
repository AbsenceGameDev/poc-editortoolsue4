// Ario Amin - MineSweeper Goedesic Test

#ifndef MINESWEEPER_EPCOMMANDS_H
#define MINESWEEPER_EPCOMMANDS_H

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MineSweeper_EPStyle.h"

class FMineSweeper_EPCommands : public TCommands<FMineSweeper_EPCommands>
{
public:
	TSharedPtr<FUICommandInfo> WindowContext;

	FMineSweeper_EPCommands()
		: TCommands<FMineSweeper_EPCommands>(TEXT("MineSweeper_EP"), NSLOCTEXT("Contexts", "MineSweeper_EP", "MineSweeper_EP Plugin"), NAME_None, FMineSweeper_EPStyle::GetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;
};

#endif // MINESWEEPER_EPCOMMANDS_H