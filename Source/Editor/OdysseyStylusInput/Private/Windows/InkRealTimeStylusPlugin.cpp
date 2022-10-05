// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "InkRealTimeStylusPlugin.h"
//#include "IOdysseyStylusInputModule.h" // for UE_LOG

//---

HRESULT FInkRealTimeStylusPlugin::QueryInterface(const IID& InterfaceID, void** Pointer)
{
	if ((InterfaceID == __uuidof(IStylusSyncPlugin)) || (InterfaceID == IID_IUnknown))
	{
		*Pointer = this;
		AddRef();
		return S_OK;
	}
	else if ((InterfaceID == IID_IMarshal) && (FreeThreadedMarshaller != nullptr))
	{
		return FreeThreadedMarshaller->QueryInterface(InterfaceID, Pointer);
	}

	*Pointer = nullptr;
	return E_NOINTERFACE;
}

//---

HRESULT FInkRealTimeStylusPlugin::StylusDown(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketSize, LONG* Packet, LONG** InOutPackets)
{
	FInkTabletContextInfo* TabletContext = FindTabletContext(StylusInfo->tcid);
	if (TabletContext != nullptr)
	{
        TabletContext->IsTouching = true;

        HandlePacket( RealTimeStylus, StylusInfo, 1, PacketSize, Packet ); // need to be done after IsTouching = true (HandlePacket uses it)
	}
	return S_OK;
}

HRESULT FInkRealTimeStylusPlugin::StylusUp(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketSize, LONG* Packet, LONG** InOutPackets)
{
	FInkTabletContextInfo* TabletContext = FindTabletContext(StylusInfo->tcid);
	if (TabletContext != nullptr)
	{
        // we know this is not touching
        TabletContext->IsTouching = false;

        HandlePacket( RealTimeStylus, StylusInfo, 1, PacketSize, Packet );
	}
	return S_OK;
}

//---

/** Called when a tablet is detected */
static void SetupPacketDescriptions(IRealTimeStylus* RealTimeStylus, FInkTabletContextInfo& TabletContext)
{
	ULONG NumPacketProperties = 0;
	PACKET_PROPERTY* PacketProperties = nullptr;
	HRESULT hr = RealTimeStylus->GetPacketDescriptionData(TabletContext.ID, nullptr, nullptr, &NumPacketProperties, &PacketProperties);
	if (SUCCEEDED(hr) && PacketProperties != nullptr)
	{
		for (ULONG PropIdx = 0; PropIdx < NumPacketProperties; ++PropIdx)
		{
			PACKET_PROPERTY CurrentProperty = PacketProperties[PropIdx];

			EInkPacketType PacketType = EInkPacketType::None;
			if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_X)
			{
				PacketType = EInkPacketType::X;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_Y)
			{
				PacketType = EInkPacketType::Y;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_Z)
			{
				PacketType = EInkPacketType::Z;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_PACKET_STATUS)
			{
				PacketType = EInkPacketType::Status;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_TIMER_TICK)
			{
				PacketType = EInkPacketType::Timer;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE)
			{
				PacketType = EInkPacketType::NormalPressure;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_TANGENT_PRESSURE)
			{
				PacketType = EInkPacketType::TangentPressure;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_BUTTON_PRESSURE)
			{
				PacketType = EInkPacketType::ButtonPressure;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_ALTITUDE_ORIENTATION)
			{
				PacketType = EInkPacketType::Altitude;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_AZIMUTH_ORIENTATION)
			{
				PacketType = EInkPacketType::Azimuth;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_TWIST_ORIENTATION)
			{
				PacketType = EInkPacketType::Twist;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_X_TILT_ORIENTATION)
			{
				PacketType = EInkPacketType::XTilt;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_Y_TILT_ORIENTATION)
			{
				PacketType = EInkPacketType::YTilt;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_WIDTH)
			{
				PacketType = EInkPacketType::Width;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_HEIGHT)
			{
				PacketType = EInkPacketType::Height;
			}

			int32 CreatedIdx = TabletContext.PacketDescriptions.Emplace();
			FInkPacketDescription& PacketDescription = TabletContext.PacketDescriptions[CreatedIdx];
			PacketDescription.Type = PacketType;
			PacketDescription.Minimum = CurrentProperty.PropertyMetrics.nLogicalMin;
			PacketDescription.Maximum = CurrentProperty.PropertyMetrics.nLogicalMax;
			PacketDescription.Resolution = CurrentProperty.PropertyMetrics.fResolution;
		}

		::CoTaskMemFree(PacketProperties);
	}
}

