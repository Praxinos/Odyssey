// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class SOdysseyTextureImportImagesDialog
    : public SCompoundWidget
    //TODO: FGCObject to keep UTexture2D objects alive
{
public:
    static bool Open(UTexture* oTexture, TArray< FString > iFilenames);

public:
    SLATE_BEGIN_ARGS(SOdysseyTextureImportImagesDialog)
        {}
        /** Called when the object value changes */
        //SLATE_ATTRIBUTE(UOdysseyBrush*, Brush)
        //SLATE_EVENT( FOnBrushChanged, OnBrushChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyTextureImportImagesDialog();
    void Construct(const FArguments& InArgs);

private:
    void Import();

};
