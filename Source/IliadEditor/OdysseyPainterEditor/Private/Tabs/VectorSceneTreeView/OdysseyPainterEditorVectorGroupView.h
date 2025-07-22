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

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGroupView : public UOdysseyPainterEditorVectorObjectView
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorGroupView();
        UOdysseyPainterEditorVectorGroupView();

    protected:
        virtual void ImportParam() override;
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory ) override;

    public:
        UPROPERTY( EditAnywhere
                 , Category = Appearance
                 , meta = ( ToolTip = "HUD Color" ) )
        FColor HUDColor;
};
