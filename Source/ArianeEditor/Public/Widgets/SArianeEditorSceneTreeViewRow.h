// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

struct FArianeObject;

class FSceneTreeViewItem
{
    public:
        ~FSceneTreeViewItem();
        FSceneTreeViewItem(FArianeObject* iVectorObject, bool iSensitive);

        FArianeObject* GetObject();
        bool IsSensitive();

    public:
        FArianeObject* Object;
        bool bSensitive;
        TArray<TSharedPtr<FSceneTreeViewItem>> Children;
};

/**
 * Implements the Scene Tree View Widget
 */
class ARIANEEDITOR_API SArianeEditorSceneTreeViewRow
    : public SMultiColumnTableRow<TSharedPtr<FSceneTreeViewItem>>
{
    public:
        ~SArianeEditorSceneTreeViewRow();
        SArianeEditorSceneTreeViewRow();

        static const uint32 DROPZONE_NONE  = 0;
        static const uint32 DROPZONE_ONTO  = 1;
        static const uint32 DROPZONE_ABOVE = 2;
        static const uint32 DROPZONE_BELOW = 3;

        void Construct( const typename STableRow<TSharedPtr<FSceneTreeViewItem>>::FArguments& InArgs
                      , const TSharedRef< STableViewBase >& InOwnerTableView
                      , const TSharedPtr<FSceneTreeViewItem> iItem );
        virtual TSharedRef< SWidget > GenerateWidgetForColumn ( const FName& InColumnName ) override;

        void Rename();
        FReply OnMouseButtonUp( const FGeometry & MyGeometry
                              , const FPointerEvent & MouseEvent ) override;
        FReply OnMouseButtonDown( const FGeometry & MyGeometry
                                , const FPointerEvent & MouseEvent ) override;

    protected:
        virtual FReply OnDragDetected ( const FGeometry& MyGeometry,
                                        const FPointerEvent& MouseEvent ) override;
        virtual FReply OnDragOver( const FGeometry& MyGeometry
                                 , const FDragDropEvent& DragDropEvent ) override;
        virtual void OnDragEnter( const FGeometry& MyGeometry
                                , const FDragDropEvent& DragDropEvent ) override;
        virtual void OnDragLeave( const FDragDropEvent& DragDropEvent ) override;
        virtual FReply OnDrop( const FGeometry& iGeometry
                             , const FDragDropEvent& iDragDropEvent ) override;
        virtual int32 OnPaint( const FPaintArgs& Args
                             , const FGeometry& AllottedGeometry
                             , const FSlateRect& MyCullingRect
                             , FSlateWindowElementList& OutDrawElementList
                             , int32 LayerId
                             , const FWidgetStyle& InWidgetStyle
                             , bool bParentEnabled ) const override;
        void OnTextChanged( const FText& InText, ETextCommit::Type CommitInfo );
        bool OnVerifyTextChanged( const FText& NewText, FText& OutErrorMessage );
        virtual ESelectionMode::Type GetSelectionMode () const;
        ECheckBoxState GetHierarchicalVisibility() const;
        void OnCheckBoxStateChanged( ECheckBoxState iState );
        bool IsVisibilityEnabled() const;
        FReply PickColor( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent );
        void OnColorCommitted( FLinearColor iColor );

    protected:
        uint32 DropZone;
        TSharedPtr<FSceneTreeViewItem> Item;
        TSharedPtr<SInlineEditableTextBlock> TextBlockWidget;
};
