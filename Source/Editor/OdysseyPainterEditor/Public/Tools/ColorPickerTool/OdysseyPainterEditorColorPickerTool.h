// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

    virtual FText GetTooltip() const override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

    void PickColorMove( const FOdysseyPoint& iPointInTexture );
    void PickColorUp( const FOdysseyPoint& iPointInTexture );

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine mPaintEngine;
    bool mIsPicking = false;
};
