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

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 OnKeyDownVector( FOdysseyVectorScene* iScene
                                      , const FKey& iKey ) override;
        virtual uint64 OnKeyUpVector( FOdysseyVectorScene* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual uint64 OnMouseHoverVector( FOdysseyVectorScene* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorScene* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual void PropertyChangedVector( FOdysseyVectorScene* iScene
        //                                  , const FName& iPropertyName ) override;

    private:
        FOdysseyPainterEditorVectorPathSmoothToolHUD* mPathSmoothHUD;
        FOdysseyVectorUndoSegmentReshape* mUndoSegmentReshape;

    public:
        UPROPERTY( EditAnywhere, Category = PathSmoothTool )
        ePathSmoothingMode SmoothingMode;
        ePathSmoothingMode SmoothingModeAtKeyDown; // when pressing shift

        UPROPERTY( EditAnywhere, Category = PathSmoothTool, meta = (ClampMin = "0.0", UIMin = "0.0"))
        double PickingRadius;

        UPROPERTY( EditAnywhere, Category = PathSmoothTool )
        bool PreserveHandleLength;

        UPROPERTY( EditAnywhere, Category = PathSmoothTool )
        bool RestrictToSelectedObjects;
};
