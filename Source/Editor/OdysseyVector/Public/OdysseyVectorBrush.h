#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorBrush.generated.h"

class FOdysseyVectorObject;
class FOdysseyVectorBrushObject;
class FOdysseyVectorChain;

UENUM(BlueprintType)
enum class eBrushExtensionMode : uint8
{
    Path    = 0,
    Segment = 1,
    Adapt   = 2
};

USTRUCT(BlueprintType)
struct ODYSSEYVECTOR_API FOdysseyVectorBrush
{
    GENERATED_BODY()

    //UPROPERTY( EditAnywhere, Category = "Default" )
    //UTexture2D* Texture;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool ColorFromBrush;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool BilinearFiltering;

    UPROPERTY( EditAnywhere, Category = "Default" )
    eBrushExtensionMode ExtensionMode;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool Revert;

    static void MakeDemoBrush( const std::list<FOdysseyVectorObject*>& iObjectList
                             , const ::ULIS::FRectD& iBoundingBox );
    static FOdysseyVectorBrush* GetDemoBrush();
    void Draw( BLContext* iBLContext
             , const ::ULIS::FRectD& iInvalidationArea
             , double iAncestorsOpacity
             , FOdysseyVectorChain* iChain
             , uint64 iDrawingFlags );

    FOdysseyVectorBrush()
    {
        width  = 0;
        height = 0;
        bitsPerPixel = 0;
        pixels = nullptr;
        texture = nullptr;
        ColorFromBrush = false;
        ExtensionMode = eBrushExtensionMode::Adapt;
        Revert = false;
        BilinearFiltering = false;
    }

    FOdysseyVectorBrush( UTexture2D* iTexture )
    {
        width  = 0;
        height = 0;
        bitsPerPixel = 0;
        pixels = nullptr;
        ColorFromBrush = false;
        ExtensionMode = eBrushExtensionMode::Adapt;
        Revert = false;
        BilinearFiltering = false;

        SetTexture( iTexture );
    }

    FOdysseyVectorBrush( const std::list<FOdysseyVectorObject*>& iObjectList
                       , const ::ULIS::FRectD& iBoundingBox );

    void SetTexture( UTexture2D* iTexture );
    UTexture2D* GetTexture() const;
    void Lock();
    void Unlock();

    FColor* pixels;
    uint32  width;
    uint32  height;
    uint32  bitsPerPixel;

    private:
        UTexture2D* texture;

        // Experimental. Vector brush
        std::vector<FOdysseyVectorBrushObject*> brushObjectArray;
};
