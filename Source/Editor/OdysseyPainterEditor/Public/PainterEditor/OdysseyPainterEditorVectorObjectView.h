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

        void Update( FOdysseyVectorGroupPaint* iScene, std::list<FOdysseyVectorObject*>& iFocusedObjectList );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual uint64 PropertyChanged( const FName& iPropertyName
                                      , const FName& iMemberPropertyName
                                      , const FName& iCategory );

    protected:
        FOdysseyVectorGroupPaint* mScene;
        std::list<FOdysseyVectorObject*> mFocusedObjectList;

    public:
        UPROPERTY( EditAnywhere
                 , Category = Identity
                 , meta = ( ToolTip = "Name" ) )
        FString Name;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "Translation X" ) )
        double TranslationX;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "Translation Y" ) )
        double TranslationY;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "Rotation" ) )
        double Rotation;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "Scaling X" ) )
        double ScalingX;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "Scaling Y" ) )
        double ScalingY;

        // unused for now
        //UPROPERTY( EditAnywhere, Category = Appearance , meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Visible" ) )
        bool Visible;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Foreground Color Mode" ) )
        eForegroundColorMode ForegroundColorMode;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Foreground Color"
                          , EditCondition = "(ForegroundColorMode == eForegroundColorMode::SolidColor)"
                          , EditConditionHides) )
        FColor ForegroundColor;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Background Color Mode" ) )
        eBackgroundColorMode BackgroundColorMode;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Background Color"
                          , EditCondition = "(BackgroundColorMode == eBackgroundColorMode::SolidColor)"
                          , EditConditionHides ) )
        FColor BackgroundColor;
};
