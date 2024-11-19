// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class SOdysseyPainterEditorVectorSceneTreeView;
class FVectorSceneTreeViewItem;
class FOdysseyVectorGroupPaint;

class UOdysseyPainterEditorVectorObjectView;
class UOdysseyPainterEditorVectorPathView;
class UOdysseyPainterEditorVectorGroupPaintView;
class UOdysseyPainterEditorVectorTagInbetweenerView;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorSceneTreeViewTab :
    public FOdysseyEditorTab, public FGCObject
{
    public:
        static const FName& StaticId();

    public:
        // Construction / Destruction
        virtual ~FOdysseyPainterEditorVectorSceneTreeViewTab();
        FOdysseyPainterEditorVectorSceneTreeViewTab( FOdysseyPainterEditor* iEditor );
        void UpdateObjectPropertiesPanel( FOdysseyVectorGroupPaint* iScene );
        void UpdateSceneTreeView( FOdysseyVectorGroupPaint* iScene );

    protected:
        // FOdysseyEditorTab interface
        virtual const FName& GetId() const override;
        virtual TSharedPtr<SWidget> CreateWidget() override;
        TSharedPtr<IDetailsView> CreateObjectPropertiesPanel();

    public:
        // Event Listeners
        void OnRefresh( FOdysseyVectorGroupPaint* iScene );
        void Update( FOdysseyVectorGroupPaint* iScene );
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
        virtual FString GetReferencerName() const override;
        int WidgetIndex() const;

    private:
        FOdysseyPainterEditor* mEditor;
        TSharedPtr<SOdysseyPainterEditorVectorSceneTreeView> mVectorSceneTreeView;
        TSharedPtr<IDetailsView> mDetailsView;

        TObjectPtr<UOdysseyPainterEditorVectorObjectView> mObjectView;
        TObjectPtr<UOdysseyPainterEditorVectorPathView> mPathView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupPaintView> mGroupPaintView;
        TObjectPtr<UOdysseyPainterEditorVectorTagInbetweenerView> mTagInbetweenerView;

        FOdysseyVectorGroupPaint* mScene = nullptr;
};
