// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "InternalEntities.h"
#include "FileCacheManager.h"
#include "AudioListenerManager.h"
#include "AudioEventListenerManager.h"
#include "AudioStandaloneFileManager.h"
#include "AudioEventManager.h"
#include "AudioObjectManager.h"
#include "AudioXMLProcessor.h"
#include <CryInput/IInput.h>

namespace CryAudio
{
class CSystem;

class CAudioTranslationLayer final : public IInputEventListener
{
public:

	CAudioTranslationLayer();
	virtual ~CAudioTranslationLayer();

	CAudioTranslationLayer(CAudioTranslationLayer const&) = delete;
	CAudioTranslationLayer(CAudioTranslationLayer&&) = delete;
	CAudioTranslationLayer& operator=(CAudioTranslationLayer const&) = delete;
	CAudioTranslationLayer& operator=(CAudioTranslationLayer&&) = delete;

	// IInputEventListener
	virtual bool OnInputEvent(SInputEvent const& event);
	// ~IInputEventListener

	bool           Initialize(CSystem* const pAudioSystem);
	bool           ShutDown();
	void           ProcessRequest(CAudioRequest& request);
	void           Update(float const deltaTime);
	bool           GetAudioTriggerId(char const* const szAudioTriggerName, ControlId& audioTriggerId) const;
	bool           GetAudioParameterId(char const* const szAudioParameterName, ControlId& audioParameterId) const;
	bool           GetAudioSwitchId(char const* const szAudioSwitchName, ControlId& audioSwitchId) const;
	bool           GetAudioSwitchStateId(ControlId const switchId, char const* const szAudioSwitchStateName, SwitchStateId& audioSwitchStateId) const;
	bool           GetAudioPreloadRequestId(char const* const szAudioPreloadRequestName, PreloadRequestId& audioPreloadRequestId) const;
	bool           GetAudioEnvironmentId(char const* const szAudioEnvironmentName, EnvironmentId& audioEnvironmentId) const;

	bool           CanProcessRequests() const { return (m_flags & eAudioInternalStates_AudioMiddlewareShuttingDown) == 0; }

	ERequestStatus ParseControlsData(char const* const szFolderPath, EDataScope const dataScope);
	ERequestStatus ClearControlsData(EDataScope const dataScope);
	ERequestStatus ParsePreloadsData(char const* const szFolderPath, EDataScope const dataScope);
	ERequestStatus ClearPreloadsData(EDataScope const dataScope);

	void           NotifyListener(CAudioRequest const& request);

private:

	ERequestStatus ProcessAudioManagerRequest(CAudioRequest const& request);
	ERequestStatus ProcessAudioCallbackManagerRequest(CAudioRequest& request);
	ERequestStatus ProcessAudioObjectRequest(CAudioRequest const& request);
	ERequestStatus ProcessAudioListenerRequest(SAudioRequestData const* const pPassedRequestData);
	ERequestStatus SetImpl(Impl::IAudioImpl* const pImpl);
	void           ReleaseImpl();

	ERequestStatus RefreshAudioSystem(char const* const szLevelName);
	void           SetImplLanguage();
	void           InitInternalControls();
	void           SetCurrentEnvironmentsOnObject(CATLAudioObject* const pObject, EntityId const entityToIgnore, Vec3 const& position);

	enum EAudioInternalStates : EnumFlagsType
	{
		eAudioInternalStates_None                        = 0,
		eAudioInternalStates_IsMuted                     = BIT(0),
		eAudioInternalStates_AudioMiddlewareShuttingDown = BIT(1),
	};

	// ATLObject containers
	AudioTriggerLookup        m_triggers;
	AudioParameterLookup      m_parameters;
	AudioSwitchLookup         m_switches;
	AudioPreloadRequestLookup m_preloadRequests;
	AudioEnvironmentLookup    m_environments;

	CATLAudioObject*          m_pGlobalAudioObject = nullptr;

	// Components
	CAudioStandaloneFileManager m_audioStandaloneFileMgr;
	CAudioEventManager          m_audioEventMgr;
	CAudioObjectManager         m_audioObjectMgr;
	CAudioListenerManager       m_audioListenerMgr;
	CFileCacheManager           m_fileCacheMgr;
	CAudioEventListenerManager  m_audioEventListenerMgr;
	CAudioXMLProcessor          m_xmlProcessor;

	SInternalControls           m_internalControls;

	// Utility members
	uint32                 m_lastMainThreadFrameId = 0;
	volatile EnumFlagsType m_flags = eAudioInternalStates_None;
	Impl::IAudioImpl*      m_pImpl = nullptr;

#if defined(INCLUDE_AUDIO_PRODUCTION_CODE)
public:
	void DrawAudioSystemDebugInfo();
	void GetAudioTriggerData(ControlId const audioTriggerId, STriggerData& audioTriggerData) const;

private:

	void DrawAudioObjectDebugInfo(IRenderAuxGeom& auxGeom);
	void DrawATLComponentDebugInfo(IRenderAuxGeom& auxGeom, float posX, float const posY);
	void RetriggerAudioControls();

	CryFixedStringT<MaxMiscStringLength> m_implNameString;
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
};
} // namespace CryAudio
