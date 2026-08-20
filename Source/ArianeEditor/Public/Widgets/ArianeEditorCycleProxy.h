// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
#include "UObject/Object.h"
// Ariane Editor Headers
#include "ArianeEditorObjectProxy.h"
// Ariane Headers

#include "ArianeEditorCycleProxy.generated.h"

struct FArianeCycle;

UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorCycleProxy : public UArianeEditorObjectProxy
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            bool Material           : 1;
            bool Color              : 1;
            //uint32 JointType : 1;
            //uint32 MiterLimit : 1;
            //uint32 Brush : 1;
        };
        uint8 raw[1];
    } FCyclePropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorCycleProxy();
        UArianeEditorCycleProxy();

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
        FCyclePropertyBits CyclePropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category=Cycle
                 , meta = ( ToolTip = "The color" ) )
        FColor Color;

        UPROPERTY( EditAnywhere
                 , Category=Cycle
                 , meta = ( ToolTip = "The material to use for this cycle." ) )
        UMaterialInterface* Material;
};
