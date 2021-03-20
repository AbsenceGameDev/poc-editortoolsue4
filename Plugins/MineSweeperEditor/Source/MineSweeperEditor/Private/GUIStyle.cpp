#include "GUIStyle.h"
#include "Slate/SlateGameResources.h"

TSharedPtr<FSlateStyleSet> FGUIStyle::GUIStyleInstance = NULL;

void
FGUIStyle::Initialize()
{
    if (!GUIStyleInstance.IsValid()) {
        GUIStyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*GUIStyleInstance);
    }
}

void
FGUIStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*GUIStyleInstance);
    ensure(GUIStyleInstance.IsUnique());
    GUIStyleInstance.Reset();
}

FName
FGUIStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("GUIStyle"));
    return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo(Style->RootToContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo(Style->RootToContentDir(RelativePath, TEXT(".otf")), __VA_ARGS__)

TSharedRef<FSlateStyleSet>
FGUIStyle::Create()
{
    // Note, these sizes are in Slate Units.
    // Slate Units do NOT have to map to pixels.
    const FVector2D Icon12x12(12.0f, 12.0f);
    const FVector2D Icon12x16(12.0f, 16.0f);
    const FVector2D Icon14x14(14.0f, 14.0f);
    const FVector2D Icon16x16(16.0f, 16.0f);

    TSharedRef<FSlateStyleSet> Style = FSlateGameResources::New(FGUIStyle::GetStyleSetName(),
                                                                "/Game/UI",
                                                                "/Game/UI");
    return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void
FGUIStyle::ReloadTextures()
{
    FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle &
FGUIStyle::Get()
{
    return *GUIStyleInstance;
}
