// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorScenePanTool.generated.h"

class FOdysseyPainterEditorVectorScenePanToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorScenePanTool : public UOdysseyPainterEditorDefaultTool
{
    public:
        GENERATED_BODY()

    public:
        static void OnMouseDragVectorStatic( FOdysseyVectorEngine* iEngine
                                           , FOdysseyVectorScene* iScene
                                           , const FOdysseyPoint& iPointInTexture );

        // Destructor
        virtual ~UOdysseyPainterEditorVectorScenePanTool();

        //Constructor
        UOdysseyPainterEditorVectorScenePanTool();

        virtual bool IsActivable() const override;
        virtual void Load() override;
        virtual void Unload() override;

        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

        void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
        void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
        bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FOdysseyPoint& iPointInTexture
                              , const FKey& iKey );
        void OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                               , FOdysseyVectorScene* iScene
                               , const FOdysseyPoint& iPointInTexture );
        void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FOdysseyPoint& iPointInTexture );
        bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                            , FOdysseyVectorScene* iScene
                            , const FOdysseyPoint& iPointInTexture
                            , const FKey& iKey );

        //OdysseyPainterEditorTool overrides
        virtual void Commit() override;

    private:
        double mDownLocalMouseX;
        double mDownLocalMouseY;
        FOdysseyPainterEditorVectorScenePanToolHUD* mScenePanHUD;
};
