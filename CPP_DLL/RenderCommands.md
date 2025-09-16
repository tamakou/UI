# Volume Rendering関連コマンド
元の３D（Volumeデータ及び）及びPolygonデータから画面上に２Dデータ（ピットマップ）を表示する。  
非同期実行のコマンドに対して、コマンド実行後にth_lib_get_statusで実行ステータスを確認し、完了後にはth_lib_close_cmdでクローズが必要。
-[UI-DLL間Rendering概念図](CPP_DLL_Rendering.pdf)
## ImageServer起動の確認
th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=200 (ThLibCmd_Check_Image_Server)
- 同期実行
 - パラメータ
- 戻り値：
- ThProcLib_CmdOutput:intParam
  - 1: ImageServer正常に起動されている
  - 0: ImageServerが起動されていない、エラーメッセージ表示。
## Volumeデータのロード
th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=125 (ThLibCmd_Render_Load_Vol3D)
- 非同期実行
 - パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
- 戻り値：　th_lib_get_status 完了時のstatus.dataIDがvolIDとなり、th_lib_get_vol_infoでVolumeデータ情報を取得する。

## Volumeデータ情報の取得
th_lib_get_vol_info
- ThProcLib_Cmd:int cmdID=123 (ThLibCmd_Render_Vol3D)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:intParam　に ThLibCmd_Render_Load_Vol3D の戻り値volIDを指定。
    
- 戻り値
 ThProcLib_Vol3DData 構造体

## Volumeデータリリース
th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=132 (ThLibCmd_Release_Vol3D)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:intParam　に volIDを指定。

## Renderingコマンド用のＧＣを新規作成
th_lib_render_GC
- ThProcLib_Cmd:int cmdID=121 (ThLibCmd_Render_Create_GC)
- 同期実行
- パラメータ
- 戻り値
 ThProcLib_RenderGC 構造体

## DLL側のＧＣ情報を取得する。
th_lib_render_GC
- ThProcLib_Cmd:int cmdID=122 (ThLibCmd_Render_Query_GC)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に render_gc_idを指定。
- 戻り値
 ThProcLib_RenderGC 構造体

## ＧＣのリリース
th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=131 (ThLibCmd_Release_RenderGC)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に render_gc_idを指定。
- 戻り値

## 画像イメージ（Renderingの出力）のリリース （ThProcLib_Image2DData）
th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=133 (ThLibCmd_Release_Image2D)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に imageIDを指定。
- 戻り値

## 3DModelのロード
th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=126 (ThLibCmd_Render_Load_3DModel)
- 同期実行
 - パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
- 戻り値　
- 
# 以下の表示コマンドはVolumeデータがロードされている状態を前提とする。
## 2Dデータ表示
### 2D Renderingコマンドの発行 
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=123 (ThLibCmd_Render_Vol3D)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に volIDを指定。
 - ThProcLib_RenderGC:render_cmd_major = 1 (ThLibRenderCmd_2D)
 - ThProcLib_RenderGC に2Dの詳細の指定
- 戻り値
 - ThProcLib_Image2DData 構造体 imageID >=0
## 結果画像イメージの取得
th_lib_get_data
- ThProcLib_Cmd:int cmdID = 124 (ThLibCmd_Render_Image2D)
- ThProcLib_Cmd:intParam = imageID (th_lib_exe_renderの戻り値)
###  2D画像Renderingのパラメータ例：
- ThProcLib_RenderGC:render_cmd_major = 1 (ThLibRenderCmd_2D)
- ThProcLib_RenderGC:target_sizeX = 300 
- ThProcLib_RenderGC:target_sizeY = 400 
- ThProcLib_RenderGC:series_type = 0 (ThLibSeriesType_Haiya)
- ThProcLib_RenderGC:zoom = 1.0
- ThProcLib_RenderGC:panX = 0 
- ThProcLib_RenderGC:panY = 0 
- ThProcLib_RenderGC:slice_position = 1 (2,3,...N) slice number

## 3Dデータ表示
### 3D Renderingコマンドの発行 
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=123 (ThLibCmd_Render_Vol3D)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に volIDを指定。
 - ThProcLib_RenderGC:render_cmd_major = 2 (ThLibRenderCmd_3D)
 - ThProcLib_RenderGC に3Dの詳細の指定
- 戻り値
 - ThProcLib_Image2DData 構造体　　imageID >=0
## 結果画像イメージの取得
th_lib_get_data
- ThProcLib_Cmd:int cmdID = 124 (ThLibCmd_Render_Image2D)
- ThProcLib_Cmd:intParam = imageID (th_lib_exe_renderの戻り値)

# CT-Markerの操作(確認画面）
## 1)縦隔／肺野の切替
①関数：　th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
- ThProcLib_Cmd:intParam　に volIDを指定。
- ThProcLib_RenderGC:render_cmd_major=ThLibRenderCmd_Setup_Series (6)
- ThProcLib_RenderGC:series_typeに　ThLibSeriesType_Haiya/ThLibSeriesType_Juukaku　を指定

②描画更新　： 戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D 

