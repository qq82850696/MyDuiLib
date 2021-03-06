#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "BasePlayer.h"
#include "utility/log.h"


CBasePlayer::CBasePlayer(void)
	:_pIPlayerCallBack(NULL),
	_hPlayerQueue(NULL),
	_state(XPS_INIT),
	_fSeekPercent(-1.0f),
	_fNewSeekPercent(0.f),
	_fCurTime(0.f),
	_fTotalTime(0.f),
	_fVolume(1.0f),
	_spOpQueue(new COperationQueue()),
	_pUserData(NULL),
	_isNewSeek(false),
	_bOnLine(false),
	_fBufPer(1.0f),
	_nReadPos(0)
{
	memset(&_audioInfo,0,sizeof(_audioInfo));
	_decoderPluginMgr.load_plugins();
	assert(_spOpQueue);
}
CBasePlayer::~CBasePlayer(void)
{
	Stop();
	if(_spOpQueue)
		_spOpQueue->stop();
	if(_hPlayerQueue)
	{
		xiamiPlayerQueue_destroy(_hPlayerQueue);
		_hPlayerQueue = NULL;
	}	
}

void CBasePlayer::SetCallBack(IPlayerCallBack* pCallbackObj)
{
	ScopedLockT lockObj(_lock);
	_pIPlayerCallBack = pCallbackObj;
}
IPlayerCallBack* CBasePlayer::GetCallBack() const
{
	CBasePlayer* pThis = const_cast<CBasePlayer*>(this);
	ScopedLockT lockObj(pThis->_lock);
	return pThis->_pIPlayerCallBack;
}

enumPlayerState CBasePlayer::GetState() const
{
	return _state;
}
void CBasePlayer::Continue()
{
	assert(XPS_PAUSE == GetState());
	if(XPS_PAUSE == GetState())
		_state = XPS_PLAYING;
}
void CBasePlayer::Pause()
{
	assert(XPS_PLAYING == GetState());
	if(XPS_PLAYING == GetState())
		_state = XPS_PAUSE;
}
void CBasePlayer::Stop()
{
	assert(_spOpQueue);
	_spOpQueue->cancle();
	_state = XPS_STOP;
}
float CBasePlayer::GetVolume() const
{
	return _fVolume;
}
void CBasePlayer::SetVolume(float fVolume)
{
	_fVolume = std::max<float>(0.0f,std::min<float>(fVolume,1.0f));
}

unsigned long CBasePlayer::GetFileBytes() const
{
	return _audioInfo.n_file_size;
}

unsigned long CBasePlayer::GetDownloadBytes() const
{
	return 0;
}

int CBasePlayer::GetReadPosition() const
{
	return _nReadPos;
}

float CBasePlayer::GetPlayMSec() const
{
	return _audioInfo.n_total_play_time_in_ms > 0 ? (float)_audioInfo.n_total_play_time_in_ms : _fTotalTime;
}
float CBasePlayer::GetCurrentPlayMSec() const
{
	return _fCurTime;
}
float CBasePlayer::GetBufPercent() const
{
	return _fBufPer;
}
void CBasePlayer::SeekPlayPosition(float fPercent)
{
	ScopedLockT lockObj(_lock);
	_fNewSeekPercent = fPercent;
	_isNewSeek       = true;
}


int CBasePlayer::GetBitRate()  const
{
	return _audioInfo.n_sample_rate*_audioInfo.n_sample_size_in_bit*_audioInfo.n_channal/8000;
}
int CBasePlayer::GetSampleRate() const
{
	return _audioInfo.n_sample_rate;
}
int CBasePlayer::GetChannelNum() const
{
	return _audioInfo.n_channal;
}
int CBasePlayer::GetChannelType() const
{
	return 0;
}
bool CBasePlayer::StartToPlay(
	const char* pwsFile,
	const char* pwsUrl,
	int nFileType,
	int nBegin,
	int nEnd,
	int nMSec,
	void* pUserData)
{
	sLog("CBasePlayer::StartToPlay<%s>,hash=%s",pwsFile,pwsUrl);
	assert(pwsFile);
	if(!pwsFile)
		return false;

	if(!pwsUrl && !CheckFileExist(pwsFile))
		return false;

	std::string wsFile = pwsFile;
	std::string wsUrl;
	if(pwsUrl)
		wsUrl += pwsUrl;

	Stop();
	std::shared_ptr<CBasePlayer> pThis(this->shared_from_this());
	assert(_spOpQueue);
	assert(pThis);
	if(!_spOpQueue || !pThis)
		return false;

	_spOpQueue->addOperation(
		std::bind(
		&CBasePlayer::Play,
		pThis,
		wsFile,
		wsUrl,
		nFileType,
		nBegin,
		nEnd,
		nMSec,
		pUserData)
		);

	return true;
}

