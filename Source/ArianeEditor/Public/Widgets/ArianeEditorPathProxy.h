// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Editor Headers
#include "ArianeEditorObjectProxy.h"

#include "ArianeEditorPathProxy.generated.h"


struct FArianePath;


UENUM()
enum class EArianeEditorPathProxyWideningMode : uint8
{
    Percent = 0 UMETA( ToolTip = "Percent" ),
    Units = 1 UMETA( ToolTip = "Units" )
};

UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorPathProxy : public UArianeEditorObjectProxy
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            uint32 PathWidthInPercent : 1;
            uint32 PathWidthInUnits   : 1;
            uint32 Material           : 1;
            //uint32 JointType : 1;
            //uint32 MiterLimit : 1;
            //uint32 Brush : 1;
        };
        uint8 raw[1];
    } FPathPropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorPathProxy();
        UArianeEditorPathProxy();

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
        FPathPropertyBits PathPropertyBits;

    public:
        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayWideningOptions;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Widen in percent or units"
                          , EditCondition = "bDisplayWideningOptions"
                          , EditConditionHides ) )
        EArianeEditorPathProxyWideningMode WideningMode;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Width in percent"
                          , Units = "Percent"
                          , EditCondition = "bDisplayWideningOptions && ( WideningMode == EArianeEditorPathProxyWideningMode::Percent )"
                          , EditConditionHides
                          //TODO: no clamping at all ?!
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        double PathWidthInPercent;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Width in units"
                          , EditCondition = "bDisplayWideningOptions && ( WideningMode == EArianeEditorPathProxyWideningMode::Units )"
                          , EditConditionHides
                          //TODO: no clamping at all ?!
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        double PathWidthInUnits;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "The material to use along the path." ) )
        UMaterialInterface* Material;

        //UPROPERTY( EditAnywhere
        //         , Category=Path
        //         , meta = ( ToolTip = "The type of joint to use between 2 vertices." ) )
        //eVectorPathJointType JointType;

        //UPROPERTY( EditAnywhere
        //         , Category=Path
        //         , meta = ( ToolTip = "The limit value to use for the Miter Joint Type"
        //                    //TODO: no clamping at all ?!
        //                    , LinearDeltaSensitivity = "15"
        //                    , Delta = "1" ) )
        //double MiterLimit;

        //UPROPERTY( EditAnywhere
        //         , Category=Path
        //         , meta = ( ToolTip = "The texture to use along the path." ) )
        //FOdysseyVectorBrush Brush;

        //UPROPERTY(EditAnywhere,Category=Path, meta = (ContentDir = ))
        //UTexture* Brush2;
};
