#include "stdafx.h"
#include "DIB.h"
#include "SnakeClass.H"
#include "math.h"
#include "fstream.h"

SnakeClass::SnakeClass(HDIB m_hDIB)
{
	int n,nBitCount;
	LPSTR lpDIB;
	BYTE *lpSrc;

	hDIB = m_hDIB;

	//锁定DIB，返回DIB内存首地址
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);

	//图像宽度
	lWidth = ::DIBWidth(lpDIB);

	//图像高度
	lHeight = ::DIBHeight(lpDIB);

	//找到DIB像素起始位置
	lpDIBBits = ::FindDIBBits(lpDIB);	

	//获得DIB文件头
	lpbmi = (LPBITMAPINFO)lpDIB;

	//每像素所占位数
	nBitCount = lpbmi->bmiHeader.biBitCount;

	//计算图像每行的字节数
	lLineBytes = (lWidth*nBitCount/8+3)/4*4;

	//开辟新空间存储DIB
	ImageData = new double[lWidth*lHeight];

	for(int i = 0; i<lHeight; i++)
	{
		for(int j = 0; j<lWidth; j++)
		{
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes*(lHeight-1-i)+j*nBitCount/8;
            n=i*lWidth+j;
			if(nBitCount==8)
				ImageData[n]=*lpSrc;
			if(nBitCount==24)
				//24位图中计算亮度值时，RGB三分量权值分别为0.3,0.59,0.11
				ImageData[n]=(*(lpSrc+1)*0.59f+*(lpSrc+2)*0.11f+*(lpSrc+0)*0.3f);	
		}
	}

	DrawContour=false;
	p_NumPos=0;
	pp=0;
	p=NULL;
	top=NULL;
	
	Gauss();
	Gradient();
}

SnakeClass::~SnakeClass()
{
	delete alpha;
	delete beta;
	delete gamma;
	delete curvature;	
}

//高斯滤波
void SnakeClass::Gauss()
{
	double lpSrc[25];	
	int	i,j,x,y,n,x1,y1,k;
	double lp;
	double tt;

	for(i = 2; i<lHeight-2; i++)
	{
		for(j = 2; j<lWidth-2; j++)
		{
			for(k=0;k<25;k++)
			{
				nbrNodeOffset_Gauss5(x,y,k);
				n=(i+y)*lWidth+j+x;
				x1=i+y;
				y1=j+x;
				lpSrc[k]=ImageData[n];
			}
			//Sobel算子
			tt=lpSrc[0]+lpSrc[1]+lpSrc[2]*2+lpSrc[3]+lpSrc[4]+
			   lpSrc[5]+lpSrc[6]*2+lpSrc[7]*4+lpSrc[8]*2+lpSrc[9]+
			   lpSrc[10]*2+lpSrc[11]*4+lpSrc[12]*8+lpSrc[13]*4+lpSrc[14]*2+
			   lpSrc[15]+lpSrc[16]*2+lpSrc[17]*4+lpSrc[18]*2+lpSrc[19]+
			   lpSrc[20]+lpSrc[21]+lpSrc[22]*2+lpSrc[23]+lpSrc[24];

			lp=tt/52;
			if(lp>255)lp=255;
			if(lp<0)lp=0;
			n=j+i*lWidth;
			ImageData[n]=lp;			
		}
	}
}

//求每点梯度和梯度值
void SnakeClass::Gradient()
{
	double lpSrc[9];

	int	i,j,index,k,x,y,n;
	double Ix,Iy;
	double G,MaxG=0;
	fG=new Grad[lHeight*lWidth];//每点梯度
	grad_mag=new double[lHeight*lWidth];//每点梯度值
	Grad Temp;

	Temp.G=0;
	Temp.Iy=0;
	Temp.Ix=0;

	//将每个像素梯度值设置为0
	for(i = 0; i<lHeight; i++)
	{
		for(j = 0; j<lWidth; j++)
		{
			index=j+i*lWidth;
			fG[index].Ix=0;
			fG[index].Iy=0;
			fG[index].G=0;
		}
	}

	for(i = 1; i<lHeight-1; i++)
	{
		for(j = 1; j<lWidth-1; j++)
		{
			for(k=0;k<9;k++)
			{
				nbrNodeOffset(x,y,k);
				n=(i+y)*lWidth+j+x;
				lpSrc[k]=ImageData[n];
			}
			Ix=0;
			Iy=0;
			G=0;

			//Sobel算子
			Iy=-lpSrc[1]-lpSrc[2]*2-lpSrc[3]+lpSrc[5]+lpSrc[6]*2+lpSrc[7];
			Ix=-lpSrc[5]-lpSrc[4]*2-lpSrc[3]+lpSrc[7]+lpSrc[0]*2+lpSrc[1];
			
			G=sqrt(Ix*Ix+Iy*Iy);
			index=j+i*lWidth;
			fG[index].G=G;
			fG[index].Ix=Ix;
			fG[index].Iy=Iy;
			grad_mag[index]=fG[index].G;
			
			fG[j+0*lWidth]=Temp;			
			fG[j+(lHeight-1)*lWidth]=Temp;		
			fG[0+i*lWidth]=Temp;
			fG[lWidth-1+i*lWidth]=Temp;
		
			MaxG=max(MaxG,G);
		}
	}	
}

