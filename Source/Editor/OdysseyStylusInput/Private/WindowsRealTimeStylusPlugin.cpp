// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "WindowsRealTimeStylusPlugin.h"
//#include "IOdysseyStylusInputModule.h" // for UE_LOG

#if PLATFORM_WINDOWS

HRESULT FWindowsRealTimeStylusPlugin::QueryInterface(const IID& InterfaceID, void** Pointer)
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

HRESULT FWindowsRealTimeStylusPlugin::StylusDown(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketSize, LONG* Packet, LONG** InOutPackets)
{
	FTabletContextInfo* TabletContext = FindTabletContext(StylusInfo->tcid);
	if (TabletContext != nullptr)
	{
        TabletContext->IsTouching = true;

        HandlePacket( RealTimeStylus, StylusInfo, 1, PacketSize, Packet ); // need to be done after IsTouching = true (HandlePacket uses it)

        //if( TabletContext->mKind == TDK_Mouse ) // Here, as we don't have pressure packet for mouse -> simulate it (and maybe other attributes)
        //    TabletContext->WindowsState.Last().NormalPressure = 1.0;
	}
	return S_OK;
}

HRESULT FWindowsRealTimeStylusPlugin::StylusUp(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketSize, LONG* Packet, LONG** InOutPackets)
{
	FTabletContextInfo* TabletContext = FindTabletContext(StylusInfo->tcid);
	if (TabletContext != nullptr)
	{
        // we know this is not touching
        TabletContext->IsTouching = false;
		//TabletContext->WindowsState2.NormalPressure = 0;

        HandlePacket( RealTimeStylus, StylusInfo, 1, PacketSize, Packet );
	}
	return S_OK;
}

static void SetupPacketDescriptions(IRealTimeStylus* RealTimeStylus, FTabletContextInfo& TabletContext)
{
	ULONG NumPacketProperties = 0;
	PACKET_PROPERTY* PacketProperties = nullptr;
	HRESULT hr = RealTimeStylus->GetPacketDescriptionData(TabletContext.ID, nullptr, nullptr, &NumPacketProperties, &PacketProperties);
	if (SUCCEEDED(hr) && PacketProperties != nullptr)
	{
		for (ULONG PropIdx = 0; PropIdx < NumPacketProperties; ++PropIdx)
		{
			PACKET_PROPERTY CurrentProperty = PacketProperties[PropIdx];

			EWindowsPacketType PacketType = EWindowsPacketType::None;
			if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_X)
			{
				PacketType = EWindowsPacketType::X;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_Y)
			{
				PacketType = EWindowsPacketType::Y;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_Z)
			{
				PacketType = EWindowsPacketType::Z;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_PACKET_STATUS)
			{
				PacketType = EWindowsPacketType::Status;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_TIMER_TICK)
			{
				PacketType = EWindowsPacketType::Timer;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE)
			{
				PacketType = EWindowsPacketType::NormalPressure;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_TANGENT_PRESSURE)
			{
				PacketType = EWindowsPacketType::TangentPressure;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_BUTTON_PRESSURE)
			{
				PacketType = EWindowsPacketType::ButtonPressure;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_ALTITUDE_ORIENTATION)
			{
				PacketType = EWindowsPacketType::Altitude;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_AZIMUTH_ORIENTATION)
			{
				PacketType = EWindowsPacketType::Azimuth;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_TWIST_ORIENTATION)
			{
				PacketType = EWindowsPacketType::Twist;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_X_TILT_ORIENTATION)
			{
				PacketType = EWindowsPacketType::XTilt;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_Y_TILT_ORIENTATION)
			{
				PacketType = EWindowsPacketType::YTilt;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_WIDTH)
			{
				PacketType = EWindowsPacketType::Width;
			}
			else if (CurrentProperty.guid == GUID_PACKETPROPERTY_GUID_HEIGHT)
			{
				PacketType = EWindowsPacketType::Height;
			}

			int32 CreatedIdx = TabletContext.PacketDescriptions.Emplace();
			FPacketDescription& PacketDescription = TabletContext.PacketDescriptions[CreatedIdx];
			PacketDescription.Type = PacketType;
			PacketDescription.Minimum = CurrentProperty.PropertyMetrics.nLogicalMin;
			PacketDescription.Maximum = CurrentProperty.PropertyMetrics.nLogicalMax;
			PacketDescription.Resolution = CurrentProperty.PropertyMetrics.fResolution;
		}

		::CoTaskMemFree(PacketProperties);
	}
}

