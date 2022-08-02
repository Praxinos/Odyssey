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

#include "hpdf.h"

#define LOCTEXT_NAMESPACE "ExportPDFExporter"

// Disable warning "interaction between '_setjmp' and C++ object destruction is non-portable"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4611)
#endif

jmp_buf sgPDFEnv;

void
pdf_error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data )
{
    printf( "ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no );

    longjmp( sgPDFEnv, 1 );
}


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

    pdf_widget->OnConstructPDFLayout( *mStruct, false );

    int32 number_of_pages = pdf_widget->GetTotalPDFPageNumber();

    //---

    HPDF_Doc pdf = HPDF_New( pdf_error_handler, NULL );
    if( !pdf )
    {
        printf( "ERROR: cannot create pdf object.\n" );
        return false;
    }
    if( setjmp( sgPDFEnv ) )
    {
        HPDF_Free( pdf );
        return 1;
    }

    HPDF_SetCompressionMode( pdf, HPDF_COMP_ALL );

    /* create default-font */
    HPDF_Font font = HPDF_GetFont( pdf, "Helvetica", NULL );

    //---

    for( int32 i = 1; i <= number_of_pages; i++ )
    {
        pdf_widget->HandlePDFPageNumber( i );

        //---

        HPDF_PageSizes page_size = HPDF_PAGE_SIZE_A4;
        HPDF_PageDirection page_direction = HPDF_PAGE_LANDSCAPE; // HPDF_PAGE_PORTRAIT;

        FVector2D window_size( 0, 0 );
        float page_ratio = 1.f;
        if( page_size == HPDF_PAGE_SIZE_A4 )
        {
            page_ratio = FMath::InvSqrt( 2.f );
            window_size = FVector2D( 1920, 1920 * page_ratio ); // as landscape
        }

        if( page_direction == HPDF_PAGE_PORTRAIT )
        {
            window_size.Set( window_size.Y, window_size.X );
        }

        //---

        //--- From ...\Editor\UMGEditor\Private\WidgetBlueprintEditorUtils.cpp#2183 : FWidgetBlueprintEditorUtils::DrawSWidgetInRenderTargetInternal(...)

        FVector2D Offset( 0.f, 0.f );
        FVector2D ScaledSize( window_size );
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

        //FString pathfile = TEXT( "C:/Users/Mike/Documents/Unreal Projects/dev_50_epos/Plugins/Epos/samples.png" );
        //TUniquePtr<FArchive> Ar( IFileManager::Get().CreateFileWriter( *pathfile ) );
        //if( !Ar )
        //    return false;

        FBufferArchive Buffer;
        bool bSuccess = FImageUtils::ExportRenderTarget2DAsPNG( TextureRenderTarget, Buffer );
        if( !bSuccess )
            return false;

        //Ar->Serialize( const_cast<uint8*>( Buffer.GetData() ), Buffer.Num() );

        float image_ratio = TextureRenderTarget->SizeX / float( TextureRenderTarget->SizeY );

        //---

        /* add a new page object. */
        HPDF_Page page = HPDF_AddPage( pdf );

        //HPDF_Page_SetWidth( page, 550 );
        //HPDF_Page_SetHeight( page, 650 );
        HPDF_Page_SetSize( page, page_size, page_direction );
        //HPDF_Page_SetSize( page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE );

        HPDF_Destination dst = HPDF_Page_CreateDestination( page );
        HPDF_Destination_SetXYZ( dst, 0, HPDF_Page_GetHeight( page ), 1 );
        HPDF_SetOpenAction( pdf, dst );

        //HPDF_Page_BeginText( page );
        //HPDF_Page_SetFontAndSize( page, font, 20 );
        //HPDF_Page_MoveTextPos( page, 220, HPDF_Page_GetHeight( page ) - 70 );
        //HPDF_Page_ShowText( page, "PngDemo" );
        //HPDF_Page_EndText( page );

        HPDF_Page_SetFontAndSize( page, font, 12 );

        HPDF_REAL page_width = HPDF_Page_GetWidth( page );
        HPDF_REAL page_height = HPDF_Page_GetHeight( page );

        HPDF_Box margin = { 10, 0, 10, 0 };
        margin.top = margin.bottom = page_height * margin.left / page_width;

        //float page_ratio = page_width / page_height;

        float new_image_x = margin.left;
        float new_image_y = margin.top;
        float new_image_width;
        float new_image_height;

        if( page_ratio < image_ratio )
        {
            new_image_width = page_width - margin.left - margin.right;
            new_image_height = new_image_width / image_ratio;
        }
        else
        {
            new_image_height = page_height - margin.top - margin.bottom;
            new_image_width = new_image_height * image_ratio;
        }

        HPDF_Image image = HPDF_LoadPngImageFromMem( pdf, const_cast<uint8*>( Buffer.GetData() ), Buffer.Num() );
        //image = HPDF_LoadPngImageFromFile( pdf, filename );

        /* Draw image to the canvas. */
        HPDF_Page_DrawImage( page
                             , image
                             , new_image_x, new_image_y /* 0, 0 is on bottom left of the page */
                             , new_image_width, new_image_height
        );
    }

    /* save the document to a file */
    FString pdf_extension( TEXT( ".pdf" ) );
    FString pathfile = mPDFOptions->ExportPath.Path / mPDFOptions->ExportFile;
    if( !pathfile.EndsWith( pdf_extension ) )
        pathfile += pdf_extension;

    HPDF_SaveToFile( pdf, StringCast<ANSICHAR>( *pathfile ).Get() );

    /* clean up */
    HPDF_Free( pdf );

    return true;
}

//---

// Renable warning "interaction between '_setjmp' and C++ object destruction is non-portable"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#undef LOCTEXT_NAMESPACE