//用于高斯滤波器的偏移量值
void SnakeClass::nbrNodeOffset_Gauss5(int &offsetX, int &offsetY, int linkIndex)
{
	switch(linkIndex)
	{
	case 0:
		offsetX = -2;
		offsetY = -2;
		break;
	case 1:
		offsetX = -1;
		offsetY = -2;
		break;
	case 2:
		offsetX = 0;
		offsetY = -2;
		break;
	case 3:
		offsetX = 1;
		offsetY = -2;
		break;
	case 4:
		offsetX = 2;
		offsetY = -2;
		break;
	case 5:
		offsetX = -2;
		offsetY = -1;
		break;
	case 6:
		offsetX = -1;
		offsetY = -1;
		break;
	case 7:
		offsetX = 0;
		offsetY = -1;
		break;
	case 8:
		offsetX = 1;
		offsetY = -1;
		break;
	case 9:
		offsetX = 2;
		offsetY = -1;
		break;
	case 10:
		offsetX = -2;
		offsetY = 0;
		break;
	case 11:
		offsetX = -1;
		offsetY = 0;
		break;
	case 12:
		offsetX = 0;
		offsetY = 0;
		break;
	case 13:
		offsetX = 1;
		offsetY = 0;
		break;
	case 14:
		offsetX = 2;
		offsetY = 0;
		break;
	case 15:
		offsetX = -2;
		offsetY = 1;
		break;
	case 16:
		offsetX = -1;
		offsetY = 1;
		break;
	case 17:
		offsetX = 0;
		offsetY = 1;
		break;
	case 18:
		offsetX = 1;
		offsetY = 1;
		break;
	case 19:
		offsetX = 2;
		offsetY = 1;
		break;
	case 20:
		offsetX = -2;
		offsetY = 2;
		break;
	case 21:
		offsetX = -1;
		offsetY = 2;
		break;
	case 22:
		offsetX = 0;
		offsetY = 2;
		break;
	case 23:
		offsetX = 1;
		offsetY = 2;
		break;
	case 24:
		offsetX = 2;
		offsetY = 2;
		break;
	}
}

//用于求梯度函数时的偏移量值
void SnakeClass::nbrNodeOffset(int &offsetX, int &offsetY, int linkIndex)
{
	/*
	 *  321
	 *  4 0
	 *  567
	 */
	switch(linkIndex)
	{
	case 0:
		offsetX = 1;
		offsetY = 0;
		break;
	case 1:
		offsetX = 1;
		offsetY = -1;
		break;
	case 2:
		offsetX = 0;
		offsetY = -1;
		break;
	case 3:
		offsetX = -1;
		offsetY = -1;
		break;
	case 4:
		offsetX = -1;
		offsetY = 0;
		break;
	case 5:
		offsetX = -1;
		offsetY = 1;
		break;
	case 6:
		offsetX = 0;
		offsetY = 1;
		break;
	case 7:
		offsetX =1;
		offsetY = 1;
		break;
	case 8:
		offsetX =0;
		offsetY = 0;
		break;
	}
}

void SnakeClass::MouseDownAndMove(CPoint point,CDC*pDC)
{
	if(DrawContour)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
		pDC->SelectObject(pen);
		pp++;
		if(pp>1)
		{
			pDC->MoveTo(p_OldPos);
			pDC->LineTo(point.x,point.y);
			p_NumPos++;
			p=new(Node);
			p->point=point;
			p->next=top;
			top=p;
			pp=0;
			p_OldPos=point;
		}
		snake_point_num=p_NumPos;
	}
}
void SnakeClass::LButtonDown(CPoint point,CDC*pDC)
{
	DrawContour=true;
}
void SnakeClass::LButtonUp(CPoint point,CDC*pDC)
{
	long num=0;
	pos=new CPoint[p_NumPos];
	while(top!=NULL)
	{
		p=top;
		pos[num]=p->point;		
		num++;
		top=p->next;
		delete(p);
	}
	DrawContour=false;
	
	alpha=new double[snake_point_num];
	beta=new double[snake_point_num];
	gamma=new double[snake_point_num];
	curvature=new double[snake_point_num];
	threshold_curvature = 0.15;
	threshold_grad = 100;
	//初始化α=1、β=1、γ=1.2
	for(int i=0;i<snake_point_num;i++)
	{
		alpha[i]=1.0;
		beta[i]=1.0;
		gamma[i]=1.2;
	}
}

