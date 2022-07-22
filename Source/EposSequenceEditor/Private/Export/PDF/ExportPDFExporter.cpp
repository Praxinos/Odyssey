// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/ExportPDFExporter.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "Input/HittestGrid.h"
#include "ISequencer.h"
#include "Serialization/BufferArchive.h"
#include "Slate/WidgetRenderer.h"

#include "Export/ExportStruct.h"
#include "Export/SceneRenderer.h"
#include "Export/PDF/ExportPDFSettings.h"
#include "Export/PDF/ExportPDFSheetWidget.h"

#define LOCTEXT_NAMESPACE "ExportPDFExporter"

//---

FExportPDFExporter::FExportPDFExporter( TWeakPtr<ISequencer> iSequencer, const FExportStruct* iStruct, const FExportPDFOptions* iOptions )
    : mSequencer( iSequencer )
    , mStruct( iStruct )
    , mPDFOptions( iOptions )
{
}

bool
FExportPDFExporter::Export()
{
    UExportPDFSheetWidget* pdf_widget = nullptr;
    UClass* pdf_sheet_class = mPDFOptions->SheetClassPath.TryLoadClass<UExportPDFSheetWidget>();
    if( !pdf_sheet_class )
        return false;

    UWorld* world = GEditor->GetEditorWorldContext().World();
    pdf_widget = CreateWidget<UExportPDFSheetWidget>( world, pdf_sheet_class );
    check( pdf_widget );

    pdf_widget->OnConstructPDFLayout( *mStruct );

    //--- From ...\Editor\UMGEditor\Private\WidgetBlueprintEditorUtils.cpp#2183 : FWidgetBlueprintEditorUtils::DrawSWidgetInRenderTargetInternal(...)

    FVector2D Offset( 0.f, 0.f );
    FVector2D ScaledSize( 1920, 1080 );
    //FVector2D ScaledSize( 0.f, 0.f );
    TSharedPtr<SWidget> WindowContent = pdf_widget->TakeWidget();

    TSharedRef<SVirtualWindow> Window = SNew( SVirtualWindow );
    TUniquePtr<FHittestGrid> HitTestGrid = MakeUnique<FHittestGrid>();
    Window->SetContent( WindowContent.ToSharedRef() );
    Window->Resize( ScaledSize );
    //Window->Resize( FVector2D( 1920, 1080 ) );

    //-

    Window->SlatePrepass( 1.0f );
    FVector2D DesiredSizeWindow = Window->GetDesiredSize();

    //ScaledSize = FVector2D( 1920, 1080 );
    //ScaledSize = UnscaledSize * Scale;

    //---

    UTextureRenderTarget2D* TextureRenderTarget = NewObject<UTextureRenderTarget2D>();

    TextureRenderTarget->Filter = TF_Bilinear;
    TextureRenderTarget->ClearColor = FLinearColor::Transparent;
    TextureRenderTarget->SRGB = true;
    TextureRenderTarget->RenderTargetFormat = RTF_RGBA8;

    uint32 ScaledSizeX = static_cast<uint32>( ScaledSize.X );
    uint32 ScaledSizeY = static_cast<uint32>( ScaledSize.Y );

    const bool bForceLinearGamma = false;
    const EPixelFormat RequestedFormat = FSlateApplication::Get().GetRenderer()->GetSlateRecommendedColorFormat();
    TextureRenderTarget->InitCustomFormat( ScaledSizeX, ScaledSizeY, RequestedFormat, bForceLinearGamma );

    //---

    FWidgetRenderer WidgetRenderer;
    WidgetRenderer.SetIsPrepassNeeded( false );

    WidgetRenderer.DrawWindow( TextureRenderTarget, *HitTestGrid, Window, 1.f, ScaledSize, 0.1f );

    //TextureRenderTarget = WidgetRenderer.DrawWidget( mPDFSheetWidget->TakeWidget(), ScaledSize );

    //---

    FString pathfile = TEXT( "C:/Users/Mike/Documents/Unreal Projects/dev_50_epos/Plugins/Epos/samples.png" );
    TUniquePtr<FArchive> Ar( IFileManager::Get().CreateFileWriter( *pathfile ) );
    if( !Ar )
        return false;

    FBufferArchive Buffer;
    bool bSuccess = FImageUtils::ExportRenderTarget2DAsPNG( TextureRenderTarget, Buffer );
    if( !bSuccess )
        return false;

    Ar->Serialize( const_cast<uint8*>( Buffer.GetData() ), Buffer.Num() );

    return true;
}

//---

#undef LOCTEXT_NAMESPACE