void CBasePlayer::PreCacheNextSong(const char* pwsUrl, void* pUserData)
{
}

void CBasePlayer::OnIPlayerCallBack(enumPlayerEventType type,void* lp,void* wp) const
{
	//assert(_pIPlayerCallBack);
	if(_pIPlayerCallBack)
	{
		_pIPlayerCallBack->OnPlayerCallBack
			(const_cast<CBasePlayer*>(this),
			type,
		    lp,
		    wp,
		    _pUserData
		    );
	}
}

void CBasePlayer::FadInOutVolume(HPlayerQueue hPlayerQueue,float fIn,float fOut) const
{
	static const float fStep  = 0.016666666f;
	static const float fMin   = 0.01f;
	static const float fMax   = 1.00f;
	static const size_t nMSec = 10;
	if(fIn >= fOut)
	{
		while(fIn > fOut && fIn > fMin)
		{
			fIn -= fStep;
			if(fIn < fMin)
				fIn = fMin;
			xiamiPlayerQueue_setVolume(hPlayerQueue,fIn);
			Sleep(nMSec);
		}
	}
	else 
	{
		while(fIn < fOut && fIn < fMax)
		{
			fIn += fStep;
			if(fIn > fMax)
				fIn = fMax;
			xiamiPlayerQueue_setVolume(hPlayerQueue,fIn);
			Sleep(nMSec);
		}
	}
}
void CBasePlayer::SyncPlayerState(HPlayerQueue hPlayerQueue)
{
	PlayerQueueState queueSt = xiamiPlayerQueue_state(hPlayerQueue);

	bool isNotify = false;
	const enumPlayerState st = GetState();
	switch(st)
	{
	case XPS_INIT:
		break;
	case XPS_PLAYING:
		if(PQS_PLAYING != queueSt)
		{
			const float fOldVolume = this->GetVolume();
			xiamiPlayerQueue_setVolume(_hPlayerQueue,0.0f);
			xiamiPlayerQueue_play(_hPlayerQueue);
			this->FadInOutVolume(_hPlayerQueue,0.0f,fOldVolume);
			isNotify = true;
		}
		break;
	case XPS_PAUSE:
		if(PQS_PLAYING == queueSt)
		{
			const float fOldVolume = this->GetVolume();
			this->FadInOutVolume(_hPlayerQueue,fOldVolume,0.0f);
			xiamiPlayerQueue_pause(_hPlayerQueue);
			//this->SetVolume(fOldVolume);
			xiamiPlayerQueue_setVolume(_hPlayerQueue,fOldVolume);
			isNotify = true;
		}
		break;
	case XPS_STOP:
		if(PQS_STOPPED != queueSt)
		{
			xiamiPlayerQueue_stop(_hPlayerQueue);
			isNotify = true;
		}
		break;
	}
	if(isNotify)
	{
		this->OnIPlayerCallBack(XPET_STATE_CHANGED,(void*)&st,NULL);
	}
}

