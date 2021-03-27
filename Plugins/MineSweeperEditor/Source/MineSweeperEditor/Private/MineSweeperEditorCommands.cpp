/**
* @file  MineSweeperEditorCommands.cpp
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#include "MineSweeperEditorCommands.h"

#define LOCTEXT_NAMESPACE "FMineSweeperEditorModule"

void FMineSweeperEditorCommands::RegisterCommands()
{
	UI_COMMAND(WindowContext, "MineSweeperEditor", "Bring up MineSweeperEditor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
