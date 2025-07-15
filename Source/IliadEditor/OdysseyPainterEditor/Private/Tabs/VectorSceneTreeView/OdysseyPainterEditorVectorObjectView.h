// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "Palette/OdysseyPalette.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#include "OdysseyPainterEditorVectorObjectView.generated.h"

class FOdysseyPainterEditor;

USTRUCT()
struct FPaletteEntrySelection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category=Appearance)
    UOdysseyPalette* OdysseyPalette = nullptr;

    UPROPERTY(EditAnywhere, Category=Appearance, meta = (EditCondition = "OdysseyPalette != nullptr", EditConditionHides))
    UOdysseyPaletteEntryColor* OdysseyPaletteEntryColor = nullptr;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (EditCondition = "false", EditConditionHides))
    FString OdysseyPaletteSet = FString();
};

UCLASS( meta = ( HideCategories = Hidden ) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorObjectView();
        UOdysseyPainterEditorVectorObjectView();

        void Update( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene, std::list<FOdysseyVectorObject*>& iFocusedObjectList );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

        FOdysseyPainterEditor* GetEditor();

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName
                                      , const FName& iMemberPropertyName
                                      , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorGroupPaint* mScene;
        std::list<FOdysseyVectorObject*> mFocusedObjectList;

    public:
        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayBackgroundProperties;

        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayForegroundProperties;

        UPROPERTY( EditAnywhere
                 , Category=Identity
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
        //UPROPERTY( EditAnywhere, Category=Appearance , meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "Visible" ) )
        bool Visible;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "Foreground Color Mode"
                          , EditCondition = "( bDisplayForegroundProperties )"
                          , EditConditionHides ) )
        eForegroundColorMode ForegroundColorMode;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "Foreground Color"
                          , EditCondition = "( bDisplayForegroundProperties ) && ( ForegroundColorMode == eForegroundColorMode::SolidColor )"
                          , EditConditionHides ) )
        FColor ForegroundColor;

        UPROPERTY( EditAnywhere,
                   Category=Appearance,
                   meta = ( EditCondition = "( bDisplayForegroundProperties ) && ( ForegroundColorMode == eForegroundColorMode::Palette )"
                          , EditConditionHides ) )
        FPaletteEntrySelection ForegroundPaletteSelection;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "Background Color Mode"
                          , EditCondition = "( bDisplayBackgroundProperties )"
                          , EditConditionHides ) )
        eBackgroundColorMode BackgroundColorMode;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "Background Color"
                          , EditCondition = "( bDisplayBackgroundProperties ) && ( BackgroundColorMode == eBackgroundColorMode::SolidColor )"
                          , EditConditionHides ) )
        FColor BackgroundColor;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( EditCondition = "( bDisplayBackgroundProperties ) && ( BackgroundColorMode == eBackgroundColorMode::Palette )"
                          , EditConditionHides ) )
        FPaletteEntrySelection BackgroundPaletteSelection;
};
