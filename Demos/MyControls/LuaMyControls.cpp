#define LUA_LIB
#include "MyControls.h"
#include "base/lua_wrapper.hpp"
#include "LuaDuiLib/lua_duilib_wrapper.hpp"
#include <algorithm>

#define LAMBDA_METHOD(x)   .def(#x, x)
#define LAMBDA_METHOD2(n, x) .def(n, x)
#define TO_LUA_META_NAME(name) name##"_meta"
namespace DuiLib
{
	template<typename T>
	inline T* Lua_NewControl()
	{
		return new T();
	}

	template<typename T>
	inline void Lua_DeleteControl(void* p)
	{
		if(!p) return;

		__try
		{
			T* pControl = static_cast<T*>(p);
			if (pControl) pControl->Delete();
		}
		__except(EXCEPTION_ACCESS_VIOLATION)
		{ 
		}
	}

	CDuiString Lua_CControlUI_ToString(CControlUI* p)
	{
		return p->ToString();
	}

	int Lua_FadeCheckBoxSetCheck(lua_State* l)
	{
		CFadeCheckBoxUI* pCheckBox = nullptr;
		lua::get(l, 1, &pCheckBox);
		bool check;
		lua::get(l, 2, &check);
		bool bSendMsg = true;
		if(lua_isboolean(l, 3))
			lua::get(l, 3, &bSendMsg);
		pCheckBox->SetCheck(check, bSendMsg);
		return 0;
	}

	int Lua_CreateMyControl(lua_State* l)
	{
		CDuiString szClass;
		lua::get(l, 1, &szClass);

		CControlUI* pControl = MyCreateControl(szClass);

		if (pControl) {
			lua::push(l, pControl);
		}
		else {
			lua_pushnil(l);
		}

		return 1;
	}

	void Lua_RegisterDuiLibControlToLua(lua_State* l)
	{
		//CFadeButtonUI
#define WRAP_METHOD(m) .def(#m, &CFadeButtonUI::m)
		lua::lua_register_t<CFadeButtonUI>(l, "DuiLib.CFadeButtonUI", Lua_DeleteControl<CFadeButtonUI>)
			.extend(TO_LUA_META_NAME("DuiLib.CButtonUI"))
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CFadeButtonUI>)
			.def("__tostring", Lua_CControlUI_ToString);
#undef WRAP_METHOD

		//CAlbumButtonUI
#define WRAP_METHOD(m) .def(#m, &CAlbumButtonUI::m)
		lua::lua_register_t<CAlbumButtonUI>(l, "DuiLib.CAlbumButtonUI", Lua_DeleteControl<CAlbumButtonUI>)
			.extend<CFadeButtonUI>()
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CAlbumButtonUI>)
			.def("__tostring", Lua_CControlUI_ToString)
			WRAP_METHOD(SetAlbumImage)
			WRAP_METHOD(SetHotBkImage)
			WRAP_METHOD(GetHotBkImage);
#undef WRAP_METHOD

		//CFadeOptionUI
#define WRAP_METHOD(m) .def(#m, &CFadeOptionUI::m)
		lua::lua_register_t<CFadeOptionUI>(l, "DuiLib.CFadeOptionUI", Lua_DeleteControl<CFadeOptionUI>)
			.extend(TO_LUA_META_NAME("DuiLib.COptionUI"))
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CFadeOptionUI>)
			.def("__tostring", Lua_CControlUI_ToString);
#undef WRAP_METHOD

		//CFadeCheckBoxUI
#define WRAP_METHOD(m) .def(#m, &CFadeCheckBoxUI::m)
		lua::lua_register_t<CFadeCheckBoxUI>(l, "DuiLib.CFadeCheckBoxUI", Lua_DeleteControl<CFadeCheckBoxUI>)
			.extend<CFadeOptionUI>()
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CFadeCheckBoxUI>)
			.def("__tostring", Lua_CControlUI_ToString)
			WRAP_METHOD(GetCheck)
			LAMBDA_METHOD2("SetCheck", Lua_FadeCheckBoxSetCheck);
#undef WRAP_METHOD

		//CLrcViewUI
#define WRAP_METHOD(m) .def(#m, &CLrcViewUI::m)
		lua::lua_register_t<CLrcViewUI>(l, "DuiLib.CLrcViewUI", Lua_DeleteControl<CFadeCheckBoxUI>)
			.extend(TO_LUA_META_NAME("DuiLib.CContainerUI"))
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CLrcViewUI>)
			.def("__tostring", Lua_CControlUI_ToString);
#undef WRAP_METHOD

		//CTemplListItemUI
#define WRAP_METHOD(m) .def(#m, &CTemplListItemUI::m)
		lua::lua_register_t<CTemplListItemUI>(l, "DuiLib.CTemplListItemUI", Lua_DeleteControl<CTemplListItemUI>)
			.extend(TO_LUA_META_NAME("DuiLib.CListContainerElementUI"))
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CTemplListItemUI>)
			.def("__tostring", Lua_CControlUI_ToString);
#undef WRAP_METHOD

		//CTemplListUI
#define WRAP_METHOD(m) .def(#m, &CTemplListUI::m)
		lua::lua_register_t<CTemplListUI>(l, "DuiLib.CTemplListUI", Lua_DeleteControl<CTemplListUI>)
			.extend(TO_LUA_META_NAME("DuiLib.CListUI"))
			.def(lua::constructor<>())
			.def("New", Lua_NewControl<CTemplListUI>)
			.def("__tostring", Lua_CControlUI_ToString)
			WRAP_METHOD(SetTemplateXML)
			WRAP_METHOD(SetCount)
			WRAP_METHOD(AddOne);
#undef WRAP_METHOD

		//MyControl
		lua::lua_register_t<void>(l, "MyControl")
			.def("CreateControl", Lua_CreateMyControl);
	}
}
#undef LAMBDA_METHOD
#undef LAMBDA_METHOD2

extern "C"
{
	LUALIB_API int luaopen_LuaMyControls(lua_State *L)
	{
		DuiLib::Lua_RegisterDuiLibControlToLua(L);
		return 1;
	}
}


LUA_DUI_CLASS_OP_T(DuiLib::CFadeButtonUI)
LUA_DUI_CLASS_OP_T(DuiLib::CFadeOptionUI)
LUA_DUI_CLASS_OP_T(DuiLib::CFadeCheckBoxUI)
LUA_DUI_CLASS_OP_T(DuiLib::CAlbumButtonUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTemplListUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTemplListItemUI)
LUA_DUI_CLASS_OP_T(DuiLib::CWkeWebkitUI)
LUA_DUI_CLASS_OP_T(DuiLib::CLrcViewUI)
