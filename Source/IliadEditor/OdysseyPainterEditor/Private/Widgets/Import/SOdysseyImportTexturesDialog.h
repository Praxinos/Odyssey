// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyImportTexturePositioning.h"

class FOdysseyImportTexturesViewportClient;

class SOdysseyImportTexturesDialog
    : public SCompoundWidget
    //TODO: FGCObject to keep UTexture2D objects alive
{
public:
    enum class ETabs : uint8
    {
        Positioning,
        ScanCleaner,
        PegsStabilization
    };

public:
    /*static bool Open(UTexture* oTexture, TArray< UTexture2D* > iTextures);
    static bool Open(UTexture* oTexture, TArray< FString > iFilenames);*/

    struct FInputParams
    {
        FText Title;
        uint32 CanvasWidth = 0;
        uint32 CanvasHeight = 0;
        TArray< UTexture2D* > Textures;
    };


    static bool Open(const FInputParams& iInputParams);

public:
    SLATE_BEGIN_ARGS(SOdysseyImportTexturesDialog)
        {}
        /** Called when the object value changes */
        //SLATE_ATTRIBUTE(UOdysseyBrush*, Brush)
        //SLATE_EVENT( FOnBrushChanged, OnBrushChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTexturesDialog();
    void Construct(const FArguments& InArgs, const FInputParams& iInputParams);

public:
    UTextureRenderTarget2D* CreateRenderTarget() const;
    void Render(UTextureRenderTarget2D* oRenderTarget, int iTextureIndex);

private:
    ETabs GetActiveTab() const;
    void OnTabChecked(ETabs iTab, ECheckBoxState iState);

    SOdysseyImportTexturePositioning::FData GetPositioningData() const;
    void OnPositioningChanged(SOdysseyImportTexturePositioning::FData iData);

    EVisibility GetCurrentTextureSliderVisibility() const;
    float GetCurrentTextureSliderStepSize() const;
    float GetCurrentTextureSliderValue() const;
    void OnCurrentTextureSliderValueChanged(float iValue);

    void UpdatePreview();

private:
    FInputParams mInputParams;

    ETabs mActiveTab = ETabs::Positioning;
    SOdysseyImportTexturePositioning::FData mPositioningData;
    uint32 mCurrentTextureIndex = 0;

    TStrongObjectPtr<UTextureRenderTarget2D> mPreviewRenderTarget;

    TSharedPtr<SViewport> mViewportWidget;
    TSharedPtr<FOdysseyImportTexturesViewportClient> mViewportClient;
    TSharedPtr<FSceneViewport> mSceneViewport;
};
