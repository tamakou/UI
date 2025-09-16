
#ifndef __H_PxSMVIEWER_SMART_H__
#define __H_PxSMVIEWER_SMART_H__


#include "PxSMViewerIf.h"


namespace PxVCoreAPI
{
   
struct deleter_for_SMVDllBase{
	void operator()(PxSMVDllBase* ptr_){
		ptr_->destroy();
	}
};
inline void SMVDllBase_Destroy(PxSMVDllBase *pp)
{
	pp->destroy();
}

//PXSMVVolumeData
#define SmartPtr_PXSMVVolumeData std::unique_ptr < PxVCoreAPI::PXSMVVolumeData,PxVCoreAPI::deleter_for_SMVDllBase> 

//PxSMVDataList<PXSMVDICOMInformation>
#define SmartPtr_PxSMVDicomInforList std::unique_ptr < PxVCoreAPI::PxSMVDataList<PxVCoreAPI::PXSMVDICOMInformation>,PxVCoreAPI::deleter_for_SMVDllBase> 
//PxSMVDataList<PXSMVDICOMData>
#define SmartPtr_PxSMVDicomDataList std::unique_ptr < PxVCoreAPI::PxSMVDataList<PxVCoreAPI::PXSMVDICOMData>,PxVCoreAPI::deleter_for_SMVDllBase> 
//PxSMVDataList<PXSMVLutEntry>
#define SmartPtr_PXSMVLutEntryList std::unique_ptr < PxVCoreAPI::PxSMVDataList<PxVCoreAPI::PXSMVLutEntry>,PxVCoreAPI::deleter_for_SMVDllBase> 
#define SmartQueryLutEntryList(param,SmartPtr) { \
	PxVCoreAPI::PxSMVDataList<PxVCoreAPI::PXSMVLutEntry> *listTemp=nullptr;\
	param->queryVRLUT(listTemp);\
	SmartPtr.reset(listTemp); \
	}

//PXSMVRenderParam
#define SmartPtr_PXSMVRenderParam std::unique_ptr < PxVCoreAPI::PXSMVRenderParam,PxVCoreAPI::deleter_for_SMVDllBase> 
 
#define create_UniquePtr(ret_p,type,newCls) { type *p_temp = newCls;ret_p.reset(p_temp);}
  
//PXSMVImage
#define SmartPtr_PXSMVImage std::unique_ptr < PxVCoreAPI::PXSMVImage,PxVCoreAPI::deleter_for_SMVDllBase> 

// ProgressIf 
#define SmartPtr_Progress std::unique_ptr < PxVCoreAPI::ProgressIf,PxVCoreAPI::deleter_for_SMVDllBase> 

//ClientNvrIf 
#define SmartPtr_ClientNvr std::unique_ptr<PxVCoreAPI::ClientNvrIf, PxVCoreAPI::deleter_for_SMVDllBase> 
//ClientMPRModel 
#define SmartPtr_ClientMPRModel std::unique_ptr < PxVCoreAPI::ClientMPRModel,PxVCoreAPI::deleter_for_SMVDllBase> 
//ClientCPRModel 
#define SmartPtr_ClientCPRModel std::unique_ptr < PxVCoreAPI::ClientCPRModel,PxVCoreAPI::deleter_for_SMVDllBase> 

#define SmartPtr_SMVDllBasePointer(T) std::unique_ptr < T,PxVCoreAPI::deleter_for_SMVDllBase> 
 

////////////
} //PxVCoreAPI

#endif //__H_PxSMVIEWER_SMART_H__