// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorPathSmoothTool.generated.h"

class FOdysseyPainterEditorVectorPathSmoothToolHUD;
class FOdysseyVectorUndoSegmentReshape;

UENUM()
enum class ePathSmoothingMode : uint8
{
    Round = 0,
    Sharp = 1
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathSmoothTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathSmoothTool();

        //Constructor
        UOdysseyPainterEditorVectorPathSmoothTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey ) override;
        virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
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
        //virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

    private:
        FOdysseyPainterEditorVectorPathSmoothToolHUD* mPathSmoothHUD;
        FOdysseyVectorUndoSegmentReshape* mUndoSegmentReshape;

    public:
        UPROPERTY( EditAnywhere
                 , Category=PathSmoothTool
                 , meta = ( ToolTip  = "Smoothing Mode" ) )
        ePathSmoothingMode SmoothingMode;
        ePathSmoothingMode SmoothingModeAtKeyDown; // when pressing shift

        UPROPERTY( EditAnywhere
                 , Category=PathSmoothTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0" ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=PathSmoothTool
                 , meta = ( ToolTip  = "Preserve Handle Length" ) )
        bool PreserveHandleLength;

        UPROPERTY( EditAnywhere
                 , Category=PathSmoothTool
                 , meta = ( ToolTip  = "Restrict To Selected Objects" ) )
        bool RestrictToSelectedObjects;
};
