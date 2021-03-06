/*
	创建日期：	2012/11/05 15:09:48
	作者：			daviyang35@gmail.com
	描述：	FlashUI
*/
#ifndef __UIFLASH_H__
#define __UIFLASH_H__
#pragma once
#include "Utils/FlashEventHandler.h"
#include "Utils/flash11.tlh"
//#import "Utils/Flash11.tlh" \
//raw_interfaces_only, \
//named_guids, \
//rename("IDispatchEx","IMyDispatchEx") \
//rename("ICanHandleException", "IMyICanHandleException")


//#if _MSC_VER<=1500     //vs2008
//#import "Utils/Flash11.tlb" raw_interfaces_only, named_guids
//#elif _MSC_VER<=1600   //vs2010
//#import "PROGID:ShockwaveFlash.ShockwaveFlash"  \
//        raw_interfaces_only,       /* Don't add raw_ to method names */ \
//        named_guids,           /* Named guids and declspecs */    \
//        rename("IDispatchEx","IMyDispatchEx")/* fix conflicting with IDispatchEx ant dispex.h */  
//#else                  //vs2013
//#import "PROGID:ShockwaveFlash.ShockwaveFlash"  \
//        raw_interfaces_only,       /* Don't add raw_ to method names */ \
//        named_guids,           /* Named guids and declspecs */    \
//        rename("IDispatchEx", "IMyDispatchEx")/* fix conflicting with IDispatchEx ant dispex.h */\
//        rename("ICanHandleException", "IMyICanHandleException")
//
//#endif

namespace DuiLib
{
	class UILIB_API CFlashUI
		: public CActiveXUI
		, public ShockwaveFlashObjects::_IShockwaveFlashEvents
		, public ITranslateAccelerator
	{
		DECLARE_CLASS(CFlashUI)
	public:
		CFlashUI(void);
		~CFlashUI(void);

		void SetFlashEventHandler(CFlashEventHandler* pHandler);
		virtual bool DoCreateControl();
		ShockwaveFlashObjects::IShockwaveFlash* m_pFlash;

		void SetFlashUrl(LPCTSTR szPath);
		LPCTSTR GetFlashUrl() const;
	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface( LPCTSTR pstrName );
	protected:
		virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
		virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

		virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );
		virtual ULONG STDMETHODCALLTYPE AddRef( void );
		virtual ULONG STDMETHODCALLTYPE Release( void );

		HRESULT OnReadyStateChange (long newState);
		HRESULT OnProgress(long percentDone );
		HRESULT FSCommand (_bstr_t command, _bstr_t args);
		HRESULT FlashCall (_bstr_t request );

		virtual void ReleaseControl();
		HRESULT RegisterEventHandler(BOOL inAdvise);

		// ITranslateAccelerator
		// Duilib消息分发给WebBrowser
		virtual LRESULT TranslateAccelerator( MSG *pMsg );
	public:
		void DoInit();
	private:
		LONG m_dwRef;
		DWORD m_dwCookie;
		CFlashEventHandler* m_pFlashEventHandler;
		CDuiString m_szFlashUrl;
	};
}

#endif // __UIFLASH_H__
