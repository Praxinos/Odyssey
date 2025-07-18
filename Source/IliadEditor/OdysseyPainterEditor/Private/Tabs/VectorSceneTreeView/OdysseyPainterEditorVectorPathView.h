// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPath.h"
#include "OdysseyPainterEditorVectorObjectView.h"

#include "OdysseyPainterEditorVectorPathView.generated.h"

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathView : public UOdysseyPainterEditorVectorObjectView
{
    typedef struct _PathPropertyBits
    {
        uint32 JointType : 1;
        uint32 MiterLimit : 1;
        uint32 Brush : 1;
    } PathPropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorPathView();
        UOdysseyPainterEditorVectorPathView();

    protected:
        virtual void ClearPropertyBits() override;
        virtual void ApplyPropertyBits( FOdysseyVectorObject* iObject ) override;
        virtual void ImportParam() override;
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory ) override;

    protected:
        PathPropertyBits mPathPropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Joint Type" ) )
        eJointType JointType;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Miter Limit" ) )
        double MiterLimit;

        UPROPERTY( EditAnywhere
                 , Category=Path
                 , meta = ( ToolTip = "Brush" ) )
        FOdysseyVectorBrush Brush;

        //UPROPERTY(EditAnywhere,Category=Path, meta = (ContentDir = ))
        //UTexture* Brush2;
};
