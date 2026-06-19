// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorPathStitchTool.generated.h"

class FOdysseyPainterEditorVectorPathStitchToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathStitchTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathStitchTool();

        //Constructor
        UOdysseyPainterEditorVectorPathStitchTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;
        virtual bool HasRadius() const override;
        virtual float GetRadius() const override;
        virtual void SetRadius(float iRadius)override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;
        virtual void OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                        , uint32 iUpdateFlags ) override;

    private:
        FOdysseyPainterEditorVectorPathStitchToolHUD* mPathStitchHUD;

    public:
        UPROPERTY( EditAnywhere
                 , Category=PathStitchTool
                 , meta = ( ToolTip = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0"
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        uint32 PickingRadius;
/*
        UPROPERTY( EditAnywhere, Category=PathStitchTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        bool RestrictToSelection;
*/
};
