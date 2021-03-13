// Ario Amin - MineSweeper Goedesic Test

#include "MineSweeper_EPStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FMineSweeper_EPStyle::StyleContext = NULL;

void FMineSweeper_EPStyle::Init()
{
	if (!StyleContext.IsValid()) {
		StyleContext = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleContext);
	}
}

void FMineSweeper_EPStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleContext);
	ensure(StyleContext.IsUnique());
	StyleContext.Reset();
}

FName FMineSweeper_EPStyle::GetName()
{
	static FName StyleSetName(TEXT("MineSweeper_EPStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FMineSweeper_EPStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MineSweeper_EPStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MineSweeper_EP")->GetBaseDir() / TEXT("Resources"));
	Style->Set("MineSweeper_EP.WindowContext", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FMineSweeper_EPStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized()) {
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMineSweeper_EPStyle::Get()
{
	return *StyleContext;
}
