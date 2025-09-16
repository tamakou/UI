
#ifndef __H_PxSMVCoreAPI_H__
#define __H_PxSMVCoreAPI_H__
 
#ifdef MakePxSMVAPI 
#define IPxSMVDefDllAPI __declspec(dllexport)
	 
#else 
#define IPxSMVDefDllAPI __declspec(dllimport)
	 
#endif

extern "C"
{

	///////////////////////////
	/// @brief initialize SMVCore 
	/// @param 
	/// @return 0 on success.
	IPxSMVDefDllAPI int PxSmvAPI_Init(void);


	///////////////////////////
	/// @brief setup SMVCore Environment
	/// @param JsonUseWString [in] json string code: 0: char, 1: wchar_t(ushort) wide string. 2: utf-8
	/// @param paramBuffer [in] json format ref: SetEnvParam.json. 
	/// @param paramBufferSize [in] size of paramBuffer.
	/// @return 0 on success.
	IPxSMVDefDllAPI int PxSmvAPI_SetupEnv(int JsonUseWString, const char *paramBuffer, int paramBufferSize);


	///////////////////////////
	/// @brief get SMVCore Environment
	/// @param JsonUseWString [in] json string code: 0: char, 1: wchar_t(ushort) wide string. 2: utf-8
	/// @param paramBuffer [out] json format ref: GetEnvParam.json. 
	/// @param paramBufferSize [in/out] size of paramBuffer.
	/// @return 0 on success.
	IPxSMVDefDllAPI int PxSmvAPI_GetEnv(int JsonUseWString, char *paramBuffer, int *paramBufferSize);


	///////////////////////////
	/// @brief Connect to server
	/// @param paramBuffer [in] json format ref: ConnectionParam.json. 
	/// @param paramBufferSize [in] size of paramBuffer.
	/// @param connectionHandler[out] return the connection handler
	/// @param statusBuffer [out] buffer to receive status. json format ref: status.json. 
	/// @param statusBufferSize [in/out] in: Max. size of statusBuffer,out: received size of status.
	/// @return 0 on success.
	IPxSMVDefDllAPI int PxSmvAPI_ConnectServer(const char *paramBuffer, int paramBufferSize, int *connectionHandler, char *statusBuffer, int *statusBufferSize);


	///////////////////////////
	/// @brief Process the command description in cmdBuffer 
	/// @param connectionHandler[in] connection handler returned by PxSmvAPI_ConnectServer
	/// @param cmdBuffer [in] command description,json format ref: cmd_xxx.json. 
	/// @param paramBufferSize [in] size of cmdBuffer.
	/// @param dataBuffer [out] buffer to receive result. json format ref: cmd_return_xxx.json. 
	/// @param dataBufferSize [in/out] in: Max. size of dataBuffer,out: received size of result.
	/// @param rawDataBuffer [out] buffer to receive raw data (for example JPEG) 
	/// @param rawDataBufferSize [in/out] in: Max. size of rawDataBuffer,out: received size of raw data.
	/// @param statusBuffer [out] buffer to receive status. json format ref: status.json. 
	/// @param statusBufferSize [in/out] in: Max. size of statusBuffer,out: received size of status.
	/// @return 0: on success, <0 : error , >0: check the status, on progress.
	IPxSMVDefDllAPI int PxSmvAPI_Process(int connectionHandler, const char *cmdBuffer, int cmdBufferSize,
		char *dataBuffer, int *dataBufferSize,
		char *rawDataBuffer, int *rawDataBufferSize,
		char *statusBuffer, int *statusBufferSize);


	///////////////////////////
	/// @brief Get the status of SMVCore.
	/// @param connectionHandler[in] connection handler returned by PxSmvAPI_ConnectServer
	/// @param cmdBuffer [in] command description,json format ref: cmd_xxx.json. 
	/// @param paramBufferSize [in] size of cmdBuffer.
	/// @param statusBuffer [out] buffer to receive status. json format ref: status.json. 
	/// @param statusBufferSize [in/out] in: Max. size of statusBuffer,out: received size of status.
	/// @return 0: on success, <0 : error , >0: on progress status.
	IPxSMVDefDllAPI int PxSmvAPI_GetStatus(int connectionHandler, const char *cmdBuffer, int cmdBufferSize, char *statusBuffer, int *statusBufferSize);


	///////////////////////////
	/// @brief Disonnect from server
	/// @param connectionHandler[in] connection handler returned by PxSmvAPI_ConnectServer 
	/// @param paramBuffer [in] json format ref: DisconnectionParam.json. 
	/// @param paramBufferSize [in] size of paramBuffer.
	/// @param statusBuffer [out] buffer to receive status. json format ref: status.json. 
	/// @param statusBufferSize [in/out] in: Max. size of statusBuffer,out: received size of status.
	/// @return 0 on success.

	IPxSMVDefDllAPI int PxSmvAPI_Disconnect(int connectionHandler, const char *paramBuffer, int paramBufferSize, char *statusBuffer, int *statusBufferSize);

}
#endif //__H_PxSMVCoreAPI_H__
 

 