 
#ifndef TH_PROC_LIB_RENDER_DUMMY_H
#define TH_PROC_LIB_RENDER_DUMMY_H
class RenderImgDummy 
{
public:
	RenderImgDummy(const ThProcLib_Image2DData* img_info) :m_image_buffer(nullptr){
		m_p_imge_info = *img_info;
	}
	bool creageImage(void)
	{
		int sizeXY = m_p_imge_info.sizeX * m_p_imge_info.sizeY;
		if (sizeXY <= 0) return false;
		if (sizeXY >= (4000 * 4000)) return false;
		int bp = 1;
		switch (m_p_imge_info.format) {
		case ImageFormat_8Gray:
			bp = 1;
			break;
		case ImageFormat_16Gray:
			bp = 2;
			break;
		case ImageFormat_24RGB:
			bp = 3;
			break;
		case ImageFormat_32ARGB:
			bp = 4;
			break;
		}
		m_image_buffer.reset(new unsigned char[bp*sizeXY]);
		return true;
	}
	ThProcLib_Image2DData m_p_imge_info;
	std::unique_ptr<unsigned char[]> m_image_buffer;

};
 
struct ARGB_def {
	unsigned char A;
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

#endif TH_PROC_LIB_RENDER_DUMMY_H