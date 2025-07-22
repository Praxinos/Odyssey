// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                              , const FKey& iKey, uint64& oSignalFlags ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        //virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                 , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
        //virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

    protected:
        void Pan( FOdysseyVectorGroupPaint* iScene
                , const FOdysseyPoint& iPointInTexture );
        void Scale( FOdysseyVectorGroupPaint* iScene
                  , const FOdysseyPoint& iPointInTexture );

    private:
        bool mDragged;
        double mDownLocalMouseX;
        double mDownLocalMouseY;
        FOdysseyPainterEditorVectorScenePanToolHUD* mScenePanHUD;
};
