namespace AppUI.Services;

public class MessageService
{
    public static string GetMessage(MessageCode code)
    {
        return code switch
        {
            // ホーム画面
            //  - 新規データ追加
            MessageCode.DlgMsgImportRomNotFound =>    "CD-ROMの挿入を検出できませんでした。",
            MessageCode.DlgMsgImportRomSetting =>     "CD-ROMをセットしてドライブを閉じてください",
            MessageCode.DlgMsgImportDriveNoCDRom =>   "指定したドライブのCD-ROMを検出できませんでした。",
            MessageCode.DlgMsgImportDataNotFound =>   "対象データが見つかりません",
            MessageCode.DlgMsgImportDataCopying =>    "データコピー中・・・",
            //  - 3Dデータリスト
            MessageCode.DlgMsgDeleteThisPatient =>    "この患者データを削除しますか",
            MessageCode.DlgMsgDeletedPatient =>       "削除しました",
            MessageCode.DlgMsgDeleteAllPatient =>     "一括削除しますか",
            MessageCode.DlgMsgDeletedAllPatient =>    "削除しました",
            //  - 転送用データリスト
            MessageCode.DlgMsgTransportCreating =>    "データ作成中・・・",
            MessageCode.DlgMsgTransportCreated =>     "データ作成完了",
            MessageCode.DlgMsgUSBDataTransfer =>      "データ転送中・・・",
            MessageCode.DlgMsgDeletedTrasport =>      "削除しました",
            //  - USB転送(バッチ)
            MessageCode.FprSuccess =>                 "データ転送完了",
            MessageCode.FprInvalidFolderPath =>       "転送するフォルダパスまたはプラットフォームが指定されていません", //ErrCode
            MessageCode.FprADBCommandNotFound =>      "ADBコマンドが見つかりません。Android SDK Platform Toolsをインストールしてください",
            MessageCode.FprADBBootFailed =>           "ADBサーバーの起動または通信に失敗しました",
            MessageCode.FprUSBUnConnected =>          "Android端末が接続されていません",
            MessageCode.FprFolderNotExist =>          "指定されたフォルダが存在しません", //ErrCode
            MessageCode.FprExtractFolderNameFailed => "フォルダ名の抽出に失敗しました",  //ErrCode
            MessageCode.FprTargetDirCreateFailed =>   "Android端末上でのディレクトリ作成に失敗しました",
            MessageCode.FprTransportFailed =>         "ファイル転送に失敗しました",
            MessageCode.FprInvalidPlatformName =>     "無効なプラットフォームが指定されました",  //ErrCode
            MessageCode.FprAppleDevLaunchFailed =>    "Apple Devicesアプリの起動に失敗しました",
            MessageCode.FprUsersCanccel =>            "データの転送を中断しました",
            MessageCode.FprBatchFileNotExists =>      "バッチファイルが見つかりません",
            MessageCode.FprUnknownError =>            "バッチファイルの処理中に未知のエラーが発生しました",  //ErrCode
            MessageCode.FprBatchFileNotFound =>       "バッチファイルが見つかりません",
            MessageCode.FprSourceNotFound =>          "転送元データが見つかりません",
            // マーカ・領域画面
            MessageCode.MarkerRoiMsgConrimMarker =>   "正しいマーカの位置をクリックしてください",
            MessageCode.MarkerRoiMsgMarkerCorrect =>  "マーカの位置は正しいですか？",
            MessageCode.DlgMsgDeleteRoiClear =>       "すべて削除しますか？",
            MessageCode.MarkerRoiMsgConfirmRoi =>     "関心領域の設定はこれでいいですか？",
            MessageCode.MarkerRoiMsgSave =>           "保存しますか？",
            // システム
            MessageCode.DLLImageServerError =>        "ImageServerが起動されていません。", // ErrCode
            MessageCode.MultipleAppLaunch =>          "アプリケーションはすでに起動されています。",
            // AppUI内部エラー
            MessageCode.VolumeKeyUndefined =>         "Volume情報キーの取得に失敗しました。", //ErrCode
            _ =>                                      "未知のエラーが発生しました。"
        };
    }