static void SetupTabletSupportedPackets(TComPtr<IRealTimeStylus> RealTimeStylus, FTabletContextInfo& TabletContext)
{
	IInkTablet* InkTablet;
	RealTimeStylus->GetTabletFromTabletContextId(TabletContext.ID, &InkTablet);

	int16 Supported;

	BSTR GuidBSTR;
	
	GuidBSTR = SysAllocString(STR_GUID_X);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::X);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_Y);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Y);
	}
	
	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_Z);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Z);
		TabletContext.AddSupportedInput(EStylusInputType::Z);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_TIMERTICK);
	
	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Timer);
		TabletContext.AddSupportedInput(EStylusInputType::Timer);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_PAKETSTATUS);
	
	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Status);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_NORMALPRESSURE);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::NormalPressure);
		TabletContext.AddSupportedInput(EStylusInputType::Pressure);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_TANGENTPRESSURE);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::TangentPressure);
		TabletContext.AddSupportedInput(EStylusInputType::TangentPressure);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_BUTTONPRESSURE);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::ButtonPressure);
		TabletContext.AddSupportedInput(EStylusInputType::ButtonPressure);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_AZIMUTHORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Azimuth);
        TabletContext.AddSupportedInput(EStylusInputType::Azimuth);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_ALTITUDEORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Altitude);
        TabletContext.AddSupportedInput(EStylusInputType::Altitude);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_XTILTORIENTATION);
	
	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::XTilt);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_YTILTORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::YTilt);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_TWISTORIENTATION);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Twist);
		TabletContext.AddSupportedInput(EStylusInputType::Twist);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_WIDTH);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Width);
	}

	SysFreeString(GuidBSTR);
	GuidBSTR = SysAllocString(STR_GUID_HEIGHT);

	InkTablet->IsPacketPropertySupported(GuidBSTR, &Supported);
	if (Supported)
	{
		TabletContext.SupportedPackets.Add(EWindowsPacketType::Height);
	}

	SysFreeString(GuidBSTR);

	if (TabletContext.SupportedPackets.Contains(EWindowsPacketType::X) &&
		TabletContext.SupportedPackets.Contains(EWindowsPacketType::Y))
	{
		TabletContext.AddSupportedInput(EStylusInputType::Position);
	}

	if (TabletContext.SupportedPackets.Contains(EWindowsPacketType::XTilt) &&
		TabletContext.SupportedPackets.Contains(EWindowsPacketType::YTilt))
	{
		TabletContext.AddSupportedInput(EStylusInputType::Tilt);
	}

	if (TabletContext.SupportedPackets.Contains(EWindowsPacketType::Width) &&
		TabletContext.SupportedPackets.Contains(EWindowsPacketType::Height))
	{
		TabletContext.AddSupportedInput(EStylusInputType::Size);
	}
}

FTabletContextInfo* FWindowsRealTimeStylusPlugin::FindTabletContext(TABLET_CONTEXT_ID TabletID)
{
	for (FTabletContextInfo& TabletContext : TabletContexts)
	{
		if (TabletContext.ID == TabletID)
		{
			return &TabletContext;
		}
	}
	return nullptr;
}

void FWindowsRealTimeStylusPlugin::AddTabletContext(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletID)
{
	FTabletContextInfo* FoundContext = FindTabletContext(TabletID);
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
            if( SUCCEEDED( iInkTablet2->get_DeviceKind( &FoundContext->mKind ) ) )
            {
                //UE_LOG( LogStylusInput, Log, TEXT( "AddTabletContext: kind %d" ), FoundContext->mKind );
            }
            iInkTablet2->Release();
        }
	}

	SetupTabletSupportedPackets(RealTimeStylus, *FoundContext);
	SetupPacketDescriptions(RealTimeStylus, *FoundContext);
}

