// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImportTexturesData.h"

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
    static bool Open(FText Title, FOdysseyImportTexturesData& ioData);

public:
    SLATE_BEGIN_ARGS(SOdysseyImportTexturesDialog)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTexturesDialog();
    void Construct(const FArguments& InArgs, const FOdysseyImportTexturesData& ioData);

public:
    FOdysseyImportTexturesData GetImportData() const;

private:
    ETabs GetActiveTab() const;
    void OnTabChecked(ETabs iTab, ECheckBoxState iState);
    void OnPositioningChanged(FOdysseyImportTexturesData iData);

    EVisibility GetCurrentTextureSliderVisibility() const;
    float GetCurrentTextureSliderStepSize() const;
    float GetCurrentTextureSliderValue() const;
    void OnCurrentTextureSliderValueChanged(float iValue);

    void UpdatePreview();

private:
    FOdysseyImportTexturesData mImportData;

    ETabs mActiveTab = ETabs::Positioning;
    uint32 mCurrentTextureIndex = 0;

    TStrongObjectPtr<UTextureRenderTarget2D> mPreviewRenderTarget;

    TSharedPtr<SViewport> mViewportWidget;
    TSharedPtr<FOdysseyImportTexturesViewportClient> mViewportClient;
    TSharedPtr<FSceneViewport> mSceneViewport;
};
