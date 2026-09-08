// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IDetailPropertyExtensionHandler.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"


struct FArianeGroup;
class FArianeEditor;
class UArianeEditorObjectProxy;
class UArianeEditorPathProxy;
class UArianeEditorGroupProxy;
class UArianeEditorEllipseProxy;
class UArianeEditorRectangleProxy;
class UArianeEditorLineProxy;
class UArianeEditorPolygonProxy;
class UArianeEditorCycleProxy;

class ARIANEEDITOR_API SArianeEditorCurrentObjectDetailsView
    : public SCompoundWidget
    , public FGCObject
{
    public:
        SLATE_BEGIN_ARGS(SArianeEditorCurrentObjectDetailsView)
            {}
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SArianeEditorCurrentObjectDetailsView();
        SArianeEditorCurrentObjectDetailsView();

        void Construct(const FArguments& InArgs, FArianeEditor* Editor );
        void Update();

        void BindComponentDelegates();
        void UnbindComponentDelegates();

    protected:
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
        virtual FString GetReferencerName() const override;
        FArianeGroup* GetRootGroup();

    protected:
        TSharedPtr<IDetailsView> CreateObjectPropertiesPanel();
        void PropertyValueChanged( const FPropertyChangedEvent& iEvent );
        void OnPre3DPaintingComponentSelectionChanged();
        void OnPost3DPaintingComponentSelectionChanged();
        void OnPreLayerStackSelectionChanged();
        void OnPostLayerStackSelectionChanged();
        void OnPreLayerStackHierarchyChanged();
        void OnPostLayerStackHierarchyChanged();
        void OnPrePainting3DComponentUpdate( bool bInteractive );
        void OnPostPainting3DComponentUpdate( bool bInteractive );
        void OnPostImageChanged();

    protected:
        FArianeEditor* Editor;
        bool bDoUpdate;
        TSharedPtr<IDetailsView> DetailsView;
        UArianeEditorObjectProxy* CurrentObjectProxy;
        UArianeEditorObjectProxy* ObjectProxy;
        UArianeEditorPathProxy* PathProxy;
        UArianeEditorGroupProxy* GroupProxy;
        UArianeEditorEllipseProxy* EllipseProxy;
        UArianeEditorRectangleProxy* RectangleProxy;
        UArianeEditorLineProxy* LineProxy;
        UArianeEditorPolygonProxy* PolygonProxy;
        UArianeEditorCycleProxy* CycleProxy;
};
