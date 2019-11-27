// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyTextureDummy/OdysseyTextureDummyFactory.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"

#include "Widgets/SWindow.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"

#include "OdysseySurface.h"
#include "OdysseyBlock.h"


/////////////////////////////////////////////////////
// Defines
#define LOCTEXT_NAMESPACE "OdysseyTextureDummyFactory"
#define MAX_CANVAS_SIZE 8192
#define MIN_CANVAS_SIZE 1
#define DEFAULT_CANVAS_SIZE 1024


/////////////////////////////////////////////////////
// SOdysseyTextureConfigureWindow
class SOdysseyTextureConfigureWindow : public SWindow
{
public:
    void Construct(const FArguments& InArgs)
    {
        Width = DEFAULT_CANVAS_SIZE;
        Height = DEFAULT_CANVAS_SIZE;
        WindowAnswer = false;

        SWindow::Construct(SWindow::FArguments()
            .Title(LOCTEXT("CreateOdysseyTextureAssetOptions", "Create Texture Asset"))
            .SizingRule(ESizingRule::Autosized)
            .SupportsMinimize(false)
            .SupportsMaximize(false)
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("Menu.Background"))
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Texture Width")))
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SNew(SNumericEntryBox<int32>)
                        .LabelVAlign(VAlign_Center)
                        .Value(this, &SOdysseyTextureConfigureWindow::GetWidth)
                        .MinValue(1)
                        .MaxValue(8192)
                        .OnValueCommitted( this, &SOdysseyTextureConfigureWindow::OnSetWidth )
                    ]

                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(2, 2)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Texture Height")))
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SNew(SNumericEntryBox<int32>)
                        .LabelVAlign(VAlign_Center)
                        .Value(this, &SOdysseyTextureConfigureWindow::GetHeight)
                        .MinValue(1)
                        .MaxValue(8192)
                        .OnValueCommitted( this, &SOdysseyTextureConfigureWindow::OnSetHeight )
                    ]

                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    [
                        SNew(SButton)
                        .Text(LOCTEXT("Create Asset", "Create Asset"))
                        .HAlign(HAlign_Center)
                        .OnClicked_Raw(this, &SOdysseyTextureConfigureWindow::OnAccept)
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SNew(SButton)
                        .Text(LOCTEXT("Cancel", "Cancel"))
                        .HAlign(HAlign_Center)
                        .OnClicked_Raw(this, &SOdysseyTextureConfigureWindow::OnCancel)
                    ]
                ]
            ]
        ]);
    }

    bool GetWindowAnswer()
    {
        return WindowAnswer;
    }

    TOptional<int32> GetWidth() const
    {
        return Width;
    }

    TOptional<int32> GetHeight() const
    {
        return Height;
    }

    void OnSetHeight( int32 NewHeightValue, ETextCommit::Type CommitInfo)
    {
        Height = NewHeightValue;

        if (Height > MAX_CANVAS_SIZE)
            Height = MAX_CANVAS_SIZE;
        if (Height < MIN_CANVAS_SIZE)
            Height = MIN_CANVAS_SIZE;
    }

    void OnSetWidth( int32 NewWidthValue, ETextCommit::Type CommitInfo)
    {
        Width = NewWidthValue;

        if (Width > MAX_CANVAS_SIZE)
            Width = MAX_CANVAS_SIZE;
        if (Width < MIN_CANVAS_SIZE)
            Width = MIN_CANVAS_SIZE;
    }

    FReply OnAccept()
    {
        WindowAnswer = true;
        RequestDestroyWindow();
        return FReply::Handled();
    }

    FReply OnCancel()
    {
        WindowAnswer = false;
        RequestDestroyWindow();
        return FReply::Handled();
    }

private:
    int Width;
    int Height;
    bool WindowAnswer;

};



/////////////////////////////////////////////////////
// UOdysseyTextureDummyFactory
UOdysseyTextureDummyFactory::UOdysseyTextureDummyFactory( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyTextureDummy::StaticClass();
}


UObject*
UOdysseyTextureDummyFactory::FactoryCreateNew(  UClass* Class,
                                                UObject* InParent,
                                                FName Name,
                                                EObjectFlags Flags,
                                                UObject* Context,
                                                FFeedbackContext* Warn )
{
    // Make sure we are trying to factory a UOdysseyTextureDummy, then create and init a UTexture2D instead.
    check( Class->IsChildOf(UOdysseyTextureDummy::StaticClass() ) );

    UTexture2D* Object = NewObject<UTexture2D>(InParent, Name, Flags | RF_Transactional);
    Object->Source.Init( textureWidth, textureHeight, 1, 1, TSF_BGRA8 );
    Object->PostEditChange();

    // Init internal data
    FOdysseyBlock block( textureWidth, textureHeight, ETextureSourceFormat::TSF_BGRA8, nullptr, nullptr, true );
    CopyBlockDataIntoUTexture( &block, Object );

    return  Object;
}

bool UOdysseyTextureDummyFactory::ConfigureProperties()
{
    //We go in here before creating the texture: Meaning we can have any modal window here.
    //If return false, we don't create the object, if true, we create it
    TSharedPtr<SOdysseyTextureConfigureWindow> TextureConfigurationWindow = SNew(SOdysseyTextureConfigureWindow);

    GEditor->EditorAddModalWindow(TextureConfigurationWindow.ToSharedRef());
    textureWidth = TextureConfigurationWindow->GetWidth().GetValue();
    textureHeight = TextureConfigurationWindow->GetHeight().GetValue();

    return TextureConfigurationWindow->GetWindowAnswer();
}

#undef LOCTEXT_NAMESPACE

