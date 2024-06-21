// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorScenePanTool.generated.h"

class FOdysseyPainterEditorVectorScenePanToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorScenePanTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorScenePanTool();

        //Constructor
        UOdysseyPainterEditorVectorScenePanTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual uint64 OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                              , const FKey& iKey ) override;
        //virtual uint64 OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        //virtual uint64 OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                 , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

    protected:
        void Pan( FOdysseyVectorEngine* iEngine
                , FOdysseyVectorGroupPaint* iScene
                , const FOdysseyPoint& iPointInTexture );
        void Scale( FOdysseyVectorEngine* iEngine
                  , FOdysseyVectorGroupPaint* iScene
                  , const FOdysseyPoint& iPointInTexture );

    private:
        bool mDragged;
        double mDownLocalMouseX;
        double mDownLocalMouseY;
        FOdysseyPainterEditorVectorScenePanToolHUD* mScenePanHUD;
};
