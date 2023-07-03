#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

#include "OdysseyPainterEditorVectorObjectView.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorObjectView();
        UOdysseyPainterEditorVectorObjectView();

        void Update( FOdysseyVectorScene* iScene );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory );

    protected:
        FOdysseyVectorScene* mScene;

    public:
        UPROPERTY(EditAnywhere, Category="Object")
        FObjectParam ObjectParam; // UStruct from OdysseyVectorObject.h
};