void SnakeClass::Snake_algorithm()
{
	CPoint temp;
	Snake_points=pos;
	double max_curvature;
	int i;
	for(i=0;i<snake_point_num;i++)
		avg_distance+=calc_distance(i, Snake_points[i]);
	avg_distance=avg_distance/(double)snake_point_num;
	max_curvature=-1000000000.0;
	for(i=0;i<snake_point_num;i++)
	{
		temp=calc_min_energy(i,Snake_points[i],avg_distance);
		if(temp!=Snake_points[i]
			&&temp!=Snake_points[(i-1+snake_point_num)%snake_point_num]
			&&temp!=Snake_points[(i+1+snake_point_num)%snake_point_num])
		{
			Snake_points[i]=temp;
		}
		curvature[i]=curvature_ene(i, Snake_points[i]);
		if(max_curvature<curvature[i])
			max_curvature=curvature[i];
	}
	avg_distance=0.0;
	
	//归一化各点曲率能量
	for(i=0;i<snake_point_num;i++)
		curvature[i]=curvature[i]/max_curvature;
	
	for(i=0;i<snake_point_num;i++)
	{
		avg_distance+=calc_distance(i, Snake_points[i]);
		if(curvature[i]>threshold_curvature
			&&curvature[i]>curvature[(i+1)%snake_point_num]
			&&curvature[i]>curvature[(i-1+snake_point_num)%snake_point_num]
			&&grad_mag[i]>threshold_grad)
			*(beta+i)=0;
	}
}

//计算两相邻点距离
double SnakeClass::calc_distance(int no, CPoint point)
{
	point = point - Snake_points[(no-1+snake_point_num)%snake_point_num];
	return(sqrt(point.x*point.x+point.y*point.y));
}

//计算曲率项能量
double SnakeClass::curvature_ene(int no, CPoint point)
{
	point=Snake_points[(no-1+snake_point_num)%snake_point_num]
		-point-point+Snake_points[(no+1)%snake_point_num];
	return(point.x*point.x+point.y*point.y);
}

//计算连续性能量
double SnakeClass::continuity_ene(int no, CPoint point, double avg_distance)
{
	return(pow(avg_distance-calc_distance(no,point),2)); //pow(x,y) 计算x的y次幂
}

//计算点的3×3临域中具有最小能量的点
CPoint SnakeClass::calc_min_energy(int no, CPoint point, double avg_distance)
{
	CPoint p, min_point;
	double max_curvature, max_continuity, max_internal, min_internal, min_energy, energy;
	double curvatures[3][3];
	double continuities[3][3];
	double internal_energies[3][3];
	int i,j;
	max_curvature=max_continuity=max_internal=-1000000000000.0;
	min_internal=1000000000000.0;

	//在3×3模板内操作
	for(i=-1;i<=1;i++)
	{
		p.y=point.y+i;
		if(p.y<0)
			p.y=0;
		if(p.y>=lHeight)//判断是否超出图像的范围
			p.y=lHeight-1;
		for(j=-1;j<=1;j++)
		{
			p.x=point.x+j;
			if(p.x<0)//判断是否超出图像的范围
				p.x=0;
			if(p.x>=lWidth)
				p.x=lWidth-1;
			curvatures[i+1][j+1]=curvature_ene(no, p);
			continuities[i+1][j+1]=continuity_ene(no,p,avg_distance);
			internal_energies[i+1][j+1]=(double)grad_mag[p.x+p.y*lHeight];
			max_curvature=max(max_curvature,curvatures[i+1][j+1]);
			max_continuity=max(max_continuity,continuities[i+1][j+1]);
            max_internal=max(max_internal,internal_energies[i+1][j+1]);
			min_internal=min(min_internal,internal_energies[i+1][j+1]);
		}
	}
	for(i=0;i<=2;i++)
	{
		for(j=0;j<=2;j++)
		{
			curvatures[i][j]=curvatures[i][j]/max_curvature;
			continuities[i][j]=continuities[i][j]/max_continuity;
			internal_energies[i][j]=
				(internal_energies[i][j]-min_internal)/(max_internal-min_internal);
		}
	}
	min_point.x=-1;
	min_point.y=-1;
	min_energy=1000000000000.0;
	for(i=-1;i<=1;i++)
	{
		for(j=-1;j<=1;j++)
		{
			energy = *(alpha+no)*continuities[i+1][j+1] +
				*(beta+no)*curvatures[i+1][j+1] -
				*(gamma+no)*internal_energies[i+1][j+1];
			if(energy<min_energy||(energy==min_energy&&i==0&&j==0))
			{
				min_energy=energy;
				min_point.x=j;
				min_point.y=i;
			}
		}
	}
	min_point.x=min_point.x+point.x;
	min_point.y=min_point.y+point.y;
	return(min_point);
}