## 2)Zoom
関数：th_lib_exe_render
- ThProcLib_Cmd:int cmdID=123 (ThLibCmd_Render_Image3D)
- 同期実行
- パラメータ
- ThProcLib_Cmd:intParam　に volIDを指定。
- ThProcLib_RenderGC:Crender_cmd_major=ThLibRenderCmd_2D
-  ThProcLib_RenderGC:zoom　= zoom*1.5
       （初期値： 1.0 全画面表示）

## 3) slice number
関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=123 (ThLibCmd_Render_Image3D)
- 同期実行
- パラメータ
- ThProcLib_Cmd:intParam　に volIDを指定。
- ThProcLib_RenderGC:Crender_cmd_major=ThLibRenderCmd_2D
-  ThProcLib_RenderGC:slice_position
    1,2,3,...指定

# CT-Markerの操作(再指定画面）

## １）２D側のマウスクリック
① 関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
-  ThProcLib_Cmd:intParam　に volIDを指定。
-  ThProcLib_RenderGC:Crender_cmd_major= ThLibRenderCmd_ProcMouse
-  ThProcLib_RenderGC:mouse_type = ThLibMouseEvent_CT_Marker_2D
-  ThProcLib_RenderGC:mouse_posX = 描画領域座標 x (dot)
-  ThProcLib_RenderGC:mouse_posY = 描画領域座標 x (dot)

② 描画更新　 戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D 

## ２）３D側のマウスクリック
① 関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
-  ThProcLib_Cmd:intParam　に volIDを指定。
-  ThProcLib_RenderGC:Crender_cmd_major= ThLibRenderCmd_ProcMouse
-  ThProcLib_RenderGC:mouse_type = ThLibMouseEvent_CT_Marker_3D
-  ThProcLib_RenderGC:mouse_posX = 描画領域座標 x (dot)
-  ThProcLib_RenderGC:mouse_posY = 描画領域座標 x (dot)

② 描画更新　 戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D 

##  3)ボタン「やり直し」
~~関数：th_lib_exe_cmd~~  
関数：th_lib_exe_render
- ThProcLib_Cmd:int cmdID=20 (ThLibCmd_CT_Marker_Clear)
- 同期実行
- パラメータ
- ThProcLib_Cmd:intParam　に volIDを指定。

## 最終確認（CTマーカーが正しい）
関数：th_lib_exe_render  
 -ThProcLib_Cmd:int cmdID=22 (ThLibCmd_CT_Marker_Final)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:intParam　に volIDを指定。

# 関心領域操作
## 全て削除
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=30 (ThLibCmd_ROI_Clear)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に volIDを指定。

② 描画更新　 戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D

## 追加モード（Pen）（モードの切り替え用）
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=31 (ThLibCmd_ROI_Add)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に volIDを指定。  
② 描画更新　 戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D

## 消去モード（消しゴム）（モードの切り替え用）
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=32 (ThLibCmd_ROI_Erase)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に volIDを指定。

## 最終確定
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=33 (ThLibCmd_ROI_Final)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に volIDを指定。
## ペンの太さの選択（消しゴム共通）
th_lib_exe_render
- ThProcLib_Cmd:int cmdID=34 (ThLibCmd_ROI_SetPen)
- 同期実行
- パラメータ
 - ThProcLib_Cmd:intParam　に ペンの太さ指定。ref: ePenType


## Penの塗る（２D側のみ）
① 関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
-  ThProcLib_Cmd:intParam　に volIDを指定。
-  ThProcLib_RenderGC:Crender_cmd_major= ThLibRenderCmd_ProcMouse
-  ThProcLib_RenderGC:mouse_type = ThLibMouseEvent_ROI_Pen
- ThProcLib_RenderGC:mouse_posX = 描画領域座標 x (dot)
- ThProcLib_RenderGC:mouse_posY = 描画領域座標 x (dot)

② 描画更新　戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D

##  消す（２D側のみ）
① 関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
-  ThProcLib_Cmd:intParam　に volIDを指定。
-  ThProcLib_RenderGC:Crender_cmd_major= ThLibRenderCmd_ProcMouse
-  ThProcLib_RenderGC:mouse_type = ThLibMouseEvent_ROI_Eraser
- ThProcLib_RenderGC:mouse_posX = 描画領域座標 x (dot)
- ThProcLib_RenderGC:mouse_posY = 描画領域座標 x (dot)

② 描画更新　戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D


## ３D側：表示(Preset)
① 関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
-  ThProcLib_Cmd:intParam　に volIDを指定。
-  ThProcLib_RenderGC:Crender_cmd_major= ThLibRenderCmd_Setup_3D_Obj (7)
-  ThProcLib_RenderGC:display_parts  
     ThLibDisplayParts_Body /ThLibDisplayParts_Lung /ThLibDisplayParts_Heart /ThLibDisplayParts_Other 組み合わせ指定  

② 描画更新　戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D

## ３D側：症例
① 関数： th_lib_exe_render
- ThProcLib_Cmd:int cmdID=134 (ThLibCmd_Render_Op)
- 同期実行
- パラメータ
-  ThProcLib_Cmd:intParam　に volIDを指定。
-  ThProcLib_RenderGC:Crender_cmd_major= ThLibRenderCmd_Change_Case
-  ThProcLib_RenderGC:study_case  
     1,2,3...指定 
     
② 描画更新　： 戻り値next_render_cmdsにより、更新なし／ 関数：th_lib_exe_render 2D／3D
 
