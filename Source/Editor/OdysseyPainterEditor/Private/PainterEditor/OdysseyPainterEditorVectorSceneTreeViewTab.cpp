// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorSceneTreeViewTab"

const FName&
FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_VectorSceneTreeView"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorSceneTreeViewTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorSceneTreeViewTab::~FOdysseyPainterEditorVectorSceneTreeViewTab()
{

}

FOdysseyPainterEditorVectorSceneTreeViewTab::FOdysseyPainterEditorVectorSceneTreeViewTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "OdysseyPainterEditorVectorSceneTreeViewTab", "Vector Scene Tree View" ),
                         FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{

}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorVectorSceneTreeViewTab::GetId() const
{
    return StaticId();
}

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
    //mItemsSource.Empty();
    //mItemsSource.Add( MakeShared<FVectorSceneTreeViewItem>(iScene) );

    //mVectorSceneTreeView.Get()->SetTreeItemsSource( &mItemsSource );

    mVectorSceneTreeView.Get()->Update( iScene );
}

#undef LOCTEXT_NAMESPACE
