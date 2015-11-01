/*
*      Copyright (C) 2005-2014 Team XBMC
*      http://xbmc.org
*
*      Copyright (C) 2014-2015 Aracnoz
*      http://github.com/aracnoz/xbmc
*
*  This Program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*
*  This Program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with XBMC; see the file COPYING.  If not, see
*  <http://www.gnu.org/licenses/>.
*
*/

#include "MadvrSettingsManager.h"
#include "mvrInterfaces.h"
#include "settings/Settings.h"
#include "settings/MediaSettings.h"
#include "Utils/Log.h"

const std::string CMadvrSettingsManager::DSPROFILE = "DSPlayer Profile";
const std::string CMadvrSettingsManager::DSGROUP = "DSPlayer Profile Group";

CMadvrSettingsManager::CMadvrSettingsManager(IUnknown* pUnk)
{
  m_pDXR = pUnk;
  CMadvrCallback::Get()->Register(this);
  InitSettings();
}

CMadvrSettingsManager::~CMadvrSettingsManager()
{
}

BOOL CMadvrSettingsManager::GetSettings(MADVR_SETTINGS_TYPE type, LPCWSTR path, int enumIndex, LPCWSTR sValue, BOOL* bValue, int* iValue, int *bufSize)
{
  if (Com::SmartQIPtr<IMadVRSettings2> pMadvrSettings2 = m_pDXR)
  {
    switch (type)
    {
    case MADVR_SETTINGS_PROFILEGROUPS:
      return pMadvrSettings2->SettingsEnumProfileGroups(path, enumIndex, sValue, bufSize);
    case MADVR_SETTINGS_PROFILES:
      pMadvrSettings2->SettingsEnumProfiles(path, enumIndex, sValue, bufSize);
      return TRUE;
    case MADVR_SETTINGS_STRING:
      return pMadvrSettings2->SettingsGetString(path, sValue, bufSize);
    case MADVR_SETTINGS_BOOL:
      return pMadvrSettings2->SettingsGetBoolean(path, bValue);
    case MADVR_SETTINGS_INT:
      return pMadvrSettings2->SettingsGetInteger(path, iValue);
    }
  }
  return FALSE;
}

BOOL CMadvrSettingsManager::GetSettings2(MADVR_SETTINGS_TYPE mType, LPCWSTR path, int enumIndex, LPCWSTR id, LPCWSTR name, LPCWSTR type, int *idBufSize, int *nameBufSize, int *typeBufSize)
{
  if (Com::SmartQIPtr<IMadVRSettings2> pMadvrSettings2 = m_pDXR)
  {
    switch (mType)
    {
    case MADVR_SETTINGS_FOLDERS:
      return pMadvrSettings2->SettingsEnumFolders(path, enumIndex, id, name, type, idBufSize, nameBufSize, typeBufSize);
    case MADVR_SETTINGS_VALUES:
      return pMadvrSettings2->SettingsEnumValues(path, enumIndex, id, name, type, idBufSize, nameBufSize, typeBufSize);
    }
  }
  return FALSE;
}

BOOL CMadvrSettingsManager::SetSettings(MADVR_SETTINGS_TYPE type, LPCWSTR path, LPCWSTR sValue, BOOL bValue, int iValue)
{
  if (Com::SmartQIPtr<IMadVRSettings2> pMadvrSettings2 = m_pDXR)
  {
    switch (type)
    {
    case MADVR_SETTINGS_STRING:
      return pMadvrSettings2->SettingsSetString(path, sValue);
    case MADVR_SETTINGS_BOOL:
      return pMadvrSettings2->SettingsSetBoolean(path, bValue);
    case MADVR_SETTINGS_INT:
      return pMadvrSettings2->SettingsSetInteger(path, iValue);
    }
  }
  return FALSE;
}

void CMadvrSettingsManager::EnumProfilesGroups(MADVR_SETTINGS_TYPE type, std::string path, std::vector<std::string> *sVector)
{
  sVector->clear();
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  std::string str;
  wchar_t* buf = NULL;
  int bufSize = 0;
  int enumIndex = 0;

  GetSettings(type, pathW.c_str(), enumIndex, NULL, NULL, NULL, &bufSize);
  while (bufSize > 0)
  {
    buf = new wchar_t[bufSize];
    GetSettings(type, pathW.c_str(), enumIndex, buf, NULL, NULL, &bufSize);
    std::wstring strW(buf);
    g_charsetConverter.wToUTF8(strW, str);
    sVector->push_back(str);
    buf = NULL;
    bufSize = 0;
    enumIndex++;
    GetSettings(type, pathW.c_str(), enumIndex, NULL, NULL, NULL, &bufSize);
  }
}

void CMadvrSettingsManager::EnumFoldersValues(MADVR_SETTINGS_TYPE type, std::string path, std::vector<std::string> *sVectorId, std::vector<std::string> *sVectorName, std::vector<std::string> *sVectorType)
{
  sVectorId->clear();
  sVectorName->clear();
  sVectorType->clear();
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  std::string strId;
  std::string strName;
  std::string strType;
  wchar_t* idBuf = NULL;
  wchar_t* nameBuf = NULL;
  wchar_t* typeBuf = NULL;
  int idBufSize = 0;
  int nameBufSize = 0;
  int typeBufSize = 0;
  int enumIndex = 0;

  GetSettings2(type, pathW.c_str(), enumIndex, NULL, NULL, NULL, &idBufSize, &nameBufSize, &typeBufSize);
  while (idBufSize > 0)
  {
    idBuf = new wchar_t[idBufSize];
    nameBuf = new wchar_t[nameBufSize];
    typeBuf = new wchar_t[typeBufSize];
    GetSettings2(type, pathW.c_str(), enumIndex, idBuf, nameBuf, typeBuf, &idBufSize, &nameBufSize, &typeBufSize);

    std::wstring strWid(idBuf);
    std::wstring strWname(nameBuf);
    std::wstring strWtype(typeBuf);
    g_charsetConverter.wToUTF8(strWid, strId);
    g_charsetConverter.wToUTF8(strWname, strName);
    g_charsetConverter.wToUTF8(strWtype, strType);
    sVectorId->push_back(strId);
    sVectorName->push_back(strName);
    sVectorType->push_back(strType);
    idBuf = NULL;
    nameBuf = NULL;
    typeBuf = NULL;
    idBufSize = 0;
    nameBufSize = 0;
    typeBufSize = 0;
    enumIndex++;
    GetSettings2(type, pathW.c_str(), enumIndex, NULL, NULL, NULL, &idBufSize, &nameBufSize, &typeBufSize);
  }
}

