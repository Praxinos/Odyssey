// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#if PLATFORM_WINDOWS

#include "WintabContexts-Windows.h"

#include "IOdysseyStylusInputModule.h" // for UE_LOG
#include <chrono>
//---

static
const FWTPacketDescription*
PacketDescriptionFromType( const TArray<FWTPacketDescription>& iPacketDescriptions, EWintabPacketType iPacketType )
{
    return iPacketDescriptions.FindByPredicate( [iPacketType]( const FWTPacketDescription& iDescription )
    {
        return iDescription.Type == iPacketType;
    } );
}

static float Normalize( int Value, const FWTPacketDescription& Desc )
{
    return (float)( Value - Desc.Minimum ) / (float)( Desc.Maximum - Desc.Minimum );
}

static float ToDegrees( int Value, const FWTPacketDescription& Desc )
{
    return Value / 10.f;
    //return Value / Desc.Resolution;
}

void 
FWTTabletContextInfo::Tick()
{
    if( !mPacketsBuffer.Num() )
        mPacketsBuffer.AddUninitialized( 50 );

    int count = FWintabLibrary::WTPacketsGet( mTabletContext, mPacketsBuffer.Num(), mPacketsBuffer.GetData() );

    const FWTPacketDescription* packet_description_z = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::Z );
    const FWTPacketDescription* packet_description_timer = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::Timer );
    const FWTPacketDescription* packet_description_npressure = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::NormalPressure );
    const FWTPacketDescription* packet_description_tpressure = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::TangentPressure );
    const FWTPacketDescription* packet_description_twist = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::Twist );
    const FWTPacketDescription* packet_description_azimuth = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::Azimuth );
    const FWTPacketDescription* packet_description_altitude = PacketDescriptionFromType( PacketDescriptions, EWintabPacketType::Altitude );

    for( int i = 0; i < count; i++ )
    {
        const PACKET& packet = mPacketsBuffer[i];

        FWintabStylusState state;
        state.Position = FVector2D( packet.pkX / 1000.f, packet.pkY / 1000.f );
        state.Z = packet_description_z ? packet.pkZ : 0.0;

        state.Timer = std::chrono::system_clock::now().time_since_epoch().count() / 10000;

        state.NormalPressure = packet_description_npressure ? Normalize( packet.pkNormalPressure, *packet_description_npressure ) : 0.0;
        state.TangentPressure = packet_description_tpressure ? Normalize( packet.pkTangentPressure, *packet_description_tpressure ) : 0.0;

        state.Twist = packet_description_twist ? ToDegrees( packet.pkOrientation.orTwist, *packet_description_twist ) : 0.0;
        state.Azimuth = packet_description_azimuth ? ToDegrees( packet.pkOrientation.orAzimuth, *packet_description_azimuth ) : 0.0;
        state.Altitude = packet_description_altitude ? ToDegrees( packet.pkOrientation.orAltitude, *packet_description_altitude ) : 0.0;

        //---

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
            //UE_LOG( LogStylusInput, Log, TEXT( "packet->pkButtons: 0x%X\n" ), packet->pkButtons );

        state.IsInverted = ( packet.pkStatus & TPS_INVERT );

        //---

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

    AXIS tablet_x = { 0 };
    UINT wWTInfoRetVal = FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_X, &tablet_x );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::X;
        packet_description.Minimum = tablet_x.axMin;
        packet_description.Maximum = tablet_x.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( tablet_x.axResolution );

        //UE_LOG( LogStylusInput, Log, TEXT( "info X: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, tablet_x.axUnits, packet_description.Resolution );
        // info X: min:0 max:40639 unit:2 res:2000.000000

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }
    else
    {
        return false;
    }

    AXIS tablet_y = { 0 };
    wWTInfoRetVal = FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_Y, &tablet_y );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::Y;
        packet_description.Minimum = tablet_y.axMin;
        packet_description.Maximum = tablet_y.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( tablet_y.axResolution );

        //UE_LOG( LogStylusInput, Log, TEXT( "info Y: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, tablet_y.axUnits, packet_description.Resolution );
        // info Y: min:0 max:30479 unit:2 res:2000.000000

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }

    AXIS tablet_z = { 0 };
    wWTInfoRetVal = FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_Z, &tablet_z );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::Z;
        packet_description.Minimum = tablet_z.axMin;
        packet_description.Maximum = tablet_z.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( tablet_z.axResolution );

        //UE_LOG( LogStylusInput, Log, TEXT( "info Z: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, tablet_z.axUnits, packet_description.Resolution );
        // info Z: min:-1023 max:1023 unit:2 res:2000.000000

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }

    AXIS normal_pressure = { 0 };
    FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_NPRESSURE, &normal_pressure );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::NormalPressure;
        packet_description.Minimum = normal_pressure.axMin;
        packet_description.Maximum = normal_pressure.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( normal_pressure.axResolution );

        //UE_LOG( LogStylusInput, Log, TEXT( "info NormalPressure: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, normal_pressure.axUnits, packet_description.Resolution );
        // info NormalPressure: min:0 max:1023 unit:0 res:0.000000

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }

    AXIS tangent_pressure = { 0 };
    FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_TPRESSURE, &tangent_pressure );
    if( wWTInfoRetVal == sizeof( AXIS ) )
    {
        packet_description.Type = EWintabPacketType::TangentPressure;
        packet_description.Minimum = tangent_pressure.axMin;
        packet_description.Maximum = tangent_pressure.axMax;
        packet_description.Resolution = FIX_TO_DOUBLE( tangent_pressure.axResolution );

        //UE_LOG( LogStylusInput, Log, TEXT( "info TangentPressure: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, tangent_pressure.axUnits, packet_description.Resolution );
        // info TangentPressure: min:0 max:1023 unit:0 res:0.000000

        ioTabletContext->PacketDescriptions.Add( packet_description );
    }

    AXIS orientation[3];
    FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_ORIENTATION, &orientation );
    if( wWTInfoRetVal )
    {
#if 0 // from tilttest sample
        /* convert azimuth resulution to double */
        tpvar = FIX_DOUBLE( TpOri[0].axResolution );
        /* convert from resolution to radians */
        aziFactor = tpvar / ( 2 * pi );

        /* convert altitude resolution to double */
        tpvar = FIX_DOUBLE( TpOri[1].axResolution );
        /* scale to arbitrary value to get decent line length */
        altFactor = tpvar / 1000;
        /* adjust for maximum value at vertical */
        altAdjust = (double)TpOri[1].axMax / altFactor;
#endif

        if( orientation[0].axResolution )
        {
            packet_description.Type = EWintabPacketType::Azimuth;
            packet_description.Minimum = orientation[0].axMin;
            packet_description.Maximum = orientation[0].axMax;
            packet_description.Resolution = FIX_TO_DOUBLE( orientation[0].axResolution );

            //UE_LOG( LogStylusInput, Log, TEXT( "info Azimuth: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, orientation[0].axUnits, packet_description.Resolution );
            // info Azimuth: min:0 max:3600 unit:3 res:3600.000000

            ioTabletContext->PacketDescriptions.Add( packet_description );
        }

        if( orientation[1].axResolution )
        {
            packet_description.Type = EWintabPacketType::Altitude;
            packet_description.Minimum = orientation[1].axMin;
            packet_description.Maximum = orientation[1].axMax;
            packet_description.Resolution = FIX_TO_DOUBLE( orientation[1].axResolution );

            //UE_LOG( LogStylusInput, Log, TEXT( "info Altitude: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, orientation[1].axUnits, packet_description.Resolution );
            // info Altitude: min:-900 max:900 unit:3 res:3600.000000

            ioTabletContext->PacketDescriptions.Add( packet_description );
        }

        if( orientation[2].axResolution )
        {
            packet_description.Type = EWintabPacketType::Twist;
            packet_description.Minimum = orientation[2].axMin;
            packet_description.Maximum = orientation[2].axMax;
            packet_description.Resolution = FIX_TO_DOUBLE( orientation[2].axResolution );

            //UE_LOG( LogStylusInput, Log, TEXT( "info Twist: min:%d max:%d unit:%d res:%f" ), packet_description.Minimum, packet_description.Maximum, orientation[2].axUnits, packet_description.Resolution );
            // info Twist: min:0 max:3600 unit:3 res:3600.000000

            ioTabletContext->PacketDescriptions.Add( packet_description );
        }
    }

    //AXIS rotation[3];
    //FWintabLibrary::WTInfoW( WTI_DEVICES + iIndexContext, DVC_ROTATION, &rotation );
    //if( wWTInfoRetVal )
    //{
    //    if( orientation[0].axResolution )
    //    {
    //        packet_description.Type = EWintabPacketType::Pitch;
    //        packet_description.Minimum = rotation[0].axMin;
    //        packet_description.Maximum = rotation[0].axMax;
    //        packet_description.Resolution = FIX_TO_DOUBLE( rotation[0].axResolution );

    //        ioTabletContext->PacketDescriptions.Add( packet_description );
    //    }
    //        
    //    if( orientation[1].axResolution )
    //    {
    //        packet_description.Type = EWintabPacketType::Roll;
    //        packet_description.Minimum = rotation[1].axMin;
    //        packet_description.Maximum = rotation[1].axMax;
    //        packet_description.Resolution = FIX_TO_DOUBLE( rotation[1].axResolution );

    //        ioTabletContext->PacketDescriptions.Add( packet_description );
    //    }
    //        
    //    if( orientation[2].axResolution )
    //    {
    //        packet_description.Type = EWintabPacketType::Yaw;
    //        packet_description.Minimum = rotation[2].axMin;
    //        packet_description.Maximum = rotation[2].axMax;
    //        packet_description.Resolution = FIX_TO_DOUBLE( rotation[2].axResolution );

    //        ioTabletContext->PacketDescriptions.Add( packet_description );
    //    }
    //}

    return true;
}

