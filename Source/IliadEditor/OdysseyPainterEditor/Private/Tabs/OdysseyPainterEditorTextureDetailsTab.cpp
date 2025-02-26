// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorTextureDetailsTab.h"

#include "SOdysseyTextureDetails.h"
#include "Texture/OdysseyTextureEditorSource.h"
#include "Texture/OdysseyTextureEditorExtension.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

const FName&
FOdysseyPainterEditorTextureDetailsTab::StaticId()
{
    static FName Id = TEXT("OdysseyTextureEditor_TextureDetails"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTextureDetailsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTextureDetailsTab::~FOdysseyPainterEditorTextureDetailsTab()
{
}

FOdysseyPainterEditorTextureDetailsTab::FOdysseyPainterEditorTextureDetailsTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "texture-details-tab.name", "Texture Details" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Trombone16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorTab interface

const FName&
FOdysseyPainterEditorTextureDetailsTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorTextureDetailsTab::CreateWidget()
{
    return SNew( SOdysseyTextureDetails )
        .Texture( this, &FOdysseyPainterEditorTextureDetailsTab::Texture );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UTexture*
FOdysseyPainterEditorTextureDetailsTab::Texture() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);

    return textureSource->GetTexture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