void CMadvrSettingsManager::EnumGroups(std::string path, std::vector<std::string> *sVector)
{
  EnumProfilesGroups(MADVR_SETTINGS_PROFILEGROUPS, path, sVector);
}

void CMadvrSettingsManager::EnumProfiles(std::string path, std::vector<std::string> *sVector)
{
  EnumProfilesGroups(MADVR_SETTINGS_PROFILES, path, sVector);
}

void CMadvrSettingsManager::EnumFolders(std::string path, std::vector<std::string> *sVectorId, std::vector<std::string> *sVectorName, std::vector<std::string> *sVectorType)
{
  EnumFoldersValues(MADVR_SETTINGS_FOLDERS, path, sVectorId, sVectorName, sVectorType);
}

void CMadvrSettingsManager::EnumValues(std::string path, std::vector<std::string> *sVectorId, std::vector<std::string> *sVectorName, std::vector<std::string> *sVectorType)
{
  EnumFoldersValues(MADVR_SETTINGS_VALUES, path, sVectorId, sVectorName, sVectorType);
}

void CMadvrSettingsManager::ListSettings(std::string path)
{
  std::vector<std::string> vecProfileGroups;
  std::vector<std::string> vecProfileName;
  std::vector<std::string> vecFoldersId;
  std::vector<std::string> vecFoldersName;
  std::vector<std::string> vecFoldersType;
  std::vector<std::string> vecValuesId;
  std::vector<std::string> vecValuesName;
  std::vector<std::string> vecValuesType;

  EnumGroups(path, &vecProfileGroups);
  for (unsigned int i = 0; i < vecProfileGroups.size(); i++)
  {
    CLog::Log(0, "madVR Profile Groups: %s", vecProfileGroups[i].c_str());

    EnumProfiles(path + "\\" + vecProfileGroups[i], &vecProfileName);
    for (unsigned int a = 0; a < vecProfileName.size(); a++)
    {
      CLog::Log(0, "madVR Profiles: %s", vecProfileName[a].c_str());

      EnumFolders(path + "\\" + vecProfileGroups[i] + "\\" + vecProfileName[a], &vecFoldersId, &vecFoldersName, &vecFoldersType);
      for (unsigned int b = 0; b < vecFoldersId.size(); b++)
      {
        CLog::Log(0, "madVR Folders %s %s %s", vecFoldersId[b].c_str(), vecFoldersName[b].c_str(), vecFoldersType[b].c_str());

        EnumValues(path + "\\" + vecProfileGroups[i] + "\\" + vecProfileName[a] + "\\" + vecFoldersId[b], &vecValuesId, &vecValuesName, &vecValuesType);
        for (unsigned int c = 0; c < vecValuesId.size(); c++)
        {
          CLog::Log(0, "madVR Values %s %s %s", vecValuesId[c].c_str(), vecValuesName[c].c_str(), vecValuesType[c].c_str());
        }
      }
    }
  }
}

void CMadvrSettingsManager::GetStr(std::string path, std::string *sValue)
{
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  wchar_t* buf = NULL;
  int bufSize = 0;
  GetSettings(MADVR_SETTINGS_STRING, pathW.c_str(), 0, NULL, NULL, NULL, &bufSize);
  buf = new wchar_t[bufSize];
  if (GetSettings(MADVR_SETTINGS_STRING, pathW.c_str(), 0, buf, NULL, NULL, &bufSize))
  {
    std::wstring strW(buf);
    g_charsetConverter.wToUTF8(strW, *sValue);
  }
}

void CMadvrSettingsManager::GetStr(std::string path, int *iValue, MADVR_SETTINGS_LIST type)
{
  std::wstring pathW;
  std::string sValue;
  GetStr(path, &sValue);
  *iValue = GetSettingsId(type, sValue);
}


void CMadvrSettingsManager::GetBool(std::string path, bool *bValue)
{
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  BOOL b;
  GetSettings(MADVR_SETTINGS_BOOL, pathW.c_str(), 0, NULL, &b, NULL, NULL);
  *bValue = b != 0;
}

void CMadvrSettingsManager::GetInt(std::string path, int* iValue)
{
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  GetSettings(MADVR_SETTINGS_INT, pathW.c_str(), 0, NULL, NULL, iValue, NULL);
}

void CMadvrSettingsManager::GetFloat(std::string path, float* fValue, int iConv)
{
  int iValue;
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  GetSettings(MADVR_SETTINGS_INT, pathW.c_str(), 0, NULL, NULL, &iValue, NULL);
  if (iValue > 0)
    *fValue = (float)iValue / (float)iConv;
  else
    *fValue = 0.0f;
};

void CMadvrSettingsManager::GetDoubling(std::string path, int* iValue)
{
  std::string strBool = "nnedi" + path + "Enable";
  std::string strInt = "nnedi" + path + "Quality";
  std::string strAlgo = path + "Algo";
  bool bValue;
  int aValue;
  int result = -1;
  std::string sValue;

  GetBool(strBool, &bValue);
  if (bValue)
  {
    GetStr(strAlgo, &sValue);
    if (sValue != "NNEDI3")
    {
      result = GetSettingsId(MADVR_LIST_DOUBLEQUALITY, sValue);
    }
    else
    {
      GetInt(strInt, &aValue);
      result = aValue;
    }
  };

  *iValue = result;
}

