
#ifndef __H_PxSMVMatrix_H__
#define __H_PxSMVMatrix_H__

namespace PxVCoreAPI
{
  
	// ref: VLIMatrix 
	////////
	// The array constructors take in column major form, 
	//the same as in OpenGL
	// 0, 4,  8, 12
	// 1, 5,  9, 13
	// 2, 6, 10, 14 
	// 3, 7, 11, 15

template<class T,int row,int col>
class PxMatrixT
{
public:
	PxMatrixT(void){
		 
	}
	PxMatrixT(T*data){

		setData(data, row*col);
	}
	PxMatrixT(const PxMatrixT<T, row, col> &obj){
		int orgSize =  row*col;
		const T *org_p = obj.getData();
		setData(org_p, orgSize);
	}
	virtual ~PxMatrixT(void){
		 
	}
	void clear(void){
		int size = row*col;
		for (int i = 0; i < size; i++){
			m_data[i] = 0;
		}
	}
	void getMatrix(T *matArray) const {
		int size = row*col;
		for (int i = 0; i < size; i++){
			matArray[i] = m_data[i];
		}
	}
	PxMatrixT<T, row, col> & operator = (const PxMatrixT<T, row, col> &obj){
		int orgSize = row*col;
		const T *org_p = obj.getData();
		setData(org_p, orgSize);
		return *this;
	}
	void setData(const T*v, int size){
		if (size > (row*col)) size = row*col;
		for (int i = 0; i < size; i++){
			m_data[i] = v[i];
		}
	}
	T* getData(void) {
		return m_data;
	}
	const T* getData(void) const {

		return m_data;
	}
protected:
	T  m_data[row*col];
	
};
typedef PxMatrixT<float, 4, 4> fMat44;
typedef PxMatrixT<float, 4, 2> fMat42;
typedef PxMatrixT<float, 3, 3> fMat33;
typedef PxMatrixT<float, 1, 3> fMat13;
typedef PxMatrixT<int, 1, 2> iMat12;
////////////
} //PxVCoreAPI

#endif //__H_PxSMVMatrix_H__

 