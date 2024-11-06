#include "Import/svg/OdysseyVectorImportSVG.h"
#include "XmlFile.h"
#include <OdysseyVectorVertex.h>
#include <OdysseyVectorSegment.h>
#include <OdysseyVectorSegmentCubic.h>
#include <OdysseyVectorPath.h>
#include <OdysseyVectorGroupPaint.h>

FOdysseyVectorImportSVG::~FOdysseyVectorImportSVG()
{
}

FOdysseyVectorImportSVG::FOdysseyVectorImportSVG()
{
}

FOdysseyVectorImportSVG::FOdysseyVectorImportSVG( FOdysseyVectorGroupPaint* iScene
                                                , const FString& iPath )
    : ProcessAttributeCallback( nullptr )
    , mScene( iScene )
    , mProcessedObject( nullptr )
    , mLastVertex( nullptr )
    , mLastSegment( nullptr )
    , mLastCommand( 0 )
{
    FText outErrorMessage;
    int32 outErrorLineNumber;

    FFastXml::ParseXmlFile ( this
                           , *iPath // const TCHAR* XmlFilePath,
                           , nullptr //TCHAR* XmlFileContents,
                           , nullptr // FFeedbackContext* FeedbackCont...,
                           , false // const bool bShowSlowTaskDialog,
                           , false // const bool bShowCancelButton,
                           , outErrorMessage
                           , outErrorLineNumber );

    iScene->SetPainted( false );
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}

static bool IsComma( TCHAR iChar )
{
    switch( iChar )
    {
        case TCHAR(','):
            return true;
        break;

        default :
        break;
    }

    return false;
}

static bool IsWhiteSpace( TCHAR iChar )
{
    switch( iChar )
    {
        case TCHAR(0x9)  :
        case TCHAR(0xA)  :
        case TCHAR(0xC)  :
        case TCHAR(0xD)  :
        case TCHAR(0x20) :
            return true;
        break;

        default :
        break;
    }

    return false;
}

static TCHAR* SkipWhitespace( const TCHAR* iCommandArgs )
{
    TCHAR* currentChar = (TCHAR*) iCommandArgs;

    while( *currentChar != TCHAR('\0') )
    {
        if( IsWhiteSpace( *currentChar ) == false )
        {
            break;
        }

        currentChar++;
    }

    return currentChar;
}

static TCHAR* SkipCommaWhitespace( const TCHAR* iCommandArgs )
{
    TCHAR* currentChar = (TCHAR*) iCommandArgs;

    while( *currentChar != TCHAR('\0') )
    {
        if( ( IsWhiteSpace( *currentChar ) == false )
         && ( IsComma     ( *currentChar ) == false ) )
        {
            break;
        }

        currentChar++;
    }

    return currentChar;
}

static TCHAR* ReadXY( const TCHAR* iCommandArgs
                    , ::ULIS::FVec2D& coords
                    , ::ULIS::FVec2D* referenceCoords )
{
    wchar_t* currentArg = (wchar_t*) iCommandArgs;
    TCHAR* currentArgChar = (TCHAR*) iCommandArgs;

    // wcstod is for wide chars. It should be portable on both windows and linux but I am unsure
    // about the result compared to the windows-only _tcstod function.
    coords.x = std::wcstod( currentArg, &currentArg );

    currentArgChar = SkipCommaWhitespace( currentArgChar );

    // wcstod is for wide chars. It should be portable on both windows and linux but I am unsure
    // about the result compared to the windows-only _tcstod function.
    coords.y = std::wcstod( currentArg, &currentArg );

    if( referenceCoords )
    {
        coords.x += referenceCoords->x;
        coords.y += referenceCoords->y;

        //*referenceCoords = coords;
    }

    return currentArgChar;
}

bool
FOdysseyVectorImportSVG::ProcessXmlDeclaration( const TCHAR* ElementData
                                              , int32 XmlFileLineNumber )
{
    return true;
}

bool
FOdysseyVectorImportSVG::ProcessElement( const TCHAR* ElementName
                                       , const TCHAR* ElementData
                                       , int32 XmlFileLineNumber )
{
    if ( FString( ElementName ).Compare( FString("path"), ESearchCase::IgnoreCase ) == 0 )
    {
        mProcessedObject = new FOdysseyVectorPath( "path" );

        mScene->AppendChild( mProcessedObject );
        // set the callback that will process further attributes
        ProcessAttributeCallback = &FOdysseyVectorImportSVG::PathProcessAttribute;
    }

    return true;
}

bool
FOdysseyVectorImportSVG::ProcessClose ( const TCHAR* Element )
{
    mLastVertex = nullptr;
    mLastSegment = nullptr;

    return true;
}

