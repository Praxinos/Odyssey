// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelVisibilityTrack/LevelVisibilitySectionOriginal.h"
#include "Sections/MovieSceneLevelVisibilitySection.h"
#include "Misc/PackageName.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "SequencerSectionPainter.h"
#include "SDropTarget.h"
#include "DragAndDrop/LevelDragDropOp.h"
#include "ScopedTransaction.h"

namespace LevelVisibilitySection
{
	constexpr int32 MaxNumLevelsToShow = 3;
}

FLevelVisibilitySectionOriginal::FLevelVisibilitySectionOriginal( UMovieSceneLevelVisibilitySection& InSectionObject )
	: SectionObject( InSectionObject )
{
	VisibleText = NSLOCTEXT( "LevelVisibilitySection", "VisibleHeader", "Visible" );
	HiddenText = NSLOCTEXT( "LevelVisibilitySection", "HiddenHeader", "Hidden" );
}

UMovieSceneSection* FLevelVisibilitySectionOriginal::GetSectionObject()
{
	return &SectionObject;
}

TSharedRef<SWidget> FLevelVisibilitySectionOriginal::GenerateSectionWidget()
{
	return
		SNew( SDropTarget )
		.OnAllowDrop( this, &FLevelVisibilitySectionOriginal::OnAllowDrop )
		.OnDrop( this, &FLevelVisibilitySectionOriginal::OnDrop )
		.Content()
		[
			SNew( SBorder )
			.BorderBackgroundColor( this, &FLevelVisibilitySectionOriginal::GetBackgroundColor )
			.BorderImage( FCoreStyle::Get().GetBrush( "WhiteBrush" ) )
			[
				SNew( STextBlock )
				.Text( this, &FLevelVisibilitySectionOriginal::GetVisibilityText )
				.ToolTipText( this, &FLevelVisibilitySectionOriginal::GetVisibilityToolTip )
			]
		];
}


int32 FLevelVisibilitySectionOriginal::OnPaintSection( FSequencerSectionPainter& InPainter ) const
{
	return InPainter.PaintSectionBackground();
}


FSlateColor FLevelVisibilitySectionOriginal::GetBackgroundColor() const
{
	return SectionObject.GetVisibility() == ELevelVisibility::Visible
		? FSlateColor( FLinearColor::Green.Desaturate( .5f ) )
		: FSlateColor( FLinearColor::Red.Desaturate( .5f ) );
}


FText FLevelVisibilitySectionOriginal::GetVisibilityText() const
{
	TArray<FString> LevelNameStrings;
	LevelNameStrings.Reserve(LevelVisibilitySection::MaxNumLevelsToShow);
	
	int32 Count = 0;
	for ( ; Count < LevelVisibilitySection::MaxNumLevelsToShow && Count < SectionObject.GetLevelNames().Num(); Count++ )
	{
		LevelNameStrings.Add( SectionObject.GetLevelNames()[Count].ToString() );
	}

	int32 NumRemaining = SectionObject.GetLevelNames().Num() - Count;
	FString LevelsText = FString::Join( LevelNameStrings, TEXT( ", " ) );

	if (SectionObject.GetLevelNames().Num() > LevelVisibilitySection::MaxNumLevelsToShow )
	{
		LevelsText.Append( FString::Format(TEXT(" (+{0} more)"), { FString::FormatAsNumber(NumRemaining) } ) );
	}

	if (LevelsText.IsEmpty())
	{
		FText VisibilityText = SectionObject.GetVisibility() == ELevelVisibility::Visible ? VisibleText : HiddenText;

		return VisibilityText;
	}

	return FText::Format( NSLOCTEXT( "LevelVisibilitySection", "SectionTextFormat", "{0}" ), FText::FromString( LevelsText ) );
}

FText FLevelVisibilitySectionOriginal::GetVisibilityToolTip() const
{
	TArray<FString> LevelNameStrings;
	for ( const FName& LevelName : SectionObject.GetLevelNames() )
	{
		LevelNameStrings.Add( LevelName.ToString() );
	}

	FText VisibilityText = SectionObject.GetVisibility() == ELevelVisibility::Visible ? VisibleText : HiddenText;
	return FText::Format( NSLOCTEXT( "LevelVisibilitySection", "ToolTipFormat", "{0}\r\n{1}" ), VisibilityText, FText::FromString( FString::Join( LevelNameStrings, TEXT( "\r\n" ) ) ) );
}


bool FLevelVisibilitySectionOriginal::OnAllowDrop( TSharedPtr<FDragDropOperation> DragDropOperation )
{
	return DragDropOperation->IsOfType<FLevelDragDropOp>() && StaticCastSharedPtr<FLevelDragDropOp>( DragDropOperation )->StreamingLevelsToDrop.Num() > 0;
}


FReply FLevelVisibilitySectionOriginal::OnDrop( TSharedPtr<FDragDropOperation> DragDropOperation )
{
	if ( DragDropOperation->IsOfType<FLevelDragDropOp>() )
	{
		TSharedPtr<FLevelDragDropOp> LevelDragDropOperation = StaticCastSharedPtr<FLevelDragDropOp>( DragDropOperation );
		if ( LevelDragDropOperation->StreamingLevelsToDrop.Num() > 0 )
		{
			FScopedTransaction Transaction(NSLOCTEXT("LevelVisibilitySection", "TransactionText", "Add Level(s) to Level Visibility Section"));
			SectionObject.Modify();

			TArray<FName> LevelNames = SectionObject.GetLevelNames();
			for ( TWeakObjectPtr<ULevelStreaming> Level : LevelDragDropOperation->StreamingLevelsToDrop )
			{
				if ( Level.IsValid() )
				{
					FName ShortLevelName = FPackageName::GetShortFName( Level->GetWorldAssetPackageFName() );
					LevelNames.AddUnique( ShortLevelName );
				}
			}

			SectionObject.SetLevelNames(LevelNames);

			return FReply::Handled();
		}
	}
	return FReply::Unhandled();
}

