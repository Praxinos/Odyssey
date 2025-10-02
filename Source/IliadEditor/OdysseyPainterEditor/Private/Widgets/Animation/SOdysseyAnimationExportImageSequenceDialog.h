// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyExportImage.h"
#include "Widgets/SCompoundWidget.h"
#include <ULIS>

#include "SOdysseyAnimationExportImageSequenceDialog.generated.h"

class UOdysseyAnimation;
class IOdysseyRenderingAbility;

UENUM()
enum class EOdysseyAnimationExportImageSequenceSource : uint8
{
    Animation,
    AllLayers,
    CurrentLayer,
    SelectedLayers
};

UENUM()
enum class EOdysseyAnimationExportImageSequenceRange : uint8
{
    AllFrames,
    AllCells,
    MarkInMarkOut UMETA( DisplayName = "Mark In / Mark Out" ),
    Custom
};

class FOdysseyAnimationImageSequenceExporter
{
public:
    FOdysseyAnimationImageSequenceExporter();
    FOdysseyAnimationImageSequenceExporter(UOdysseyAnimation* iAnimation);

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
    EOdysseyExportImageFormat mFormat = EOdysseyExportImageFormat::PNG;
    EOdysseyAnimationExportImageSequenceSource mSource = EOdysseyAnimationExportImageSequenceSource::Animation;
    EOdysseyAnimationExportImageSequenceRange mRange = EOdysseyAnimationExportImageSequenceRange::AllCells;
    FInt32Range mCustomRange;
    bool mUniqueFramesOnly = true;
};

class SOdysseyAnimationExportImageSequenceDialog : public SCompoundWidget
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
    SLATE_BEGIN_ARGS(SOdysseyAnimationExportImageSequenceDialog)
    {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UOdysseyAnimation* iAnimation);

private:
    FString GetSaveFileDialogExtension();
    void Export();

private:
    FOdysseyAnimationImageSequenceExporter mExporter;
};
