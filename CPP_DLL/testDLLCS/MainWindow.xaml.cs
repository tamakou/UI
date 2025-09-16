using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Drawing;
using System.Security.Permissions;
using System;


namespace testDLLCS
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct ThProcLib_Env
    {
        //#16_changed_dll_interface_05_18
        //   [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string LogFolder;
        public int LogLevel;
        //#16_changed_dll_interface_05_18 added following
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string HomeFolder;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string OutputFolder;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string DcmRawDataFolder;
        public int ThProLibDummy;//0: system, 1: dummy  
    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct CsPatientInfo
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string PatientName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string PatientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string Birthday; // YYYY/MM/DD
        public int Age;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 8)] public string Gender;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string StudyDate;// YYYY/MM/DD
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string StudyTime;// HH:MM:SS
        public float SliceThickness;  //unit : mm
                                      //    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string CTMode;
        public int ProcessStatus;//関心領域　0: 空白、 1: 〇
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string UpdateDate;// YYYY/MM/DD
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string UpdateTime;// HH:MM:SS
        public int StudyPattern; //症例
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudDescription;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudyUID;
        public int OutputType;//#16_changed_dll_interface_05_18  0: Xreal, 1:Xreal/iPhone
        public int CheckDig;//check bit, always  1234;
    }
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct CsDicomImageInfo
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudyUID;
        public int sizeX;
        public int sizeY;
        public int sizeZ;
        public int WindowWidth;
        public int WindowCenter;
        public int CheckDig;//check bit, always  2345;
    }
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct CsCmd
    {
        public int cmdID;//0: import data, 1: export mesh data
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam1;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam2;
        //#16_changed_dll_interface_05_18
        public int intParam;
        public float floatParam;

    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct CsCmdOutput
    {
        public int cmdType; //0:sync, 1:async
        public int cmdID; //実行コマンドの元ID
        public int cmdUID;//コマンド実行毎に生成させるID
        //#16_changed_dll_interface_05_18
        public int cdmExtDataSize;  //コマンド返値の拡張データ。0: 拡張データなし、>0: 拡張データあり(バイト数）、別途コマンドth_lib_get_dataで結果データを取得する。
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam1;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam2;
        public int intParam;
        public float floatParam;
    };
    //#16_changed_dll_interface_05_18
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct CsCmdStatsu
    {
        public int cmdID;//実行コマンドの元ID
        public int cmdStatus; //-1: failed, 0: succeeded, 1: in progress
        public int cmdProgress;//0,1,...,100
        public int dataID;// 
        //#17 added member strParam1 <- 2025/05/20
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam1;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string errMessage;

    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct ThProcLib_Vol3DData
    {
        public int volID;//内部生成、管理
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudyUID;
        public int sizeX;
        public int sizeY;
        public int sizeZ;
        public float ptichX;//mm
        public float ptichY;//mm
        public float ptichZ;//mm
        public int reserved_int1;
        public int reserved_int2;
        public int reserved_int3;
        public float reserved_float1;
        public float reserved_float2;
        public float reserved_float3;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string reserved_char;
 
    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct ThProcLib_RenderGC
    {
        public int render_gc_id; //内部生成、管理
                                 //define command
        public int render_cmd_major;
        public int render_cmd_minor;
        //draw area of window
        //常に対象ウィンドウの描画アリアと一致する!
        public int target_sizeX;
        public int target_sizeY;
        //reander data
        public int series_type; //ref: eSeriesType
        //render parameter
        public float zoom;
        public float panX;
        public float panY;
        public float slice_position;//slice number
        public int display_parts;//ref: eDisplayParts
        public int study_case; //TBD: 病症
        public int render_preset;//for 3D Lut
                                 //window leve/window width
        public int window_levle;
        public int window_width;
        //
        public int display_mode;//ref: eDisplayMode //表示モードの指定 ビットの組み合わせ

        //processing mouse event
        public int mouse_type;//ref: eMouseEvent
        public int mouse_posX;
        public int mouse_posY;
        //--------
        public int reserved_int1;
        public int reserved_int2;
        public float reserved_float1;
        public float reserved_float2;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string reserved_char;
    };
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct ThProcLib_Image2DData
    {
        public int imageID;//内部生成、管理
        public int sizeX;
        public int sizeY;
        public float ptichX;//mm
        public float ptichY;//mm
        public int format; //ref: eImageFormat
        //#123_request_next_render_cmd_from_DLL
        int next_render_cmds; //ref: eRequestRenderCmd  //ビットの組み合わせ
        public int reserved_int1;
        public int reserved_int2;
        public float reserved_float1;
        public float reserved_float2;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string reserved_char;
    };

    public partial class MainWindow : Window
    {
        
        public MainWindow()
        {
            InitializeComponent();
        }
        /// <summary>
        /// DLLの呼び出しは１つクラスに纏めて、行う！
        /// </summary>

        CsCmd cmd = new CsCmd();
        CsCmdOutput cmd_ret = new CsCmdOutput();
        ThProcLib_Vol3DData ret_vol = new ThProcLib_Vol3DData();
        ThProcLib_RenderGC ret_GC = new ThProcLib_RenderGC();
        ThProcLib_Image2DData ret_image = new ThProcLib_Image2DData();
        private void button_Click_load_dll(object sender, RoutedEventArgs e)
        {
            //     [DllImport("ThProcLib.dll", CallingConvention = CallingConvention.Cdecl)]

            [DllImport("ThProcLib.dll",EntryPoint = "th_lib_init")]
            static extern int th_lib_init(int a);

            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_setup_env", CharSet = CharSet.Unicode)]
            static extern int th_lib_setup_env(ref ThProcLib_Env env);

            [DllImport("ThProcLib.dll",EntryPoint = "th_lib_get_env", CharSet = CharSet.Unicode)]
            static extern int th_lib_get_env(ref ThProcLib_Env env);

          　
            int i = th_lib_init(0);//最初の実行は必須。 //#16_changed_dll_interface_05_18
            ThProcLib_Env env = new ThProcLib_Env() ;
            th_lib_get_env(ref env);//先に取得。　//#16_changed_dll_interface_05_18
            bool isDummy = env.ThProLibDummy == 1;//#15 check dll_lib type;
            //   env.LogFolder = "c/temp";
            th_lib_setup_env(ref env);
             
           th_lib_get_env(ref env);
            //
             
        }
        private void button_Click_test_dll(object sender, RoutedEventArgs e)
        {

            //     [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_datalist", CharSet = CharSet.Unicode)]
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_datalist_size", CharSet = CharSet.None)]
            static extern int th_lib_get_datalist_size(int type);

            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_datalist", CharSet = CharSet.None)]
            static extern int th_lib_get_datalist(ref CsPatientInfo out_info, int type, int no);
            //#16_changed_dll_interface_05_18

            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_data")]
            static extern int th_lib_get_data(ref CsCmd cmd,char[] intDataBuffer, int BufferSize);

            //#16_changed_dll_interface_05_18
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_image_info")]
            static extern int th_lib_get_image_info(ref CsCmd cmd, ref CsDicomImageInfo out_info);
            ///
            //CTデータ一覧の取得
            //一覧データを取得するため、先にサイズを取得する。
            int pat_info_size = th_lib_get_datalist_size(0);
            for (int no_i = 0; no_i < pat_info_size; no_i++)
            {
                //個々データを取得し、画面表示の一覧に表示する。
                int no = no_i;
                CsPatientInfo pat_info = new CsPatientInfo();
                int ret_sts = th_lib_get_datalist(ref pat_info, 0, no);//#16_changed_dll_interface_05_18

            }
            //転送データ一覧の取得
            //一覧データを取得するため、先にサイズを取得する。
             
            int output_data_size = th_lib_get_datalist_size(1);
            for (int no_i = 0; no_i < output_data_size; no_i++)
            {
                //個々データを取得し、画面表示の一覧に表示する。
                int no = no_i;
                CsPatientInfo pat_info = new CsPatientInfo();
                int ret_sts = th_lib_get_datalist(ref pat_info, 1, no);//#16_changed_dll_interface_05_18
                int outptu_type = pat_info.OutputType;//check
            }
             
            
            {//#16_changed_dll_interface_05_18
                cmd.cmdID = 51;// ThLibCmd_Get_DicomImageInfo
                cmd.strParam1 = "123.333.55";
                CsDicomImageInfo dicom_image_info = new CsDicomImageInfo();
                int sts = th_lib_get_image_info( ref cmd, ref dicom_image_info);
            }
        }
        private void button_Click_test_cmd(object sender, RoutedEventArgs e)
        {
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_exe_cmd")]
            static extern int th_lib_exe_cmd(ref CsCmd cmd, ref CsCmdOutput cmd_ret);

            cmd.cmdID = 0;//impoort CD-Data;
            th_lib_exe_cmd(ref cmd, ref cmd_ret);

            //cmd_ret.cmdUID　を記憶しておく、  th_lib_get_statusに使う

        }
        private void button_Click_test_status(object sender, RoutedEventArgs e)
        {
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_datalist", CharSet = CharSet.None)]
            static extern int th_lib_get_datalist(ref CsPatientInfo out_info,int type, int no);

            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_status")]
            static extern int th_lib_get_status(int cmdUID, ref CsCmdStatsu status);

            CsCmdStatsu status = new CsCmdStatsu();
            th_lib_get_status(cmd_ret.cmdUID, ref status);
            if (status.cmdStatus == 0)
            {//コマンド実行完了
                if((status.cmdID == cmd_ret.cmdID) &&
                    (status.cmdID == 0))//<-Import CD-Data command
                {
                    //Import CD-Data完了後に、新しいData項目を取得、画面一覧に追加する。
                    {
                        int no = status.dataID;
                        CsPatientInfo pat_info = new CsPatientInfo();
                        int ret_sts = th_lib_get_datalist(ref pat_info,0, no);

                    }
                }
            }
            int dbg_xx = 0;
        }
        private void button_Click_close_cmd(object sender, RoutedEventArgs e)
        {
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_close_cmd")]
            static extern int th_lib_close_cmd(int cmdUID);
            th_lib_close_cmd(cmd_ret.cmdUID);
        }
        private void button_Click_test_image(object sender, RoutedEventArgs e)
        {
            // 画像作成
            int width = 512;
            int height = 100;
            Bitmap bitmap = new Bitmap(width, height);
            for (int x = 0; x < width; x++)
            {
                int value = (int)Math.Floor((double)x / ((double)width / 256));
                System.Drawing.Color color = System.Drawing.Color.FromArgb(255, value, value, value);
                for (int y = 0; y < height; y++)
                {
                    bitmap.SetPixel(x, y, color);
                }
            }

            // 表示
            IntPtr hbitmap = bitmap.GetHbitmap();
            image_draw_area.Source = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(hbitmap, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
       //     DeleteObject(hbitmap);
        }
        private void button_Click_load_vol(object sender, RoutedEventArgs e)
        {
            //exe load command at first
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_exe_cmd")]
            static extern int th_lib_exe_cmd(ref CsCmd cmd, ref CsCmdOutput cmd_ret);

            cmd.cmdID = 125;//ThLibCmd_Render_Load_Vol3D;
         //   cmd.strParam1 = "1.2.392.200069.17.10338133111.1202505010003201";
            cmd.strParam1 = "1.2.392.200069.17.10338133111.1202402190071101";
            th_lib_exe_cmd(ref cmd, ref cmd_ret);
            //wait
            //
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_status")]
            static extern int th_lib_get_status(int cmdUID, ref CsCmdStatsu status);

            CsCmdStatsu status = new CsCmdStatsu();
            th_lib_get_status(cmd_ret.cmdUID, ref status);
            if (status.cmdStatus == 0)
            {//コマンド実行完了

            }
                   
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_vol_info")]
            static extern int th_lib_get_vol_info(ref CsCmd cmd, ref ThProcLib_Vol3DData outVol);
             
            cmd.cmdID = 123;//ThLibCmd_Render_Vol3D
            cmd.intParam = status.dataID;
            th_lib_get_vol_info(ref cmd, ref ret_vol);
            
        }
        private void button_Click_create_GC(object sender, RoutedEventArgs e)
        {
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_render_GC")]
            static extern int th_lib_render_GC(ref CsCmd cmd,ref ThProcLib_RenderGC outGC);
         
            cmd.cmdID = 121; //ThLibCmd_Render_Create_GC
            th_lib_render_GC(ref cmd, ref ret_GC);
        }
        private void button_Click_render_vol(object sender, RoutedEventArgs e)
        {
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_exe_render")]
            static extern int th_lib_exe_render(ref CsCmd cmd, ref ThProcLib_RenderGC GC,ref ThProcLib_Image2DData outImg);
            cmd.cmdID = 123;//ThLibCmd_Render_Vol3D
            
            ret_GC.target_sizeX = (int)image_draw_area.Width;
            ret_GC.target_sizeY = (int)image_draw_area.Height;
            ret_GC.render_cmd_major = 1;
            ret_GC.slice_position = 163;
            ret_GC.zoom = 1.0f;
            ret_GC.window_levle = -550;
            ret_GC.window_width = 1500;
            ret_GC.series_type = 0;//ThLibSeriesType_Haiya
            ret_GC.display_mode = 1;//ThLibDisplayMode_Volume
            th_lib_exe_render(ref cmd, ref ret_GC, ref ret_image);
        }
        private void button_Click_get_image(object sender, RoutedEventArgs e)
        {
           
            [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_data")]
            static extern int th_lib_get_data(ref CsCmd cmd, byte[] intDataBuffer, int BufferSize);

            int buffer_size = ret_image.sizeX * ret_image.sizeY * 4;
            byte[] dataBuffer = new byte[buffer_size];
            for (int ii = 0; ii < buffer_size; ii++)
            {
                dataBuffer[ii] = (byte)(0);
            }
            cmd.cmdID = 124;//ThLibCmd_Render_Image2D
            cmd.intParam = ret_image.imageID;
            th_lib_get_data(ref cmd, dataBuffer, buffer_size);
            {
                // 画像作成
                int width = ret_image.sizeX;
                int height = ret_image.sizeY;
                Bitmap bitmap = new Bitmap(width, height);
                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        int index = (y * width + x)*4;
                        byte v_a = dataBuffer[index + 3];
                        byte v_r = dataBuffer[index + 2];
                        byte v_g = dataBuffer[index + 1];
                        byte v_b = dataBuffer[index + 0];
                        System.Drawing.Color color = System.Drawing.Color.FromArgb(v_a, v_r, v_g, v_b);
                        bitmap.SetPixel(x, y, color);
                    }
                }

                // 表示
                IntPtr hbitmap = bitmap.GetHbitmap();
                image_draw_area.Source = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(hbitmap, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
                //     DeleteObject(hbitmap);
            }
        }
    }
}