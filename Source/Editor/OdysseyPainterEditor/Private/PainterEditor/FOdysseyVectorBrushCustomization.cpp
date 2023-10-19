#include "FOdysseyVectorBrushCustomization.h"
#include "OdysseyVectorBrush.h"
#include "ContentBrowserModule.h" // for FContentBrowserModule
#include "IContentBrowserSingleton.h" // for FAssetPickerConfig
#include "PropertyCustomizationHelpers.h" // for SObjectPropertyEntryBox

#include "SlateBasics.h"

#define LOCTEXT_NAMESPACE "OdysseyVectorBrushCustomization"

void
FOdysseyVectorBrushCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> StructPropertyHandle
	                            , class FDetailWidgetRow& HeaderRow
	                            , IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
    mAssetThumbnailPool = MakeShareable( new FAssetThumbnailPool( 1024 ) );

    mBrushIcon = MakeShareable( new FSlateBrush() );
    mBrushIcon.Get()->SetResourceObject(nullptr);
    mBrushIcon.Get()->ImageSize.X = 64;
    mBrushIcon.Get()->ImageSize.Y = 16;
    //mBrushIcon->DrawAs = ImageType;

     // the hell with this syntax....
    TAttribute<const FSlateBrush*> BrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateRaw( this, &FOdysseyVectorBrushCustomization::UpdateButton, StructPropertyHandle ));

    mBrushButton = SNew(SButton)
			      .Visibility( EVisibility::Visible )
			      .Text( LOCTEXT("VectorBrushButton", "Vector Brush Button") )
			      .ToolTipText( LOCTEXT("VectorBrushButtonToolTip", "Vector Brush Button") )
			      .OnClicked(this, &FOdysseyVectorBrushCustomization::OnClicked, StructPropertyHandle )
			      .HAlign(HAlign_Center)
			      .VAlign(VAlign_Center)
                  [
                      SNew(SImage)
                      .Image( BrushAttribute )
                  ];

    UpdateButton( StructPropertyHandle );

    // We don't use the SObjectPropertyEntryBox widget because I had troubles with setting the filter.
    // MoreOver, this widget was taking to much room, so I chose to use a SButton instead and display
    // an asset picker on mouse click.
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
	.ValueContent()[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			mBrushButton.ToSharedRef()
/*
            SNew(SObjectPropertyEntryBox)
                .AllowedClass(          UTexture2D::StaticClass() )
                .ObjectPath(            this, &FOdysseyVectorBrushCustomization::ObjectPath, StructPropertyHandle )
                .ThumbnailPool(         mAssetThumbnailPool )
                .OnObjectChanged(       this, &FOdysseyVectorBrushCustomization::OnObjectChanged, StructPropertyHandle )
                .OnShouldFilterAsset(   this, &FOdysseyVectorBrushCustomization::FilterAsset )
                .AllowClear(            true )
                .DisplayUseSelected(    true )
                .DisplayBrowse(         true )
                .EnableContentPicker(   true )
                .DisplayCompactSize(    true )
                .DisplayThumbnail(      true )
                .AllowCreate(           true )
                .ThumbnailSizeOverride( FIntPoint( 64, 16 ) )
*/
		]
	];
}

FReply
FOdysseyVectorBrushCustomization::OnClicked( TSharedRef<IPropertyHandle> StructPropertyHandle )
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    FString brushPath = FPaths::ProjectContentDir() + FString("/Brushes/Vector");
    // Configure filter for asset picker
    FAssetPickerConfig Config;

    Config.InitialAssetViewType = EAssetViewType::List;
    Config.ThumbnailScale = 1.0f; // make thumbnails as small as possible
    Config.OnAssetSelected = FOnAssetSelected::CreateRaw( this, &FOdysseyVectorBrushCustomization::OnAssetSelected, StructPropertyHandle );
    // All the filter I tried didn't work, so I ended up filtering via the OnShouldFilterAsset delegate
    Config.OnShouldFilterAsset = FOnShouldFilterAsset::CreateRaw( this, &FOdysseyVectorBrushCustomization::FilterAsset );
    Config.bAllowNullSelection = true;
    Config.bAllowDragging = false;

    FSlateApplication::Get().AddModalWindow( SNew(SWindow)
                                              .Title( LOCTEXT( "Pick Vector Brush", "Pick Vector Brush" ) )
                                              .ClientSize( FVector2D( 256, 256 ) )
                                               [
                                                   SNew(SBox)
                                                   .WidthOverride(300.f)
                                                   .HeightOverride(300.f)
                                                   [
                                                       ContentBrowserModule.Get().CreateAssetPicker(Config)
                                                   ]
                                               ]
                                              , mBrushButton
                                              , false
                                             );

    return FReply::Handled();
}

