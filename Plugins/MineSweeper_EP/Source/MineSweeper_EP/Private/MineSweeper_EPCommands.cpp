// Ario Amin - MineSweeper Goedesic Test

#include "MineSweeper_EPCommands.h"

#define LOCTEXT_NAMESPACE "FMineSweeper_EPModule"

void FMineSweeper_EPCommands::RegisterCommands()
{
	UI_COMMAND(WindowContext, "MineSweeper_EP", "Bring up MineSweeper_EP window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
