# CPP_DLL
C#アプリからCPPで開発されたDLLの呼び出し  
# 参考資料
- [C#とC++DLL間の構造体、配列、コールバックなどの受け渡し方法](https://sksp-tech.net/archives/504#toc2)
- [C/C++で作られたDLLをC#から利用する際の引数の渡し方色々,ポインターなど...](https://ymegane88.hatenablog.com/entry/2019/08/12/013017)
- [C#からC++のDLLを呼び出す (構造体編) [C#]](https://nprogram.hatenablog.com/entry/2018/05/17/055306)
- [★　C#とC++間の配列と構造体の授受まとめ](https://qiita.com/Miyukiumoo/items/c3fcaf5fcf806ee250a0)  
# インタフェースの定義
- THProcLib/include/ 以下のファイルを参照。  
- [コマンド詳細](Commands.md)
# CSから呼び出しのテスト
- testDLLCS 参考程度
- CPP_DLL/bin/ 以下のThProcLib.dll　を　CSプロジェクトのビルドしたフォルダ以下にコピーしてください。
- CS内には　 [DllImport("ThProcLib.dll", EntryPoint =xxxx] のように使ってください。