void CMadvrSettingsManager::GetDeintActive(std::string path, int* iValue)
{
  std::string strAuto = "autoActivateDeinterlacing";
  std::string strIfDoubt = "ifInDoubtDeinterlace";

  bool bValue1;
  bool bValue2;
  int result = -1;

  GetBool(strAuto, &bValue1);
  if (bValue1)
  {
    GetBool(strIfDoubt, &bValue2);
    result = !bValue2;
  };

  *iValue = result;
}

void CMadvrSettingsManager::GetBoolValue(std::string path, std::string sValue, int* iValue)
{
  bool bValue;
  int result = -1;

  GetBool(path, &bValue);
  if (bValue)
  {
    GetInt(sValue, iValue);
    result = *iValue;
  };

  *iValue = result;
}

void CMadvrSettingsManager::GetMultiBool(std::string path, std::string sValue, int* iValue)
{
  bool bValue;
  bool bValue2;
  int result = -1;

  GetBool(path, &bValue);
  if (bValue)
  {
    GetBool(sValue, &bValue2);
    bValue2 ? result = 1 : result = 0;
  };

  *iValue = result;
}

void CMadvrSettingsManager::GetSmoothmotion(std::string path, int* iValue)
{
  std::string stEnabled = "smoothMotionEnabled";
  std::string strMode = "smoothMotionMode";

  bool bValue;
  std::string sValue;
  int result = -1;

  GetBool(stEnabled, &bValue);
  if (bValue)
  {
    GetStr(strMode, &sValue);
    result = GetSettingsId(MADVR_LIST_SMOOTHMOTION, sValue);
  };

  *iValue = result;
}

void CMadvrSettingsManager::GetDithering(std::string path, int* iValue)
{
  std::string stDisable = "dontDither";
  std::string strMode = "ditheringAlgo";

  bool bValue;
  std::string sValue;
  int result = -1;

  GetBool(stDisable, &bValue);
  if (!bValue)
  {
    GetStr(strMode, &sValue);
    result = GetSettingsId(MADVR_LIST_DITHERING, sValue);
  };

  *iValue = result;
}

void CMadvrSettingsManager::SetStr(std::string path, std::string str)
{
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  std::wstring strW;
  g_charsetConverter.utf8ToW(str, strW, false);
  SetSettings(MADVR_SETTINGS_STRING, pathW.c_str(), strW.c_str(), NULL, NULL);
}

void CMadvrSettingsManager::SetBool(std::string path, bool bValue)
{
  BOOL b;
  std::wstring pathW;
  bValue ? b = 1 : b = 0;
  g_charsetConverter.utf8ToW(path, pathW, false);
  SetSettings(MADVR_SETTINGS_BOOL, pathW.c_str(), NULL, b, NULL);
}

void CMadvrSettingsManager::SetInt(std::string path, int iValue)
{
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  SetSettings(MADVR_SETTINGS_INT, pathW.c_str(), NULL, NULL, iValue);
}

void CMadvrSettingsManager::SetFloat(std::string path, float fValue, int iConv)
{
  int iValue = (int)round(fValue * iConv);
  std::wstring pathW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  SetSettings(MADVR_SETTINGS_INT, pathW.c_str(), NULL, NULL, iValue);
};

void CMadvrSettingsManager::SetDoubling(std::string path, int iValue)
{
  std::string strBool = "nnedi" + path + "Enable";
  std::string strInt = "nnedi" + path + "Quality";
  std::string strAlgo = path + "Algo";

  SetBool(strBool, (iValue>-1));
  if (iValue > -1)
  {
    SetInt(strInt, iValue);
    SetStr(strAlgo, GetSettingsName(MADVR_LIST_DOUBLEQUALITY,iValue));
  }
}

void CMadvrSettingsManager::SetDeintActive(std::string path, int iValue)
{
  std::string strAuto = "autoActivateDeinterlacing";
  std::string strIfDoubt = "ifInDoubtDeinterlace";

  SetBool(strAuto, (iValue > -1));
  SetBool(strIfDoubt, (iValue != MADVR_DEFAULT_DEINTACTIVE));
}

void CMadvrSettingsManager::SetBoolValue(std::string path, std::string sValue, int iValue)
{
  SetBool(path, (iValue > -1));
  if (iValue > -1)
    SetInt(sValue, iValue);
}

void CMadvrSettingsManager::SetMultiBool(std::string path, std::string sValue, int iValue)
{
  SetBool(path, (iValue > -1));
  if (iValue > -1)
    SetBool(sValue, (iValue == 1));
}

void CMadvrSettingsManager::SetSmoothmotion(std::string path, int iValue)
{
  std::string stEnabled = "smoothMotionEnabled";
  std::string strMode = "smoothMotionMode";

  SetBool(stEnabled, (iValue > -1));
  if (iValue > -1)
    SetStr(strMode, GetSettingsName(MADVR_LIST_SMOOTHMOTION, iValue));
}

void CMadvrSettingsManager::SetDithering(std::string path, int iValue)
{
  std::string stDisable = "dontDither";
  std::string strMode = "ditheringAlgo";

  SetBool(stDisable, (iValue == -1));
  if (iValue > -1)
    SetStr(strMode, GetSettingsName(MADVR_LIST_DITHERING, iValue));
}

bool CMadvrSettingsManager::IsProfileActive(std::string path, std::string profile)
{
  bool result = false;
  BOOL b;
  if (Com::SmartQIPtr<IMadVRSettings2> pMadvrSettings2 = m_pDXR)
  {
    std::wstring pathW;
    std::wstring profileW;
    g_charsetConverter.utf8ToW(path, pathW, false);
    g_charsetConverter.utf8ToW(profile, profileW, false);
    b = pMadvrSettings2->SettingsIsProfileActive(pathW.c_str(), profileW.c_str());
    result = b != 0;
  }
  return result;
}

