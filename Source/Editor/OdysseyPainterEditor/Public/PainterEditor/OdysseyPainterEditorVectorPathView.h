#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPath.h"
#include "OdysseyPainterEditorVectorObjectView.h"

#include "OdysseyPainterEditorVectorPathView.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathView : public UOdysseyPainterEditorVectorObjectView
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorPathView();
        UOdysseyPainterEditorVectorPathView();

    protected:
        virtual void ImportParam() override;
        virtual uint64 PropertyChanged( const FName& iPropertyName
                                      , const FName& iMemberPropertyName
                                      , const FName& iCategory ) override;
    public:
        UPROPERTY( EditAnywhere, Category = Path )
        eJointType JointType;

        UPROPERTY( EditAnywhere, Category = Path )
        double MiterLimit;

        UPROPERTY( EditAnywhere, Category = Path )
        FOdysseyVectorBrush Brush;

        //UPROPERTY(EditAnywhere,Category = Path, meta = (ContentDir = ))
        //UTexture* Brush2;
};