void FWindowsRealTimeStylusPlugin::RemoveTabletContext(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletID)
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

HRESULT FWindowsRealTimeStylusPlugin::RealTimeStylusEnabled(IRealTimeStylus* RealTimeStylus, ULONG Num, const TABLET_CONTEXT_ID* InTabletContexts)
{
	for (ULONG TabletIdx = 0; TabletIdx < Num; ++TabletIdx)
	{
		AddTabletContext(RealTimeStylus, InTabletContexts[TabletIdx]);
	}
	return S_OK;
}

HRESULT FWindowsRealTimeStylusPlugin::RealTimeStylusDisabled(IRealTimeStylus* RealTimeStylus, ULONG Num, const TABLET_CONTEXT_ID* InTabletContexts)
{
	for (ULONG TabletIdx = 0; TabletIdx < Num; ++TabletIdx)
	{
		RemoveTabletContext(RealTimeStylus, InTabletContexts[TabletIdx]);
	}
	return S_OK;
}

HRESULT FWindowsRealTimeStylusPlugin::TabletAdded(IRealTimeStylus* RealTimeStylus, IInkTablet* InkTablet)
{
	TABLET_CONTEXT_ID TabletID;
	if (SUCCEEDED(RealTimeStylus->GetTabletContextIdFromTablet(InkTablet, &TabletID)))
	{
		AddTabletContext(RealTimeStylus, TabletID);
	}
	return S_OK;
}

HRESULT FWindowsRealTimeStylusPlugin::TabletRemoved(IRealTimeStylus* RealTimeStylus, LONG iTabletIndex)
{
	TabletContexts.RemoveAt(iTabletIndex);
	return S_OK;
}

static float Normalize(int Value, const FPacketDescription& Desc)
{
	return (float) (Value - Desc.Minimum) / (float) (Desc.Maximum - Desc.Minimum);
}

static float ToDegrees(int Value, const FPacketDescription& Desc)
{
	return Value / Desc.Resolution;
}

FCriticalSection sgMutex;

void 
FTabletContextInfo::Tick()
{
    PreviousState = CurrentState;
    CurrentState.Empty();

    sgMutex.Lock();
    TArray<FWindowsStylusState> tmp( WindowsState );
    WindowsState.Empty();
    sgMutex.Unlock();

    for( FWindowsStylusState& window_state : tmp )
    {
        CurrentState.Push( window_state.ToPublicState() );
    }

    Dirty = false;
}

