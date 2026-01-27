// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyExportImage.h"
#include "Widgets/SCompoundWidget.h"
#include <ULIS>

#include "SOdysseyTextureExportAsImageDialog.generated.h"

class IOdysseyRenderingAbility;

UENUM()
enum class EOdysseyTextureExportAsImageSource : uint8
{
    AllLayers,
    CurrentLayer,
    SelectedLayers
};

class FOdysseyTextureAsImageExporter
{
public:
    FOdysseyTextureAsImageExporter();
    FOdysseyTextureAsImageExporter(UTexture2D* iTexture);

public:
    struct FSource
    {
        UObject* mTextureRenderingAbility;
        FString mFilename;
    };
    TArray<FSource> GetSources();

    void Export(const FString& iFilename);
    void ExportSource(const FSource& iSource, const FString& iFilename);

public:
    UTexture2D* mTexture = nullptr;
    EOdysseyExportImageFormat mFormat = EOdysseyExportImageFormat::PNG;
    EOdysseyTextureExportAsImageSource mSource = EOdysseyTextureExportAsImageSource::AllLayers;
};

class SOdysseyTextureExportAsImageDialog : public SCompoundWidget
{
public:
    /**
     * @brief Opens the Export Image Dialog for the given texture
     * And Exports the texture according to the selected options if user clicks OK
     *
     * @param iTexture
     * @return true If user clicked on OK
     * @return false If user Cancelled
     */
    static bool Open(UTexture2D* iTexture);

public:
    SLATE_BEGIN_ARGS(SOdysseyTextureExportAsImageDialog)
    {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UTexture2D* iTexture);

private:
    FString GetSaveFileDialogExtension();
    void Export();

private:
    FOdysseyTextureAsImageExporter mExporter;
};
