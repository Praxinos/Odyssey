#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

#include "OdysseyPainterEditorVectorObjectView.generated.h"

class FOdysseyPainterEditor;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorObjectView();
        UOdysseyPainterEditorVectorObjectView();

        void Update( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene, std::list<FOdysseyVectorObject*>& iFocusedObjectList );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual uint64 PropertyChanged( const FName& iPropertyName
                                      , const FName& iMemberPropertyName
                                      , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorGroupPaint* mScene;
        std::list<FOdysseyVectorObject*> mFocusedObjectList;

    public:
        UPROPERTY( EditAnywhere, Category = Identity )
        FString Name;

        UPROPERTY( EditAnywhere, Category= Transform )
        double TranslationX;

        UPROPERTY( EditAnywhere, Category= Transform )
        double TranslationY;

        UPROPERTY( EditAnywhere, Category= Transform )
        double Rotation;

        UPROPERTY( EditAnywhere, Category= Transform )
        double ScalingX;

        UPROPERTY( EditAnywhere, Category= Transform )
        double ScalingY;

        // unused for now
        //UPROPERTY( EditAnywhere, Category = Appearance , meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere, Category = Appearance )
        bool Visible;

        UPROPERTY( EditAnywhere, Category = Appearance )
        FColor ForegroundColor;

        UPROPERTY( EditAnywhere, Category = Appearance )
        FColor BackgroundColor;
};
