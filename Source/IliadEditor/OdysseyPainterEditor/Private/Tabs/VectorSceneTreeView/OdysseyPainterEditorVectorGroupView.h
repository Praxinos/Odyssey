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
#include "OdysseyPainterEditorVectorObjectView.h"

#include "OdysseyPainterEditorVectorGroupView.generated.h"

class FOdysseyPainterEditor;

UCLASS( HideCategories = "Hidden" )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGroupView : public UOdysseyPainterEditorVectorObjectView
{
    // we use a bitfields in case we need more than 64 flags
    typedef union {
        struct
        {
            uint32 HUDColor : 1;
        };
        uint8 raw[1];
    } PropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorGroupView();
        UOdysseyPainterEditorVectorGroupView();

    public:
        virtual bool GetPropertyBit( const FName& iPropertyName ) override;
        virtual void SetPropertyBit( const FName& iPropertyName
                                   , const FName& iMemberPropertyName
                                   , const FName& iCategory
                                   , bool iState ) override;
        virtual bool HasAnyPropertyBit() override;
        virtual void ImportParamFromOtherView( UOdysseyPainterEditorVectorObjectView* iOtherView ) override;

    protected:
        virtual void ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList ) override;
        virtual void ClearPropertyBits() override;
        virtual void ApplyPropertyBits( FOdysseyVectorObject* iObject ) override;

    private:
        PropertyBits mGroupPropertyBits;

    public:
        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "Defines the color of the HUD." ) )
        FColor HUDColor;
};
