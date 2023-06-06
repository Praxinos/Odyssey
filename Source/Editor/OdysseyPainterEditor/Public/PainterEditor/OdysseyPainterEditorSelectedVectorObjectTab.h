// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyVector.h"
#include "OdysseyPainterEditorVectorObjectView.h"
#include "OdysseyPainterEditorVectorPathView.h"
#include "OdysseyPainterEditorVectorEllipseView.h"
#include "OdysseyPainterEditorVectorGroupPaintView.h"
#include <ULIS>

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorSelectedVectorObjectTab :
    public FOdysseyEditorTab, public FNotifyHook, public FGCObject
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorSelectedVectorObjectTab();
    FOdysseyPainterEditorSelectedVectorObjectTab(FOdysseyPainterEditor* iEditor);

    UOdysseyPainterEditorVectorObjectView* GetObjectView();
    UOdysseyPainterEditorVectorPathView* GetPathView();
    UOdysseyPainterEditorVectorEllipseView* GetEllipseView();
    UOdysseyPainterEditorVectorGroupPaintView* GetGroupPaintView();

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

public:
    // Event Listeners
    void OnRefresh( FOdysseyVectorScene* iScene );
    void Update( FOdysseyVectorScene* iScene );

protected:
    // Methods

private:
    FOdysseyPainterEditor* mEditor;
    TSharedPtr<IDetailsView> mDetailsView;

    UOdysseyPainterEditorVectorObjectView* mObjectView;
    UOdysseyPainterEditorVectorPathView* mPathView;
    UOdysseyPainterEditorVectorEllipseView* mEllipseView;
    UOdysseyPainterEditorVectorGroupPaintView* mGroupPaintView;
};
