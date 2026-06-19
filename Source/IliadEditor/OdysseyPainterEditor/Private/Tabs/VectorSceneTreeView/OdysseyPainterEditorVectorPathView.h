// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPath.h"
#include "OdysseyPainterEditorVectorObjectView.h"

#include "OdysseyPainterEditorVectorPathView.generated.h"

UENUM()
enum class EPathViewWideningMode : uint8
{
    Percent = 0 UMETA( ToolTip = "Percent" ),
    Units = 1 UMETA( ToolTip = "Units" )
};

UCLASS( HideCategories = "Hidden" )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathView : public UOdysseyPainterEditorVectorObjectView
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            uint32 PathWidthInPercent : 1;
            uint32 PathWidthInUnits : 1;
            uint32 JointType : 1;
            uint32 MiterLimit : 1;
            uint32 Brush : 1;
        };
        uint8 raw[1];
    } PathPropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorPathView();
        UOdysseyPainterEditorVectorPathView();

        void SetDisplayWideningOptions( bool iValue );

    public:
        virtual bool GetPropertyBit( const FName& iPropertyName ) override;
        virtual void SetPropertyBit( const FName& iPropertyName
                                   , const FName& iMemberPropertyName
                                   , const FName& iCategory
                                   , bool iState ) override;

        virtual bool HasAnyPropertyBit() override;
        virtual void ImportParamFromOtherView( UOdysseyPainterEditorVectorObjectView* iOtherView ) override;

    protected:
        virtual void ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList ) override;
        virtual void ClearPropertyBits() override;
        virtual void ApplyPropertyBits( FOdysseyVectorObject* iObject ) override;

    private:
        PathPropertyBits mPathPropertyBits;

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
        EPathViewWideningMode WideningMode;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Width in percent"
                          , Units = "Percent"
                          , EditCondition = "bDisplayWideningOptions && ( WideningMode == EPathViewWideningMode::Percent )"
                          , EditConditionHides
                          //TODO: no clamping at all ?!
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        double PathWidthInPercent;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Width in units"
                          , EditCondition = "bDisplayWideningOptions && ( WideningMode == EPathViewWideningMode::Units )"
                          , EditConditionHides
                          //TODO: no clamping at all ?!
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        double PathWidthInUnits;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "The type of joint to use between 2 vertices." ) )
        eVectorPathJointType JointType;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "The limit value to use for the Miter Joint Type"
                            //TODO: no clamping at all ?!
                            , LinearDeltaSensitivity = "15"
                            , Delta = "1" ) )
        double MiterLimit;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "The texture to use along the path." ) )
        FOdysseyVectorBrush Brush;

        //UPROPERTY(EditAnywhere,Category=Path, meta = (ContentDir = ))
        //UTexture* Brush2;
};
