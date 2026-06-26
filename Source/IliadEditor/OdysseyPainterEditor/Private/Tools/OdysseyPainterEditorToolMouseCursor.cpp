// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/OdysseyPainterEditorToolMouseCursor.h"

#include "Framework/Application/SlateApplication.h"
#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
#include "Internationalization/Regex.h"

//#include "OdysseyPainterEditorSettings.h"

//---

/*static*/ TMap<EMouseCursorCustom, void*> FMouseCursor::smMouseCursorCustomMap;

FMouseCursor::FMouseCursor( EMouseCursor::Type iMouseCursor )
    : mMouseCursorNative( iMouseCursor )
    , mMouseCursorCustom()
{
    check( iMouseCursor != EMouseCursor::Custom );
}
FMouseCursor::FMouseCursor( EMouseCursorCustom iMouseCursorCustom )
    : mMouseCursorNative( EMouseCursor::Custom )
    , mMouseCursorCustom( iMouseCursorCustom )
{
    check( iMouseCursorCustom != EMouseCursorCustom::None );
}

FMouseCursor::~FMouseCursor()
{
}

bool
FMouseCursor::IsMouseCursorNative() const
{
    return mMouseCursorNative != EMouseCursor::Custom;
}

EMouseCursor::Type
FMouseCursor::GetMouseCursorNative() const
{
    return mMouseCursorNative;
}
TOptional<EMouseCursorCustom>
FMouseCursor::GetMouseCursorCustom() const
{
    return mMouseCursorCustom;
}

FMouseCursor&
FMouseCursor::SetMouseCursorNative( EMouseCursor::Type iMouseCursor )
{
    check( iMouseCursor != EMouseCursor::Custom );

    mMouseCursorNative = iMouseCursor;
    mMouseCursorCustom.Reset();

    return *this;
}
FMouseCursor&
FMouseCursor::SetMouseCursorCustom( EMouseCursorCustom iMouseCursorCustom )
{
    check( iMouseCursorCustom != EMouseCursorCustom::None );

    mMouseCursorNative = EMouseCursor::Custom;
    mMouseCursorCustom = iMouseCursorCustom;

    return *this;
}

void
FMouseCursor::UpdateCursor() const
{
    InitializeCustomCursorMap();

    static EMouseCursorCustom sLastCursorCustom = EMouseCursorCustom::None;

    if( IsMouseCursorNative() )
    {
        check( !mMouseCursorCustom.IsSet() );

        // To reset the potential previous custom cursor image
        TSharedPtr<ICursor> PlatformCursor = FSlateApplication::Get().GetPlatformCursor();
        PlatformCursor->SetTypeShape( EMouseCursor::Custom, nullptr );

        sLastCursorCustom = EMouseCursorCustom::None;

        return;
    }

    //---

    check( mMouseCursorNative == EMouseCursor::Custom );
    check( mMouseCursorCustom.IsSet() );

    if( *mMouseCursorCustom != sLastCursorCustom )
    {
        TSharedPtr<ICursor> PlatformCursor = FSlateApplication::Get().GetPlatformCursor();
        PlatformCursor->SetTypeShape( mMouseCursorNative, smMouseCursorCustomMap.FindChecked( *mMouseCursorCustom ) );
        sLastCursorCustom = *mMouseCursorCustom;
    }
}

//static
void
FMouseCursor::InitializeCustomCursorMap()
{
    if( !smMouseCursorCustomMap.IsEmpty() )
        return;

    auto AddCursor = []( TMap<EMouseCursorCustom, void*>& ioCustomCursorMap, EMouseCursorCustom iCursorId, const FString& iCursorName )
        {
            auto FindFilename = []( const FString& iPath, const FString& iCursorNamePrefix ) -> FString
                {
                    TArray<FString> cursorFilenames;
                    IFileManager::Get().FindFiles( cursorFilenames, *( iPath / TEXT( "*" ) ), true, false );

                    TSet<FString> filenames;
                    for( const FString& cursorFilename : cursorFilenames )
                    {
                        if( !cursorFilename.StartsWith( iCursorNamePrefix ) )
                            continue;

                        // Remove the extension
                        FString cursorFilenameWithoutExtension = FPaths::GetBaseFilename( cursorFilename );

                        filenames.Add( cursorFilenameWithoutExtension );
                    }

                    check( filenames.Num() == 1 );
                    return filenames.Array()[0];
                };

            auto GetHotSpot = []( const FString& iFilename ) -> FIntVector2
                {
                    const FString regex = TEXT( ".*\\.([0-9]+)x([0-9]+)(\\..*)?" );
                    FRegexPattern hotspotPattern( regex );

                    FRegexMatcher matcher( hotspotPattern, iFilename );
                    check( matcher.FindNext() );

                    FString hotspotX_group = matcher.GetCaptureGroup( 1 );
                    checkf( !hotspotX_group.IsEmpty(), TEXT( "Can't find hotspot X coordinates inside filename: must be mycursor.XxY.ext" ) );
                    FString hotspotY_group = matcher.GetCaptureGroup( 2 );
                    checkf( !hotspotY_group.IsEmpty(), TEXT( "Can't find hotspot Y coordinates inside filename: must be mycursor.XxY.ext" ) );

                    int32 hotspotX = FCString::Atoi( *hotspotX_group );
                    int32 hotspotY = FCString::Atoi( *hotspotY_group );

                    return FIntVector2( hotspotX, hotspotY );
                };

            //---

            FString cursorPath = IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) / TEXT( "PainterEditor" ) / TEXT( "ToolsCursor" );

            FString cursorFilenameWithoutExtension = FindFilename( cursorPath, iCursorName );
            FIntVector2 hotspot = GetHotSpot( cursorFilenameWithoutExtension );

            TSharedPtr<ICursor> PlatformCursor = FSlateApplication::Get().GetPlatformCursor();
            void* cursor = PlatformCursor->CreateCursorFromFile( cursorPath / cursorFilenameWithoutExtension, FVector2D( hotspot ) );
            ioCustomCursorMap.Add( iCursorId, cursor );
        };

    smMouseCursorCustomMap.Add( EMouseCursorCustom::None, nullptr );
    AddCursor( smMouseCursorCustomMap, EMouseCursorCustom::Cross, "cross.cursor" );
    AddCursor( smMouseCursorCustomMap, EMouseCursorCustom::Dot, "dot.cursor" );
    AddCursor( smMouseCursorCustomMap, EMouseCursorCustom::Circle, "circle.cursor" );
    AddCursor( smMouseCursorCustomMap, EMouseCursorCustom::CircleClockwise, "circle-clockwise.cursor" );
    AddCursor( smMouseCursorCustomMap, EMouseCursorCustom::CircleAntiClockwise, "circle-anticlockwise.cursor" );
    // Make corresponding the cursor id to the cursor file (like for svg in styling)
    // cursor file DON'T have extension ! (because different files are used for each OS)
}
