#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once
#include <map>
namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

typedef void* (*ControlCreator)();
class UILIB_API CControlFactory
{
private:
	CControlFactory() {};
public:
	static void RegisterDuiClass(LPCTSTR pstrClassName, ControlCreator _createDuiInstanceFun, LPCTSTR category = _T("控件"));
	static void* createInstance(LPCTSTR pstrClassName);
	static bool isRegister(LPCTSTR pstrClassName);
    static const std::map<CDuiString, std::vector<CDuiString> >& GetAllControls();
};
template<typename T>
class UILIB_API ControlCreatorDelegate
{
public:
	ControlCreatorDelegate(LPCTSTR pstrClassName, LPCTSTR category = _T("控件"))
	{
		CControlFactory::RegisterDuiClass(pstrClassName, (ControlCreator)&(ControlCreatorDelegate::CreateControl), category);
	}
	static T* CreateControl()
	{
		return static_cast<T*>(new T);
	}
};
template<class TObj>
inline TObj CreateControl(LPCTSTR pstrClassName) {
	return static_cast<TObj>(CControlFactory::createInstance(pstrClassName));
}

#ifdef REGIST_DUICONTROL
#undef REGIST_DUICONTROL
#endif
#ifdef REGIST_DUICONTROLEX
#undef REGIST_DUICONTROLEX
#endif
#define REGIST_DUICONTROL(ControlClass) ControlCreatorDelegate<ControlClass> __class_##ControlClass( _T(#ControlClass) );
#define REGIST_DUICONTROLEX(ControlClass, category) ControlCreatorDelegate<ControlClass> __class_##ControlClass( _T(#ControlClass), category );


typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class UILIB_API CControlUI
{
    DECLARE_CLASS(CControlUI)
public:
    CControlUI();
    virtual void Delete();

protected:
    virtual ~CControlUI();

public:
	virtual CDuiString ToString() const;
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
	virtual bool IsName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
	virtual bool IsClass(LPCTSTR pstrName);
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;
	virtual HWND GetNativeWindow() const;

    virtual bool Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
    virtual CControlUI* GetParent() const;
    virtual CControlUI* GetCover() const;
    virtual void SetCover(CControlUI *pControl);

    // 文本相关
    virtual CDuiString GetText() const;
    virtual void SetText(LPCTSTR pstrText);

    // 图形相关
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    LPCTSTR GetBkImage();
    virtual void SetBkImage(LPCTSTR pStrImage);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    bool DrawImage(HDC hDC, TDrawInfo& drawInfo);

	//边框相关
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);
	RECT GetBorderSize() const;
	void SetBorderSize(RECT rc);
	void SetBorderSize(int iSize);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

    // 位置相关
    virtual const RECT& GetPos() const;
	virtual RECT GetRelativePos() const; // 相对(父控件)位置
	virtual RECT GetClientPos() const; // 客户区域（除去scrollbar和inset）
	// 只有控件为float的时候，外部调用SetPos和Move才是有效的，位置参数是相对父控件的位置
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
	virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
	virtual TPercentInfo GetFloatPercent() const;
	virtual void SetFloatPercent(TPercentInfo piFloatPercent);
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
	// 声音 -- Modify by dfn.li
	virtual CDuiString GetSoundNameOver() const;
	virtual void SetSoundNameOver(LPCTSTR pstrText) ;
	virtual CDuiString GetSoundNameDown() const ;
	virtual void SetSoundNameDown(LPCTSTR pstrText);

    // 鼠标提示
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户属性
    virtual const CDuiString& GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用
	virtual CControlUI* GetTagCtrl() const;
	virtual void SetTagCtrl(CControlUI* pCtrl);

    // 一些重要的属性
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);

	// 自定义(未处理的)属性
	void AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
	LPCTSTR GetCustomAttribute(LPCTSTR pstrName) const;
	bool RemoveCustomAttribute(LPCTSTR pstrName);
	void RemoveAllCustomAttribute();

    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

	virtual void Notify(TNotifyUI& msg);
	virtual void DoNotify(TNotifyUI& msg);

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);

    virtual void DoSize();

protected:
    virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
public:
    virtual CDuiString GetXMLAttribute(LPCTSTR pstrName);
    bool SetXMLAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    const CDuiStringPtrMap& GetXMLAttributeMap() { return m_XMLAttrHash; }
	virtual CDuiString GetAttributeList(bool bIgnoreDefault = true);
    virtual void SetAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

	virtual bool Paint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl=NULL); // 返回要不要继续绘制
    virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	//虚拟窗口参数
	void SetVirtualWnd(LPCTSTR pstrValue);
	CDuiString GetVirtualWnd() const;
	//begin 2018/6/6
	virtual CDuiString GetStyleName() const;
	virtual void SetStyleName(LPCTSTR pstrStyleName);
	void SetBindEventCtrl(CControlUI* pControl);
	void SetBindNotifyCtrl(CControlUI* pControl);
	//end
public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;
	CEventSource OnPaint;
	CEventSource OnPostPaint;

protected:
    CPaintManagerUI* m_pManager;
    CControlUI* m_pParent;
    CControlUI* m_pCover;
	CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
	bool m_bAsyncNotify;
    RECT m_rcItem;
    RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    bool m_bFocused;
    bool m_bFloat;
	TPercentInfo m_piFloatPercent;
    bool m_bSetPos; // 防止SetPos循环调用

    CDuiString m_sText;
    CDuiString m_sToolTip;
    TCHAR m_chShortcut;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;
	CControlUI* m_pTagCtrl;

    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    TDrawInfo m_diBk;
	TDrawInfo m_diFore;
    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    bool m_bColorHSL;

	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
	RECT m_rcBorderSize;
	CDuiStringPtrMap m_mCustomAttrHash;
	//begin 2018/6/6
	CDuiString m_sStyleName;
	
	// 声音
	CDuiString m_sSoundNameOver;
	CDuiString m_sSoundNameDown;
	CControlUI* m_pBindEventCtrl;
	CControlUI* m_pBindNotifyCtrl;
	//end

public://For Designer
    virtual void GetPropertyList(std::vector<UIPropertyGrid>& property_list);
 protected:
      CDuiStringPtrMap m_XMLAttrHash;
};

} // namespace DuiLib

#endif // __UICONTROL_H__
