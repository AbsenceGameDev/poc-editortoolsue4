// Ario Amin - MineSweeper Goedesic Test

#ifndef MINESWEEPER_EDITOR_COMMANDS_H
#define MINESWEEPER_EDITOR_COMMANDS_H

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MineSweeperEditorStyle.h"

class FMineSweeperEditorCommands : public TCommands<FMineSweeperEditorCommands>
{
public:
	TSharedPtr<FUICommandInfo> WindowContext;

	FMineSweeperEditorCommands()
		: TCommands<FMineSweeperEditorCommands>(TEXT("MineSweeperEditor"), NSLOCTEXT("Contexts", "MineSweeperEditor", "MineSweeperEditor Plugin"), NAME_None, FMineSweeperEditorStyle::GetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;
};

#endif // MINESWEEPER_EDITOR_COMMANDS_H