// Ario Amin - MineSweeper Goedesic Test

#ifndef GUI_STYLE_HH
#define GUI_STYLE_HH

#include "Slate.h"

/**
 * @brief top-level gui class 
 **/
class FGUIStyle {
public:
    static void
    Initialize();
    static void
    Shutdown();
    static void
    ReloadTextures(); /** reloads textures used by slate renderer */

    /**
     * @brief Return Slate Style
     * @return ISlateStyle ref for the module
     */
    static const ISlateStyle &
    Get();

    /**
    * @brief Return Slate Style Name
    * @return FName for the Style
    */
    static FName
    GetStyleSetName();

private:
    static TSharedPtr<class FSlateStyleSet> GUIStyleInstance;
    static TSharedRef<class FSlateStyleSet>
    Create();
};

#endif // GUI_STYLE_HH
