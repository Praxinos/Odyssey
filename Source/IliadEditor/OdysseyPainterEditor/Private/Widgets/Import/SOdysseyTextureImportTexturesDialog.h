// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyImportTexturePositioning.h"
class SOdysseyTextureImportTexturesDialog
    : public SCompoundWidget
    //TODO: FGCObject to keep UTexture2D objects alive
{
public:
    static bool Open(UTexture* oTexture, TArray< UTexture2D* > iTextures);
    static bool Open(UTexture* oTexture, TArray< FString > iFilenames);

public:
    SLATE_BEGIN_ARGS(SOdysseyTextureImportTexturesDialog)
        {}
        /** Called when the object value changes */
        //SLATE_ATTRIBUTE(UOdysseyBrush*, Brush)
        //SLATE_EVENT( FOnBrushChanged, OnBrushChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyTextureImportTexturesDialog();
    void Construct(const FArguments& InArgs);

private:
    static bool Open(FText iTitle, UTexture* oTexture, TArray< UTexture2D* > iTextures);
    void Import();

private:
    SOdysseyImportTexturePositioning::FData mPositioningData;
};
