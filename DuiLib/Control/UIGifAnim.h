#ifndef UIGIFANIM_H__
#define UIGIFANIM_H__
#pragma once

#define _USE_GDIPLUS 1

#ifdef _USE_GDIPLUS
#include <GdiPlus.h>
#include <gdiplusimaging.h>
#pragma comment( lib, "GdiPlus.lib" )
// UMU: DO NOT use "using namespace" in .h file
//using namespace Gdiplus;
class UILIB_API Gdiplus::Image;
class UILIB_API Gdiplus::PropertyItem;
using Gdiplus::Image;
using Gdiplus::PropertyItem;
#endif

namespace DuiLib
{
	#define EVENT_TIEM_ID	100

	class UILIB_API CGifAnimUI : public CControlUI
	{
		DECLARE_CLASS(CGifAnimUI)
	public:
		CGifAnimUI(void);
		~CGifAnimUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoInit() override;
		bool	DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void	DoEvent(TEventUI& event);
		void	SetVisible(bool bVisible = true );
	protected:
		virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		void    SetBkImage(LPCTSTR pStrImage);
		void	SetAutoPlay(bool bIsAuto = true );
		bool	IsAutoPlay() const;
		void	SetAutoSize(bool bIsAuto = true );
		bool	IsAutoSize() const;
		void	PlayGif();
		void	PauseGif();
		void	StopGif();
	public://For Designer
		virtual void GetPropertyList(std::vector<UIPropertyGrid>& property_list);
	private:
		void	InitGifImage();
		void	DeleteGif();
		void    OnTimer( UINT_PTR idEvent );
		void	DrawFrame( HDC hDC );		// 绘制GIF每帧
		Image*	LoadGifFromFile(LPCTSTR pstrGifPath);
		Image*  LoadGifFromMemory( LPVOID pBuf,size_t dwSize );
	private:
		Image		*   m_pGifImage;
		UINT			m_nFrameCount;				// gif图片总帧数
		UINT			m_nFramePosition;			// 当前放到第几帧
		PropertyItem*	m_pPropertyItem;	// 帧与帧之间间隔时间

		bool			m_bIsAutoPlay;				// 是否自动播放gif
		bool			m_bIsAutoSize;				// 是否自动根据图片设置大小
		bool			m_bIsPlaying;

	};
}

#endif // UIGIFANIM_H__