TCHAR* FOdysseyVectorImportSVG::PathProcessAttributeDCommandZ( const TCHAR* iCommandArgs
                                                             , bool iAbsolute )
{
    TCHAR* currentArg = (TCHAR*) iCommandArgs;

    mLastVertex = nullptr;
    mLastSegment = nullptr;

    return currentArg;
}

TCHAR* FOdysseyVectorImportSVG::PathProcessAttributeDCommandM( const TCHAR* iCommandArgs
                                                             , bool iAbsolute )
{
    FOdysseyVectorPath* processedPath = static_cast<FOdysseyVectorPath*>(mProcessedObject);
    TCHAR* currentArg = (TCHAR*) iCommandArgs;
    ::ULIS::FVec2D referenceCoords = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D absoluteCoords;

    if( ( iAbsolute == false ) && mLastVertex )
    {
        referenceCoords = mLastVertex->GetCoords();
    }

    currentArg = ReadXY( currentArg, absoluteCoords, iAbsolute ? nullptr : &referenceCoords );

    mLastVertex = new FOdysseyVectorVertex( absoluteCoords.x, absoluteCoords.y, 1.0f );
    processedPath->AddVertex( mLastVertex );

    UE_LOG(LogTemp, Warning, TEXT("Vertex: x:%f y:%f"), absoluteCoords.x, absoluteCoords.y );

    return currentArg;
}

TCHAR* FOdysseyVectorImportSVG::PathProcessAttributeDCommandC( const TCHAR* iCommandArgs
                                                             , bool iAbsolute )
{
    FOdysseyVectorVertex* vertex0 = mLastVertex;
    FOdysseyVectorVertex* vertex1;
    FOdysseyVectorSegmentCubic* cubicSegment;
    FOdysseyVectorPath* processedPath = static_cast<FOdysseyVectorPath*>(mProcessedObject);
    TCHAR* currentArg = (TCHAR*) iCommandArgs;
    ::ULIS::FVec2D referenceCoords = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D absoluteCoords1;
    ::ULIS::FVec2D absoluteCoords2;
    ::ULIS::FVec2D absoluteCoords3;

    if( ( iAbsolute == false ) && mLastVertex )
    {
        referenceCoords = mLastVertex->GetCoords();
    }

    currentArg = ReadXY( currentArg, absoluteCoords1, iAbsolute ? nullptr : &referenceCoords );
    currentArg = SkipCommaWhitespace( currentArg );
    currentArg = ReadXY( currentArg, absoluteCoords2, iAbsolute ? nullptr : &referenceCoords );
    currentArg = SkipCommaWhitespace( currentArg );
    currentArg = ReadXY( currentArg, absoluteCoords3, iAbsolute ? nullptr : &referenceCoords );

    //UE_LOG(LogTemp, Warning, TEXT("x:%f y:%f"), x, y );
    UE_LOG(LogTemp, Warning, TEXT("Cubic Segment x:%f y:%f | x:%f y:%f | x:%f y:%f"), absoluteCoords1.x, absoluteCoords1.y, absoluteCoords2.x, absoluteCoords2.y, absoluteCoords3.x, absoluteCoords3.y );

    vertex1 = new FOdysseyVectorVertex( absoluteCoords3.x, absoluteCoords3.y, 1.0f );
    cubicSegment = new FOdysseyVectorSegmentCubic( processedPath
                                                 , vertex0
                                                 , absoluteCoords1.x
                                                 , absoluteCoords1.y
                                                 , absoluteCoords2.x
                                                 , absoluteCoords2.y
                                                 , vertex1
                                                 , true );

    processedPath->AddVertex( vertex1 );
    processedPath->AddSegment( cubicSegment );

    mLastVertex = vertex1;
    mLastSegment = cubicSegment;

    return currentArg;
}

