// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBlendingMode.h"
#include "OdysseyBlendMode.h"
#include "OdysseyBlendParameters.generated.h"

USTRUCT(BlueprintType)
struct ODYSSEYPAINTEREDITOR_API FOdysseyBlendParameters
{
    GENERATED_BODY()

    FOdysseyBlendParameters();
    FOdysseyBlendParameters(EOdysseyBlendMode InBlendMode, float InOpacity);
    FOdysseyBlendParameters(EOdysseyColorBlendMode InColorBlendMode, EOdysseyAlphaBlendMode InAlphaBlendMode, float InOpacity);

    bool operator==(const FOdysseyBlendParameters& Other) const;
    bool operator!=(const FOdysseyBlendParameters& Other) const;

    bool Serialize(FArchive& Ar);
    void PostSerialize(const FArchive& Ar);

public:
    void SetOpacity(float InOpacity);
    float GetOpacity() const;

    void SetBlendMode(EOdysseyBlendMode InBlendMode);
    EOdysseyBlendMode GetBlendMode() const;
    EOdysseyColorBlendMode GetColorBlendMode() const;
    EOdysseyAlphaBlendMode GetAlphaBlendMode() const;

    /**
     * A composite BlendMode uses 2 seperated blendmodes :
     * - The Color Blend Mode
     * - The Alpha Blend Mode
     *
     * A non composite BlendMode uses a single BlendMode
     * that is later interpreted as Color + Alpha Blend Modes
     */
    void SetIsComposite(bool InIsComposite);
    bool GetIsComposite() const;

    void SetEraserMode(bool InEraserMode);
    bool GetEraserMode() const;

    void SetCompositeColorBlendMode(EOdysseyColorBlendMode InColorBlendMode);
    void SetCompositeAlphaBlendMode(EOdysseyAlphaBlendMode InAlphaBlendMode);
    EOdysseyColorBlendMode GetCompositeColorBlendMode() const;
    EOdysseyAlphaBlendMode GetCompositeAlphaBlendMode() const;

private:
    //Deprecated properties
    UPROPERTY()
    EOdysseyBlendingMode BlendingMode_DEPRECATED;
    UPROPERTY()
    EOdysseyAlphaMode AlphaMode_DEPRECATED;

protected:
    UPROPERTY(EditAnywhere, Category="Blending")
    bool bEraserMode = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blending")
    bool bIsComposite = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blending",
        meta=(
            EditCondition="!bIsComposite && !bEraserMode",
            EditConditionHides,
            InvalidEnumValues="Stencil"
        )
    )
    EOdysseyBlendMode BlendMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blending",
        meta=(
            DisplayName="Color Blend Mode",
            EditCondition="bIsComposite && !bEraserMode",
            EditConditionHides,
            InvalidEnumValues="Top,Back"
        )
    )
    EOdysseyColorBlendMode CompositeColorBlendMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blending",
        meta=(
            DisplayName="Alpha Blend Mode",
            EditCondition="bIsComposite && !bEraserMode",
            EditConditionHides,
            InvalidEnumValues="Min,Multiply,Top"
        )
    )
    EOdysseyAlphaBlendMode CompositeAlphaBlendMode;

    /** The opacity. */
    UPROPERTY( EditAnywhere, Category="Blending", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Units = "Percent"))
    float Opacity;
};

/**
 * The following struct specification just informs Unreal's serialization system
 * that this struct has a PostSerialize() function that needs to be called.
 *
 * See TStructOpsTypeTraitsBase2 to see all the flags an UStruct can define.
 */
template<>
struct TStructOpsTypeTraits<FOdysseyBlendParameters>
    : public TStructOpsTypeTraitsBase2<FOdysseyBlendParameters>
{
    enum
    {
        WithIdenticalViaEquality = true,
        WithSerializer = true,
        WithPostSerialize = true
    };
};
