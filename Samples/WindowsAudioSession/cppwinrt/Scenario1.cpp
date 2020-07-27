//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario1.h"
#include "Scenario1.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Devices::Enumeration;

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    void Scenario1::StatusMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Hello", NotifyType::StatusMessage);
    }

    void Scenario1::ErrorMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Oh dear.\nSomething went wrong.", NotifyType::ErrorMessage);
    }

    void Scenario1::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
    }

    void Scenario1::Enumerate_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        auto b = sender.try_as<Controls::Button>();
        if (b)
        {
            DevicesList().Items().Clear();
            EnumerateAudioDevicesAsync();
        }
    }

    fire_and_forget Scenario1::EnumerateAudioDevicesAsync()
    {
        const hstring PKEY_AudioEndpoint_Supports_EventDriven_Mode{ L"{1da5d803-d492-4edd-8c23-e0c0ffee7f0e} 7" };

        // Get the string identifier of the audio renderer
        auto audioSelector = MediaDevice::GetAudioRenderSelector();

        // Perform the asynchronous callback with the custom properties in the query
        auto lifetime = get_strong();
        auto deviceInfoCollection = co_await DeviceInformation::FindAllAsync(audioSelector, { PKEY_AudioEndpoint_Supports_EventDriven_Mode });

        if (!deviceInfoCollection || deviceInfoCollection.Size() == 0)
        {
            rootPage.NotifyUser(L"No Devices Found.", NotifyType::ErrorMessage);
            co_return;
        }

        try
        {
            for (auto deviceInfo : deviceInfoCollection)
            {
                auto deviceInfoString = deviceInfo.Name();
                if (deviceInfo.Properties().Size() > 0)
                {
                    auto devicePropString = deviceInfo.Properties().TryLookup(PKEY_AudioEndpoint_Supports_EventDriven_Mode);
                    if (devicePropString)
                    {
                        deviceInfoString = deviceInfoString + L" --> EventDriven(" + to_hstring(unbox_value<uint32_t>(devicePropString)) + L")";
                    }
                }
                DevicesList().Items().Append(box_value(deviceInfoString));
            }
            
            auto strMsg = L"Enumerated " + to_hstring(deviceInfoCollection.Size()) + L" Device(s).";
            rootPage.NotifyUser(strMsg, NotifyType::StatusMessage);
        }
        catch (winrt::hresult_error const& e)
        {
            rootPage.NotifyUser(e.message(), NotifyType::ErrorMessage);
        }

        co_return;
    }
}