    public static string GetMessage(FolderPusherResult fprResult)
    {
        var messageCode = fprResult switch
        {
            FolderPusherResult.Success => MessageCode.FprSuccess,
            FolderPusherResult.UnknownError => MessageCode.UnknownError,
            FolderPusherResult.InvalidFolderPath => MessageCode.FprInvalidFolderPath,
            FolderPusherResult.ADBCommandNotFound => MessageCode.FprADBCommandNotFound,
            FolderPusherResult.ADBBootFailed => MessageCode.FprADBBootFailed,
            FolderPusherResult.USBUnConnected => MessageCode.FprUSBUnConnected,
            FolderPusherResult.FolderNotExist => MessageCode.FprFolderNotExist,
            FolderPusherResult.ExtractFolderNameFailed => MessageCode.FprExtractFolderNameFailed,
            FolderPusherResult.TargetDirCreateFailed => MessageCode.FprTargetDirCreateFailed,
            FolderPusherResult.TransportFailed => MessageCode.FprTransportFailed,
            FolderPusherResult.InvalidPlatformName => MessageCode.FprInvalidPlatformName,
            FolderPusherResult.AppleDevLaunchFailed => MessageCode.FprAppleDevLaunchFailed,
            FolderPusherResult.UsersCanncel => MessageCode.FprUsersCanccel,
            FolderPusherResult.BatchFileNotExists => MessageCode.FprBatchFileNotExists,
            FolderPusherResult.SourceFileNotFound => MessageCode.FprSourceNotFound,
            _ => MessageCode.FprUnknownError,
        };
        return GetMessage(messageCode);
    }
}
public enum MessageCode
{
    None,
    UnknownError,
    // HomeScreenViewModel.cs
    FprSuccess,                     // データ転送完了
    FprInvalidFolderPath,           // 転送するフォルダパスまたはプラットフォームが指定されていません
    FprADBCommandNotFound,          // ADBコマンドが見つかりません。Android SDK Platform Toolsをインストールしてください
    FprADBBootFailed,               // ADBサーバーの起動または通信に失敗しました
    FprUSBUnConnected,              // Android端末が接続されていません
    FprFolderNotExist,              // 指定されたフォルダが存在しません
    FprExtractFolderNameFailed,     // フォルダ名の抽出に失敗しました
    FprTargetDirCreateFailed,       // Android端末上でのディレクトリ作成に失敗しました
    FprTransportFailed,             // ファイル転送に失敗しました
    FprInvalidPlatformName,         // 無効なプラットフォームが指定されました
    FprAppleDevLaunchFailed,        // Apple Devicesアプリの起動に失敗しました
    FprUsersCanccel,                // データの転送を中断しました
    FprBatchFileNotExists,          // バッチファイルが見つかりません
    FprUnknownError,                // バッチファイルの処理中に未知のエラーが発生しました
    FprBatchFileNotFound,           // バッチファイルが見つかりません
    FprSourceNotFound,              // 転送元データが見つかりません
    DLLImageServerError,            // ImageServerが起動されていません。
    DlgMsgUSBDataTransfer,          // データ転送中
    DlgMsgImportRomSetting,         // CD-ROMをセットしてドライブを閉じてください
    DlgMsgImportRomNotFound,        // CD-ROMの挿入を検出できませんでした。
    DlgMsgImportDriveNoCDRom,       // 指定したドライブのCD-ROMを検出できませんでした。
    DlgMsgImportDataNotFound,       // 対象データが見つかりません
    DlgMsgImportDataCopying,        // データコピー中・・・
    DlgMsgDeleteThisPatient,        // この患者データを削除しますか
    DlgMsgDeletedPatient,           // 削除しました
    DlgMsgDeleteAllPatient,         // 一括削除しますか
    DlgMsgDeletedAllPatient,        // 削除しました
    DlgMsgTransportCreating,        // データ作成中・・・
    DlgMsgTransportCreated,         // データ作成完了
    DlgMsgDeletedTrasport,          // 削除しました
    MarkerRoiMsgConrimMarker,       // 正しいマーカの位置をクリックしてください
    MarkerRoiMsgSave,               // 保存しますか？
    MarkerRoiMsgMarkerCorrect,      // マーカの位置は正しいですか？
    MarkerRoiMsgConfirmRoi,         // 関心領域の設定はこれでいいですか？
    DlgMsgDeleteRoiClear,           // すべて削除しますか？
    // システムエラー
    MultipleAppLaunch,
    VolumeKeyUndefined,
}

