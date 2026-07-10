// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Editor Headers
#include "ArianeEditorPrimitiveProxy.h"

#include "ArianeEditorPolygonProxy.generated.h"

struct FArianePolygon;

UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorPolygonProxy : public UArianeEditorPrimitiveProxy
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            bool CornerCount : 1;
            bool Radius      : 1;
        };
        uint8 raw[1];
    } FPolygonPropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorPolygonProxy();
        UArianeEditorPolygonProxy();

    public:
        virtual bool GetPropertyBit( const FName& PropertyName ) override;
        virtual void SetPropertyBit( const FName& PropertyName
                                   , const FName& MemberPropertyName
                                   , const FName& Category
                                   , bool bState ) override;

        virtual bool HasAnyPropertyBit() override;
        virtual void ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy ) override;

    protected:
        virtual void ImportParam( const TArray<FArianeObject*>& ModifiedObjects ) override;
        virtual void ClearPropertyBits() override;
        virtual void ApplyPropertyBits( FArianeObject* Object ) override;

    private:
        FPolygonPropertyBits PolygonPropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category=Line
                 , meta = ( ToolTip = "CornerCount" ) )
        int CornerCount;

        UPROPERTY( EditAnywhere
                 , Category=Line
                 , meta = ( ToolTip = "Radius" ) )
        double Radius;
};
