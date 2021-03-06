#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib
{
	class UILIB_API COptionUI : public CButtonUI
	{
		DECLARE_CLASS(COptionUI)
	public:
		COptionUI();
		~COptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

		bool Activate();
		void SetEnabled(bool bEnable = true);

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedHotImage();
		void SetSelectedHotImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedPushedImage();
		void SetSelectedPushedImage(LPCTSTR pStrImage);

		void SetSelectedTextColor(DWORD dwTextColor);
		DWORD GetSelectedTextColor();

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectedBkColor();
		DUI_DEPRECATED DWORD GetSelectBkColor(); // deprecated, use GetSelectedBkColor instead

		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		bool IsSelected() const;
		virtual void Selected(bool bSelected, bool bTriggerEvent=true);

		SIZE EstimateSize(SIZE szAvailable);
	protected:
		virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		//begin add 2018/6/10
		void SetSelectedToolTip(LPCTSTR szTooltip);
		LPCTSTR GetSelectedToolTip() const;

		void SetNormalTooltip(LPCTSTR szTooltip);
		LPCTSTR GetNormalTooltip() const;
		//end

	public://For Designer
		virtual void GetPropertyList(std::vector<UIPropertyGrid>& property_list);
	protected:
		bool			m_bSelected;
		CDuiString		m_sGroupName;

		DWORD			m_dwSelectedBkColor;
		DWORD			m_dwSelectedTextColor;

		TDrawInfo		m_diSelected;
		TDrawInfo		m_diSelectedHot;
		TDrawInfo		m_diSelectedPushed;
		TDrawInfo		m_diFore;
		//begin add 2018/6/10
		CDuiString m_sSelectedTooltip;
		CDuiString m_sNormalTooltip;
		//end
	};

} // namespace DuiLib

#endif // __UIOPTION_H__