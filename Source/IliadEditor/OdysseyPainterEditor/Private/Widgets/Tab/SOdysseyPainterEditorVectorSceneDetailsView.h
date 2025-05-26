// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyPainterEditorVectorObjectView;
class UOdysseyPainterEditorVectorPathView;
class UOdysseyPainterEditorVectorGroupView;
class UOdysseyPainterEditorVectorGroupPaintView;
class UOdysseyPainterEditorVectorTagInbetweenerView;
class FOdysseyVectorGroupPaint;
class FOdysseyPainterEditor;

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneDetailsView
    : public SCompoundWidget
    , public FGCObject
{
    SLATE_DECLARE_WIDGET(SOdysseyPainterEditorVectorSceneDetailsView, SCompoundWidget)

    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorVectorSceneDetailsView)
            {}
            SLATE_ATTRIBUTE(FOdysseyVectorGroupPaint*, Scene)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorVectorSceneDetailsView();
        SOdysseyPainterEditorVectorSceneDetailsView();

        void Construct(const FArguments& InArgs, FOdysseyPainterEditor* iEditor);
        void Update();

    protected:
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
        virtual FString GetReferencerName() const override;

    protected:
        TSharedPtr<IDetailsView> CreateObjectPropertiesPanel();
        void OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
        void ParseVectorNotifications( uint64 iSignalFlags );
        void OnSceneChanged();

    protected:
        FOdysseyPainterEditor* mEditor;
        TSlateAttribute<FOdysseyVectorGroupPaint*> mScene;

        TSharedPtr<IDetailsView> mDetailsView;
        TObjectPtr<UOdysseyPainterEditorVectorObjectView> mObjectView;
        TObjectPtr<UOdysseyPainterEditorVectorPathView> mPathView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupView> mGroupView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupPaintView> mGroupPaintView;
        TObjectPtr<UOdysseyPainterEditorVectorTagInbetweenerView> mTagInbetweenerView;
};