/** Called when a tablet is detected */
static void SetupTabletSupportedPackets(TComPtr<IRealTimeStylus> RealTimeStylus, FInkTabletContextInfo& TabletContext)
{
	IInkTablet* InkTablet;
	RealTimeStylus->GetTabletFromTabletContextId(TabletContext.ID, &InkTablet);

	int16 Supported;

	BSTR GuidBSTR;
	
	GuidBSTR = SysAllocString(STR_GUID_X);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::X);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_Y);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Y);
	}
	
	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_Z);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Z);
		TabletContext.AddSupportedInput(EStylusInputType::Z);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_TIMERTICK);
	
	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Timer);
		TabletContext.AddSupportedInput(EStylusInputType::Timer);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_PAKETSTATUS);
	
	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Status);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_NORMALPRESSURE);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::NormalPressure);
		TabletContext.AddSupportedInput(EStylusInputType::Pressure);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_TANGENTPRESSURE);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::TangentPressure);
		TabletContext.AddSupportedInput(EStylusInputType::TangentPressure);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_BUTTONPRESSURE);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::ButtonPressure);
		TabletContext.AddSupportedInput(EStylusInputType::ButtonPressure);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_AZIMUTHORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Azimuth);
        TabletContext.AddSupportedInput(EStylusInputType::Azimuth);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_ALTITUDEORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Altitude);
        TabletContext.AddSupportedInput(EStylusInputType::Altitude);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_XTILTORIENTATION);
	
	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::XTilt);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_YTILTORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::YTilt);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_TWISTORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Twist);
		TabletContext.AddSupportedInput(EStylusInputType::Twist);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_WIDTH);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Width);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_HEIGHT);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add( EInkPacketType::Height);
	}

	SysFreeString(GuidBSTR);

	if (TabletContext.SupportedPackets.Contains( EInkPacketType::X) &&
		TabletContext.SupportedPackets.Contains( EInkPacketType::Y))
	{
		TabletContext.AddSupportedInput(EStylusInputType::Position);
	}

	if (TabletContext.SupportedPackets.Contains( EInkPacketType::XTilt) &&
		TabletContext.SupportedPackets.Contains( EInkPacketType::YTilt))
	{
		TabletContext.AddSupportedInput(EStylusInputType::Tilt);
	}

	if (TabletContext.SupportedPackets.Contains( EInkPacketType::Width) &&
		TabletContext.SupportedPackets.Contains( EInkPacketType::Height))
	{
		TabletContext.AddSupportedInput(EStylusInputType::Size);
	}
}

FInkTabletContextInfo* FInkRealTimeStylusPlugin::FindTabletContext(TABLET_CONTEXT_ID TabletID)
{
	for ( FInkTabletContextInfo& TabletContext : TabletContexts)
	{
		if (TabletContext.ID == TabletID)
		{
			return &TabletContext;
		}
	}
	return nullptr;
}

void FInkRealTimeStylusPlugin::AddTabletContext(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletID)
{
	FInkTabletContextInfo* FoundContext = FindTabletContext(TabletID);
	if (FoundContext == nullptr)
	{
		int32 Created = TabletContexts.Emplace();
		FoundContext = &TabletContexts[Created];
		FoundContext->ID = TabletID;

        IInkTablet* pInkTablet = NULL;
        if( SUCCEEDED( RealTimeStylus->GetTabletFromTabletContextId( TabletID, &pInkTablet ) ) )
        {
            IInkTablet2* iInkTablet2 = 0;
            pInkTablet->QueryInterface( __uuidof( IInkTablet2 ), (void**)&iInkTablet2 );
            if( SUCCEEDED( iInkTablet2->get_DeviceKind( &FoundContext->Kind ) ) )
            {
                //UE_LOG( LogStylusInput, Log, TEXT( "AddTabletContext: kind %d" ), FoundContext->Kind );
            }
            iInkTablet2->Release();
        }
	}

	SetupTabletSupportedPackets(RealTimeStylus, *FoundContext);
	SetupPacketDescriptions(RealTimeStylus, *FoundContext);
}

