// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyDropWidget.h"

void SOdysseyDropWidget::Construct( const SOdysseyDropWidget::FArguments& InArgs )
{
    mOnDrop = InArgs._OnDrop;
	mOnDragEnter = InArgs._OnDragEnter;
	mOnDragOver = InArgs._OnDragOver;
	mOnDragLeave = InArgs._OnDragLeave;

	ChildSlot
	[
		InArgs._Content.Widget
	];
}

void
SOdysseyDropWidget::OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mOnDragEnter.IsBound())
	{
		mOnDragEnter.Execute(iGeometry, iDragDropEvent);
	}
}

void
SOdysseyDropWidget::OnDragLeave(const FDragDropEvent& iDragDropEvent)
{
	if (mOnDragLeave.IsBound())
	{
		mOnDragLeave.Execute(iDragDropEvent);
	}
}

FReply
SOdysseyDropWidget::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mOnDragOver.IsBound())
	{
		return mOnDragOver.Execute(iGeometry, iDragDropEvent);
	}
	return FReply::Unhandled();
}


FReply
SOdysseyDropWidget::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mOnDrop.IsBound())
	{
		return mOnDrop.Execute(iGeometry, iDragDropEvent);
	}
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE 
