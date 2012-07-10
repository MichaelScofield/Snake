#ifndef _INC_SNAKESAPP
#define _INC_SNAKESAPP
#include "DIB.h"

//�ݶȽṹ
struct Grad
{
	double Ix,Iy,G;
};

struct Node{
	CPoint point;
	Node *next;
};

//Snake������
class SnakeClass
{
public:
	//����ͼ�����
	int lWidth;

	//����ͼ��߶�
	int lHeight;

	//�ж��Ƿ�ʼ����ʼ����
	bool DrawContour;

	CPoint *pos;

	//�ܹ����������Ƶ������ϵĵ�
	int p_NumPos;
	
	//���ڼ�����������Ƶ������ϵĵ�
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

	//Snake�㷨��Ҫ��������

	//����Snake���������Ĳ���
	double *alpha,*beta,*gamma;

	//��������
	double *curvature;

	//Snake�����ϵ�����
	int snake_point_num;

	//�洢Snake�����ϵĵ�
	CPoint *Snake_points;

	//ÿ���ݶ�ֵ
	double *grad_mag;

	//������ֵ��С�ڴ���ֵ����һ�����������Ϊ0
	double threshold_curvature; 

	//�ݶ���ֵ�����ڴ���ֵ����һ�����������Ϊ0
	int threshold_grad;
	
	void Snake_algorithm();
	double calc_distance(int no, CPoint point);
	CPoint calc_min_energy(int no, CPoint point, double avg_distance);
	double curvature_ene(int no, CPoint point);
	double continuity_ene(int no, CPoint point, double avg_distance);
};

#endif//INC_INFSCISSOR