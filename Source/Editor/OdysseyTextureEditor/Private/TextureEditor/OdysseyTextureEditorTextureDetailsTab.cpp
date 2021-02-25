// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorTextureDetailsTab.h"

#include "OdysseyTextureEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorTextureDetailsTab"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorTextureDetailsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorTextureDetailsTab::~FOdysseyTextureEditorTextureDetailsTab()
{
}

FOdysseyTextureEditorTextureDetailsTab::FOdysseyTextureEditorTextureDetailsTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyTextureEditor_TextureDetails"),
                            LOCTEXT( "OdysseyTextureEditorTextureDetailsTab", "Texture Details" ),
                            FSlateIcon( "OdysseyStyle", "TextureEditor.Layers16" ))
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
