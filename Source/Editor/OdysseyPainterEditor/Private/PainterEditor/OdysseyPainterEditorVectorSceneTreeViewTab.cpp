// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorSceneTreeViewTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorSceneTreeViewTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorSceneTreeViewTab::~FOdysseyPainterEditorVectorSceneTreeViewTab()
{

}

FOdysseyPainterEditorVectorSceneTreeViewTab::FOdysseyPainterEditorVectorSceneTreeViewTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(TEXT("OdysseyPainterEditor_VectorSceneTreeView"),
                            LOCTEXT( "OdysseyPainterEditorVectorSceneTreeViewTab", "Vector Scene Tree View" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{

}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorSceneTreeViewTab::CreateWidget()
{
    mVectorSceneTreeView = SNew( SOdysseyPainterEditorVectorSceneTreeView );

    return mVectorSceneTreeView;
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::OnRefresh( FOdysseyVectorScene* iScene )
{
    Update( iScene );
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::Update( FOdysseyVectorScene* iScene )
{
    mItemsSource.Empty();
    mItemsSource.Add( TSharedPtr<FOdysseyVectorObject>(iScene) );

    mVectorSceneTreeView.Get()->SetTreeItemsSource( &mItemsSource );
}

#undef LOCTEXT_NAMESPACE
