// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "TextureEditor/OdysseyTextureEditorTextureDetailsTab.h"

#include "Widgets/SOdysseyTextureDetails.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

const FName&
FOdysseyTextureEditorTextureDetailsTab::StaticId()
{
    static FName Id = TEXT("OdysseyTextureEditor_TextureDetails"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyTextureEditorTextureDetailsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorTextureDetailsTab::~FOdysseyTextureEditorTextureDetailsTab()
{
}

FOdysseyTextureEditorTextureDetailsTab::FOdysseyTextureEditorTextureDetailsTab(FOdysseyTextureEditorExtension* iExtension)
    : FOdysseyEditorTab( LOCTEXT( "texture-details-tab.name", "Texture Details" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Trombone16" ))
    , mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorTab interface

const FName&
FOdysseyTextureEditorTextureDetailsTab::GetId() const
{
    return StaticId();
}

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
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);

    return textureSource->GetTexture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