TCHAR* FOdysseyVectorImportSVG::PathProcessAttributeDCommandS( const TCHAR* iCommandArgs
                                                             , bool iAbsolute )
{
    // If there is no previous command or if the previous command was not an C, c, S or s,
    // assume the first control point is coincident with the current point.)
    FOdysseyVectorHandleSegment* lastHandle = ( ( mLastCommand == TCHAR('C') )
                                             || ( mLastCommand == TCHAR('c') )
                                             || ( mLastCommand == TCHAR('S') )
                                             || ( mLastCommand == TCHAR('s') ) ) && mLastSegment ? mLastSegment->GetHandle( mLastVertex ) : nullptr;
    FOdysseyVectorVertex* vertex0 = mLastVertex;
    FOdysseyVectorVertex* vertex1;
    FOdysseyVectorSegmentCubic* cubicSegment;
    FOdysseyVectorPath* processedPath = static_cast<FOdysseyVectorPath*>(mProcessedObject);
    TCHAR* currentArg = (TCHAR*) iCommandArgs;
    ::ULIS::FVec2D referenceCoords = ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D absoluteCoords1;
    ::ULIS::FVec2D absoluteCoords2;
    ::ULIS::FVec2D absoluteCoords3;
    ::ULIS::FVec2D diffCoords = lastHandle ? lastHandle->GetCoords() - mLastVertex->GetCoords()
                                          : ::ULIS::FVec2D( 0.0f, 0.0f );

    if( mLastVertex )
    {
        referenceCoords = mLastVertex->GetCoords();
    }

    absoluteCoords1.x = referenceCoords.x - diffCoords.x;
    absoluteCoords1.y = referenceCoords.y - diffCoords.y;

    currentArg = ReadXY( currentArg, absoluteCoords2, iAbsolute ? nullptr : &referenceCoords );
    currentArg = SkipCommaWhitespace( currentArg );
    currentArg = ReadXY( currentArg, absoluteCoords3, iAbsolute ? nullptr : &referenceCoords  );

    UE_LOG(LogTemp, Warning, TEXT("Short Cubic Segment x:%f y:%f | x:%f y:%f | x:%f y:%f"), absoluteCoords1.x, absoluteCoords1.y, absoluteCoords2.x, absoluteCoords2.y, absoluteCoords3.x, absoluteCoords3.y );

    vertex1 = new FOdysseyVectorVertex( absoluteCoords3.x, absoluteCoords3.y, 1.0f );
    cubicSegment = new FOdysseyVectorSegmentCubic( processedPath
                                                 , vertex0
                                                 , absoluteCoords1.x
                                                 , absoluteCoords1.y
                                                 , absoluteCoords2.x
                                                 , absoluteCoords2.y
                                                 , vertex1
                                                 , true );

    processedPath->AddVertex( vertex1 );
    processedPath->AddSegment( cubicSegment );

    mLastVertex = vertex1;
    mLastSegment = cubicSegment;

    return currentArg;
}

static bool IsPathCommand( const TCHAR* iCommandName )
{
    switch( *iCommandName )
    {
        case TCHAR('M'):
        case TCHAR('m'):
        case TCHAR('C'):
        case TCHAR('c'):
        case TCHAR('S'):
        case TCHAR('s'):
        case TCHAR('L'):
        case TCHAR('l'):
        case TCHAR('H'):
        case TCHAR('h'):
        case TCHAR('V'):
        case TCHAR('v'):
        case TCHAR('Q'):
        case TCHAR('q'):
        case TCHAR('T'):
        case TCHAR('t'):
        case TCHAR('A'):
        case TCHAR('a'):
        case TCHAR('Z'):
        case TCHAR('z'):
            return true;
        break;

        default: break;
    }

    return false;
}

static TCHAR* SkipToNextCommand( const TCHAR* iCommandArgs )
{
    TCHAR* nextCommand = (TCHAR*) iCommandArgs;

    while( ( IsPathCommand( nextCommand ) == false ) && ( *nextCommand != TCHAR('\0') ) )
    {
        nextCommand++;
    }

    return nextCommand;
}

TCHAR* FOdysseyVectorImportSVG::PathProcessAttributeDCommand( const TCHAR* iCommandName
                                                            , const TCHAR* iCommandArgs )
{
    switch( *iCommandName )
    {
        case TCHAR('M'):
            return PathProcessAttributeDCommandM( iCommandArgs, true );
        break;

        case TCHAR('m'):
            return PathProcessAttributeDCommandM( iCommandArgs, false );
        break;

        case TCHAR('C'):
            return PathProcessAttributeDCommandC( iCommandArgs, true );
        break;

        case TCHAR('c'):
            return PathProcessAttributeDCommandC( iCommandArgs, false );
        break;

        case TCHAR('S'):
            return PathProcessAttributeDCommandS( iCommandArgs, true );
        break;

        case TCHAR('s'):
            return PathProcessAttributeDCommandS( iCommandArgs, false );
        break;

        case TCHAR('Z'):
            return PathProcessAttributeDCommandZ( iCommandArgs, true );
        break;

        case TCHAR('z'):
            return PathProcessAttributeDCommandZ( iCommandArgs, false );
        break;

        // unsupported commands
        case TCHAR('L'):
        case TCHAR('l'):
        case TCHAR('H'):
        case TCHAR('h'):
        case TCHAR('V'):
        case TCHAR('v'):
        case TCHAR('Q'):
        case TCHAR('q'):
        case TCHAR('T'):
        case TCHAR('t'):
        case TCHAR('A'):
        case TCHAR('a'):

        default: break;
    }

    return SkipToNextCommand( iCommandArgs );
}