void CMadvrSettingsManager::GetProfileActiveName(std::string *profile)
{
  std::vector<std::string> vecProfileGroups;
  std::vector<std::string> vecProfileName;
  std::string path = "scalingParent";
  std::string result = "";

  EnumGroups(path, &vecProfileGroups);
  for (unsigned int i = 0; i < vecProfileGroups.size(); i++)
  {
    CLog::Log(0, "madVR Profile Groups: %s", vecProfileGroups[i].c_str());

    EnumProfiles(path + "\\" + vecProfileGroups[i], &vecProfileName);
    for (unsigned int a = 0; a < vecProfileName.size(); a++)
    {
      if (IsProfileActive(path + "\\" + vecProfileGroups[i], vecProfileName[a]))
      {
        result = vecProfileName[a];
        break;
      }
    }
  }
  *profile = result;
}

void CMadvrSettingsManager::CreateProfile(std::string path, std::string pageList, std::string profileGroup, std::string profile)
{
  Com::SmartQIPtr<IMadVRSettings2> pMadvrSettings2 = m_pDXR;
  if (pMadvrSettings2 == NULL)
    return;

  bool existProfile;
  std::vector<std::string> vecProfileGroups;
  std::vector<std::string> vecProfilesName;
  std::wstring pathW;
  std::wstring newPathW;
  std::wstring profileW;
  std::wstring profileGroupW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  g_charsetConverter.utf8ToW(profile, profileW, false);

  // Enum madVR Groups
  EnumGroups(path, &vecProfileGroups);  
  for (unsigned int a = 0; a < vecProfileGroups.size(); a++)
  {
    existProfile = false;

    // Enum madVR profiles and add a DSPlayer profile if don't exist
    EnumProfiles(path + "\\" + vecProfileGroups[a], &vecProfilesName);
    for (unsigned int b = 0; b < vecProfilesName.size(); b++)
    {
      if (vecProfilesName[b] == profile)
      {
        existProfile = true;
        break;
      }
    }
    if (!existProfile)
    {
      g_charsetConverter.utf8ToW(vecProfileGroups[a], profileGroupW, false);
      newPathW = pathW + L"\\" + profileGroupW;
      pMadvrSettings2->SettingsAddProfile(newPathW.c_str(), profileW.c_str());
    }
  }

  // to be sure that all madVR folders are handled by the DSPlayer profiles add a complete DSPlayer Group
  std::wstring pageListW;
  g_charsetConverter.utf8ToW(pageList, pageListW, false);
  g_charsetConverter.utf8ToW(profileGroup, profileGroupW, false);
  pMadvrSettings2->SettingsCreateProfileGroup(pathW.c_str(), pageListW.c_str(), profileGroupW.c_str(), profileW.c_str());
}

void CMadvrSettingsManager::ActivateProfile(std::string path, std::string profile)
{
  Com::SmartQIPtr<IMadVRSettings2> pMadvrSettings2 = m_pDXR;
  if (pMadvrSettings2 == NULL)
    return;

  std::vector<std::string> vecProfileGroups;
  std::wstring pathW;
  std::wstring newPathW;
  std::wstring profileW;
  std::wstring profileGroupW;
  g_charsetConverter.utf8ToW(path, pathW, false);
  g_charsetConverter.utf8ToW(profile, profileW, false);

  // Enum madVR groups and activate all DSPlayer profiles
  EnumGroups(path, &vecProfileGroups);
  for (unsigned int i = 0; i < vecProfileGroups.size(); i++)
  {
    g_charsetConverter.utf8ToW(vecProfileGroups[i], profileGroupW, false);
    newPathW = pathW + L"\\" + profileGroupW;
    pMadvrSettings2->SettingsActivateProfile(newPathW.c_str(), profileW.c_str());
  }
}