bool
FOdysseyVectorBrushCustomization::FilterAsset( const struct FAssetData& InAssetData )
{
    static FString vectorBrushesDir = FString("/Brushes/Vector");
    // If the asset lies in the Vector Brush directory, show it (return false).
    // It was the only way to have the content browser display the vector brushes onyl.
    // All other solutions I've tried using the FAssetPickerConfig.Filter didn't seem
    // to offer the possibility to filter a specific folder.
    if( InAssetData.ObjectPath.ToString().Contains( vectorBrushesDir ) )
    {
        return false;
    }

    return true;
}

//FString
void
FOdysseyVectorBrushCustomization::OnPropertyValueChanged( TSharedRef<IPropertyHandle> StructPropertyHandle ) 
{
    FProperty *property = StructPropertyHandle.Get().GetProperty();
    TArray<UObject*> OuterObjects;

    StructPropertyHandle.Get().GetOuterObjects( OuterObjects );

    if( OuterObjects.Num() == 1 )
    {
        UObject* OuterObject = OuterObjects[0];
        const FOdysseyVectorBrush* vectorBrush = property->ContainerPtrToValuePtr<FOdysseyVectorBrush>( OuterObject, 0 ); 

        mBrushIcon.Get()->SetResourceObject( vectorBrush->texture );
    }
}

const FSlateBrush*
FOdysseyVectorBrushCustomization::UpdateButton( TSharedRef<IPropertyHandle> StructPropertyHandle )
{
    FProperty *property = StructPropertyHandle.Get().GetProperty();
    TArray<UObject*> OuterObjects;

    StructPropertyHandle.Get().GetOuterObjects( OuterObjects );

    if( OuterObjects.Num() == 1 )
    {
        UObject* OuterObject = OuterObjects[0];
        const FOdysseyVectorBrush* vectorBrush = property->ContainerPtrToValuePtr<FOdysseyVectorBrush>( OuterObject, 0 ); 

        mBrushIcon.Get()->SetResourceObject( vectorBrush->texture );
    }

    return mBrushIcon.Get();
}

void
FOdysseyVectorBrushCustomization::OnAssetSelected( const FAssetData& AssetData
                                                 , TSharedRef<IPropertyHandle> StructPropertyHandle )
{
    UTexture2D* texture = AssetData.IsValid() ? CastChecked<UTexture2D>( AssetData.GetAsset() ) : nullptr;
    FProperty *property = StructPropertyHandle.Get().GetProperty();
    TArray<UObject*> OuterObjects;

    StructPropertyHandle.Get().GetOuterObjects( OuterObjects );

    if( texture )
    {
        // We have to change the texture settings or else we won't be able to read the pixels.
        // This loader is only vector-brush oriented, so it does not really matter, we can
        // leave the textures that way.
        texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        texture->SRGB = false;
        texture->UpdateResource();
    }

    mBrushIcon.Get()->SetResourceObject( texture );

    for ( int i = 0; i < OuterObjects.Num(); i++ )
    {
        UObject* OuterObject = OuterObjects[0];
        FPropertyChangedEvent propertyChangedEvent = FPropertyChangedEvent( property
                                                                          , EPropertyChangeType::ValueSet
                                                                          , OuterObjects );
        FOdysseyVectorBrush* vectorBrush = property->ContainerPtrToValuePtr<FOdysseyVectorBrush>( OuterObject, 0 ); 

        vectorBrush->texture = texture;
        

        OuterObject->PostEditChangeProperty( propertyChangedEvent );
    }
}

void
FOdysseyVectorBrushCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> StructPropertyHandle
                                  , class IDetailChildrenBuilder& StructBuilder
		                          , IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
//    TSharedPtr<IPropertyHandle> textureProperty = StructPropertyHandle->GetChildHandle("texture");

//    textureProperty->SetOnPropertyValueChanged( FSimpleDelegate::CreateRaw( this, &FOdysseyVectorBrushCustomization::OnPropertyValueChanged ) );
}

//static
void
FOdysseyVectorBrushCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
    PropertyModule.RegisterCustomPropertyTypeLayout( FOdysseyVectorBrush::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FOdysseyVectorBrushCustomization::MakeInstance ) );

    PropertyModule.NotifyCustomizationModuleChanged();
}

//static
void
FOdysseyVectorBrushCustomization::Unregister()
{
	if( FModuleManager::Get().IsModuleLoaded( "PropertyEditor" ) )
	{
		// unregister properties when the module is shutdown
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FOdysseyVectorBrush::StaticStruct()->GetFName());

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE