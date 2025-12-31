// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Engine/Texture2D.h"
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <list>

#include "OdysseyVectorBrush.generated.h"

class FOdysseyVectorObject;
class FOdysseyVectorBrushObject;
class FOdysseyVectorChain;

UENUM(BlueprintType)
enum class eBrushExtensionMode : uint8
{
    Segment = 0,
    Path    = 1,
    Adapt   = 2
};

USTRUCT(BlueprintType)
struct ODYSSEYVECTOR_API FOdysseyVectorBrush
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, Category="Default" )
    UTexture2D* Texture;

    UPROPERTY( EditAnywhere
             , Category="Default"
             , meta = ( ToolTip = "Color From Brush" ) )
    bool ColorFromBrush;

    UPROPERTY( EditAnywhere
             , Category="Default"
             , meta = ( ToolTip = "Bilinear Filtering" ) )
    bool BilinearFiltering;

    UPROPERTY( EditAnywhere
             , Category="Default"
             , meta = ( ToolTip = "Extension Mode" ) )
    eBrushExtensionMode ExtensionMode;

    UPROPERTY( EditAnywhere
             , Category="Default"
             , meta = ( ToolTip = "Revert" ) )
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
        : FOdysseyVectorBrush( nullptr )
    {
    }

    FOdysseyVectorBrush( FOdysseyVectorObject* iOwner  /* can be NULL*/ )
    {
        owner = iOwner;
        width  = 0;
        height = 0;
        bitsPerPixel = 0;
        pixels = nullptr;
        Texture = nullptr;
        ColorFromBrush = false;
        ExtensionMode = eBrushExtensionMode::Adapt;
        Revert = false;
        BilinearFiltering = false;
    }

    // copy operator is mandatory to load the texture properly by calling SetTexture
    FOdysseyVectorBrush& operator=(const FOdysseyVectorBrush& iOther)
    {
        if ( this != &iOther ) // not a self-assignment
        {
            memcpy( this, &iOther, sizeof( FOdysseyVectorBrush ) ); // copy all members
            // force load the texture and make its pixels accessible
            SetTexture( iOther.Texture );
        }

        return *this;
    }

    FOdysseyVectorBrush( FOdysseyVectorObject* iOwner // can be NULL
                       , UTexture2D* iTexture )
        : FOdysseyVectorBrush( iOwner )
    {
        SetTexture( iTexture );
    }

    FOdysseyVectorBrush( FOdysseyVectorObject* iOwner // can be NULL
                       , const std::list<FOdysseyVectorObject*>& iObjectList
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
        //UTexture2D* texture;
        FOdysseyVectorObject* owner;
        // Experimental. Vector brush
        std::vector<FOdysseyVectorBrushObject*> brushObjectArray;
};
