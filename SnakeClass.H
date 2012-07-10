#ifndef _INC_SNAKESAPP
#define _INC_SNAKESAPP
#include "DIB.h"

//梯度结构
struct Grad
{
	double Ix,Iy,G;
};

struct Node{
	CPoint point;
	Node *next;
};

//Snake曲线类
class SnakeClass
{
public:
	//输入图像宽度
	int lWidth;

	//输入图像高度
	int lHeight;

	//判断是否开始画初始轮廓
	bool DrawContour;

	CPoint *pos;

	//总共采样到绘制的曲线上的点
	int p_NumPos;
	
	//用于间隔采样鼠标绘制的曲线上的点
	int pp;
private:
	HDIB hDIB;	
	Grad *fG;
	LONG lLineBytes;	
	LPSTR lpDIBBits;	
	LPSTR lpDIB;
	LPBITMAPINFO lpbmi;	
	CDC*pDC;
	double *ImageData;
	CPoint p_OldPos;
	Node *p,*top;
public:
	double avg_distance;
	SnakeClass(HDIB m_hDIB);
	~SnakeClass();
	void Gauss();
	void nbrNodeOffset(int &offsetX, int &offsetY, int linkIndex);
	void nbrNodeOffset_Gauss5(int &offsetX, int &offsetY, int linkIndex);
	void LButtonDown(CPoint point,CDC*pDC);
	void LButtonUp(CPoint point,CDC*pDC);
	void MouseDownAndMove(CPoint point,CDC*pDC);
	void Gradient();

	//Snake算法主要处理函数

	//控制Snake曲线能量的参数
	double *alpha,*beta,*gamma;

	//曲率能量
	double *curvature;

	//Snake曲线上点总数
	int snake_point_num;

	//存储Snake曲线上的点
	CPoint *Snake_points;

	//每点梯度值
	double *grad_mag;

	//曲率阈值，小于此阈值则下一点的曲率能量为0
	double threshold_curvature; 

	//梯度阈值，大于此阈值则下一点的曲率能量为0
	int threshold_grad;
	
	void Snake_algorithm();
	double calc_distance(int no, CPoint point);
	CPoint calc_min_energy(int no, CPoint point, double avg_distance);
	double curvature_ene(int no, CPoint point);
	double continuity_ene(int no, CPoint point, double avg_distance);
};

#endif//INC_INFSCISSOR