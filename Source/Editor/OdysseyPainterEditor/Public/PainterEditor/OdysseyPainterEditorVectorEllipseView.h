#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorEllipse.h"
#include "OdysseyPainterEditorVectorPathView.h"

#include "OdysseyPainterEditorVectorEllipseView.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorEllipseView : public UOdysseyPainterEditorVectorPathView
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorEllipseView();
        UOdysseyPainterEditorVectorEllipseView();

    protected:
        virtual void ImportParam( FOdysseyVectorObject* iObject ) override;
        virtual void ExportParam( FOdysseyVectorObject* iObject ) override;
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory ) override;

    public:
        UPROPERTY(EditAnywhere, Category="Geometry")
        FEllipseParam EllipseParam; // UStruct from OdysseyVectorEllipse.h
};
