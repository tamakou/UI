#ifndef TH_PROC_OUT_LIB__H
#define TH_PROC_OUT_LIB__H

#pragma warning (disable: 4305)
#include "IThProcLibData.h"
#ifdef MakeThProcLib 
#define IThProcLibDefDllAPI __declspec(dllexport)
#else 
#define IThProcLibDefDllAPI __declspec(dllimport)
#endif
extern "C" {
	/// <summary>
	/// DLL�C���^�t�F�[�X�̒�`
	/// </summary>


	/// <summary>
	/// DLL�̏�����
	/// </summary>
	/// <param name="mode">: mode�w��</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F</returns>
	/// 
	IThProcLibDefDllAPI int th_lib_init(int mode); 
	
	/// <summary>
	/// DLL�̏I�� //#183_th_lib_close
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	IThProcLibDefDllAPI void th_lib_close(void);


	/// <summary>
	/// DLL�̎��s����ݒ肷��
	/// </summary>
	/// <param name="env">: ���s��</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F</returns>

	IThProcLibDefDllAPI int th_lib_setup_env(const ThProcLib_Env* env);

	/// <summary>
	/// DLL�̎��s�����擾����B
	/// </summary>
	/// <param name="env">: ���s����ۑ�����o�b�t�@�[</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F </returns>
	IThProcLibDefDllAPI int th_lib_get_env(ThProcLib_Env* env);

	/// <summary>
	///  �f�[�^�ꗗ���擾����O�ɁA��Ƀf�[�^�ꗗ�̃T�C�Y���擾����B
	/// </summary>
	/// <param name="type">: �擾����f�[�^��ނ̎w��Bref: eThLibQueryList</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F </returns>
	IThProcLibDefDllAPI int th_lib_get_datalist_size(int type);

	/// <summary>
	/// CT�f�[�^��(�RD�f�[�^�j�ꗗ���擾����B
	/// </summary>
	/// <param name="out_info">: �f�[�^���ڂ�ۑ�����o�b�t�@�[</param>
	/// <param name="type">: �擾����f�[�^��ނ̎w��Bref: eThLibQueryList</param>
	/// <param name="no">: �擾����f�[�^�ԍ��̎w��</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F </returns>
	IThProcLibDefDllAPI int th_lib_get_datalist(ThProcLib_PatientInfo* out_info,int type, int no);

	/// <summary>
	/// �R�}���h�̎��s
	///  cmd_ret.cmdType==1 �̏ꍇ�͔񓯊����s�ƂȂ�Bth_lib_get_status�Ői���m�F�B
	/// </summary>
	/// <param name="cmd">: ���s�R�}���h</param>
	/// <param name="cmd_ret">: ���s�̃��^�[���l</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F </returns>
	IThProcLibDefDllAPI int th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret);

	/// <summary>
	/// DICOM�摜�f�[�^���̎擾�B
	/// </summary>
	/// <param name="cmd">: ThProcLib_Cmd:cmdID�Ɂ@ThLibCmd_Get_DicomImageInfo���w��AThProcLib_Cmd:strParam1 ��StudyUID</param>
	/// <param name="image_info">�f�[�^���ڂ�ۑ�����o�b�t�@�[</param>
	/// <returns></returns>
	IThProcLibDefDllAPI int th_lib_get_image_info(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo *image_info);

	/// <summary>
	/// �ėp�I�ȃo�C�i���f�[�^�̏o�́B
	/// </summary>
	/// <param name="cmd">cmdID�Ɏ擾�f�[�^�̎�ށBintParam��ImageID�̎w��</param>
	/// <param name="DataBuffer">: �f�[�^�����̃o�b�t�@�[�ɃR�s�[����</param>
	/// <param name="BufferSize">: �o�b�t�@�[�T�C�Y�̎w��</param>
	/// <returns>�O�F����,����ȊO�͎��s�B���s�̏ꍇ��th_lib_get_status�Ŋm�F </returns>
	IThProcLibDefDllAPI int th_lib_get_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize);
	                                                     //#43_changed----^^^
	/// <summary>
	/// �R�}���h���s�̃X�e�[�^�X���擾����
	/// </summary>
	/// <param name="cmdUID">: th_lib_exe_cmd�̕ԒlThProcLib_Cmd_Output�ɂ���cmdUID, ref: eThLibCmd�AThLibCmd_Unknown: �Ō�Ɏ��s���ꂽ�R�}���h</param>
	/// <param name="status">: �X�e�[�^�X��Ԃ�</param>
	/// <returns>�O�F����,����ȊO�͎��s�B</returns>
	IThProcLibDefDllAPI int th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status);

	/// <summary>
	/// �񓯊����s����Ă���R�}���h�̃N���[�Y�B 
	/// th_lib_exe_cmd�̕ԒlThProcLib_Cmd_Output�ɂ���cmdType��ThLibCmdType_Async�̎��A
	/// th_lib_get_status�̃R�}���h���s�̊���Status���m�F���ꂽ��ɃN���[�Y����;
	/// </summary>
	/// <param name="cmdUID">: th_lib_exe_cmd�̕ԒlThProcLib_Cmd_Output�ɂ���cmdUID</param>
	/// <returns>�O�F����,����ȊO�͎��s�B</returns>
	IThProcLibDefDllAPI int th_lib_close_cmd(int cmdUID);

