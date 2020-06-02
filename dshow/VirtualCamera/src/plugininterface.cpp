/* akvirtualcamera, virtual camera for Mac and Windows.
 * Copyright (C) 2020  Gonzalo Exequiel Pedone
 *
 * akvirtualcamera is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * akvirtualcamera is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with akvirtualcamera. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

#include <vector>
#include <combaseapi.h>
#include <winreg.h>
#include <uuids.h>

#include "plugininterface.h"
#include "PlatformUtils/src/utils.h"
#include "VCamUtils/src/utils.h"

#define AK_CUR_INTERFACE "PluginInterface"

namespace AkVCam
{
    class PluginInterfacePrivate
    {
        public:
            HINSTANCE m_pluginHinstance {nullptr};

            LONG deleteTree(HKEY hKey, LPCTSTR lpSubKey);
    };
}

AkVCam::PluginInterface::PluginInterface()
{
    this->d = new PluginInterfacePrivate;
}

AkVCam::PluginInterface::~PluginInterface()
{
    delete this->d;
}

HINSTANCE AkVCam::PluginInterface::pluginHinstance() const
{
    return this->d->m_pluginHinstance;
}

HINSTANCE &AkVCam::PluginInterface::pluginHinstance()
{
    return this->d->m_pluginHinstance;
}

bool AkVCam::PluginInterface::registerServer(const std::wstring &deviceId,
                                             const std::wstring &description) const
{
    AkLogMethod();

    // Define the layout in registry of the filter.

    auto clsid = createClsidWStrFromStr(deviceId);
    auto fileName = AkVCam::moduleFileNameW(this->d->m_pluginHinstance);
    std::wstring threadingModel = L"Both";

    AkLoggerLog("CLSID: ", std::string(clsid.begin(), clsid.end()));
    AkLoggerLog("Description: ", std::string(description.begin(), description.end()));
    AkLoggerLog("Filename: ", std::string(fileName.begin(), fileName.end()));

    auto subkey = L"CLSID\\" + clsid;

    HKEY keyCLSID = nullptr;
    HKEY keyServerType = nullptr;
    LONG result = RegCreateKey(HKEY_CLASSES_ROOT, subkey.c_str(), &keyCLSID);
    bool ok = false;

    if (result != ERROR_SUCCESS)
        goto registerServer_failed;

    result =
            RegSetValue(keyCLSID,
                        nullptr,
                        REG_SZ,
                        description.c_str(),
                        DWORD(description.size()));

    if (result != ERROR_SUCCESS)
        goto registerServer_failed;

    result = RegCreateKey(keyCLSID, L"InprocServer32", &keyServerType);

    if (result != ERROR_SUCCESS)
        goto registerServer_failed;

    result =
            RegSetValue(keyServerType,
                        nullptr,
                        REG_SZ,
                        fileName.c_str(),
                        DWORD(fileName.size()));

    if (result != ERROR_SUCCESS)
        goto registerServer_failed;

    result =
            RegSetValueEx(keyServerType,
                          L"ThreadingModel",
                          0L,
                          REG_SZ,
                          reinterpret_cast<const BYTE *>(threadingModel.c_str()),
                          DWORD((threadingModel.size() + 1) * sizeof(wchar_t)));

    ok = true;

registerServer_failed:
    if (keyServerType)
        RegCloseKey(keyServerType);

    if (keyCLSID)
        RegCloseKey(keyCLSID);

    AkLoggerLog("Result: ", stringFromResult(result));

    return ok;
}

void AkVCam::PluginInterface::unregisterServer(const std::string &deviceId) const
{
    AkLogMethod();

    this->unregisterServer(createClsidFromStr(deviceId));
}

void AkVCam::PluginInterface::unregisterServer(const std::wstring &deviceId) const
{
    AkLogMethod();

    this->unregisterServer(createClsidFromStr(deviceId));
}

void AkVCam::PluginInterface::unregisterServer(const CLSID &clsid) const
{
    AkLogMethod();

    auto clsidStr = stringFromClsid(clsid);
    AkLoggerLog("CLSID: ", clsidStr);
    auto subkey = L"CLSID\\" + std::wstring(clsidStr.begin(), clsidStr.end());

    this->d->deleteTree(HKEY_CLASSES_ROOT, subkey.c_str());
}

bool AkVCam::PluginInterface::registerFilter(const std::wstring &deviceId,
                                             const std::wstring &description) const
{
    AkLogMethod();

    auto clsid = AkVCam::createClsidFromStr(deviceId);
    IFilterMapper2 *filterMapper = nullptr;
    IMoniker *pMoniker = nullptr;
    std::vector<REGPINTYPES> pinTypes {
        {&MEDIATYPE_Video, &MEDIASUBTYPE_NULL}
    };
    std::vector<REGPINMEDIUM> mediums;
    std::vector<REGFILTERPINS2> pins {
        {
            REG_PINFLAG_B_OUTPUT,
            1,
            UINT(pinTypes.size()),
            pinTypes.data(),
            UINT(mediums.size()),
            mediums.data(),
            &PIN_CATEGORY_CAPTURE
        }
    };
    REGFILTER2 regFilter;
    regFilter.dwVersion = 2;
    regFilter.dwMerit = MERIT_DO_NOT_USE;
    regFilter.cPins2 = ULONG(pins.size());
    regFilter.rgPins2 = pins.data();

    auto result = CoInitialize(nullptr);
    bool ok = false;

    if (FAILED(result))
        goto registerFilter_failed;

    result = CoCreateInstance(CLSID_FilterMapper2,
                              nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_IFilterMapper2,
                              reinterpret_cast<void **>(&filterMapper));

    if (FAILED(result))
        goto registerFilter_failed;

    result = filterMapper->RegisterFilter(clsid,
                                          description.c_str(),
                                          &pMoniker,
                                          &CLSID_VideoInputDeviceCategory,
                                          nullptr,
                                          &regFilter);

    ok = true;

registerFilter_failed:

    if (filterMapper)
        filterMapper->Release();

    CoUninitialize();

    AkLoggerLog("Result: ", stringFromResult(result));

    return ok;
}

void AkVCam::PluginInterface::unregisterFilter(const std::string &deviceId) const
{
    AkLogMethod();

    this->unregisterFilter(AkVCam::createClsidFromStr(deviceId));
}

void AkVCam::PluginInterface::unregisterFilter(const std::wstring &deviceId) const
{
    AkLogMethod();

    this->unregisterFilter(AkVCam::createClsidFromStr(deviceId));
}

void AkVCam::PluginInterface::unregisterFilter(const CLSID &clsid) const
{
    AkLogMethod();
    IFilterMapper2 *filterMapper = nullptr;
    auto result = CoInitialize(nullptr);

    if (FAILED(result))
        goto unregisterFilter_failed;

    result = CoCreateInstance(CLSID_FilterMapper2,
                              nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_IFilterMapper2,
                              reinterpret_cast<void **>(&filterMapper));

    if (FAILED(result))
        goto unregisterFilter_failed;

    result = filterMapper->UnregisterFilter(&CLSID_VideoInputDeviceCategory,
                                            nullptr,
                                            clsid);

unregisterFilter_failed:

    if (filterMapper)
        filterMapper->Release();

    CoUninitialize();
    AkLoggerLog("Result: ", stringFromResult(result));
}

bool AkVCam::PluginInterface::setDevicePath(const std::wstring &deviceId) const
{
    AkLogMethod();

    std::wstring subKey =
            L"CLSID\\"
            + wstringFromIid(CLSID_VideoInputDeviceCategory)
            + L"\\Instance\\"
            + createClsidWStrFromStr(deviceId);
    AkLoggerLog("Key: HKEY_CLASSES_ROOT");
    AkLoggerLog("SubKey: ", std::string(subKey.begin(), subKey.end()));

    HKEY hKey = nullptr;
    auto result = RegOpenKeyEx(HKEY_CLASSES_ROOT,
                               subKey.c_str(),
                               0,
                               KEY_ALL_ACCESS,
                               &hKey);
    bool ok = false;

    if (result != ERROR_SUCCESS)
        goto setDevicePath_failed;

    result = RegSetValueEx(hKey,
                           TEXT("DevicePath"),
                           0,
                           REG_SZ,
                           reinterpret_cast<const BYTE *>(deviceId.c_str()),
                           DWORD((deviceId.size() + 1) * sizeof(wchar_t)));

    if (result != ERROR_SUCCESS)
        goto setDevicePath_failed;

    ok = true;

setDevicePath_failed:
    if (hKey)
        RegCloseKey(hKey);

    AkLoggerLog("Result: ", stringFromResult(result));

    return ok;
}

bool AkVCam::PluginInterface::createDevice(const std::wstring &deviceId,
                                           const std::wstring &description)
{
    AkLogMethod();

    if (!this->registerServer(deviceId, description))
        goto createDevice_failed;

    if (!this->registerFilter(deviceId, description))
        goto createDevice_failed;

    if (!this->setDevicePath(deviceId))
        goto createDevice_failed;

    return true;

createDevice_failed:
    this->destroyDevice(deviceId);

    return false;
}

void AkVCam::PluginInterface::destroyDevice(const std::string &deviceId)
{
    AkLogMethod();

    this->unregisterFilter(deviceId);
    this->unregisterServer(deviceId);
}

void AkVCam::PluginInterface::destroyDevice(const std::wstring &deviceId)
{
    AkLogMethod();

    this->unregisterFilter(deviceId);
    this->unregisterServer(deviceId);
}

void AkVCam::PluginInterface::destroyDevice(const CLSID &clsid)
{
    AkLogMethod();

    this->unregisterFilter(clsid);
    this->unregisterServer(clsid);
}

LONG AkVCam::PluginInterfacePrivate::deleteTree(HKEY hKey, LPCTSTR lpSubKey)
{
    HKEY key = nullptr;
    auto result = RegOpenKeyEx(hKey, lpSubKey, 0, MAXIMUM_ALLOWED, &key);

    if (result != ERROR_SUCCESS)
        return result;

    TCHAR subKey[MAX_PATH];
    DWORD subKeyLen = MAX_PATH;
    FILETIME lastWrite;

    while (RegEnumKeyEx(key,
                        0,
                        subKey,
                        &subKeyLen,
                        nullptr,
                        nullptr,
                        nullptr,
                        &lastWrite) == ERROR_SUCCESS) {
        this->deleteTree(key, subKey);
    }

    RegCloseKey(key);
    RegDeleteKey(hKey, lpSubKey);

    return ERROR_SUCCESS;
}
