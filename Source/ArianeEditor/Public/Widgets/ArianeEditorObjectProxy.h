// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "ArianeEditorObjectProxy.generated.h"

class FArianeEditor;
struct FArianeObject;

/*
USTRUCT()
struct FPaletteEntrySelection
{
    GENERATED_BODY()

    UPROPERTY(
        EditAnywhere,
        Category=Appearance,
        meta=(
            Tooltip="The Palette containing the color to use"
        ))
    UOdysseyPalette* OdysseyPalette = nullptr;

    UPROPERTY(
        EditAnywhere,
        Category=Appearance,
        meta = (
            Tooltip="The Palette's Color to use",
            EditCondition = "OdysseyPalette != nullptr",
            EditConditionHides
        ))
    UOdysseyPaletteEntryColor* OdysseyPaletteEntryColor = nullptr;
};
*/

UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorObjectProxy : public UObject
{
    // we use a bitfields in case we have more than 64 flags
    typedef union {
        struct
        {
            bool Name : 1;
            //uint32 TranslationX : 1;
            //uint32 TranslationY : 1;
            //uint32 Rotation : 1;
            //uint32 ScalingX : 1;
            //uint32 ScalingY : 1;
            //uint32 SkewX : 1;
            //uint32 SkewY : 1;
            //uint32 Opacity : 1;
            bool Visible : 1;
            //uint32 ForegroundColorMode : 1;
            //uint32 ForegroundColor : 1;
            //uint32 ForegroundPaletteSelection : 1;
            //uint32 BackgroundColorMode : 1;
            //uint32 BackgroundColor : 1;
            //uint32 BackgroundPaletteSelection : 1;
        };
        uint8 raw[1];
    } ObjectPropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorObjectProxy();
        UArianeEditorObjectProxy();

        void Update( const TArray<FArianeObject*>& ModifiedObjects );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

        void ValidateProperties( TArray<FArianeObject*>& ModifiedObjects, bool bClearBits );

        // Pointer to the layer (useful to retrieve palette sets needed by FOdysseyVectorObjectViewPaletteCustomization)
        //TSharedPtr<FOdysseyVectorLayer> GetVectorLayer();
        virtual void ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherView );

    public:
        virtual bool GetPropertyBit( const FName& iPropertyName );
        virtual void SetPropertyBit( const FName& iPropertyName
                                   , const FName& iMemberPropertyName
                                   , const FName& iCategory
                                   , bool iState  );
        bool HasProperty( const FName& iPropertyName );
        virtual bool HasAnyPropertyBit();

    protected:
        virtual void ClearPropertyBits();
        virtual void ApplyPropertyBits( FArianeObject* iObject );

        virtual void ImportParam( const TArray<FArianeObject*>& SelectedObjects );
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );

    protected:
        FArianeEditor* Editor;

    private:
        ObjectPropertyBits ObjectPropertyBits;

    public:
/*
        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayBackgroundProperties;

        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category=Hidden )
        bool bDisplayForegroundProperties;
*/
        UPROPERTY( EditAnywhere
                 , Category=Identity
                 , meta = ( ToolTip = "The name of this vector object" ) )
        FName Name;

/*
        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The horizontal position of this vector object."
                            , LinearDeltaSensitivity = "5"
                            , Delta = "1" ) )
        double TranslationX;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The vertical position of this vector object."
                            , LinearDeltaSensitivity = "5"
                            , Delta = "1" ) )
        double TranslationY;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The rotation of this vector object around its pivot point"
                            , LinearDeltaSensitivity = "15"
                            , Delta = "1"
                            , Units = "degrees" ) )
        double Rotation;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The horizontal scaling of this vector object."
                            , LinearDeltaSensitivity = "15"
                            , Delta = "0.01"
                            //, Units = "degrees" //TODO: but only once the displayed value will be 0-100
                            ) )
        double ScalingX;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The vertical scaling of this vector object."
                            , LinearDeltaSensitivity = "15"
                            , Delta = "0.01"
                            //, Units = "degrees" //TODO: but only once the displayed value will be 0-100
                            ) )
        double ScalingY;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The horizontal Skew of this vector object."
                            , LinearDeltaSensitivity = "15"
                            , Delta = "0.01" ) )
        double SkewX;

        UPROPERTY( EditAnywhere
                 , Category= Transform
                 , meta = ( ToolTip = "The vertical Skew of this vector object."
                            , LinearDeltaSensitivity = "15"
                            , Delta = "0.01" ) )
        double SkewY;

        // unused for now
        //UPROPERTY( EditAnywhere, Category=Appearance , meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;
*/

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "If checked, this vector object is visible." ) )
        bool bVisible;

/*
        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "This vector object's Foreground Color Mode"
                          , EditCondition = "( bDisplayForegroundProperties )"
                          , EditConditionHides ) )
        eForegroundColorMode ForegroundColorMode;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "This vector object's Foreground Color"
                          , EditCondition = "( bDisplayForegroundProperties ) && ( ForegroundColorMode == eForegroundColorMode::SolidColor )"
                          , EditConditionHides ) )
        FColor ForegroundColor;

        UPROPERTY( EditAnywhere,
                   Category=Appearance,
                   meta = (
                            Tooltip = "The Palette Color used as this vector object Foreground Color."
                          , EditCondition = "( bDisplayForegroundProperties ) && ( ForegroundColorMode == eForegroundColorMode::Palette )"
                          , EditConditionHides ) )
        FPaletteEntrySelection ForegroundPaletteSelection;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "This vector object's Background Color Mode"
                          , EditCondition = "( bDisplayBackgroundProperties )"
                          , EditConditionHides ) )
        eBackgroundColorMode BackgroundColorMode;

        UPROPERTY( EditAnywhere
                 , Category=Appearance
                 , meta = ( ToolTip = "This vector object's Background Color"
                          , EditCondition = "( bDisplayBackgroundProperties ) && ( BackgroundColorMode == eBackgroundColorMode::SolidColor )"
                          , EditConditionHides ) )
        FColor BackgroundColor;

        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = (
                            Tooltip = "The Palette Color used as this vector object Foreground Color."
                          , EditCondition = "( bDisplayBackgroundProperties ) && ( BackgroundColorMode == eBackgroundColorMode::Palette )"
                          , EditConditionHides ) )
        FPaletteEntrySelection BackgroundPaletteSelection;
*/
};
