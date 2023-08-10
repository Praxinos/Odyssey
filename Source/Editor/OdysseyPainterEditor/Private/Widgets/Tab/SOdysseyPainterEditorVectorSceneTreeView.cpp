// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyStyleSet.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorVectorSceneTreeView"

SOdysseyPainterEditorVectorSceneTreeView::~SOdysseyPainterEditorVectorSceneTreeView()
{
}

SOdysseyPainterEditorVectorSceneTreeView::SOdysseyPainterEditorVectorSceneTreeView()
{
}

void SOdysseyPainterEditorVectorSceneTreeView::Construct( const FArguments& InArgs )
{
    STreeView<TSharedPtr<FOdysseyVectorObject>>::Construct(
        STreeView<TSharedPtr<FOdysseyVectorObject>>::FArguments()
        .TreeItemsSource(nullptr)
        //.OnGenerateRow( InArgs._OnGenerateRow )
        .OnGetChildren( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren )
        //.OnExpansionChanged( this, &SOdysseyLayerStackTreeView::OnExpansionChanged )
        //.OnSelectionChanged( this, &SOdysseyLayerStackTreeView::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        //.OnContextMenuOpening( this, &SOdysseyLayerStackTreeView::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren( TSharedPtr<FOdysseyVectorObject> iParent
                                                       , TArray<TSharedPtr<FOdysseyVectorObject>>& oChildren) const
{
    std::list<FOdysseyVectorObject*>& childrenList = iParent->GetChildrenList();

    for( FOdysseyVectorObject* child : childrenList )
    {
        oChildren.Add( TSharedPtr<FOdysseyVectorObject>(child) );
    }
}