static 
void 
SetupTabletSupportedPackets( FWTTabletContextInfo* ioTabletContext )
{
    ioTabletContext->SupportedPackets.Empty();
    ioTabletContext->CleanSupportedInput();

    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::X ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::X );
    }
    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Y ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Y );
    }
    if( ioTabletContext->SupportedPackets.Contains( EWintabPacketType::X ) 
        && ioTabletContext->SupportedPackets.Contains( EWintabPacketType::Y ) )
    {
        ioTabletContext->AddSupportedInput( EStylusInputType::Position );
    }

    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Z ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Z );
        ioTabletContext->AddSupportedInput( EStylusInputType::Z );
    }

    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Timer ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Timer );
        ioTabletContext->AddSupportedInput( EStylusInputType::Timer );
    }

    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::NormalPressure ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::NormalPressure );
        ioTabletContext->AddSupportedInput( EStylusInputType::Pressure );
    }
    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::TangentPressure ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::TangentPressure );
        ioTabletContext->AddSupportedInput( EStylusInputType::TangentPressure );
    }

    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Azimuth ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Azimuth );
        ioTabletContext->AddSupportedInput( EStylusInputType::Azimuth );
    }
    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Altitude ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Altitude );
        ioTabletContext->AddSupportedInput( EStylusInputType::Altitude );
    }
    if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Twist ) )
    {
        ioTabletContext->SupportedPackets.Add( EWintabPacketType::Twist );
        ioTabletContext->AddSupportedInput( EStylusInputType::Twist );
    }

    //if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Pitch ) )
    //{
    //    ioTabletContext->SupportedPackets.Add( EWintabPacketType::Pitch );
    //    ioTabletContext->AddSupportedInput( EStylusInputType::Pitch );
    //}
    //if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Roll ) )
    //{
    //    ioTabletContext->SupportedPackets.Add( EWintabPacketType::Roll );
    //    ioTabletContext->AddSupportedInput( EStylusInputType::Roll );
    //}
    //if( PacketDescriptionFromType( ioTabletContext->PacketDescriptions, EWintabPacketType::Yaw ) )
    //{
    //    ioTabletContext->SupportedPackets.Add( EWintabPacketType::Yaw );
    //    ioTabletContext->AddSupportedInput( EStylusInputType::Yaw );
    //}
}

