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

        void Update( FOdysseyVectorScene* iScene, std::list<FOdysseyVectorObject*>& iFocusedObjectList );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory );

    protected:
        FOdysseyVectorScene* mScene;
        std::list<FOdysseyVectorObject*> mFocusedObjectList;

    public:
        UPROPERTY(EditAnywhere, Category="Object")
        FObjectParam ObjectParam; // UStruct from OdysseyVectorObject.h

        UPROPERTY(EditAnywhere, Category="Object")
        FColor ForegroundColor;

        UPROPERTY(EditAnywhere,Category="Object")
        FColor BackgroundColor;
};