bool CBasePlayer::IsNeedExitPlayPro() const
{
	return (XPS_STOP == GetState()) || (_spOpQueue && _spOpQueue->size() > 0 );
}
void CBasePlayer::Play(
	const std::string& wsFile,
	const std::string& wsUrl,
	int nFileType,
	int nBegin,
	int nEnd,
	int nMSec,
	void* pUserData)
{
#if FLIB_COMPILER_WINDOWS
	::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#endif
	
	sLog("CBasePlayer::Play<%s>,hash=%s",wsFile.c_str(),wsUrl.c_str());	
	assert(!wsFile.empty());

	if(_spOpQueue->size() > 0)
		return;

	if(!_hPlayerQueue)
		_hPlayerQueue = xiamiPlayerQueue_create(3);

	assert(_hPlayerQueue);
	if(!_hPlayerQueue)
		return this->OnIPlayerCallBack(XPET_ERROR,(void*)wsFile.c_str(),NULL);

	try
	{
		_state               = XPS_PLAYING;
		_fSeekPercent        = -1.0f;
		_fNewSeekPercent     = 0.0f;
		_fCurTime            = 0.0f;
		_fTotalTime          = nMSec;
		_pUserData           = pUserData;
		_bOnLine             = !wsUrl.empty();
		_fBufPer             = 1.0f;
		memset(&_audioInfo,0,sizeof(_audioInfo));

		this->OnPlayBegin();
		this->OnPlay(
			wsFile,
			wsUrl,
			nFileType,
			nBegin,
			nEnd);
	}
	catch (...)
	{
		assert(false);
	}

	this->OnPlayEnd();


	this->OnIPlayerCallBack(XPET_PLAY_FINISHED,(void*)wsFile.c_str(),NULL);
	
	const bool isNextSong = (XPS_PLAYING == GetState() && !IsNeedExitPlayPro());
	_state = XPS_STOP;
	if(isNextSong)
	{
		sLog("CBasePlayer::开始切换下一曲,hash = %s",wsUrl.c_str());
		this->OnIPlayerCallBack(XPET_PLAY_NEXT,(void*)wsFile.c_str(),NULL);
	}
	sLog("CBasePlayer::Play End<%s>,hash=%s",wsFile.c_str(),wsUrl.c_str());	
}

