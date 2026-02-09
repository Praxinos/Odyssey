// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImportTexturesParameters.h"

class FOdysseyImportTexturesViewportClient;
class FOdysseyHUDRectangle;

class SOdysseyImportTexturesDialog
    : public SCompoundWidget
{
public:
    enum class ETabs : uint8
    {
        Positioning,
        ScanCleaner,
        PegsStabilization
    };

public:
    static bool Open(FText Title, FOdysseyImportTexturesParameters& ioData);

public:
    SLATE_BEGIN_ARGS(SOdysseyImportTexturesDialog)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTexturesDialog();
    void Construct(const FArguments& InArgs, const FOdysseyImportTexturesParameters& ioData);

public:
    FOdysseyImportTexturesParameters GetImportData() const;

private:
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    ETabs GetActiveTab() const;
    int32 GetActiveTabIndex() const;
    void OnTabChecked(ETabs iTab, ECheckBoxState iState);
    void OnPositioningChanged();
    void OnScanCleanerChanged();

    EVisibility GetCurrentTextureSliderVisibility() const;
    float GetCurrentTextureSliderStepSize() const;
    float GetCurrentTextureSliderValue() const;
    void OnCurrentTextureSliderValueChanged(float iValue);

    TSharedRef<FOdysseyHUDRectangle> CreateTextureOutlineHUD() const;
    void UpdatePreview();

private:
    FOdysseyImportTexturesParameters mImportData;

    ETabs mActiveTab = ETabs::Positioning;
    uint32 mCurrentTextureIndex = 0;

    TStrongObjectPtr<UTextureRenderTarget2D> mPreviewRenderTarget;

    TSharedPtr<SViewport> mViewportWidget;
    TSharedPtr<FOdysseyImportTexturesViewportClient> mViewportClient;
    TSharedPtr<FSceneViewport> mSceneViewport;
    TSharedPtr<FOdysseyHUDRectangle> mTextureOutlineHUD;
};