void FInkRealTimeStylusPlugin::RemoveTabletContext(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletID)
{
	for (int32 ExistingIdx = 0; ExistingIdx < TabletContexts.Num(); ++ExistingIdx)
	{
		if (TabletContexts[ExistingIdx].ID == TabletID)
		{
			TabletContexts.RemoveAt(ExistingIdx);
			break;
		}
	}
}

//---

HRESULT FInkRealTimeStylusPlugin::RealTimeStylusEnabled(IRealTimeStylus* RealTimeStylus, ULONG Num, const TABLET_CONTEXT_ID* InTabletContexts)
{
	for (ULONG TabletIdx = 0; TabletIdx < Num; ++TabletIdx)
	{
		AddTabletContext(RealTimeStylus, InTabletContexts[TabletIdx]);
	}
	return S_OK;
}

HRESULT FInkRealTimeStylusPlugin::RealTimeStylusDisabled(IRealTimeStylus* RealTimeStylus, ULONG Num, const TABLET_CONTEXT_ID* InTabletContexts)
{
	for (ULONG TabletIdx = 0; TabletIdx < Num; ++TabletIdx)
	{
		RemoveTabletContext(RealTimeStylus, InTabletContexts[TabletIdx]);
	}
	return S_OK;
}

HRESULT FInkRealTimeStylusPlugin::TabletAdded(IRealTimeStylus* RealTimeStylus, IInkTablet* InkTablet)
{
	TABLET_CONTEXT_ID TabletID;
	if (SUCCEEDED(RealTimeStylus->GetTabletContextIdFromTablet(InkTablet, &TabletID)))
	{
		AddTabletContext(RealTimeStylus, TabletID);
	}
	return S_OK;
}

HRESULT FInkRealTimeStylusPlugin::TabletRemoved(IRealTimeStylus* RealTimeStylus, LONG iTabletIndex)
{
	TabletContexts.RemoveAt(iTabletIndex);
	return S_OK;
}

//---

// This is used to manage concurrency of the InkStates variable between:
// - the subsystem tick which copy its content and empty the array
// - the HandlePacket "thread" which get all packets from ink driver and fill InkStates
FCriticalSection sgMutex;

//---

void 
FInkTabletContextInfo::Tick()
{
	// Move the generic current states to the previous ones
    PreviousState = CurrentState;
    CurrentState.Empty();

	// Get all current ink states and reset the buffer
    sgMutex.Lock();
    TArray<FInkStylusState> tmp( InkStates );
	InkStates.Empty();
    sgMutex.Unlock();

	// Convert all ink states to the generic ones and fill the generic state buffer
    for( FInkStylusState& ink_state : tmp )
    {
        CurrentState.Push( ink_state.ToPublicState() );
    }

	// Everything is clean now
    Dirty = false;
}

//---

static float Normalize(int Value, const FInkPacketDescription& Desc)
{
	return (float) (Value - Desc.Minimum) / (float) (Desc.Maximum - Desc.Minimum);
}

static float ToDegrees(int Value, const FInkPacketDescription& Desc)
{
	return Value / Desc.Resolution;
}

static void GetDPI( IRealTimeStylus* RealTimeStylus, int& oDPIX, int& oDPIY, POINT& oClientUL, POINT& oClientLR )
{
	HANDLE_PTR HCurrentWnd;
	RealTimeStylus->get_HWND( &HCurrentWnd );
	HWND Hwnd = reinterpret_cast<HWND>( HCurrentWnd );
	HDC hdc = GetDC( Hwnd );

	RECT rcClient;
	GetClientRect( Hwnd, &rcClient );
	oClientUL.x = rcClient.left;
	oClientUL.y = rcClient.top;
	oClientLR.x = rcClient.right + 1;
	oClientLR.y = rcClient.bottom + 1;
	ClientToScreen( Hwnd, &oClientUL );
	ClientToScreen( Hwnd, &oClientLR );

	//UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket: UL:%ld %ld LR:%ld %ld" ), ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y );

	oDPIX = GetDeviceCaps( hdc, LOGPIXELSX );
	oDPIY = GetDeviceCaps( hdc, LOGPIXELSY );

	ReleaseDC( Hwnd, hdc );
}

