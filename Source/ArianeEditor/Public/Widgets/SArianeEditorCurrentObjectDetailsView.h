// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IDetailPropertyExtensionHandler.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"

class UOdysseyPainterEditorVectorObjectView;
class UOdysseyPainterEditorVectorPathView;
class UOdysseyPainterEditorVectorGroupView;
class UOdysseyPainterEditorVectorGroupPaintView;
class UOdysseyPainterEditorVectorTagInbetweenerView;
class FOdysseyVectorGroupPaint;
class FOdysseyPainterEditor;
class FOdysseyVectorLayer;
class UOdysseyLayerStack;
struct FPropertyChangedEvent;
struct FOdysseyVectorObjectInvalidationFlags;

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
            SLATE_ARGUMENT(FOdysseyPainterEditor*, Editor)
            SLATE_ATTRIBUTE(FOdysseyVectorGroupPaint*, Scene)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorVectorSceneDetailsView();
        SOdysseyPainterEditorVectorSceneDetailsView();

        void Construct(const FArguments& InArgs );
        void Update();

        void BindLayerDelegates();
        void UnbindLayerDelegates();

    protected:
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
        virtual FString GetReferencerName() const override;

    protected:
        TSharedPtr<IDetailsView> CreateObjectPropertiesPanel();
        void OnVectorLayerNotify( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                , uint32 iUpdateFlags );
        void ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags );
        void OnSceneChanged();
        void OnSourceChanged();
        void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );
        void PropertyValueChanged( const FPropertyChangedEvent& iEvent );
        void UpdateCurrentLayer( UOdysseyLayerStack* iLayerStack );

    protected:
        FOdysseyPainterEditor* mEditor;
        TSlateAttribute<FOdysseyVectorGroupPaint*> mScene;
        TSharedPtr<FOdysseyVectorLayer> mVectorLayer;

        TSharedPtr<IDetailsView> mDetailsView;
        TObjectPtr<UOdysseyPainterEditorVectorObjectView> mCurrentObjectView;
        TObjectPtr<UOdysseyPainterEditorVectorObjectView> mObjectView;
        TObjectPtr<UOdysseyPainterEditorVectorPathView> mPathView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupView> mGroupView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupPaintView> mGroupPaintView;
        TObjectPtr<UOdysseyPainterEditorVectorTagInbetweenerView> mTagInbetweenerView;
};
