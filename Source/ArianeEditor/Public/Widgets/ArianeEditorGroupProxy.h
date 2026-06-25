// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Palette/OdysseyPalette.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "ArianeEditorObjectProxy.h"

#include "ArianeEditorGroupProxy.generated.h"

class FArianeEditor;

UCLASS( HideCategories = "Hidden" )
class ARIANEEDITOR_API UArianeEditorGroupProxy : public UArianeEditorObjectProxy
{
    // we use a bitfields in case we need more than 64 flags
    typedef union {
        struct
        {
            uint32 HUDForegroundColor : 1;
        };
        uint8 raw[1];
    } PropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UArianeEditorGroupProxy();
        UArianeEditorGroupProxy();

    public:
        virtual bool GetPropertyBit( const FName& iPropertyName ) override;
        virtual void SetPropertyBit( const FName& iPropertyName
                                   , const FName& iMemberPropertyName
                                   , const FName& iCategory
                                   , bool iState ) override;
        virtual bool HasAnyPropertyBit() override;
        virtual void ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy ) override;

    protected:
        virtual void ImportParam( const TArray<FArianeObject*>& EditedObjects ) override;
        virtual void ClearPropertyBits() override;
        virtual void ApplyPropertyBits( FArianeObject* Object ) override;

    private:
        PropertyBits GroupPropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Defines the color of the HUD." ) )
        FColor HUDForegroundColor;
};