//#43_rendering_command
	/// <summary>
	/// Volume�f�[�^���̎擾�B
	/// ���[�hVolume�f�[�^�́@ref: ThLibCmd_Render_Load_Vol3D
	/// </summary>
	/// <param name="cmd"> cmd.cmdID ��ThLibCmd_Render_Vol3D���w��,cmd.intParam��volID���w�� </param>
	/// <param name="outVol"> Volume�f�[�^����Ԃ�</param>
	/// <returns>�O�F����,����ȊO�͎��s�B</returns>
	IThProcLibDefDllAPI int th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData *outVol);
	
	/// <summary>
	/// �����_�����O�p��GC�𐶐��A�y�ь���DLL����GC�̍ŐV�����擾����B(���E�B���h�E���ɐ�������j
	/// �ėp�R�}���h th_lib_exe_cmd �� ThLibCmd_Release_RenderGC �Ń����[�X�B
	/// </summary>
	/// <param name="cmd">cmd.cmdID ��ThLibCmd_Render_Create_GC��������ThLibCmd_Render_Query_GC (cmd->intParam��render_gc_id)���w��</param>
	/// <param name="outGC"></param>
	/// <returns></returns>
	IThProcLibDefDllAPI int th_lib_render_GC(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC);

	/// <summary>
	/// �����_�����O�֘A�̃R�}���h���s�B
	/// �������ꂽ�QDImage��imageID>=0�̏ꍇ�́A�ėp�R�}���h th_lib_exe_cmd ThLibCmd_Release_Image2D �Ń����[�X�B
	/// �������ꂽ�QDImage�̃o�b�t�@�[�́@th_lib_get_data�@�Ŏ擾����B
	/// </summary>
	/// <param name="cmd">cmd.cmdID ��ThLibCmd_Render_Vol3D,cmd.intParam��volID</param>
	/// <param name="param">�����_�����O�p�����[�^</param>
	/// <param name="outImage">�����_�����O���ʂ̏o�� (2D Image) </param>
	/// <returns>�O�F����,����ȊO�͎��s�B</returns>
	IThProcLibDefDllAPI int th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData *outImage);

#define TH_LIB_EXIT_FAILURE (ThLibCmdStatus_Failed)
#define TH_LIB_EXIT_SUCCESS (ThLibCmdStatus_Succeeded)
#define TH_LIB_EXIT_IN_PROGRESS (ThLibCmdStatus_Progress)
 
};
#endif //TH_PROC_OUT_LIB__H