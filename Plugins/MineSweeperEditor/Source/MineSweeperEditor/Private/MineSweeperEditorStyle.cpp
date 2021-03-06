/**
* @file  MineSweeperEditorStyle.cpp
* @author Ario Amin
* @project  MineSweeper Geodesic Test
**/
#include "MineSweeperEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FMineSweeperEditorStyle::StyleContext = NULL;

void
FMineSweeperEditorStyle::Init()
{
    if (!StyleContext.IsValid()) {
        StyleContext = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleContext);
    }
}

void
FMineSweeperEditorStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleContext);
    ensure(StyleContext.IsUnique());
    StyleContext.Reset();
}

void
FMineSweeperEditorStyle::ReloadTextures()
{
    if (FSlateApplication::IsInitialized()) {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}

FName
FMineSweeperEditorStyle::GetName()
{
    static FName StyleSetName(TEXT("MineSweeperEditorStyle"));
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

TSharedRef<FSlateStyleSet>
FMineSweeperEditorStyle::Create()
{
    TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("MineSweeperEditorStyle"));
    Style->SetContentRoot(
        IPluginManager::Get().FindPlugin("MineSweeperEditor")->GetBaseDir() / TEXT("Resources"));
    Style->Set("MineSweeperEditor.WindowContext", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));
    return Style;
}

template<EBtnStyles BtnStyle>
TSharedRef<FSlateStyleSet>
FMineSweeperEditorStyle::CreateBtn()
{
    TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("MineSweeperEditorStyle"));
    Style->SetContentRoot(
        IPluginManager::Get().FindPlugin("MineSweeperEditor")->GetBaseDir() / TEXT("Resources"));

    if constexpr (BtnStyle == EBtnStyles::BtnFlag) {
        Style->Set("MineSweeperEditor.WindowContext", new IMAGE_BRUSH(TEXT("flag16x16"), Icon16x16));
    } else if constexpr (BtnStyle == EBtnStyles::BtnQ) {
        Style->Set("MineSweeperEditor.WindowContext", new IMAGE_BRUSH(TEXT("q16x16"), Icon16x16));
    } else if constexpr (BtnStyle == EBtnStyles::BtnBomb) {
        Style->Set("MineSweeperEditor.WindowContext", new IMAGE_BRUSH(TEXT("bomb16x16"), Icon16x16));
    } else if constexpr (BtnStyle == EBtnStyles::BtnInt) {
        // this path should not return an image tho, should be a an integer, maybe colored but atleast in integer
        Style->Set("MineSweeperEditor.WindowContext", new IMAGE_BRUSH(TEXT("int16x16"), Icon16x16));
    }

    return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT


const ISlateStyle &
FMineSweeperEditorStyle::Get()
{
    return *StyleContext;
}
