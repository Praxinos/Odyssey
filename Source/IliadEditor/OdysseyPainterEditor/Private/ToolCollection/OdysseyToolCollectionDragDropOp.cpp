// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyToolCollectionDragDropOp.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyToolCollection.h"
#include "OdysseyPainterEditorToolConfiguration.h"

TSharedRef<FOdysseyToolCollectionDragDropOp>
FOdysseyToolCollectionDragDropOp::Create(UOdysseyPainterEditorToolConfiguration* iToolConfig, UOdysseyToolCollection* iSource, FOdysseyPainterEditor* iEditor)
{
    TSharedRef<FOdysseyToolCollectionDragDropOp> op = MakeShareable(new FOdysseyToolCollectionDragDropOp);
    op->mToolConfig = iToolConfig;
    op->mSourceCollection = iSource;
    op->mEditor = iEditor;
    op->Construct();
    return op;
}

TSharedPtr<SWidget>
FOdysseyToolCollectionDragDropOp::GetDefaultDecorator() const
{
    return SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("Graph.ConnectorFeedback.Border")) //weird, but everyone does this
        .Content()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0.f, 0.f, 2.f, 0.f)
                [
                    SNew(SImage)
                        .Image(this, &FOdysseyToolCollectionDragDropOp::GetIcon)
                ]
                /*+ SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                        .Text(this, &FOdysseyToolCollectionDragDropOp::GetText)
                ]*/
        ];
}

UOdysseyPainterEditorToolConfiguration* FOdysseyToolCollectionDragDropOp::GetToolConfig() const
{
    return mToolConfig;
}

TWeakObjectPtr<UOdysseyToolCollection> FOdysseyToolCollectionDragDropOp::GetSourceCollection() const
{
    return mSourceCollection;
}

FOdysseyPainterEditor* FOdysseyToolCollectionDragDropOp::GetEditor() const
{
    return mEditor;
}

FText
FOdysseyToolCollectionDragDropOp::GetText() const
{
    return FText::FromString(mToolConfig ? mToolConfig->GetName() : TEXT("Tool"));
}

const FSlateBrush*
FOdysseyToolCollectionDragDropOp::GetIcon() const
{
    if (!mToolConfig)
        return FAppStyle::GetBrush("ClassIcon.Default");
    else
        return &mToolConfig->mIcon;
}
