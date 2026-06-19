// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "SOdysseyAnimationExportTextureSequenceDialog.generated.h"

class UOdysseyAnimation;
class IOdysseyRenderingAbility;

UENUM()
enum class EOdysseyAnimationExportTextureSequenceSource : uint8
{
    Animation,
    AllLayers,
    CurrentLayer,
    SelectedLayers
};

UENUM()
enum class EOdysseyAnimationExportTextureSequenceRange : uint8
{
    AllFrames,
    AllCells,
    MarkInMarkOut UMETA( DisplayName = "Mark In / Mark Out" ),
    Custom
};

class FOdysseyAnimationTextureSequenceExporter
{
public:
    FOdysseyAnimationTextureSequenceExporter();
    FOdysseyAnimationTextureSequenceExporter(UOdysseyAnimation* iAnimation);

public:
    struct FSource
    {
        UObject* mTextureRenderingAbility;
        FString mFilename;
        FInt32Range mRange;
    };
    TArray<FSource> GetSources();
    FInt32Range GetSourceRange(const FSource& iSource);

    void Export(const FString& iFilename);
    void ExportSource(const FSource& iSource, const FString& iFilename, int iNumZero);

public:
    UOdysseyAnimation* mAnimation = nullptr;
    EOdysseyAnimationExportTextureSequenceSource mSource = EOdysseyAnimationExportTextureSequenceSource::Animation;
    EOdysseyAnimationExportTextureSequenceRange mRange = EOdysseyAnimationExportTextureSequenceRange::AllCells;
    FInt32Range mCustomRange;
    bool mUniqueFramesOnly = true;
};

class SOdysseyAnimationExportTextureSequenceDialog : public SCompoundWidget
{
public:
    /**
     * @brief Opens the Export Image Sequence Dialog for the given animation
     * And Exports the animation according to the selected options if user clicks OK
     *
     * @param iAnimation
     * @return true If user clicked on OK
     * @return false If user Cancelled
     */
    static bool Open(UOdysseyAnimation* iAnimation);

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationExportTextureSequenceDialog)
    {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UOdysseyAnimation* iAnimation);

private:
    void Export();

private:
    FOdysseyAnimationTextureSequenceExporter mExporter;
};