void FWindowsRealTimeStylusPlugin::HandlePacket(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets)
{
	FTabletContextInfo* TabletContext = FindTabletContext(StylusInfo->tcid);
	if (TabletContext == nullptr)
	{
		return;
	}

	TArray<FWindowsStylusState> states;

    //TODO: find another place to get data, not for each packet, but be sure to update data when moving/resizing HWND
    // can't be in AddTabletContext() because it is NOT called when moving HWND
    HANDLE_PTR HCurrentWnd;
    RealTimeStylus->get_HWND( &HCurrentWnd );
    HWND Hwnd = reinterpret_cast<HWND>( HCurrentWnd );
    HDC hdc = GetDC( Hwnd );

    RECT rcClient;
    GetClientRect( Hwnd, &rcClient );
    POINT ptClientUL;              // client upper left corner 
    POINT ptClientLR;              // client lower right corner 
    ptClientUL.x = rcClient.left;
    ptClientUL.y = rcClient.top;
    ptClientLR.x = rcClient.right + 1;
    ptClientLR.y = rcClient.bottom + 1;
    ClientToScreen( Hwnd, &ptClientUL );
    ClientToScreen( Hwnd, &ptClientLR );
    //UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket: UL:%ld %ld LR:%ld %ld" ), ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y );

    int dpix = GetDeviceCaps( hdc, LOGPIXELSX );
    int dpiy = GetDeviceCaps( hdc, LOGPIXELSY );

    ReleaseDC( Hwnd, hdc );
    //---

	ULONG PropertyCount = PacketBufferLength / PacketCount;
    //UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket" ) ); // also remove #include
	//UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket : %d %d %s" ), PacketCount, PropertyCount, TabletContext->IsTouching ? "down" : "up"); // also remove #include

	for (ULONG j = 0; j < PacketCount; ++j)
	{
		FWindowsStylusState windows_state;
		windows_state.IsInverted = StylusInfo->bIsInvertedCursor;

		for (ULONG i = 0; i < PropertyCount; ++i)
		{
			const FPacketDescription& PacketDescription = TabletContext->PacketDescriptions[i];

			float Normalized = Normalize(Packets[i], PacketDescription);

			switch (PacketDescription.Type)
			{
				case EWindowsPacketType::X:
				{
					float x = Packets[i] / (1000.0 * 2.54 / dpix); // http://code.rawlinson.us/2007/01/pixelspace-to-inkspace.html
					//float x = Packets[i] / 11.76;
					windows_state.Position.X = x + ptClientUL.x;
					break;
				}
				case EWindowsPacketType::Y:
				{
					float y = Packets[i] / (1000.0 * 2.54 / dpiy);
					windows_state.Position.Y = y + ptClientUL.y;
					break;
				}
				case EWindowsPacketType::Timer:
					windows_state.Timer = Packets[i];
					break;
				case EWindowsPacketType::Status:
					break;
				case EWindowsPacketType::Z:
					windows_state.Z = Normalized;
					break;
				case EWindowsPacketType::NormalPressure:
					windows_state.NormalPressure = Normalized;
					break;
				case EWindowsPacketType::TangentPressure:
					windows_state.TangentPressure = Normalized;
					break;
				case EWindowsPacketType::Twist:
					windows_state.Twist = ToDegrees(Packets[i], PacketDescription);
					break;
				case EWindowsPacketType::XTilt:
					windows_state.Tilt.X = ToDegrees(Packets[i], PacketDescription);
					break;
				case EWindowsPacketType::YTilt:
					windows_state.Tilt.Y = ToDegrees(Packets[i], PacketDescription);
					break;
				case EWindowsPacketType::Azimuth:
					windows_state.Azimuth = ToDegrees(Packets[i], PacketDescription);
					break;
				case EWindowsPacketType::Altitude:
					windows_state.Altitude = ToDegrees(Packets[i], PacketDescription);
					break;
				case EWindowsPacketType::Width:
					windows_state.Size.X = Normalized;
					break;
				case EWindowsPacketType::Height:
					windows_state.Size.Y = Normalized;
					break;
			}
		}
		windows_state.IsTouching = TabletContext->IsTouching;

		if( TabletContext->IsTouching && TabletContext->mKind == TDK_Mouse ) // Here, as we don't have pressure packet for mouse -> simulate it (and maybe other attributes)
			windows_state.NormalPressure = 1.0;

		states.Add(windows_state);
		//UE_LOG( LogStylusInput, Log, TEXT( "HandlePacket x:%f y:%f" ), windows_state.Position.X, windows_state.Position.Y );
	}

	sgMutex.Lock();
	TabletContext->SetDirty();
	TabletContext->WindowsState.Append(states);
	sgMutex.Unlock();
}

HRESULT FWindowsRealTimeStylusPlugin::Packets(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo,
	ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets, ULONG* InOutPackets, LONG** PtrInOutPackets)
{
	HandlePacket(RealTimeStylus, StylusInfo, PacketCount, PacketBufferLength, Packets);
	return S_OK;
}

HRESULT FWindowsRealTimeStylusPlugin::InAirPackets(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo,
	ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets, ULONG* InOutPackets, LONG** PtrInOutPackets)
{
	HandlePacket(RealTimeStylus, StylusInfo, PacketCount, PacketBufferLength, Packets);
	return S_OK;
}

#endif // PLATFORM_WINDOWS