void CMadvrSettingsManager::RestoreSettings()
{
  if (CSettings::GetInstance().GetInt(CSettings::SETTING_DSPLAYER_MANAGEMADVRWITHKODI) != KODIGUI_LOAD_DSPLAYER)
    return;

  CMadvrSettings &madvrSettings = CMediaSettings::GetInstance().GetCurrentMadvrSettings();

  // Create dummy DSPlayer Profile
  CreateProfile("processing", "deinterlacing|artifactRemoval|enhancements|zoomControl", DSGROUP, DSPROFILE);
  CreateProfile("scalingParent", "chromaUp|lumaUp|lumaDown|imageDoubling|upRefine", DSGROUP, DSPROFILE);
  CreateProfile("rendering", "smoothMotion|dithering", DSGROUP, DSPROFILE);

  // Activate dymmy DSPlayer Profile
  ActivateProfile("processing", DSPROFILE);
  ActivateProfile("scalingParent", DSPROFILE);
  ActivateProfile("rendering", DSPROFILE);

  SetStr("chromaUp", GetSettingsName(MADVR_LIST_CHROMAUP, madvrSettings.m_ChromaUpscaling));
  SetBool("chromaAntiRinging", madvrSettings.m_ChromaAntiRing);
  SetBool("superChromaRes", madvrSettings.m_ChromaSuperRes);
  SetInt("superChromaResPasses", madvrSettings.m_ChromaSuperResPasses);
  SetFloat("superChromaResStrength", madvrSettings.m_ChromaSuperResStrength);
  SetFloat("superChromaResSoftness", madvrSettings.m_ChromaSuperResSoftness);
  SetStr("lumaUp", GetSettingsName(MADVR_LIST_LUMAUP, madvrSettings.m_ImageUpscaling));
  SetBool("lumaUpAntiRinging", madvrSettings.m_ImageUpAntiRing);
  SetBool("lumaUpLinear", madvrSettings.m_ImageUpLinear);
  SetStr("lumaDown", GetSettingsName(MADVR_LIST_LUMADOWN, madvrSettings.m_ImageDownscaling));
  SetBool("lumaDownAntiRinging", madvrSettings.m_ImageDownAntiRing);
  SetBool("lumaDownLinear", madvrSettings.m_ImageDownLinear);
  SetDoubling("DL", madvrSettings.m_ImageDoubleLuma);
  SetStr("nnediDLScalingFactor", GetSettingsName(MADVR_LIST_DOUBLEFACTOR, madvrSettings.m_ImageDoubleLumaFactor));
  SetDoubling("DC", madvrSettings.m_ImageDoubleChroma);
  SetStr("nnediDCScalingFactor", GetSettingsName(MADVR_LIST_DOUBLEFACTOR, madvrSettings.m_ImageDoubleChromaFactor));
  SetDoubling("QL", madvrSettings.m_ImageQuadrupleLuma);
  SetStr("nnediQLScalingFactor", GetSettingsName(MADVR_LIST_QUADRUPLEFACTOR, madvrSettings.m_ImageQuadrupleLumaFactor));
  SetDoubling("QC", madvrSettings.m_ImageQuadrupleChroma);
  SetStr("nnediQCScalingFactor", GetSettingsName(MADVR_LIST_QUADRUPLEFACTOR, madvrSettings.m_ImageQuadrupleChromaFactor));
  SetDeintActive("", madvrSettings.m_deintactive);
  SetStr("contentType", GetSettingsName(MADVR_LIST_DEINTFORCE, madvrSettings.m_deintforce));
  SetBool("scanPartialFrame", madvrSettings.m_deintlookpixels);
  SetBool("debandActive", madvrSettings.m_deband);
  SetInt("debandLevel", madvrSettings.m_debandLevel);
  SetInt("debandFadeLevel", madvrSettings.m_debandFadeLevel);
  SetDithering("", madvrSettings.m_dithering);
  SetBool("coloredDither", madvrSettings.m_ditheringColoredNoise);
  SetBool("dynamicDither", madvrSettings.m_ditheringEveryFrame);
  SetSmoothmotion("", madvrSettings.m_smoothMotion);

  SetBool("fineSharp", madvrSettings.m_fineSharp);
  SetFloat("fineSharpStrength", madvrSettings.m_fineSharpStrength, 10);
  SetBool("lumaSharpen", madvrSettings.m_lumaSharpen);
  SetFloat("lumaSharpenStrength", madvrSettings.m_lumaSharpenStrength);
  SetFloat("lumaSharpenClamp", madvrSettings.m_lumaSharpenClamp,1000);
  SetFloat("lumaSharpenRadius", madvrSettings.m_lumaSharpenRadius,10);
  SetBool("adaptiveSharpen", madvrSettings.m_adaptiveSharpen);
  SetFloat("adaptiveSharpenStrength", madvrSettings.m_adaptiveSharpenStrength, 10);

  SetBoolValue("noSmallScaling","noSmallScalingValue", madvrSettings.m_noSmallScaling);
  SetMultiBool("moveSubs", "moveSubsUp", madvrSettings.m_moveSubs);

  SetBool("upRefFineSharp", madvrSettings.m_UpRefFineSharp);
  SetFloat("upRefFineSharpStrength", madvrSettings.m_UpRefFineSharpStrength, 10);
  SetBool("upRefLumaSharpen", madvrSettings.m_UpRefLumaSharpen);
  SetFloat("upRefLumaSharpenStrength", madvrSettings.m_UpRefLumaSharpenStrength);
  SetFloat("upRefLumaSharpenClamp", madvrSettings.m_UpRefLumaSharpenClamp,1000);
  SetFloat("upRefLumaSharpenRadius", madvrSettings.m_UpRefLumaSharpenRadius,10);
  SetBool("upRefAdaptiveSharpen", madvrSettings.m_UpRefAdaptiveSharpen);
  SetFloat("upRefAdaptiveSharpenStrength", madvrSettings.m_UpRefAdaptiveSharpenStrength, 10);

  SetBool("superRes", madvrSettings.m_superRes);
  SetFloat("superResStrength", madvrSettings.m_superResStrength, 1);
  SetFloat("superResSharpness", madvrSettings.m_superResSharpness, 1);
  SetFloat("superResRadius", madvrSettings.m_superResRadius);
  SetBool("superResLinear", madvrSettings.m_superResLinear);

  SetBool("refineOnce", !madvrSettings.m_refineOnce);
  SetBool("superResFirst", madvrSettings.m_superResFirst);
}

