#ifndef TH_PROC_LIB_MAIN_DUMMY_H
#define TH_PROC_LIB_MAIN_DUMMY_H

#include "ThProcLibMain.h"
#include "ThProcLibRenderDummy.h"
class CThProcMainDummy : public CThProcMain
{
public:
	CThProcMainDummy();
	virtual ~CThProcMainDummy();
 
	/// <summary>
	/// common inteface
	/// </summary>
	
	int th_lib_init(int mode) override;
	int th_lib_setup_env(const ThProcLib_Env* env)  override;
	int th_lib_get_env(ThProcLib_Env* env)  override;
	int th_lib_get_datalist_size(int type)  override;
	int th_lib_get_datalist(ThProcLib_PatientInfo* out_info, int type, int no)  override;
	int th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)  override;
	int th_lib_get_image_info(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo* image_info) override;
	//#97_get_mesh_folder
	int exe_get_path(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;

	//int th_lib_get_data(const ThProcLib_Cmd* cmd, char* DataBuffer, int BufferSize)  override;
	int th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status)  override;
	int th_lib_close_cmd(int cmdUID)  override;
	//
	int th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol) override;;

	int th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)  override;
	int check_cmd_async(int cmdID, int cmdUID,int chkNo=0) override;
	int proc_load_vol_in(void) override;

protected:
	int get_new_data_ID(void) override;
	int exe_cmd_import(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)  override;;
	int exe_cmd_async_cancel(const ThProcLib_Cmd* cmd)  override;;
	int exe_cmd_delete_import_data(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)  override;
	int exe_cmd_AiSegmentator(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;
	int exe_cmd_load_vol(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)  override;;
	//#103_load3DModel_command
	int exe_cmd_load_3DModle(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)  override;;
	//#137_check_image_server
	int exe_cmd_check_image_server(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;
	//#32_ThLibCmd_Output_as_async
	int exe_cmd_add_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;;
	int exe_cmd_delete_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;;
	int exe_get_image_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize) override;

	int finish_cmd_async(int cmdID, int cmdUID) override;
	//#29_command_added
	//int proc_CT_Marker(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;
	int proc_CT_Marker(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds) override;
	int proc_ROI(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;
	int proc_Output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) override;
	//
	int th_lib_render_vol_dummy(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage);
	RenderImgDummy* th_lib_get_image_dummy(int imageID);
	void  th_lib_delete_image_dummy(int imageID);
	std::map<int, RenderImgDummy*> m_render_img_dummy;
};
#endif //TH_PROC_LIB_MAIN_DUMMY_H