void CBasePlayer::DefaultPlayPro(
	const std::string& wsFile,
	decoder_plugin* pPlugin,
	spDecoderHandleT spDecoder,
	int nBegin,
	int nEnd,
	std::function<bool (void)> isExitLoopPred/* = NULL*/)
{
	sLog("CBasePlayer::DefaultPlayPro<%s>",wsFile.c_str());
	assert(pPlugin);
	assert(spDecoder);

	if(!pPlugin || !spDecoder)
	{
		assert(false);
		this->OnIPlayerCallBack(XPET_ERROR,(void*)wsFile.c_str(),NULL);
		return;
	}

	decoder_handle  hDecoder = spDecoder.get();
	assert(hDecoder);

	std::shared_ptr<void> spScopedPlayerQueue(_hPlayerQueue,xiamiPlayerQueue_stop);
	float fVolume = this->GetVolume();
	xiamiPlayerQueue_setVolume(_hPlayerQueue,fVolume);

	unsigned char*  pcmBuf = new unsigned char[PCM_BUFSIZE];
	assert(pcmBuf);
	if(!pcmBuf) return ;
#if FLIB_COMPILER_WINDOWS
	ZeroMemory(pcmBuf,PCM_BUFSIZE);
#else
	memset(pcmBuf, 0, PCM_BUFSIZE);
#endif

	size_t          pcmCnt              = 0;
	bool            isDecoderErr        = false;
	float           fLastSeekTime       = 0.0f;


	auto isRunning = [&]() -> bool const
	{
		return
		!IsNeedExitPlayPro() &&
		!isDecoderErr &&
		!(isExitLoopPred ? isExitLoopPred() : false);
	};

	auto flushPcmBuffer = [&]() -> void const
	{
		if(pcmCnt > 0)
		{
			AUDIO_INFO AudioInfo;
			AudioInfo.nBitPerSample = _audioInfo.n_sample_size_in_bit;
			AudioInfo.nChannel      = _audioInfo.n_channal;
			AudioInfo.nSampleRate   = _audioInfo.n_sample_rate;
			AudioInfo.nSampleCnt    = pcmCnt;

			this->OnIPlayerCallBack(XPET_PLAY_DATA,pcmBuf,&AudioInfo);

			xiamiPlayerQueue_pushBuffer(
				_hPlayerQueue,
				pcmBuf,
				pcmCnt,
				_audioInfo.n_sample_rate,
				_audioInfo.n_channal,
				_audioInfo.n_sample_size_in_bit);

			pcmCnt = 0;
		}
	};

	
	assert(MIN_PCM_BUFSIZE <= PCM_BUFSIZE);
	while (isRunning())
	{
		if(this->GetVolume() != fVolume)
		{
			fVolume = this->GetVolume();
			xiamiPlayerQueue_setVolume(_hPlayerQueue,fVolume);
		}


		//sync seek pos
		if(_isNewSeek)
		{
			float fSeekPer = -1.0f;
			{
				ScopedLockT lockObj(_lock);
				fSeekPer = _fNewSeekPercent;
				_isNewSeek = false;
			}

			if(_fSeekPercent != fSeekPer)
			{
				int nResult = pPlugin->get_audio_info(hDecoder,&_audioInfo);
				assert(DECODER_ERROR != nResult);
				if(DECODER_ERROR != pPlugin->seek(hDecoder,fSeekPer))
				{
					_fSeekPercent = fSeekPer;
					fLastSeekTime = this->GetPlayMSec()*_fSeekPercent;
					_fCurTime     = fLastSeekTime;
					pcmCnt        = 0;
					_fSeekPercent = -1.0f;

					const float fOldVolume = xiamiPlayerQueue_volume(_hPlayerQueue);
					this->FadInOutVolume(_hPlayerQueue,fOldVolume,0.0f);
					xiamiPlayerQueue_stop(_hPlayerQueue);
					xiamiPlayerQueue_setVolume(_hPlayerQueue,fOldVolume);

				}
			}
		}

		unsigned long nIdleBufferCnt = xiamiPlayerQueue_idleBufferCnt(_hPlayerQueue);
		while(nIdleBufferCnt-- && isRunning())
		{
			if(pPlugin->is_finish(hDecoder))
			{
				flushPcmBuffer();
				break;
			}

			if(DECODER_ERROR == pPlugin->decode_once(hDecoder))
			{
				isDecoderErr = true;
				assert(false);
				break;
			} 

			int nResult = pPlugin->get_audio_info(hDecoder,&_audioInfo);
			if(DECODER_ERROR == nResult)
			{
				isDecoderErr = true;
				assert(false);
				break;
			}

			unsigned long nBytes = pPlugin->get_data(hDecoder,pcmBuf+pcmCnt,PCM_BUFSIZE - pcmCnt);
			if (0 >= nBytes)
				continue;

			pcmCnt += nBytes;

			if(pcmCnt < MIN_PCM_BUFSIZE && !pPlugin->is_finish(hDecoder))
			{
				nIdleBufferCnt++;
				continue;
			}

			_fBufPer = 1.0;
			flushPcmBuffer();
		}

		SyncPlayerState(_hPlayerQueue);	

		if(XPS_PLAYING == GetState())  
		{
			PlayerQueueState st = xiamiPlayerQueue_state(_hPlayerQueue);
			if(PQS_PLAYING != st)
			{
				unsigned long nInBufferCnt = xiamiPlayerQueue_inBufferCnt(_hPlayerQueue);
				if(nInBufferCnt > 0)
				{
					xiamiPlayerQueue_play(_hPlayerQueue);
				}
				else
				{
					if(pPlugin->is_finish(hDecoder))
					{
						sLog("CBasePlayer::播放完成,%s",wsFile.c_str());
						break;
					}
					else
					{
						if (_bOnLine)
						{
							_nReadPos = pPlugin->get_current_position(hDecoder);
							_fBufPer  = (float)pcmCnt / MIN_PCM_BUFSIZE;
						}
					}
				}
			}
		}

		_fCurTime = fLastSeekTime + xiamiPlayerQueue_duration(_hPlayerQueue)*1000.0f;
	
		Sleep(10);

	} 

	delete []pcmBuf;
	if(!isRunning())
		sLog("CBasePlayer::isRunning() = false");
 	if(isDecoderErr)
		sLog("CBasePlayer::isDecoderErr=error");
	if(IsNeedExitPlayPro())
		sLog("CBasePlayer::IsNeedExitPlayPro=true");
	if(XPS_STOP == GetState())
		sLog("CBasePlayer::GetState() = XPS_STOP,当前是STOP状态");
	if((_spOpQueue && _spOpQueue->size() > 0 ))
		sLog("CBasePlayer::_spOpQueue->size() > 0,播放队列有歌曲等待");

	sLog("CBasePlayer::DefaultPlayProEnd<%s>",wsFile.c_str());
}


bool CBasePlayer::CheckFileExist(const char*pszFileName) const
{
	return _FStd(FFileExists)(pszFileName);
}

decoder_handle CBasePlayer::OpenDecoder(
	const std::string& wsFile,
	bool isOnline,
	decoder_plugin **pPlugin,
	int nFileType /*= 0*/,
	int nBegin/* = 0*/,
	int nEnd/* = 0*/)
{
	return  _decoderPluginMgr.open(wsFile.c_str(),isOnline,pPlugin,nFileType,nBegin,nEnd);
}