void CMadvrSettingsManager::LoadSettings(MADVR_LOAD_TYPE type)
{
  if (CSettings::GetInstance().GetInt(CSettings::SETTING_DSPLAYER_MANAGEMADVRWITHKODI) != KODIGUI_LOAD_MADVR)
    return;

  CMadvrSettings &madvrSettings = CMediaSettings::GetInstance().GetCurrentMadvrSettings();

  if (type == MADVR_LOAD_PROCESSING)
  {
    GetDeintActive("", &madvrSettings.m_deintactive);
    GetStr("contentType", &madvrSettings.m_deintforce, MADVR_LIST_DEINTFORCE);
    GetBool("scanPartialFrame", &madvrSettings.m_deintlookpixels);
    GetBool("debandActive", &madvrSettings.m_deband);
    GetInt("debandLevel", &madvrSettings.m_debandLevel);
    GetInt("debandFadeLevel", &madvrSettings.m_debandFadeLevel);

    GetBool("fineSharp", &madvrSettings.m_fineSharp);
    GetFloat("fineSharpStrength", &madvrSettings.m_fineSharpStrength, 10);
    GetBool("lumaSharpen", &madvrSettings.m_lumaSharpen);
    GetFloat("lumaSharpenStrength", &madvrSettings.m_lumaSharpenStrength);
    GetFloat("lumaSharpenClamp", &madvrSettings.m_lumaSharpenClamp, 1000);
    GetFloat("lumaSharpenRadius", &madvrSettings.m_lumaSharpenRadius, 10);
    GetBool("adaptiveSharpen", &madvrSettings.m_adaptiveSharpen);
    GetFloat("adaptiveSharpenStrength", &madvrSettings.m_adaptiveSharpenStrength, 10);

    GetBoolValue("noSmallScaling", "noSmallScalingValue", &madvrSettings.m_noSmallScaling);
    GetMultiBool("moveSubs", "moveSubsUp", &madvrSettings.m_moveSubs);

    GetDithering("", &madvrSettings.m_dithering);
    GetBool("coloredDither", &madvrSettings.m_ditheringColoredNoise);
    GetBool("dynamicDither", &madvrSettings.m_ditheringEveryFrame);
    GetSmoothmotion("", &madvrSettings.m_smoothMotion);
  }

  if (type == MADVR_LOAD_SCALING)
  {
    GetStr("chromaUp", &madvrSettings.m_ChromaUpscaling,MADVR_LIST_CHROMAUP);
    GetBool("chromaAntiRinging", & madvrSettings.m_ChromaAntiRing);
    GetBool("superChromaRes", &madvrSettings.m_ChromaSuperRes);
    GetInt("superChromaResPasses", &madvrSettings.m_ChromaSuperResPasses);
    GetFloat("superChromaResStrength", &madvrSettings.m_ChromaSuperResStrength);
    GetFloat("superChromaResSoftness", &madvrSettings.m_ChromaSuperResSoftness);
    GetStr("lumaUp", &madvrSettings.m_ImageUpscaling, MADVR_LIST_LUMAUP);

    GetBool("lumaUpAntiRinging", &madvrSettings.m_ImageUpAntiRing);
    GetBool("lumaUpLinear", &madvrSettings.m_ImageUpLinear);
    GetStr("lumaDown", &madvrSettings.m_ImageDownscaling,MADVR_LIST_LUMADOWN);
    GetBool("lumaDownAntiRinging", &madvrSettings.m_ImageDownAntiRing);
    GetBool("lumaDownLinear", &madvrSettings.m_ImageDownLinear);
    GetDoubling("DL", &madvrSettings.m_ImageDoubleLuma);
    GetStr("nnediDLScalingFactor", &madvrSettings.m_ImageDoubleLumaFactor,MADVR_LIST_DOUBLEFACTOR);
    GetDoubling("DC", &madvrSettings.m_ImageDoubleChroma);
    GetStr("nnediDCScalingFactor", &madvrSettings.m_ImageDoubleChromaFactor,MADVR_LIST_DOUBLEFACTOR);
    GetDoubling("QL", &madvrSettings.m_ImageQuadrupleLuma);
    GetStr("nnediQLScalingFactor", &madvrSettings.m_ImageQuadrupleLumaFactor,MADVR_LIST_QUADRUPLEFACTOR);
    GetDoubling("QC", &madvrSettings.m_ImageQuadrupleChroma);
    GetStr("nnediQCScalingFactor", &madvrSettings.m_ImageQuadrupleChromaFactor,MADVR_LIST_QUADRUPLEFACTOR);

    GetBool("upRefFineSharp", &madvrSettings.m_UpRefFineSharp);
    GetFloat("upRefFineSharpStrength", &madvrSettings.m_UpRefFineSharpStrength, 10);
    GetBool("upRefLumaSharpen", &madvrSettings.m_UpRefLumaSharpen);
    GetFloat("upRefLumaSharpenStrength", &madvrSettings.m_UpRefLumaSharpenStrength);
    GetFloat("upRefLumaSharpenClamp", &madvrSettings.m_UpRefLumaSharpenClamp,1000);
    GetFloat("upRefLumaSharpenRadius", &madvrSettings.m_UpRefLumaSharpenRadius,10);
    GetBool("upRefAdaptiveSharpen", &madvrSettings.m_UpRefAdaptiveSharpen);
    GetFloat("upRefAdaptiveSharpenStrength", &madvrSettings.m_UpRefAdaptiveSharpenStrength, 10);

    GetBool("superRes", &madvrSettings.m_superRes);
    GetFloat("superResStrength", &madvrSettings.m_superResStrength, 1);
    GetFloat("superResSharpness", &madvrSettings.m_superResSharpness, 1);
    GetFloat("superResRadius", &madvrSettings.m_superResRadius);
    GetBool("superResLinear", &madvrSettings.m_superResLinear);

    GetBool("refineOnce", &madvrSettings.m_refineOnce);
    madvrSettings.m_refineOnce = !madvrSettings.m_refineOnce;
    GetBool("superResFirst", &madvrSettings.m_superResFirst);
  }
}

std::vector<CMadvrSettingsList*>* CMadvrSettingsManager::GetSettingsVector(MADVR_SETTINGS_LIST type)
{
  std::vector< CMadvrSettingsList *> *vec;
  switch (type)
  {
  case MADVR_LIST_CHROMAUP:
    vec = &m_settingsChromaUp;
    break;
  case MADVR_LIST_LUMAUP:
    vec = &m_settingsLumaUp;
    break;
  case MADVR_LIST_LUMADOWN:
    vec = &m_settingsLumaDown;
    break;
  case MADVR_LIST_DOUBLEQUALITY:
    vec = &m_settingsDoubleQuality;
    break;
  case MADVR_LIST_DOUBLEFACTOR:
    vec = &m_settingsDoubleFactor;
    break;
  case MADVR_LIST_QUADRUPLEFACTOR:
    vec = &m_settingsQuadrupleFactor;
    break;
  case MADVR_LIST_DEINTFORCE:
    vec = &m_settingsDeintForce;
    break;
  case MADVR_LIST_DEINTACTIVE:
    vec = &m_settingsDeintActive;
    break;
  case MADVR_LIST_NOSMALLSCALING:
    vec = &m_settingsNoSmallScaling;
    break;
  case MADVR_LIST_MOVESUBS:
    vec = &m_settingsMoveSubs;
    break;
  case MADVR_LIST_SMOOTHMOTION:
    vec = &m_settingsSmoothMotion;
    break;
  case MADVR_LIST_DITHERING:
    vec = &m_settingsDithering;
    break;
  case MADVR_LIST_DEBAND:
    vec = &m_settingsDeband;
    break;
  }
  return vec;
}

void CMadvrSettingsManager::AddSettingsListScaler(std::string name, int label, int id, bool chromaUp, bool lumaUp, bool lumaDown)
{
  if (chromaUp)
    m_settingsChromaUp.push_back(DNew CMadvrSettingsList(name, label, id));
  if (lumaUp)
    m_settingsLumaUp.push_back(DNew CMadvrSettingsList(name, label, id));
  if (lumaDown)
    m_settingsLumaDown.push_back(DNew CMadvrSettingsList(name, label, id));
}

