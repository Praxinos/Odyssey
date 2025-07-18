// IDDN.FR.001.060015.014.S.X.2019.000.00000
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

UENUM()
enum class EObjectViewApplyPolicy : uint8
{
    Selection = 0,
    AllInCell = 1,
    AllInAllCells = 2
};

USTRUCT()
struct FPaletteEntrySelection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category=Appearance)
    UOdysseyPalette* OdysseyPalette = nullptr;

    UPROPERTY(EditAnywhere, Category=Appearance, meta = (EditCondition = "OdysseyPalette != nullptr", EditConditionHides))
    UOdysseyPaletteEntryColor* OdysseyPaletteEntryColor = nullptr;
};

UCLASS( meta = ( HideCategories = Hidden, prioritizeCategories = Options ) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectView : public UObject
{
    public:
        GENERATED_BODY()

    typedef struct _ObjectPropertyBits
    {
        uint32 Name : 1;
        uint32 TranslationX : 1;
        uint32 TranslationY : 1;
        uint32 Rotation : 1;
        uint32 ScalingX : 1;
        uint32 ScalingY : 1;
        uint32 Opacity : 1;
        uint32 Visible : 1;
        uint32 ForegroundColorMode : 1;
        uint32 ForegroundColor : 1;
        uint32 ForegroundPaletteSelection : 1;
        uint32 BackgroundColorMode : 1;
        uint32 BackgroundColor : 1;
        uint32 BackgroundPaletteSelection : 1;
    } ObjectPropertyBits;

    enum class EditionMode : uint8
    {
        Direct = 0,
        OnValidation = 1
    };

    public:
        ~UOdysseyPainterEditorVectorObjectView();
        UOdysseyPainterEditorVectorObjectView();

        void Update( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene, std::list<FOdysseyVectorObject*>& iFocusedObjectList );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

        FOdysseyPainterEditor* GetEditor();

        void SetEditionMode( EditionMode iEditionMode );
        void ValidateProperties();

    protected:
        virtual void ClearPropertyBits();
        virtual void ApplyPropertyBits( FOdysseyVectorObject* iObject );
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorGroupPaint* mScene;
        std::list<FOdysseyVectorObject*> mFocusedObjectList;
        EditionMode mEditionMode;
        ObjectPropertyBits mObjectPropertyBits;

    public:
        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayBackgroundProperties;

        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayForegroundProperties;

        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Options )
        EObjectViewApplyPolicy ApplyTo;

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
