// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/ITableRow.h"
#include "SArianeEditorLayerRow.h"

class UArianePainting3DComponent;
class FArianeEditor;
class FArianeEditorLayerRowItem;
class UArianeLayerStack;

class ARIANEEDITOR_API SArianeEditorLayerStack
    : public STreeView<TSharedPtr<FArianeEditorLayerRowItem>>
{
    public:
        static constexpr const char* LAYER_VISIBLE   = "Visible";
        static constexpr const char* LAYER_LOCKED    = "Locked";
        static constexpr const char* LAYER_NAME      = "Name";
        static constexpr const char* LAYER_TRANSFORM = "Transform";

    public:
/*
        SLATE_BEGIN_ARGS(SArianeEditorLayerStack)
        : _Painting3DComponent(nullptr)
            {}
            SLATE_ATTRIBUTE(UArianePainting3DComponent*, Painting3DComponent)
        SLATE_END_ARGS()
*/
    public:
        // Construction / Destruction
        ~SArianeEditorLayerStack();
        SArianeEditorLayerStack();

        void Construct(const FArguments& InArgs, FArianeEditor* InEditor );

        void Update();

        void RenameSelectedItem();
        void DeleteSelectedItem();

        TSharedPtr<FArianeEditorLayerRowItem> GetRootItem();
        FArianeEditor* GetEditor() const;

    protected:
        /**
         * @brief Called when the treeview asks for the children of a specific item
         *
         * @param iParent
         * @param oChildren
         */
        void OnGetChildren(TSharedPtr<FArianeEditorLayerRowItem> iParent, TArray<TSharedPtr<FArianeEditorLayerRowItem>>& oChildren) const;
        TSharedPtr<SWidget> OnContextMenuOpening();
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FArianeEditorLayerRowItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable );
        void OnSelectionChanged( TSharedPtr<FArianeEditorLayerRowItem> iItem, ESelectInfo::Type SelectInfo );
        void OnExpansionChanged( TSharedPtr<FArianeEditorLayerRowItem> iItem, bool mExpanded );
        void ExpandTree( const TSharedPtr<FArianeEditorLayerRowItem> iItem );
        void BuildTree( const TSharedPtr<FArianeEditorLayerRowItem> iItem );
        //void SelectTree( const TSharedPtr<FArianeEditorLayerRowItem> iItem );

        void MapActionsToCommandList();

        // actions
        void CopySelectedLayers();
        void PasteLayers();
        void RemoveSelectedLayers();
        void SelectAllLayers();
        void CutSelectedLayers();

        FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent );
/*
        void OnVectorLayerNotify( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags
                                , uint32 iUpdateFlags );
        void ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags );
*/
        //void OnPainting3DComponentChanged();
        void OnPre3DPaintingComponentSelectionChanged();
        void OnPost3DPaintingComponentSelectionChanged();
        void UnbindDelegates();
        void BindDelegates();
        void OnPostLayerStackHierarchyChanged();
        void OnPostLayerStackSelectionChanged();
        void ModifyLayerStackAndSelectedLayers( UArianeLayerStack* LayerStack
                                              , const TArray<UArianeLayer*>& OldSelectedLayers
                                              , const TArray<UArianeLayer*>& NewSelectedLayers );

/*
        void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );
        void OnSourceChanged();
        void UpdateCurrentLayer( UOdysseyLayerStack* iLayerStack );
*/
    protected:
        FArianeEditor* Editor;
        //TSlateAttribute<UArianePainting3DComponent*> Painting3DComponent;
        //TSharedPtr<FOdysseyVectorLayer> mVectorLayer;

        TSharedPtr<FArianeEditorLayerRowItem> RootItem;
        TArray<TSharedPtr<FArianeEditorLayerRowItem>> ItemsSource;
        TSharedRef<FUICommandList> CommandList;
};
