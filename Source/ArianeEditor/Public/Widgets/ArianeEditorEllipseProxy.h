// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
#include "UObject/Object.h"
// Ariane Editor Headers
#include "ArianeEditorPrimitiveProxy.h"

#include "ArianeEditorEllipseProxy.generated.h"


struct FArianeEllipse;


UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorEllipseProxy : public UArianeEditorPrimitiveProxy
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            bool RadiusX     : 1;
            bool RadiusY     : 1;
        };
        uint8 raw[1];
    } FEllipsePropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorEllipseProxy();
        UArianeEditorEllipseProxy();

        void SetDisplayWideningOptions( bool nValue );

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
        FEllipsePropertyBits EllipsePropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category=Ellipse
                 , meta = ( ToolTip = "Radius X" ) )
        double RadiusX;

        UPROPERTY( EditAnywhere
                 , Category=Ellipse
                 , meta = ( ToolTip = "Radius Y" ) )
        double RadiusY;
};
