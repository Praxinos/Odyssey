// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorTextureDetailsTab.h"

#include "TextureEditor/OdysseyTextureEditor.h"
#include "SOdysseyTextureDetails.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorTextureDetailsTab"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorTextureDetailsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorTextureDetailsTab::~FOdysseyTextureEditorTextureDetailsTab()
{
}

FOdysseyTextureEditorTextureDetailsTab::FOdysseyTextureEditorTextureDetailsTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyTextureEditor_TextureDetails")
    , LOCTEXT( "OdysseyTextureEditorTextureDetailsTab", "Texture Details" )
    , FSlateIcon( "OdysseyStyle", "PainterEditor.Trombone16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorTab interface

TSharedPtr<SWidget>
FOdysseyTextureEditorTextureDetailsTab::CreateWidget()
{
    return SNew( SOdysseyTextureDetails )
        .Texture( this, &FOdysseyTextureEditorTextureDetailsTab::Texture );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UTexture*
FOdysseyTextureEditorTextureDetailsTab::Texture() const
{
    return mEditor->Texture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
