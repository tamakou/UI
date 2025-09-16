#include "stdafx.h"
#include "ThProcLibDllLoader.h"
 
 
CThProcLibDllLoader::CThProcLibDllLoader():
	m_dll_hander(nullptr)
{

}
CThProcLibDllLoader::~CThProcLibDllLoader()
{
	if (m_dll_hander != nullptr) {
		FreeLibrary(m_dll_hander);
	}
}
bool CThProcLibDllLoader::init(void)
{
	m_dll_hander = LoadLibrary(L"ThProcLib.dll");
	if (m_dll_hander == nullptr) return false;

	_dll_th_lib_init = (th_lib_init)GetProcAddress(m_dll_hander, "th_lib_init");
	if (_dll_th_lib_init == nullptr) return false;

	_dll_th_lib_close = (th_lib_close)GetProcAddress(m_dll_hander, "th_lib_close");
	if (_dll_th_lib_close == nullptr) return false;

	_dll_th_lib_setup_env = (th_lib_setup_env)GetProcAddress(m_dll_hander, "th_lib_setup_env");
	if (_dll_th_lib_setup_env == nullptr) return false;

	_dll_th_lib_get_env = (th_lib_get_env)GetProcAddress(m_dll_hander, "th_lib_get_env");
	if (_dll_th_lib_get_env == nullptr) return false;

	_dll_th_lib_get_data_size = (th_lib_get_data_size)GetProcAddress(m_dll_hander, "th_lib_get_datalist_size"); 
	if (_dll_th_lib_get_data_size == nullptr) return false;

	_dll_th_lib_get_datalist = (th_lib_get_datalist)GetProcAddress(m_dll_hander, "th_lib_get_datalist");
	if (_dll_th_lib_get_datalist == nullptr) return false;

	_dll_th_lib_exe_cmd = (th_lib_exe_cmd)GetProcAddress(m_dll_hander, "th_lib_exe_cmd");
	if (_dll_th_lib_exe_cmd == nullptr) return false;

	_dll_th_lib_get_image_info = (th_lib_get_image_info)GetProcAddress(m_dll_hander, "th_lib_get_image_info");
	if (_dll_th_lib_get_image_info == nullptr) return false;

	_dll_th_lib_get_data = (th_lib_get_data)GetProcAddress(m_dll_hander, "th_lib_get_data");
	if (_dll_th_lib_get_data == nullptr) return false;
	
	_dll_th_lib_get_status = (th_lib_get_status)GetProcAddress(m_dll_hander, "th_lib_get_status");
	if (_dll_th_lib_get_status == nullptr) return false;
 
	_dll_th_lib_close_cmd = (th_lib_close_cmd)GetProcAddress(m_dll_hander, "th_lib_close_cmd");
	if (_dll_th_lib_close_cmd == nullptr) return false;
	 
	_dll_th_lib_get_vol_info = (th_lib_get_vol_info)GetProcAddress(m_dll_hander, "th_lib_get_vol_info");
	if (_dll_th_lib_get_vol_info == nullptr) return false;

	_dll_th_lib_render_GC = (th_lib_render_GC)GetProcAddress(m_dll_hander, "th_lib_render_GC");
	if (_dll_th_lib_render_GC == nullptr) return false;

	_dll_th_lib_exe_render = (th_lib_exe_render)GetProcAddress(m_dll_hander, "th_lib_exe_render");
	if (_dll_th_lib_exe_render == nullptr) return false;


	return true;
}
 