// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureDefines.h"
#include "SEnumCombo.h"
#include "Widgets/SWindow.h"

#include "SOdysseyTextureConfigureWindow.generated.h"

UENUM()
enum class EOdysseyTextureBackgroundColor : uint8
{
    kTransparent UMETA(DisplayName = "Transparent"),
    kWhite UMETA(DisplayName = "White"),
    kNormal UMETA(DisplayName = "Purple (127, 127, 255)")
};

UENUM()
enum class EOdysseyTextureSourceFormat : uint8
{
    kG8 UMETA(DisplayName = "Grey 8"),
    kG16 UMETA(DisplayName = "Grey 16"),
    kBGRA8 UMETA(DisplayName = "BGRA 8"),
    kBGRE8 UMETA(DisplayName = "BGRE 8"),
    kRGBA16 UMETA(DisplayName = "RGBA 16"),
    kRGBA16F UMETA(DisplayName = "RGBA 16 F"),
    kCustom UMETA(Hidden)
};

UENUM()
enum class EOdysseyTextureDefaultLayerType : uint8
{
    kNone UMETA(Hidden),
    kRaster UMETA(DisplayName = "Raster"),
    kVector UMETA(DisplayName = "Vector")
};

USTRUCT()
struct ODYSSEYWIDGETS_API FOdysseyTextureConfiguration
{
    GENERATED_BODY()

public:
    ETextureSourceFormat TextureSourceFormat() const;
    FLinearColor GetBackgroundColor() const;

    UTexture2D* CreateTexture(UObject* iParent, FName iName, EObjectFlags iFlags) const;

public:
    /**
     * Name of the Texture.
     *
     * By naming convention, it should be like "T_TextureAssetName".
     */
    UPROPERTY(EditAnywhere, Category="OdysseyTextureConfiguration")
    FName                   Name = "T_Drawing";

    /**
     * Width of the texture in pixels.
     *
     * Cannot be changed afterward. For textures used with a Material,
     * prefer a power of 2 (128, 256, 512, 1024...).
     */
    UPROPERTY(EditAnywhere, Category="OdysseyTextureConfiguration", meta = (ClampMin = "16", ClampMax = "8192", UIMin = "16", UIMax = "8192", Multiple = "4", DisplayName = "Width (px)"))
    uint32                  Width = 1024;

    /**
     * Height of the texture in pixels.
     *
     * Cannot be changed afterward. For textures used with a Material,
     * prefer a power of 2 (128, 256, 512, 1024...).
     */
    UPROPERTY(EditAnywhere, Category="OdysseyTextureConfiguration", meta = (ClampMin = "16", ClampMax = "8192", UIMin = "16", UIMax = "8192", Multiple = "4", DisplayName = "Height (px)"))
    uint32                  Height = 1024;

    /**
     * Image format of the texture.
     *
     * - Grey 8
     * - BGRA 8
     * - BGRE 8
     * - RGBA 16
     * - RGBA 16 F
     *
     * If you don't know which Format to use, keep the default value.
     */
    UPROPERTY(EditAnywhere, Category="OdysseyTextureConfiguration")
    EOdysseyTextureSourceFormat Format = EOdysseyTextureSourceFormat::kBGRA8;

    UPROPERTY()
    TEnumAsByte<ETextureSourceFormat> CustomFormat = ETextureSourceFormat::TSF_BGRA8;

    /**
     * Color of the background.
     *
     * Transparent, white or purple (useful to paint Normal Maps).
     */
    UPROPERTY(EditAnywhere, Category="OdysseyTextureConfiguration")
    EOdysseyTextureBackgroundColor BackgroundColor = EOdysseyTextureBackgroundColor::kTransparent;

    /** Default layer type: Raster Layer or Vector Layer. */
    UPROPERTY(EditAnywhere, Category="OdysseyTextureConfiguration")
    EOdysseyTextureDefaultLayerType LayerType = EOdysseyTextureDefaultLayerType::kRaster;
};

class ODYSSEYWIDGETS_API SOdysseyTextureConfigureWindow
    : public SWindow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyTextureConfigureWindow)
    {

    }
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs);
    void Construct( const FArguments& iArgs, const FOdysseyTextureConfiguration& iDefaultConfiguration);

    bool GetWindowAnswer();

public:
    //Getters
    const FOdysseyTextureConfiguration& GetConfiguration() const;

private:
    //Internal

    FReply OnAccept();
    FReply OnCancel();

private:
    FOdysseyTextureConfiguration  mConfiguration;
    bool                            mWindowAnswer = false;
    TSharedPtr< SEnumComboBox >     mFormatComboBox;
};
