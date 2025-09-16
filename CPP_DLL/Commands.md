# 共通コマンドth_lib_exe_cmd
 非同期実行のコマンドに対して、コマンド実行後にth_lib_get_statusで実行ステータスを確認し、完了後にはth_lib_close_cmdでクローズが必要。
 
## CDデータのインポート
関数：th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=0 (ThLibCmd_Exe_ImportData)
- 非同期実行
 - パラメータ
## CDデータインポート処理のキャンセル
関数：th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=5 (ThLibCmd_Exe_AsyncCmd_Cancel)
- 同期実行
 - パラメータ
 - ThProcLib_Cmd:intParam　に　キャンセルの対象となる発行された非同期コマンドのcmdUID
## 転送用ファイルの作成
- ~~ThProcLib_Cmd:int cmdID=1 (ThLibCmd_Exe_OutputData)~~
- 非同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
  - ThProcLib_Cmd:intParam　に
    - ThLibCmdOutput_Xreal = 0,
    - ThLibCmdOutput_iPhone = 1,
    - ThLibCmdOutput_Xreal_iPhone = 2,
#  データの削除 
関数：th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=2 (ThLibCmd_Exe_DeleteData)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
  - ThProcLib_Cmd:intParam にデータ種類の指定。
    - 0 (ThLibQueryList_CTData)  インポートしたデータ
    - 1 (ThLibQueryList_OutputData)　転送用データ (変更->ThLibCmd_Output_Remove　)
## バックグランドで３D化処理
関数：th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=3 (ThLibCmd_Exe_AISegmentator)
- 非同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
## ２D確認用の画像取得
- ThProcLib_Cmd:int cmdID=10 (ThLibCmd_Query_CTSlice)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
 - 返値
   - ThProcLib_CmdOutput:strParam1 にFolder名
## Studyフォルダの取得
 システム内部の各フォルダ以下にStudy毎に生成されるフォルダ名（既定）  
 (各フォルダ以下のSubfolder名）  
 関数：th_lib_exe_cmd
 - ThProcLib_Cmd:int cmdID=50 (ThLibCmd_Get_StudyPath)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
 - 返値
   - ThProcLib_CmdOutput:strParam1 にFolder名
## 出力用のMeshファイルのフォルダの取得
 デバイスに転送用のMeshファイルが保存されているフォルダ名の取得。  
 関数：th_lib_exe_cmd
 - ThProcLib_Cmd:int cmdID=52 (ThLibCmd_Get_MeshPath)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
  - ThProcLib_Cmd:intParam　に
    - ThLibCmdOutput_Xreal = 0,
    - ThLibCmdOutput_iPhone = 1,
 - 返値
   - ThProcLib_CmdOutput:strParam1 にFolder名（ fullpath）  
# th_lib_get_image_info
## ２D画像情報の取得
関数：th_lib_exe_cmd
- ThProcLib_Cmd:int cmdID=51 (ThLibCmd_Get_DicomImageInfo)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
  - ~~ThProcLib_Cmd:intParam 0: juukaku, 1:haiya~~
  - ThProcLib_Cmd:intParam 0: haiya, 1: juukaku
- 返値
   - ThProcLib_DicomImageInfo にDICOM Image 情報
 
# CT-Markerの操作
## 全てのCTマーカーを削除する
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

# 関心領域関連の操作
## 関心領域の削除
関数：th_lib_exe_render  
- ThProcLib_Cmd:int cmdID=30 (ThLibCmd_ROI_Clear)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
## 最終確認（関心領域が正しい）
関数：th_lib_exe_render  
 -ThProcLib_Cmd:int cmdID=33 (ThLibCmd_ROI_Final)
- 同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。

# 転送用ファイル一覧の操作
## 転送ファイルの削除
Xreal/iPhone個別に削除する
- ThProcLib_Cmd:int cmdID=40 (ThLibCmd_Output_Remove)
- 非同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
   - ThProcLib_Cmd:intParam に削除対象の指定。
     -  0 (ThLibCmdOutput_Xreal)
     -  1 (ThLibCmdOutput_iPhone)
     -  2 (ThLibCmdOutput_Xreal/ThLibCmdOutput_iPhone) 両方
## 転送ファイルの追加
- ThProcLib_Cmd:int cmdID=41 (ThLibCmd_Output_Add)
- 非同期実行
- パラメータ
  - ThProcLib_Cmd:strParam1 にStudyUID指定。
   - ThProcLib_Cmd:intParam に削除対象の指定。
     -  0 (ThLibCmdOutput_Xreal)
     -  1 (ThLibCmdOutput_iPhone)
     -  2 (ThLibCmdOutput_Xreal/ThLibCmdOutput_iPhone) 両方
     
