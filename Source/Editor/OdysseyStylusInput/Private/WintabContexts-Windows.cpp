// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#if PLATFORM_WINDOWS

#include "WintabContexts-Windows.h"

#define PACKETDATA	(PK_X | PK_Y | PK_Z | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE | PK_CURSOR | PK_SERIAL_NUMBER | PK_TIME | PK_CHANGED | PK_STATUS | PK_ORIENTATION | PK_ROTATION )
#define PACKETMODE	PK_BUTTONS
#include "Windows/PKTDEF.H"

#include "IOdysseyStylusInputModule.h" // for UE_LOG

//---

TArray< PACKET > sgPacketsBuffer;

void 
FWTTabletContextInfo::Tick()
{
    if( !sgPacketsBuffer.Num() )
        sgPacketsBuffer.AddUninitialized( 50 );

    int count = FWintabLibrary::gpWTPacketsGet( mTabletContext, sgPacketsBuffer.Num(), sgPacketsBuffer.GetData() );

    const FWTPacketDescription* packet_description_pressure = PacketDescriptions.FindByPredicate( []( const FWTPacketDescription& iDescription ) { return iDescription.Type == EWintabPacketType::NormalPressure; } );

    for( int i = 0; i < count; i++ )
    {
        const PACKET& packet = sgPacketsBuffer[i];

        FWintabStylusState state;
        state.Position = FVector2D( packet.pkX, packet.pkY );
        state.NormalPressure = packet.pkNormalPressure / float( packet_description_pressure->Maximum );

        if( HIWORD( packet.pkButtons ) & TBN_DOWN
            && !LOWORD( packet.pkButtons ) ) // to not take side buttons
        {
            IsTouching = true;
        }
        else if( HIWORD( packet.pkButtons ) & TBN_UP
                 && !LOWORD( packet.pkButtons ) )
        {
            IsTouching = false;
        }

        //if( packet->pkButtons )
        //    UE_LOG( LogStylusInput, Log, TEXT( "packet->pkButtons: 0x%X\n" ), packet->pkButtons );

        state.IsInverted = ( packet.pkStatus & TPS_INVERT );

        WindowsState.Add( state );
    }

    //---

    TArray<FWintabStylusState> tmp( WindowsState );
    WindowsState.Empty();

    PreviousState = CurrentState;
    CurrentState.Empty();

    for( FWintabStylusState& window_state : tmp )
    {
        window_state.IsTouching = IsTouching;
        CurrentState.Push( window_state.ToPublicState() );
    }

    //Dirty = false;
}

//---

FWintabContexts::FWintabContexts()
{
}

FWintabContexts::~FWintabContexts()
{
    CloseTabletContexts();
}

#define FIX_TO_DOUBLE(x)   ((double)(INT(x))+((double)FRAC(x)/65536))

static 
bool 
SetupPacketDescriptions( int iIndexContext, FWTTabletContextInfo* ioTabletContext )
{
    ioTabletContext->PacketDescriptions.Empty();

    FWTPacketDescription packet_description;

    AXIS TabletX = { 0 };
    UINT wWTInfoRetVal = FWintabLibrary::gpWTInfoW( WTI_DEVICES + iIndexContext, DVC_X, &TabletX );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::X;
        packet_description.Minimum = TabletX.axMin;
        packet_description.Maximum = TabletX.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( TabletX.axResolution );

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }
    else
    {
        return false;
    }

    AXIS TabletY = { 0 };
    wWTInfoRetVal = FWintabLibrary::gpWTInfoW( WTI_DEVICES + iIndexContext, DVC_Y, &TabletY );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::Y;
        packet_description.Minimum = TabletY.axMin;
        packet_description.Maximum = TabletY.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( TabletY.axResolution );

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }

    AXIS Pressure = { 0 };
    FWintabLibrary::gpWTInfoW( WTI_DEVICES + iIndexContext, DVC_NPRESSURE, &Pressure );
    UE_LOG( LogStylusInput, Log, TEXT( "Pressure: %i, %i\n" ), Pressure.axMin, Pressure.axMax );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::NormalPressure;
        packet_description.Minimum = Pressure.axMin;
        packet_description.Maximum = Pressure.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( Pressure.axResolution );

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }

    return true;
}

static
bool
PacketDescriptionIsSupported( const TArray<FWTPacketDescription>& iPacketDescriptions, EWintabPacketType iPacketType )
{
    return !!iPacketDescriptions.FindByPredicate( [iPacketType]( const FWTPacketDescription& iDescription )
    {
        return iDescription.Type == iPacketType;
    } );
}

static 
void 
SetupTabletSupportedPackets( FWTTabletContextInfo* ioTabletContext )
{
    ioTabletContext->SupportedPackets.Empty();
    ioTabletContext->CleanSupportedInput();

    if( PacketDescriptionIsSupported( ioTabletContext->PacketDescriptions, EWintabPacketType::X ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::X );
    }
    if( PacketDescriptionIsSupported( ioTabletContext->PacketDescriptions, EWintabPacketType::Y ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Y );
    }
    if( ioTabletContext->SupportedPackets.Contains( EWintabPacketType::X ) 
        && ioTabletContext->SupportedPackets.Contains( EWintabPacketType::Y ) )
    {
        ioTabletContext->AddSupportedInput( EStylusInputType::Position );
    }

    if( PacketDescriptionIsSupported( ioTabletContext->PacketDescriptions, EWintabPacketType::NormalPressure ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::NormalPressure );
        ioTabletContext->AddSupportedInput( EStylusInputType::Pressure );
    }
}

