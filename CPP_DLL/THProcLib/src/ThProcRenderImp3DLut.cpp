 
#include "ThProcLibMain.h"
#include "ThProcRenderImp.h"
#include "ThStringUtils.h"
#include "MultiThExe.h"
  
void CThProcRenderImp::setup3DLut(int lut_no)
{
	 
	switch (lut_no) {
	case 0:
	{//setup default VR-LUT
		int entrySize = 2;
		SmartPtr_PXSMVLutEntryList entries(PxVCoreAPI::CPXSMAPILib::createLutEntryList(entrySize));
		PxVCoreAPI::PXSMVLutEntry* pSetEntries = entries->getList();
		{
			PxVCoreAPI::PXSMVLutEntry& dest_entry = pSetEntries[0];

			dest_entry.CurveID = 3;
			dest_entry.WW = 500;
			dest_entry.WL = 400;
			dest_entry.Opacity = 1.0;
			dest_entry.LColor[0] = 255;
			dest_entry.LColor[1] = 255;
			dest_entry.LColor[2] = 255;
			dest_entry.HColor[0] = 255;
			dest_entry.HColor[1] = 255;
			dest_entry.HColor[2] = 255;
		}
		{
			PxVCoreAPI::PXSMVLutEntry& dest_entry = pSetEntries[1];

			dest_entry.CurveID = 0;
			dest_entry.WW = 200;
			dest_entry.WL = 220;
			dest_entry.Opacity = 0.2;
			dest_entry.LColor[0] = 200;
			dest_entry.LColor[1] = 4;
			dest_entry.LColor[2] = 10;
			dest_entry.HColor[0] = 255;
			dest_entry.HColor[1] = 255;
			dest_entry.HColor[2] = 128;
		}

		int bgRGB[3];

		bgRGB[0] = 0;
		bgRGB[1] = 0;
		bgRGB[2] = 0;

		m_renderParam3D->updateVRLUT(m_VolData.get(), entries.get(), bgRGB);
		//
		PxVCoreAPI::PXSMVLight setupLight;
		//get existed light at first
		m_renderParam3D->queryVRLight(setupLight);

		//
		setupLight.HeadLight = 1;
		setupLight.fIntensity = 1.0f;
		m_renderParam3D->updateVRLight(setupLight);
	}
	break;
	case 1:
	{//setup default VR-LUT
		int entrySize = 1;
		SmartPtr_PXSMVLutEntryList entries(PxVCoreAPI::CPXSMAPILib::createLutEntryList(entrySize));
		PxVCoreAPI::PXSMVLutEntry* pSetEntries = entries->getList();
		{
			PxVCoreAPI::PXSMVLutEntry& dest_entry = pSetEntries[0];

			dest_entry.CurveID = 3;
			dest_entry.WW = 626;
			dest_entry.WL = 400;// 348;
			dest_entry.Opacity = 1.0;//#125_insert_CT_Marker for pickup3DPoint
			dest_entry.LColor[0] = 255;
			dest_entry.LColor[1] = 255;
			dest_entry.LColor[2] = 255;
			dest_entry.HColor[0] = 255;
			dest_entry.HColor[1] = 255;
			dest_entry.HColor[2] = 255;
		}

		int bgRGB[3];
		bgRGB[0] = 0;
		bgRGB[1] = 0;
		bgRGB[2] = 0;

		m_renderParam3D->updateVRLUT(m_VolData.get(), entries.get(), bgRGB);
		//
		PxVCoreAPI::PXSMVLight setupLight;
		//get existed light at first
		m_renderParam3D->queryVRLight(setupLight);

		setupLight.HeadLight = 1;
		setupLight.fContrast = 0.25f;
		setupLight.fIntensity = 0.5f;
		m_renderParam3D->updateVRLight(setupLight);
	}
	break;
	case 2:
	{
		int entrySize = 2;
		SmartPtr_PXSMVLutEntryList entries(PxVCoreAPI::CPXSMAPILib::createLutEntryList(entrySize));
		PxVCoreAPI::PXSMVLutEntry* pSetEntries = entries->getList();
		{
			PxVCoreAPI::PXSMVLutEntry& dest_entry = pSetEntries[0];
			dest_entry.CurveID = 3;
			dest_entry.WW = 475;
			dest_entry.WL = 335;
			dest_entry.Opacity = 1.0;
			dest_entry.LColor[0] = 255;
			dest_entry.LColor[1] = 255;
			dest_entry.LColor[2] = 255;
			dest_entry.HColor[0] = 255;
			dest_entry.HColor[1] = 255;
			dest_entry.HColor[2] = 255;
		}
		{
			PxVCoreAPI::PXSMVLutEntry& dest_entry = pSetEntries[1];
			dest_entry.CurveID = 0;
			dest_entry.WW = 656;
			dest_entry.WL = -562;
			dest_entry.Opacity = 0.4;
			dest_entry.LColor[0] = 255;
			dest_entry.LColor[1] = 255;
			dest_entry.LColor[2] = 255;
			dest_entry.HColor[0] = 255;
			dest_entry.HColor[1] = 255;
			dest_entry.HColor[2] = 255;
		}

		int bgRGB[3];
		bgRGB[0] = 0;
		bgRGB[1] = 0;
		bgRGB[2] = 0;

		m_renderParam3D->updateVRLUT(m_VolData.get(), entries.get(), bgRGB);
		//
		PxVCoreAPI::PXSMVLight setupLight;
		//get existed light at first
		m_renderParam3D->queryVRLight(setupLight);

		setupLight.HeadLight = 1;
		setupLight.fContrast = 0.25f;
		setupLight.fIntensity = 0.75f;
		setupLight.fSpecular = 0.009f;
		m_renderParam3D->updateVRLight(setupLight);
	}
	break;
	}
	return;
}
 