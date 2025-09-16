 
#include "ThProcLibMain.h"
#include "IThPCCoreLibDummy.h"
#include "ThStringUtils.h"
#include <memory>
#include "ThProcLibRenderDummy.h"
#include "ThProcLibMainDummy.h"
int CThProcMainDummy::th_lib_render_vol_dummy(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)
{
	
//	switch(param->render_type)
	outImage->format = ImageFormat_32ARGB;
	outImage->sizeX = param->target_sizeX;
	outImage->sizeY = param->target_sizeY;
	outImage->ptichX = outImage->ptichY = 0.1f;
	int imageID = outImage->imageID;
	m_render_img_dummy.insert(std::map<int, RenderImgDummy*>::value_type(outImage->imageID, new RenderImgDummy(outImage)));
	RenderImgDummy* render_img = th_lib_get_image_dummy(imageID);
	if (render_img == nullptr) return TH_LIB_EXIT_FAILURE;
	if (!render_img->creageImage()) return TH_LIB_EXIT_FAILURE;
	ARGB_def* p_ARGB_Img = (ARGB_def * )(render_img->m_image_buffer.get());
	int slice_pos = param->slice_position;
	if (slice_pos  < 0) slice_pos = 0;
	for (int y_i = 0; y_i < outImage->sizeY; y_i++) {
		for (int x_i = 0; x_i < outImage->sizeX; x_i++) {
			ARGB_def& pixel = p_ARGB_Img[y_i * outImage->sizeX + x_i];
			float vx = ((x_i+ slice_pos )%125)/125.0f ;
			float vy = 0.5f+0.5f*(((y_i) % 125)/125.0f)  ;
			unsigned char v = (unsigned char)(vx*vy*255);
			pixel.A = 255;
			pixel.R = v;
			pixel.G = v;
			pixel.B = v;
		}
	}
	return TH_LIB_EXIT_SUCCESS;
}
 
RenderImgDummy* CThProcMainDummy::th_lib_get_image_dummy(int imageID)
{
	RenderImgDummy* ret_p = nullptr;
	for (auto it = m_render_img_dummy.begin(); it != m_render_img_dummy.end(); it++) {
		if (it->second->m_p_imge_info.imageID == imageID) {
			ret_p =  it->second;
			break;
		}
	}
	return ret_p;
}
void  CThProcMainDummy::th_lib_delete_image_dummy(int imageID)
{
	RenderImgDummy* p = nullptr;
	for (auto it = m_render_img_dummy.begin(); it != m_render_img_dummy.end(); it++) {
		if (it->second->m_p_imge_info.imageID == imageID) {
			p = it->second;
			
		}
	}
	if (p) {
		m_render_img_dummy.erase(imageID);
		delete p;
	}
	return ;
}

