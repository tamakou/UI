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
	/// DLLインタフェースの定義
	/// </summary>


	/// <summary>
	/// DLLの初期化
	/// </summary>
	/// <param name="mode">: mode指定</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認</returns>
	/// 
	IThProcLibDefDllAPI int th_lib_init(int mode); 
	
	/// <summary>
	/// DLLの終了 //#183_th_lib_close
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	IThProcLibDefDllAPI void th_lib_close(void);


	/// <summary>
	/// DLLの実行環境を設定する
	/// </summary>
	/// <param name="env">: 実行環境</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認</returns>

	IThProcLibDefDllAPI int th_lib_setup_env(const ThProcLib_Env* env);

	/// <summary>
	/// DLLの実行環境を取得する。
	/// </summary>
	/// <param name="env">: 実行環境を保存するバッファー</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認 </returns>
	IThProcLibDefDllAPI int th_lib_get_env(ThProcLib_Env* env);

	/// <summary>
	///  データ一覧を取得する前に、先にデータ一覧のサイズを取得する。
	/// </summary>
	/// <param name="type">: 取得するデータ種類の指定。ref: eThLibQueryList</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認 </returns>
	IThProcLibDefDllAPI int th_lib_get_datalist_size(int type);

	/// <summary>
	/// CTデータ一(３Dデータ）一覧を取得する。
	/// </summary>
	/// <param name="out_info">: データ項目を保存するバッファー</param>
	/// <param name="type">: 取得するデータ種類の指定。ref: eThLibQueryList</param>
	/// <param name="no">: 取得するデータ番号の指定</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認 </returns>
	IThProcLibDefDllAPI int th_lib_get_datalist(ThProcLib_PatientInfo* out_info,int type, int no);

	/// <summary>
	/// コマンドの実行
	///  cmd_ret.cmdType==1 の場合は非同期実行となり。th_lib_get_statusで進捗確認。
	/// </summary>
	/// <param name="cmd">: 実行コマンド</param>
	/// <param name="cmd_ret">: 実行のリターン値</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認 </returns>
	IThProcLibDefDllAPI int th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret);

	/// <summary>
	/// DICOM画像データ情報の取得。
	/// </summary>
	/// <param name="cmd">: ThProcLib_Cmd:cmdIDに　ThLibCmd_Get_DicomImageInfoを指定、ThProcLib_Cmd:strParam1 にStudyUID</param>
	/// <param name="image_info">データ項目を保存するバッファー</param>
	/// <returns></returns>
	IThProcLibDefDllAPI int th_lib_get_image_info(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo *image_info);

	/// <summary>
	/// 汎用的なバイナリデータの出力。
	/// </summary>
	/// <param name="cmd">cmdIDに取得データの種類。intParamにImageIDの指定</param>
	/// <param name="DataBuffer">: データをこのバッファーにコピーする</param>
	/// <param name="BufferSize">: バッファーサイズの指定</param>
	/// <returns>０：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認 </returns>
	IThProcLibDefDllAPI int th_lib_get_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize);
	                                                     //#43_changed----^^^
	/// <summary>
	/// コマンド実行のステータスを取得する
	/// </summary>
	/// <param name="cmdUID">: th_lib_exe_cmdの返値ThProcLib_Cmd_OutputにあるcmdUID, ref: eThLibCmd、ThLibCmd_Unknown: 最後に実行されたコマンド</param>
	/// <param name="status">: ステータスを返す</param>
	/// <returns>０：成功,それ以外は失敗。</returns>
	IThProcLibDefDllAPI int th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status);

	/// <summary>
	/// 非同期実行されているコマンドのクローズ。 
	/// th_lib_exe_cmdの返値ThProcLib_Cmd_OutputにあるcmdTypeがThLibCmdType_Asyncの時、
	/// th_lib_get_statusのコマンド実行の完了Statusが確認された後にクローズする;
	/// </summary>
	/// <param name="cmdUID">: th_lib_exe_cmdの返値ThProcLib_Cmd_OutputにあるcmdUID</param>
	/// <returns>０：成功,それ以外は失敗。</returns>
	IThProcLibDefDllAPI int th_lib_close_cmd(int cmdUID);

//#43_rendering_command
	/// <summary>
	/// Volumeデータ情報の取得。
	/// ロードVolumeデータは　ref: ThLibCmd_Render_Load_Vol3D
	/// </summary>
	/// <param name="cmd"> cmd.cmdID にThLibCmd_Render_Vol3Dを指定,cmd.intParamにvolIDを指定 </param>
	/// <param name="outVol"> Volumeデータ情報を返す</param>
	/// <returns>０：成功,それ以外は失敗。</returns>
	IThProcLibDefDllAPI int th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData *outVol);
	
	/// <summary>
	/// レンダリング用のGCを生成、及び現在DLL側のGCの最新情報を取得する。(実ウィンドウ毎に生成する）
	/// 汎用コマンド th_lib_exe_cmd で ThLibCmd_Release_RenderGC でリリース。
	/// </summary>
	/// <param name="cmd">cmd.cmdID にThLibCmd_Render_Create_GCもしくはThLibCmd_Render_Query_GC (cmd->intParamにrender_gc_id)を指定</param>
	/// <param name="outGC"></param>
	/// <returns></returns>
	IThProcLibDefDllAPI int th_lib_render_GC(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC);

	/// <summary>
	/// レンダリング関連のコマンド実行。
	/// 生成された２DImageにimageID>=0の場合は、汎用コマンド th_lib_exe_cmd ThLibCmd_Release_Image2D でリリース。
	/// 生成された２DImageのバッファーは　th_lib_get_data　で取得する。
	/// </summary>
	/// <param name="cmd">cmd.cmdID にThLibCmd_Render_Vol3D,cmd.intParamにvolID</param>
	/// <param name="param">レンダリングパラメータ</param>
	/// <param name="outImage">レンダリング結果の出力 (2D Image) </param>
	/// <returns>０：成功,それ以外は失敗。</returns>
	IThProcLibDefDllAPI int th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData *outImage);

#define TH_LIB_EXIT_FAILURE (ThLibCmdStatus_Failed)
#define TH_LIB_EXIT_SUCCESS (ThLibCmdStatus_Succeeded)
#define TH_LIB_EXIT_IN_PROGRESS (ThLibCmdStatus_Progress)
 
};
#endif //TH_PROC_OUT_LIB__H