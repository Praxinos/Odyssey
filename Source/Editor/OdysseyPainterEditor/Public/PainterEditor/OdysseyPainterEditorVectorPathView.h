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
        virtual uint64 PropertyChanged( const FName& iPropertyName, const FName& iCategory ) override;

    public:
        UPROPERTY(EditAnywhere, Category="Path")
        FPathParam PathParam; // UStruct from OdysseyVectorPath.h
};