void CMadvrSettingsManager::AddSettingsList(MADVR_SETTINGS_LIST type, std::string name, int label, int id)
{
  GetSettingsVector(type)->push_back(DNew CMadvrSettingsList(name, label, id));
}

int CMadvrSettingsManager::GetSettingsId(MADVR_SETTINGS_LIST type, std::string sValue)
{
  std::vector<CMadvrSettingsList *> *vec = GetSettingsVector(type);
  int result = -1;

  for (unsigned int i = 0; i < vec->size(); i++)
  {
    if ((*vec)[i]->m_name == sValue)
    {
      result = (*vec)[i]->m_id;
      break;
    }
  }
  return result;
}

std::string CMadvrSettingsManager::GetSettingsName(MADVR_SETTINGS_LIST type, int iValue)
{
  std::vector<CMadvrSettingsList *> *vec = GetSettingsVector(type);
  std::string result = "";

  for (unsigned int i = 0; i < vec->size(); i++)
  {
    if ((*vec)[i]->m_id == iValue)
    {
      result = (*vec)[i]->m_name;
      break;
    }
  }

  return result;
}

void CMadvrSettingsManager::AddEntry(MADVR_SETTINGS_LIST type, StaticIntegerSettingOptions *entry)
{
  std::vector<CMadvrSettingsList *> *vec = GetSettingsVector(type);
  for (unsigned int i = 0; i < vec->size(); i++)
  {
    entry->push_back(std::make_pair((*vec)[i]->m_label, (*vec)[i]->m_id));
  }
}

void CMadvrSettingsManager::UpdateImageDouble()
{
  CMadvrSettings &madvrSettings = CMediaSettings::GetInstance().GetCurrentMadvrSettings();

  int &iDoubleLuma = madvrSettings.m_ImageDoubleLuma;
  int &iDoubleChroma = madvrSettings.m_ImageDoubleChroma;
  int &iQuadrupleLuma = madvrSettings.m_ImageQuadrupleLuma;
  int &iQuadrupleChroma = madvrSettings.m_ImageQuadrupleChroma;
  int &iDoubleLumaFactor = madvrSettings.m_ImageDoubleLumaFactor;
  int &iDoubleChromaFactor = madvrSettings.m_ImageDoubleChromaFactor;
  int &iQuadrupleLumaFactor = madvrSettings.m_ImageQuadrupleLumaFactor;
  int &iQuadrupleChromaFactor = madvrSettings.m_ImageQuadrupleChromaFactor;

  // Update double factor
  if (!IsNNEDI3(iDoubleLuma)
    || (IsNNEDI3(iDoubleLuma) && iDoubleChromaFactor > iDoubleLumaFactor))
    iDoubleChromaFactor = iDoubleLumaFactor;

  // Update quadruple factor
  if (!IsNNEDI3(iQuadrupleLuma)
    || (IsNNEDI3(iQuadrupleLuma) && iQuadrupleChromaFactor > iQuadrupleLumaFactor))
    iQuadrupleChromaFactor = iQuadrupleLumaFactor;

  // Update double chroma
  if (!IsNNEDI3(iDoubleLuma)
    || (IsNNEDI3(iDoubleLuma) && iDoubleChroma > iDoubleLuma))
    iDoubleChroma = iDoubleLuma;

  // Update quadruple luma
  if (((!IsNNEDI3(iDoubleLuma) && IsNNEDI3(iQuadrupleLuma))
    || (IsNNEDI3(iDoubleLuma) && IsNNEDI3(iQuadrupleLuma) && iQuadrupleLuma > iDoubleLuma)
    || (!IsEnabled(iDoubleLuma)))
    && IsEnabled(iQuadrupleLuma))
    iQuadrupleLuma = iDoubleLuma;

  // Update quadruple chroma
  if (!IsNNEDI3(iQuadrupleLuma)
    || (IsNNEDI3(iQuadrupleLuma) && iQuadrupleChroma > iQuadrupleLuma))
    iQuadrupleChroma = iQuadrupleLuma;

  if (IsNNEDI3(iQuadrupleLuma) && iQuadrupleChroma > iDoubleChroma)
    iQuadrupleChroma = iDoubleChroma;

  // Set New settings in madVR
  SetStr("nnediDLScalingFactor", GetSettingsName(MADVR_LIST_DOUBLEFACTOR, iDoubleLumaFactor));
  SetStr("nnediDCScalingFactor", GetSettingsName(MADVR_LIST_DOUBLEFACTOR, iDoubleChromaFactor));
  SetStr("nnediQLScalingFactor", GetSettingsName(MADVR_LIST_QUADRUPLEFACTOR, iQuadrupleLumaFactor));
  SetStr("nnediQCScalingFactor", GetSettingsName(MADVR_LIST_QUADRUPLEFACTOR, iQuadrupleChromaFactor));
  SetDoubling("DL", iDoubleLuma);
  SetDoubling("DC", iDoubleChroma);
  SetDoubling("QL", iQuadrupleLuma);
  SetDoubling("QC", iQuadrupleChroma);
}