bool
FOdysseyVectorImportSVG::PathProcessAttributeD ( const TCHAR* AttributeValue )
{
    TCHAR* currentChar = (TCHAR*) AttributeValue;
    TCHAR  commandName = TCHAR('\0');
    TCHAR* commandArgs = currentChar;

    while( *currentChar != TCHAR('\0') )
    {
        if( FChar::IsAlpha( *currentChar ) )
        {
            commandName = *currentChar;
            commandArgs =  currentChar + 1;

            switch( commandName )
            {
                case TCHAR('M'):
                case TCHAR('m'):
                case TCHAR('C'):
                case TCHAR('c'):
                case TCHAR('S'):
                case TCHAR('s'):
                case TCHAR('Z'):
                case TCHAR('z'):
                    commandArgs = SkipWhitespace( commandArgs );

                    currentChar = PathProcessAttributeDCommand( &commandName, commandArgs );
                break;

                case TCHAR('L'):
                case TCHAR('l'):
                case TCHAR('H'):
                case TCHAR('h'):
                case TCHAR('V'):
                case TCHAR('v'):
                case TCHAR('Q'):
                case TCHAR('q'):
                case TCHAR('T'):
                case TCHAR('t'):
                case TCHAR('A'):
                case TCHAR('a'):
                default:
                    currentChar = SkipToNextCommand( commandArgs );
                break;
            }
        }
        else
        {
            commandArgs =  currentChar;
            // continue with current command
            commandArgs = SkipCommaWhitespace( commandArgs );

            currentChar = PathProcessAttributeDCommand( &commandName, commandArgs );
        }

        mLastCommand = commandName;
    }

    return true;
}

bool
FOdysseyVectorImportSVG::PathProcessAttribute ( const TCHAR* AttributeName
                                              , const TCHAR* AttributeValue )
{
    UE_LOG(LogTemp, Warning, TEXT("%s %s"), AttributeName, AttributeValue );

    if ( FString( AttributeName ).Compare( FString("D"), ESearchCase::IgnoreCase ) == 0 )
    {
        PathProcessAttributeD( AttributeValue );
    }

    return true;
};

bool
FOdysseyVectorImportSVG::ProcessAttribute ( const TCHAR* AttributeName
                                          , const TCHAR* AttributeValue )
{
    if( ProcessAttributeCallback )
    {
        return (this->*ProcessAttributeCallback)( AttributeName, AttributeValue );
    }

    return true;
};

/*
// returns true to accept char, false otherwise
// 0 - continue and accept
// 1 - continue and ignore
// 2 - stop
static int ReadFloatChar( TCHAR iChar, TCHAR* iPreviousChar )
{
    static int signCount;
    static int dotCount;
    int ret;

    switch( iChar )
    {
        case TCHAR('+') :
        case TCHAR('-') :
            if( iPreviousChar )
            {
                switch( *iPreviousChar )
                {
                    case TCHAR('e') :
                    case TCHAR('E') :
                        ret = 0;
                    break;

                    default :
                        ret = 2;
                    break;
                }
            }
            ret = 0;
        break;

        case TCHAR('e') :
        case TCHAR('E') :
            ret = ( iPreviousChar && FChar::IsDigit( *iPreviousChar ) ) ? 0 : 2;
        break;

        case TCHAR(',') :
            ret = ( iPreviousChar && ( FChar::IsDigit     ( *iPreviousChar ) 
                                           || IsWhiteSpace( *iPreviousChar )  ) ) ? 1 : 2;
        break;

        case TCHAR('.') :
            dotCount++;

            ret = ( dotCount > 1 ) ? 2 : 0;
        break;

        // accepted whitespace (#x9 | #x20 | #xA | #xC | #xD)
        case TCHAR(0x9)  :
        case TCHAR(0xA)  :
        case TCHAR(0xC)  :
        case TCHAR(0xD)  :
        case TCHAR(0x20) :
            if( iPreviousChar )
            {
                ret = ( IsWhiteSpace( *iPreviousChar ) || IsComma( *iPreviousChar ) ) ? 1 : 2;
            }
            else
            {
                ret = 1;
            }
        break;

        case TCHAR('0') :
        case TCHAR('1') :
        case TCHAR('2') :
        case TCHAR('3') :
        case TCHAR('4') :
        case TCHAR('5') :
        case TCHAR('6') :
        case TCHAR('7') :
        case TCHAR('8') :
        case TCHAR('9') :
            ret = 0;
        break;

        default:
            ret = 2;
        break;
    }

    if( ret == 2 )
    {
        signCount = 0;
        dotCount = 0;
    }

    return ret;
}

static TCHAR* ReadFloat( const TCHAR* CommandValue, double& oValue )
{
    int i = 0, n = 0;
    TCHAR* currentChar = &CommandValue[i++];
    TCHAR* previousChar = nullptr;
    int ret;
    char number[32] = { 0 };
    TCHAR endPtr = NULL;

    while( ( ret = ReadFloatChar( *currentChar, previousChar ) ) != 0 )
    {
        if( ret == 0 )
        {
            number[n++] = *currentChar;
        }

        previousChar = currentChar;
    }

    atof( number );

    return currentChar;
}
*/

