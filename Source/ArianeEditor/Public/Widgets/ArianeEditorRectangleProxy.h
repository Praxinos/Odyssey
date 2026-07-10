// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Editor Headers
#include "ArianeEditorPrimitiveProxy.h"

#include "ArianeEditorRectangleProxy.generated.h"


struct FArianeRectangle;


UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorRectangleProxy : public UArianeEditorPrimitiveProxy
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            bool Width       : 1;
            bool Height      : 1;
        };
        uint8 raw[1];
    } FRectanglePropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorRectangleProxy();
        UArianeEditorRectangleProxy();

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
        FRectanglePropertyBits RectanglePropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Width" ) )
        double Width;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Height" ) )
        double Height;
};
