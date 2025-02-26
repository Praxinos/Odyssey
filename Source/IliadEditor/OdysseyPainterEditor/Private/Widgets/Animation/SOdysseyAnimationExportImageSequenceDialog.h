// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyExportImageFormat.h"
#include "Widgets/SCompoundWidget.h"
#include <ULIS>

#include "SOdysseyAnimationExportImageSequenceDialog.generated.h"

class UOdysseyAnimation;
class FOdysseyImageRenderingAbility;

UENUM()
enum class EOdysseyAnimationExportImageSequenceSource : uint8
{
    Animation,
    AllLayers,
    CurrentLayer,
    //SelectedLayers //TODO:
};

UENUM()
enum class EOdysseyAnimationExportImageSequenceRange : uint8
{
    AllFrames,
    AllCells,
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
        FOdysseyImageRenderingAbility* mImageRenderingAbility;
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