void CMadvrSettingsManager::InitSettings()
{
  //Scalers
  AddSettingsListScaler("Nearest Neighbor", 70001, 0, true, true, true);
  AddSettingsListScaler("Bilinear", 70002, 1, true, true, true);
  AddSettingsListScaler("Dxva", 70003, 2, false, true, true);
  AddSettingsListScaler("Mitchell-Netravali", 70004, 3, true, true, true);
  AddSettingsListScaler("Catmull-Rom", 70005, 4, true, true, true);
  AddSettingsListScaler("Bicubic50", 70006, 5, true, true, true);
  AddSettingsListScaler("Bicubic60", 70007, 6, true, true, true);
  AddSettingsListScaler("Bicubic75", 70008, 7, true, true, true);
  AddSettingsListScaler("Bicubic100", 70009, 8, true, true, true);
  AddSettingsListScaler("SoftCubic50", 70010, 9, true, true, true);
  AddSettingsListScaler("SoftCubic60", 70011, 10, true, true, true);
  AddSettingsListScaler("SoftCubic70", 70012, 11, true, true, true);
  AddSettingsListScaler("SoftCubic80", 70013, 12, true, true, true);
  AddSettingsListScaler("SoftCubic100", 70014, 13, true, true, true);
  AddSettingsListScaler("Lanczos3", 70015, 14, true, true, true);
  AddSettingsListScaler("Lanczos4", 70016, 15, true, true, true);
  AddSettingsListScaler("Lanczos8", 70017, 16, false, false, false);
  AddSettingsListScaler("Spline36", 70018, 17, true, true, true);
  AddSettingsListScaler("Spline64", 70019, 18, true, true, true);
  AddSettingsListScaler("Jinc3", 70020, 19, true, true, false);
  AddSettingsListScaler("Jinc4", 70021, 20, false, false, false);
  AddSettingsListScaler("Jinc8", 70022, 21, false, false, false);
  AddSettingsListScaler("Bilateral", 70033, 22,  true, false, false);
  AddSettingsListScaler("SuperXbr25", 70034, 23, true, false, false);
  AddSettingsListScaler("SuperXbr50", 70035, 24, true, false, false);
  AddSettingsListScaler("SuperXbr75", 70036, 25, true, false, false);
  AddSettingsListScaler("SuperXbr100", 70037, 26, true, false, false);
  AddSettingsListScaler("SuperXbr125", 70038, 27, true, false, false);
  AddSettingsListScaler("SuperXbr150", 70039, 28,  true, false, false);
  AddSettingsListScaler("Nedi", 70040, 29, true, false, false);
  AddSettingsListScaler("Nnedi16", 70023, 30, true, false, false);
  AddSettingsListScaler("Nnedi32", 70024, 31, true, false, false);
  AddSettingsListScaler("Nnedi64", 70025, 32, true, false, false);
  AddSettingsListScaler("Nnedi128", 70026, 33, true, false, false);
  AddSettingsListScaler("Nnedi256", 70027, 34,  true, false, false);

  //Image Double Quality
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "SuperXbr25", 70034, 5);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "SuperXbr50", 70035, 6);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "SuperXbr75", 70036, 7);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "SuperXbr100", 70037, 8);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "SuperXbr125", 70038, 9);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "SuperXbr150", 70039, 10);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "NEDI", 70040, 11);
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "NNEDI3", 70023, 0); //16NEURONS
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "NNEDI3", 70024, 1); //32NEURONS
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "NNEDI3", 70025, 2); //64NEURONS
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "NNEDI3", 70026, 3); //128NEURONS
  AddSettingsList(MADVR_LIST_DOUBLEQUALITY, "NNEDI3", 70027, 4); //256NEURONS

  //Image Double Factor
  AddSettingsList(MADVR_LIST_DOUBLEFACTOR, "2.0x", 70109, 0);
  AddSettingsList(MADVR_LIST_DOUBLEFACTOR, "1.5x", 70110, 1);
  AddSettingsList(MADVR_LIST_DOUBLEFACTOR, "1.2x", 70111, 2);
  AddSettingsList(MADVR_LIST_DOUBLEFACTOR, "always", 70112, 3);

  //Image Quadruple Factor
  AddSettingsList(MADVR_LIST_QUADRUPLEFACTOR, "4.0x", 70113, 0);
  AddSettingsList(MADVR_LIST_QUADRUPLEFACTOR, "3.0x", 70114, 1);
  AddSettingsList(MADVR_LIST_QUADRUPLEFACTOR, "2.4x", 70115, 2);
  AddSettingsList(MADVR_LIST_QUADRUPLEFACTOR, "always", 70112, 3);

  // Deint Force
  AddSettingsList(MADVR_LIST_DEINTFORCE, "auto", 70202, 0 );
  AddSettingsList(MADVR_LIST_DEINTFORCE, "film", 70203, 1 );
  AddSettingsList(MADVR_LIST_DEINTFORCE, "video", 70204, 2 );

  // Deint Active
  AddSettingsList(MADVR_LIST_DEINTACTIVE, "ifdoubt_active", 70205, 0 );
  AddSettingsList(MADVR_LIST_DEINTACTIVE, "ifdoubt_deactive", 70206, 1);

  // No Small Scaling
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "1lines", 70209, 1);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "2lines", 70210, 2);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "3lines", 70211, 3);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "5lines", 70212, 5);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "7lines", 70213, 7);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "10lines", 70214, 10);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "15lines", 70215, 15);
  AddSettingsList(MADVR_LIST_NOSMALLSCALING, "25lines", 70216, 25);

  // MoveSubs
  AddSettingsList(MADVR_LIST_MOVESUBS, "bottom", 70218, 0);
  AddSettingsList(MADVR_LIST_MOVESUBS, "up", 70219, 1);

  // Smoothmotion
  AddSettingsList(MADVR_LIST_SMOOTHMOTION, "avoidJudder", 70301, 0 );
  AddSettingsList(MADVR_LIST_SMOOTHMOTION, "almostAlways", 70302, 1);
  AddSettingsList(MADVR_LIST_SMOOTHMOTION, "always", 70303, 2);

  // Dithering
  AddSettingsList(MADVR_LIST_DITHERING, "random", 70401, 0 );
  AddSettingsList(MADVR_LIST_DITHERING, "ordered", 70402, 1);
  AddSettingsList(MADVR_LIST_DITHERING, "errorDifMedNoise", 70403, 2);
  AddSettingsList(MADVR_LIST_DITHERING, "errorDifLowNoise", 70404, 3);

  // Deband
  AddSettingsList(MADVR_LIST_DEBAND, "debandlow", 70503, 0 );
  AddSettingsList(MADVR_LIST_DEBAND, "debandmedium", 70504, 1);
  AddSettingsList(MADVR_LIST_DEBAND, "debandhigh", 70505, 2);
}