void FInkRealTimeStylusPlugin::HandlePacket(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets)
{
	FInkTabletContextInfo* TabletContext = FindTabletContext(StylusInfo->tcid);
	if (TabletContext == nullptr)
	{
		return;
	}

	//---

	//TODO: find another place to get dpis, not for each packet, but be sure to update data when moving/resizing HWND
	// it can't be in AddTabletContext() because it is NOT called when moving HWND
	int dpix;
	int dpiy;
	POINT ptClientUL; // client upper left corner 
	POINT ptClientLR; // client lower right corner 
	GetDPI( RealTimeStylus, dpix, dpiy, ptClientUL, ptClientLR );

	//---

	TArray<FInkStylusState> states;

	// Number of properties for a packet
	ULONG PropertyCount = PacketBufferLength / PacketCount;

    //UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket" ) ); // also remove #include
	//UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket : %d %d %s" ), PacketCount, PropertyCount, TabletContext->IsTouching ? "down" : "up"); // also remove #include

	for (ULONG j = 0; j < PacketCount; ++j)
	{
		FInkStylusState ink_state;
		ink_state.IsInverted = StylusInfo->bIsInvertedCursor;

		for (ULONG i = 0; i < PropertyCount; ++i)
		{
			const FInkPacketDescription& PacketDescription = TabletContext->PacketDescriptions[i];

			float Normalized = Normalize(Packets[i], PacketDescription);

			switch (PacketDescription.Type)
			{
				case EInkPacketType::X:
				{
					float x = Packets[i] / (1000.0 * 2.54 / dpix); // http://code.rawlinson.us/2007/01/pixelspace-to-inkspace.html
					ink_state.Position.X = x + ptClientUL.x;
					break;
				}
				case EInkPacketType::Y:
				{
					float y = Packets[i] / (1000.0 * 2.54 / dpiy);
					ink_state.Position.Y = y + ptClientUL.y;
					break;
				}
				case EInkPacketType::Timer:
					ink_state.Timer = Packets[i];
					break;
				case EInkPacketType::Status:
					break;
				case EInkPacketType::Z:
					ink_state.Z = Normalized;
					break;
				case EInkPacketType::NormalPressure:
					ink_state.NormalPressure = Normalized;
					break;
				case EInkPacketType::TangentPressure:
					ink_state.TangentPressure = Normalized;
					break;
				case EInkPacketType::Twist:
					ink_state.Twist = ToDegrees(Packets[i], PacketDescription);
					break;
				case EInkPacketType::XTilt:
					ink_state.Tilt.X = ToDegrees(Packets[i], PacketDescription);
					break;
				case EInkPacketType::YTilt:
					ink_state.Tilt.Y = ToDegrees(Packets[i], PacketDescription);
					break;
				case EInkPacketType::Azimuth:
					ink_state.Azimuth = ToDegrees(Packets[i], PacketDescription);
					break;
				case EInkPacketType::Altitude:
					ink_state.Altitude = ToDegrees(Packets[i], PacketDescription);
					break;
				case EInkPacketType::Width:
					ink_state.Size.X = Normalized;
					break;
				case EInkPacketType::Height:
					ink_state.Size.Y = Normalized;
					break;
			}
		}

		ink_state.IsTouching = TabletContext->IsTouching;

		if( TabletContext->IsTouching && TabletContext->Kind == TDK_Mouse ) // Here, as we don't have pressure packet for mouse -> simulate it (TODO: maybe do it for other attributes)
			ink_state.NormalPressure = 1.0;

		states.Add( ink_state );

		//UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket x:%f y:%f" ), ink_state.Position.X, ink_state.Position.Y );
	}

	// Add the new states to the list of all states between 2 subsystem ticks
	sgMutex.Lock();
	TabletContext->SetDirty();
	TabletContext->InkStates.Append(states);
	sgMutex.Unlock();
}
