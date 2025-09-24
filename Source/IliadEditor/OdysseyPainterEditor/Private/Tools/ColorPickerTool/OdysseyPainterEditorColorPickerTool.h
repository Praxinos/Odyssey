// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyVector.h"

#include "OdysseyPainterEditorColorPickerTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorColorPickerTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorColorPickerTool();

    //Constructor
    UOdysseyPainterEditorColorPickerTool();

    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    //OdysseyPainterEditorTool overrides
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

    virtual void Commit() override;
    virtual bool IsActivable() const override;

    virtual FText GetTooltip() const override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

    void PickColorMove( const FOdysseyPoint& iPointInTexture );
    void PickColorUp( const FOdysseyPoint& iPointInTexture );

public:
    virtual bool IsSameAs(const UOdysseyPainterEditorTool* Other) const override;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine mPaintEngine;
    bool mIsPicking = false;
};