bool
FWintabContexts::OpenTabletContexts( HWND iHwnd )
{
    check( !mTabletContexts.Num() );

    int attached_devices = 0;
    FWintabLibrary::gpWTInfoW( WTI_INTERFACE, IFC_NDEVICES, &attached_devices );
    UE_LOG( LogStylusInput, Log, TEXT( "Number of attached devices: %i" ), attached_devices );

    int ctxIndex = 0;
    // Open/save contexts until first failure to open a context.
    // Note that gpWTInfoA(WTI_STATUS, STA_CONTEXTS, &nOpenContexts);
    // will not always let you enumerate through all contexts.
    do
    {
        UE_LOG( LogStylusInput, Log, TEXT( "Getting info on contextIndex: %i ..." ), ctxIndex );

        LOGCONTEXT lcMine = { 0 };
        int foundCtx = FWintabLibrary::gpWTInfoW( WTI_DDCTXS + ctxIndex, 0, &lcMine );

        if( foundCtx > 0 )
        {
            lcMine.lcPktData = PACKETDATA;
            lcMine.lcOptions |= CXO_MESSAGES;
            lcMine.lcOptions |= CXO_SYSTEM;
            lcMine.lcPktMode = PACKETMODE;
            lcMine.lcMoveMask = PACKETDATA;
            lcMine.lcBtnUpMask = lcMine.lcBtnDnMask;

            //---

            FWTTabletContextInfo tablet_context_info;
            tablet_context_info.SetDirty(); // Mandatory! Sometimes may be 0 -_- ?!

            if( !SetupPacketDescriptions( ctxIndex, &tablet_context_info ) )
            {
                UE_LOG( LogStylusInput, Log, TEXT( "This context should not be opened." ) );
                continue;
            }
            SetupTabletSupportedPackets( &tablet_context_info );
            
            const FWTPacketDescription* packet_description_x = tablet_context_info.PacketDescriptions.FindByPredicate( []( const FWTPacketDescription& iDescription ) { return iDescription.Type == EWintabPacketType::X; } );
            const FWTPacketDescription* packet_description_y = tablet_context_info.PacketDescriptions.FindByPredicate( []( const FWTPacketDescription& iDescription ) { return iDescription.Type == EWintabPacketType::Y; } );

            //---

            lcMine.lcInOrgX = 0;
            lcMine.lcInOrgY = 0;
            lcMine.lcInExtX = packet_description_x->Maximum;
            lcMine.lcInExtY = packet_description_y->Maximum;

            // Guarantee the output coordinate space to be in screen coordinates.  
            lcMine.lcOutOrgX = GetSystemMetrics( SM_XVIRTUALSCREEN );
            lcMine.lcOutOrgY = GetSystemMetrics( SM_YVIRTUALSCREEN );
            lcMine.lcOutExtX = GetSystemMetrics( SM_CXVIRTUALSCREEN );

            // In Wintab, the tablet origin is lower left.  Move origin to upper left
            // so that it coincides with screen origin.
            lcMine.lcOutExtY = -GetSystemMetrics( SM_CYVIRTUALSCREEN );

            // Leave the system origin and extents as received:
            // lcSysOrgX, lcSysOrgY, lcSysExtX, lcSysExtY

            // Open the context enabled.
            HCTX context = FWintabLibrary::gpWTOpenW( iHwnd, &lcMine, Windows::TRUE );

            if( context )
            {
                tablet_context_info.mTabletContext = context;
                mTabletContexts.Add( tablet_context_info );

                UE_LOG( LogStylusInput, Log, TEXT( "Opened context: 0x%X for ctxIndex: %i" ), context, ctxIndex );
            }
            else
            {
                UE_LOG( LogStylusInput, Log, TEXT( "Did NOT open context for ctxIndex: %i" ), ctxIndex );
            }
        }
        else
        {
            //UE_LOG( LogStylusInput, Log, TEXT( "No context info for ctxIndex: %i, bailing out...\n" ), ctxIndex );
            break;
        }

        ctxIndex++;
    } while( Windows::TRUE );

    if( mTabletContexts.Num() < attached_devices )
    {
        UE_LOG( LogStylusInput, Error, TEXT( "Oops - did not open a context for each attached device" ) );
    }

    UE_LOG( LogStylusInput, Log, TEXT( "#tablet(s) attached: %d - really opened: %d" ), attached_devices, mTabletContexts.Num() );

    return attached_devices > 0;
}

void
FWintabContexts::CloseTabletContexts()
{
    // Close all contexts we opened so we don't have them lying around in prefs.
    for( FWTTabletContextInfo& tablet_context_info: mTabletContexts )
    {
        UE_LOG( LogStylusInput, Log, TEXT( "Closing context: 0x%X" ), tablet_context_info.mTabletContext );

        FWintabLibrary::gpWTClose( tablet_context_info.mTabletContext );
    }

    mTabletContexts.Empty();
}

#endif // PLATFORM_WINDOWS