bool
FWintabContexts::OpenTabletContexts( HWND iHwnd )
{
    check( !mTabletContexts.Num() );

    int attached_devices = 0;
    FWintabLibrary::WTInfoW( WTI_INTERFACE, IFC_NDEVICES, &attached_devices );
    UE_LOG( LogStylusInput, Log, TEXT( "Number of attached devices: %i" ), attached_devices );

    int ctxIndex = 0;
    // Open/save contexts until first failure to open a context.
    // Note that WTInfoA(WTI_STATUS, STA_CONTEXTS, &nOpenContexts);
    // will not always let you enumerate through all contexts.
    do
    {
        UE_LOG( LogStylusInput, Log, TEXT( "Getting info on contextIndex: %i ..." ), ctxIndex );

        LOGCONTEXT lcMine = { 0 };
        int foundCtx = FWintabLibrary::WTInfoW( WTI_DDCTXS + ctxIndex, 0, &lcMine );

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
            tablet_context_info.IsTouching = false;
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
            lcMine.lcInExtX = packet_description_x->Maximum + 1;
            lcMine.lcInExtY = packet_description_y->Maximum + 1;

            // Guarantee the output coordinate space to be in screen coordinates.  
            lcMine.lcOutOrgX = GetSystemMetrics( SM_XVIRTUALSCREEN ) * 1000.f; // Scaled to have subpixel with packet.pkX / 1000.f
            lcMine.lcOutOrgY = GetSystemMetrics( SM_YVIRTUALSCREEN ) * 1000.f;
            lcMine.lcOutExtX = GetSystemMetrics( SM_CXVIRTUALSCREEN ) * 1000.f;
            // In Wintab, the tablet origin is lower left. Move origin to upper left so that it coincides with screen origin.
            lcMine.lcOutExtY = -GetSystemMetrics( SM_CYVIRTUALSCREEN ) * 1000.f;

            // Leave the system origin and extents as received:
            // lcSysOrgX, lcSysOrgY, lcSysExtX, lcSysExtY

            // Open the context enabled.
            HCTX context = FWintabLibrary::WTOpenW( iHwnd, &lcMine, Windows::TRUE );

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

        FWintabLibrary::WTClose( tablet_context_info.mTabletContext );
    }

    mTabletContexts.Empty();
}

#endif // PLATFORM_WINDOWS