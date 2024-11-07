// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"

class FOdysseyVectorObject;

class FVectorSceneTreeViewItem
{
    public:
        ~FVectorSceneTreeViewItem();
        FVectorSceneTreeViewItem(FOdysseyVectorObject* iVectorObject);

        FOdysseyVectorObject* GetVectorObject();

    public:
        FOdysseyVectorObject* mVectorObject;
        TArray<TSharedPtr<FVectorSceneTreeViewItem>> mChildren;
};

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneTreeViewRow
    : public STableRow<TSharedPtr<FVectorSceneTreeViewItem>>
{
    public:
        ~SOdysseyPainterEditorVectorSceneTreeViewRow();
        SOdysseyPainterEditorVectorSceneTreeViewRow();

        static const uint32 DROPZONE_NONE  = 0;
        static const uint32 DROPZONE_ONTO  = 1;
        static const uint32 DROPZONE_ABOVE = 2;
        static const uint32 DROPZONE_BELOW = 3;

        void Construct( const typename STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments& InArgs
                      , const TSharedRef< STableViewBase >& InOwnerTableView
                      , const TSharedPtr<FVectorSceneTreeViewItem> iItem );

        void Rename();

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

    protected:
        uint32 mDropZone;
        TSharedPtr<FVectorSceneTreeViewItem> mItem;
        TSharedPtr<SInlineEditableTextBlock> mTextBlockWidget;
};
