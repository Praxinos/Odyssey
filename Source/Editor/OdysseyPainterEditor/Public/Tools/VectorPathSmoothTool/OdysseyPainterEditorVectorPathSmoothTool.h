// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"

#include "OdysseyPainterEditorVectorPathSmoothTool.generated.h"

class FOdysseyPainterEditorVectorPathSmoothToolHUD;

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

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
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
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=PathSmoothTool
                 , meta = ( ToolTip  = "Preserve Handle Length" ) )
        bool PreserveHandleLength;

        UPROPERTY( EditAnywhere
                 , Category=PathSmoothTool
                 , meta = ( ToolTip  = "Restrict To Selected Objects" ) )
        bool RestrictToSelectedObjects;
};
