// fong3d.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Lab.h"
#include <cmath>

#define MAX_LOADSTRING 100



#define ID_ROTATE_X     9004
#define ID_ROTATE_Y     9005
#define ID_MOVE_X_LEFT  9006
#define ID_MOVE_X_RIGHT 9007
#define ID_MOVE_Y_UP    9008
#define ID_MOVE_Y_DOWN  9009
#define ID_REFLECT_X    9010
#define ID_REFLECT_Y    9011
#define ID_RESIZE       9012
#define ID_RESET        9013
#define IDC_MAIN_BUTTON 9014

#define RUSSIAN_MAGIC_OFFSET 1104

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LAB));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_LAB);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}






// переменные для буффера
BITMAPINFO bmi;
DWORD * pixels = NULL;


// определяем типы данных для векторов и вершин
struct vector3d { double x, y, z; };
typedef struct vector3d VECTOR3D;
struct vertex3d { VECTOR3D point; VECTOR3D normal; };
typedef struct vertex3d VERTEX3D;
struct vector2d { int x, y; };
typedef struct vector2d VECTOR2D;
struct vertex2d { VECTOR2D point; VECTOR3D normal; };
typedef struct vertex2d VERTEX2D;

#define points_count  8	// точек
#define faces  6		// граней
#define fvertex  4		// вершин грани

// матрица точек для инициализации точек фигуры
double cube_template[points_count][3] = {
	{ -20, -25, -25 }, { 10, -25, -25 }, { 10, 25, -25 }, { -20, 25, -25 },
	{ -25, -25, 25 }, { 25, -25, 25 }, { 25, 25, 25 }, { -25, 25, 25 }
};

//int colors[faces][3] = { { 41, 247, 42 }, { 60, 188, 128 }, { 96, 23, 14 }, { 42, 84, 159 }, { 207, 164, 159 }, { 254, 248, 169 } };

double cubeMultiplier = 2;

VERTEX3D points3d[points_count]; // точки куба 3D
VERTEX2D points2d[points_count]; // точки куба 2D

int center[2]; // координаты центра экрана

// используются при проецировании
double ctn = cos(atan(2.0)) / 2, stn = sin(atan(2.0)) / 2;

// нумеруем точки составляющие грань из массивов cube_template, points3d и points2d
int grani[faces][fvertex] = { { 0, 4, 5, 1 }, { 0, 1, 2, 3 }, { 0, 3, 7, 4 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 2, 6, 7, 3 } };
// цвета граней
int colors[faces][3] = {
	{ 0, 100, 255 }, { 100, 255, 0 }, { 255, 100, 0 },
	{ 0, 255, 255 }, { 255, 0, 255 }, { 255, 255, 0 }
};

// массив для временного хранения точек ребер с освещенностью
VERTEX2D plist[4096];
int count = 0;

RECT rect;
int a = 0;

// вращение
void RotateY(double alpha, int rotateX, int rotateY)
{
	double x, sinA = sin(alpha), cosA = cos(alpha);
	//
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x -= rotateX;
		points3d[i].point.y -= rotateY;
		//
		x = points3d[i].point.x * cosA - points3d[i].point.z * sinA;
		points3d[i].point.z = points3d[i].point.x * sinA + points3d[i].point.z * cosA;
		points3d[i].point.x = x;
		//
		points3d[i].point.x += rotateX;
		points3d[i].point.y += rotateY;
		//
		x = points3d[i].normal.x * cosA - points3d[i].normal.z * sinA;
		points3d[i].normal.z = points3d[i].normal.x * sinA + points3d[i].normal.z * cosA;
		points3d[i].normal.x = x;
	}
}

void RotateX(double alpha, int rotateX, int rotateY)
{
	double y, sinA = sin(alpha), cosA = cos(alpha);
	//
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x -= rotateX;
		points3d[i].point.y -= rotateY;
		//
		y = points3d[i].point.y * cosA + points3d[i].point.z * sinA;
		points3d[i].point.z = points3d[i].point.z * cosA - points3d[i].point.y * sinA;
		points3d[i].point.y = y;
		//
		points3d[i].point.x += rotateX;
		points3d[i].point.y += rotateY;
		//
		y = points3d[i].normal.y * cosA + points3d[i].normal.z * sinA;
		points3d[i].normal.z = points3d[i].normal.z * cosA - points3d[i].normal.y * sinA;
		points3d[i].normal.y = y;
	}
}

void Resize(double n)
{
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x -= center[0];
		points3d[i].point.y -= center[1];
		
		points3d[i].point.x *= n;
		points3d[i].point.y *= n;
		points3d[i].point.z *= n;
		
		points3d[i].point.x += center[0];
		points3d[i].point.y += center[1];
	}
}

void ReflectX()
{
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x -= center[0];
		points3d[i].point.x *= -1;
		points3d[i].point.x += center[0];
	}
}

void ReflectY()
{
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.y -= center[1];
		points3d[i].point.y *= -1;
		points3d[i].point.y += center[1];
	}
}
void swap(int *x, int *y)
{
	*x += *y;
	*y = *x - *y;
	*x -= *y;
}
void swap(double *x, double *y)
{
	*x += *y;
	*y = *x - *y;
	*x -= *y;
}
void swap(VECTOR3D *x, VECTOR3D *y) {
	VECTOR3D tmp;
	memcpy(&tmp, x, sizeof(VECTOR3D));
	memcpy(x, y, sizeof(VECTOR3D));
	memcpy(y, &tmp, sizeof(VECTOR3D));
}

void Reset()
{
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x = cube_template[i][0] * cubeMultiplier;
		points3d[i].point.y = cube_template[i][1] * cubeMultiplier;
		points3d[i].point.z = cube_template[i][2] * cubeMultiplier;
		
		//
		points3d[i].point.x += center[0];
		points3d[i].point.y += center[1];
	}
}

void ReflectXY()
{
	//TODO: & or not?
	for (int i = 0; i < points_count; i++)
	{
		//swap(points3d[i].point.x, points3d[i].point.y);

		double temp = points3d[i].point.x;
		points3d[i].point.x = points3d[i].point.y;
		points3d[i].point.y = temp;

	}
}

void MoveX(double offset)
{
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x += offset;
	}
}

void MoveY(double offset)
{
	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.y -= offset;
	}
}

// отсечение невидимых граней
int IsVisible(int num) // roberts
{
	int j;
	double sum = 0;

	for (int i = 0; i < fvertex; i++)
	{
		j = i == fvertex - 1 ? 0 : i + 1;
		sum += (points2d[grani[num][i]].point.x - points2d[grani[num][j]].point.x)*(points2d[grani[num][i]].point.y + points2d[grani[num][j]].point.y);
	}

	return (sum > 0) ? 1 : 0;
}


// косоугольная проекция
void project()
{
	for (int i = 0; i < points_count; i++)
	{
		//		x' = x + z*(1/2)*cos(atan(2)); y' = y + z*(1/2)*sin(atan(2));
		points2d[i].point.x = (long)floor(points3d[i].point.x - points3d[i].point.z * ctn);
		points2d[i].point.y = (long)floor(points3d[i].point.y - points3d[i].point.z * stn);
		points2d[i].normal = points3d[i].normal;
	}
}



//нормализация векторов ------------------------------------------------------
void normalize(VECTOR3D * v)
{
	double vec_length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	if (vec_length != 0) v->x /= vec_length, v->y /= vec_length, v->z /= vec_length; else v->x = 0, v->y = 0, v->z = 0;
}

VECTOR3D normalize_tmp(VECTOR3D * v)
{
	VECTOR3D tmp;

	tmp.x = v->x;
	tmp.y = v->y;
	tmp.z = v->z;

	double vec_length = sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);

	if (vec_length != 0)
	{
		tmp.x /= vec_length;
		tmp.y /= vec_length;
		tmp.z /= vec_length;
	}
	else
	{
		tmp.x = 0;
		tmp.y = 0;
		tmp.z = 0;
	}

	return tmp;
}
//----------------------------------------------------------------------------
double vector_size(VECTOR3D * v)
{
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
double vector_size(VECTOR2D * v)
{
	return sqrt(double(v->x * v->x + v->y * v->y));
}

// ***
// **
// * вычисление интенсивности освещения в вершине p_num грани gr_num
double amp = 0.5; // яркость источника (0-1)
double ambient = 0.5;//рассеянный свет (0-1)
double light[3] = { -0.5, -0.5, -0.5 }; // координаты источника света (x,y,z : 0-1)

double light_intense(VECTOR3D normal)
{
	int n = 3;

	//т.к. вектора нормализованы, то сумму делить не нужно
	double cos_fi = light[0] * normal.x + light[1] * normal.y + light[2] * normal.z;

	double val = ambient + amp * cos_fi;

	return val < 0 ? 0 : min(val, 1);
}


int comp(const void *A, const void *B)
{
	return  (*(VERTEX2D*)A).point.y >(*(VERTEX2D*)B).point.y ? 1 : ((*(VERTEX2D*)A).point.y == (*(VERTEX2D*)B).point.y) ? 0 : -1;
}

// создает массив точек линии строко с отличающейся координатой Y
void make_line(VERTEX2D p1, VERTEX2D p2)
{
	int dx = abs(p2.point.x - p1.point.x), dy = abs(p2.point.y - p1.point.y);
	VECTOR3D tmp_norm, n1 = p1.normal, n2 = p2.normal;
	int sx = p1.point.x < p2.point.x ? 1 : -1, sy = p1.point.y < p2.point.y ? 1 : -1;

	double t;

	double len = sqrt(double(dx*dx + dy*dy));
	
	double dxc, dyc;

	int x = p1.point.x;
	int y = p1.point.y;
	int prev = -1;

	int error = dx - dy, err;

	for (;;) {
		// Вычисляем нормаль в точке линии
		err = error << 1;

		if (y != prev)
		{
			dxc = p1.point.x - x, dyc = p1.point.y - y;
			t = sqrt(dxc*dxc + dyc*dyc) / len;

			tmp_norm.x = (1 - t)*n1.x + t*n2.x;
			tmp_norm.y = (1 - t)*n1.y + t*n2.y;
			tmp_norm.z = (1 - t)*n1.z + t*n2.z;

			plist[count].point.x = x;
			plist[count].point.y = y;
			plist[count].normal = tmp_norm;

			count++;
		}

		prev = y;
		if (x == p2.point.x && y == p2.point.y)
		{
			break;
		}

		if (err > -dy)
		{
			error -= dy;
			x += sx;
		}
		if (err < dx)
		{
			dxc = p1.point.x - x;
			dyc = p1.point.y - y;

			t = sqrt(dxc*dxc + dyc*dyc) / len;

			tmp_norm.x = (1 - t)*n1.x + t*n2.x;
			tmp_norm.y = (1 - t)*n1.y + t*n2.y;
			tmp_norm.z = (1 - t)*n1.z + t*n2.z;

			plist[count].point.x = x;
			plist[count].point.y = y;
			plist[count].normal = tmp_norm;

			count++;

			error += dx, y += sy;
		}
	}
}

void SetPoint(DWORD* pixels, int x, int y, COLORREF color)
{
	if (x < 0 || y < 0 || x > rect.right - 1 || y > rect.bottom - 1)
	{
		return;
	}

	pixels[x + y * rect.right] = GetRValue(color) << 16 | (WORD)GetGValue(color) << 8 | GetBValue(color);
}

void line(DWORD* pixels, int x1, int y1, int x2, int y2, COLORREF color)
{
	int deltaX = abs(x2 - x1); int deltaY = abs(y2 - y1);
	int signX = x1 < x2 ? 1 : -1; int signY = y1 < y2 ? 1 : -1;
	int error = deltaX - deltaY;

	for (;;)
	{
		SetPoint(pixels, x1, y1, color);

		if (x1 == x2 && y1 == y2)
		{
			break;
		}

		int error2 = error * 2;

		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}
		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
}

void circle(DWORD* pixels, int x0, int y0, int radius, COLORREF color)
{
	int x = 0;
	int y = radius;
	int delta = 2 - 2 * radius;
	int error = 0;

	while (y >= 0)
	{
		SetPoint(pixels, x0 + x, y0 + y, color);
		SetPoint(pixels, x0 + x, y0 - y, color);
		SetPoint(pixels, x0 - x, y0 + y, color);
		SetPoint(pixels, x0 - x, y0 - y, color);

		error = 2 * (delta + y) - 1;

		if (delta < 0 && error <= 0)
		{
			++x;
			delta += 2 * x + 1;
			
			continue;
		}

		error = 2 * (delta - x) - 1;

		if (delta > 0 && error > 0)
		{
			--y;
			delta += 1 - 2 * y;
			continue;
		}

		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void FillRect(DWORD* pixels, int x1, int y1, int x2, int y2, COLORREF color)
{
	int dx = abs(x2 - x1) + 1;
	int dy = abs(y2 - y1) + 1;
	int i;

	if (min(x1, x2) + dx >= rect.right)
	{
		dx = rect.right - min(x1, x2);
	}

	if (min(y1, y2) + dy >= rect.bottom)
	{
		dy = rect.bottom - min(y1, y2);
	}

	color = GetRValue(color) << 16 | (WORD)GetGValue(color) << 8 | GetBValue(color);

	DWORD * line = new DWORD[dx];

	for (i = 0; i < dx; i++)
	{
		line[i] = color;
	}

	for (i = 0; i < dy; i++)
	{
		memcpy(&pixels[min(x1, x2) + (min(y1, y2) + i)*rect.right], line, sizeof(DWORD)*dx);
	}

	delete line;
}

/*закрашивание полигона методом Гуро. Интенсивность считается только в вершинах, в остальных апроксимируется
points	- координаты вершин грани
light	- интенсивности отраженного света в вершинах
count	- количество точек
gr_num	- номер грани, для закрашивания цветом из массива цветов
*/
void fong_fill(PDWORD pixels, int gr_num, int pcount)
{
	// будем использовать массив пикселей для построения грани
	int i, next, x, y, x1, x2, dx;
	double I, dnx, dny, dnz;

	count = 0;

	// создаем растровый массив точек контура с учетом освещенности
	for (i = 0; i < pcount; i++)
	{
		// добавление в массив точек очередного ребра полигона
		next = i != pcount - 1 ? i + 1 : 0;
		make_line(points2d[grani[gr_num][i]], points2d[grani[gr_num][next]]);
	}

	// сортируем точки по Y
	qsort(plist, count, sizeof(VERTEX2D), comp);

	// закрашиваем грань
	for (i = 0; i < count - 1; i++)
	{
		y = plist[i].point.y;
		if (y != plist[i + 1].point.y) continue;
		x1 = plist[i].point.x, x2 = plist[i + 1].point.x;
		VECTOR3D n1 = plist[i].normal, n2 = plist[i + 1].normal, tmp_norm;
		//рисуем горизонтальную линию
		if (x1 > x2) { swap(&x1, &x2); swap(&n1, &n2); }
		dx = x2 - x1;
		dnx = (n2.x - n1.x) / dx;
		dny = (n2.y - n1.y) / dx;
		dnz = (n2.z - n1.z) / dx;
		tmp_norm = n1;
		for (x = x1; x <= x2; x++)
		{
			I = light_intense(tmp_norm);
			tmp_norm.x += dnx;
			tmp_norm.y += dny;
			tmp_norm.z += dnz;
			
			SetPoint(pixels,x,y,RGB(colors[gr_num][0]*(I),colors[gr_num][1]*(I),colors[gr_num][2]*(I)));
			//SetPoint(pixels, x, y, RGB(0 * (I), 255 * (I), 0 * (I)));
			//SetPoint(pixels, x, y, RGB(colors[gr_num][0] * (I), colors[gr_num][1] * (I), colors[gr_num][2](I)));
		}
	}
}


void ShowCube(PDWORD pixels, int width, int height)
{
	// закрашиваем каждую грань
	for (int i = 0; i < faces; i++)
	{
		if (IsVisible(i))
		{
			fong_fill(pixels, i, fvertex); // только для видимых граней
		}
	}

	// отобразим нормали (для проверки)
	/*for(int i = 0; i < points_count; i++)
	{
		VECTOR3D normal = normalize_tmp(&points3d[i].normal);
		VECTOR3D norm_p;
		norm_p.x = points3d[i].point.x + 100 * normal.x;
		norm_p.y = points3d[i].point.y + 100 * normal.y;
		norm_p.z = points3d[i].point.z + 100 * normal.z;
		line(pixels,points3d[i].point.x - points3d[i].point.z*ctn,points3d[i].point.y - points3d[i].point.z*stn, norm_p.x - norm_p.z*ctn,norm_p.y - norm_p.z*stn, RGB(0,255,0));
	}*/
	
}

VECTOR3D dec_vect(VECTOR3D t1, VECTOR3D t2)
{
	VECTOR3D summ;

	summ.x = t1.x - t2.x;
	summ.y = t1.y - t2.y;
	summ.z = t1.z - t2.z;

	return summ;
}

enum { ID_LABEL = 1, ID_IMAGE, ID_EDIT, ID_LIST, ID_BUTTON=228, ID_COMBO, ID_BUTTON2 };

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 380 * 2, 380 * 2, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	// подготовка координат
	GetClientRect(hWnd, &rect);

	center[0] = rect.right / 2;
	center[1] = rect.bottom / 2;

	for (int i = 0; i < points_count; i++)
	{
		points3d[i].point.x = cube_template[i][0] *cubeMultiplier;
		points3d[i].point.y = cube_template[i][1] *cubeMultiplier;
		points3d[i].point.z = cube_template[i][2] *cubeMultiplier;

		points3d[i].point.x += center[0];
		points3d[i].point.y += center[1];
	}

	//вычислим нормали во всех вершинах
	VECTOR3D v1, v2;
	for (int j = 0; j < points_count; j++)
	{
		ZeroMemory(&points3d[j].normal, sizeof VECTOR3D);

		for (int i = 0; i < faces; i++)
		{
			for (int k = 0; k < fvertex; k++)
				if (grani[i][k] == j)
				{
					v1 = dec_vect(points3d[grani[i][0]].point, points3d[grani[i][1]].point);
					v2 = dec_vect(points3d[grani[i][2]].point, points3d[grani[i][1]].point);

					points3d[j].normal.x += v1.y * v2.z - v1.z * v2.y;
					points3d[j].normal.y += v1.z * v2.x - v1.x * v2.z;
					points3d[j].normal.z += v1.x * v2.y - v1.y * v2.x;
				}
		}
		normalize(&points3d[j].normal); // сразу нормализуем
	}


	HWND hWndButton = CreateWindowEx(NULL,
		L"BUTTON",
		L"Reset",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10,
		10,
		100,
		24,
		hWnd,
		(HMENU)IDC_MAIN_BUTTON,
		GetModuleHandle(NULL),
		NULL);

	HMENU hMenu = CreateMenu();

	// Rotate
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, ID_ROTATE_X, L"RotateX");
	AppendMenu(hSubMenu, MF_STRING, ID_ROTATE_Y, L"RotateY");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"Rotate");

	
	// Move
	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, ID_MOVE_X_LEFT, L"← MoveX");
	AppendMenu(hSubMenu, MF_STRING, ID_MOVE_X_RIGHT, L"MoveX →");
	AppendMenu(hSubMenu, MF_STRING, ID_MOVE_Y_UP, L"MoveY ↑");
	AppendMenu(hSubMenu, MF_STRING, ID_MOVE_Y_DOWN, L"MoveY ↓");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"Move");

	// Move
	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, ID_REFLECT_X, L"ReflectX");
	AppendMenu(hSubMenu, MF_STRING, ID_REFLECT_Y, L"ReflectY");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"Reflect");

	SetMenu(hWnd, hMenu);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, NULL, 10, NULL);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
WPARAM key = 0;

int clickX = 0;
int clickY = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	double alpha = 0.025;
	double offset = 10;
	
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		
		case ID_MOVE_X_LEFT:
			MoveX(-offset);
			break;
		case ID_MOVE_X_RIGHT:
			MoveX(offset);
			break;

		case IDC_MAIN_BUTTON:
			Reset();
			SetFocus(hWnd);
			break;

		case ID_MOVE_Y_UP:
			MoveY(offset);
			break;
		case ID_MOVE_Y_DOWN:
			MoveY(-offset);
			break;
		case ID_ROTATE_X:
			if (clickY && clickX)
			{
				RotateX(alpha, clickX, clickY);
			}
			else
			{
				RotateX(alpha, center[0], center[1]);
			}

			break;
		case ID_ROTATE_Y:
			if (clickY && clickX)
			{
				RotateY(alpha, clickX, clickY);
			}
			else
			{
				RotateY(alpha, center[0], center[1]);
			}

			break;

		case ID_REFLECT_X:
			ReflectX();
			break;
		case ID_REFLECT_Y:
			ReflectY();
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	
	case WM_CREATE:
	{
		// создание буфера для построения изображения
		// ----------------------------------------------------------------------
		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = rect.right;
		bmi.bmiHeader.biHeight = -rect.bottom;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32; /* Request a 32bpp bitmap. */
		bmi.bmiHeader.biCompression = BI_RGB;
		pixels = new DWORD[rect.right * rect.bottom];
		
		break;
	}
	case WM_SIZE:
		if (pixels)
		{
			delete pixels;
		}

		for (int i = 0; i < 8; i++)
		{
			points3d[i].point.x -= center[0];
			points3d[i].point.y -= center[1];
		}

		GetClientRect(hWnd, &rect);

		center[0] = rect.right / 2;
		center[1] = rect.bottom / 2;

		for (int i = 0; i < 8; i++)
		{
			points3d[i].point.x += center[0];
			points3d[i].point.y += center[1];
		}

		pixels = new DWORD[rect.right * rect.bottom];

		bmi.bmiHeader.biWidth = rect.right;
		bmi.bmiHeader.biHeight = -rect.bottom;

		break;
	case WM_LBUTTONDOWN:
	{
		//KillTimer(hWnd, NULL);
		clickX = LOWORD(lParam);
		clickY = HIWORD(lParam);

		while (FALSE) {}
		break;
	}
	case WM_RBUTTONDOWN:
	{		
		clickY = clickX = 0;
		break;
	}
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			MoveX(-offset);
			break;
		case VK_RIGHT:
			MoveX(offset);
			break;
		case VK_UP:
			MoveY(offset);
			break;
		case VK_DOWN:
			MoveY(-offset);
			break;
		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		// Rotate
		case 'W':
		case 'w':
		case 'ц'+RUSSIAN_MAGIC_OFFSET:
		case 'Ц' + RUSSIAN_MAGIC_OFFSET:
			if (clickY && clickX)
			{
				RotateX(alpha, clickX, clickY);
			}
			else
			{
				RotateX(alpha, center[0], center[1]);
			}
			break;
		case 'A':
		case 'a':
		case 'Ф' + RUSSIAN_MAGIC_OFFSET:
		case 'ф' + RUSSIAN_MAGIC_OFFSET:
			if (clickY && clickX)
			{
				RotateY(-alpha, clickX, clickY);
			}
			else
			{
				RotateY(-alpha, center[0], center[1]);
			}
			break;
		case 'S':
		case 's':
		case 'Ы' + RUSSIAN_MAGIC_OFFSET:
		case 'ы' + RUSSIAN_MAGIC_OFFSET:
			if (clickY && clickX)
			{
				RotateX(-alpha, clickX, clickY);
			}
			else
			{
				RotateX(-alpha, center[0], center[1]);
			}
			break;
		case 'D':
		case 'd':
		case 'В' + RUSSIAN_MAGIC_OFFSET:
		case 'в' + RUSSIAN_MAGIC_OFFSET:
			if (clickY && clickX)
			{
				RotateY(alpha, clickX, clickY);
			}
			else
			{
				RotateY(alpha, center[0], center[1]);
			}
			break;

		// Reflection
		case 'X':
		case 'x':
		case 'Ч' + RUSSIAN_MAGIC_OFFSET:
		case 'ч' + RUSSIAN_MAGIC_OFFSET:
			ReflectX();
			break;
		case 'Z':
		case 'z':
		case 'Я' + RUSSIAN_MAGIC_OFFSET:
		case 'я' + RUSSIAN_MAGIC_OFFSET:
			ReflectY();
			break;
		case 'C':
		case 'c':
		case 'С' + RUSSIAN_MAGIC_OFFSET:
		case 'с' + RUSSIAN_MAGIC_OFFSET:
			ReflectXY();
			break;

		// Resizing
		case '+':
		case '=':
			Resize(1.5);
			break;
		case '-':
			Resize(0.5);
			break;

		// Resetting
		case 'R':
		case 'r':
		case 'К' + RUSSIAN_MAGIC_OFFSET:
		case 'к' + RUSSIAN_MAGIC_OFFSET:
			Reset();
			break;
		default:
			break;
		}
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		if (!pixels) break;
		// ----------------------------------------------------------------------

		// Здесь рисуем в массив pixels
		FillRect(pixels, 0, 0, rect.right, rect.bottom, RGB(0, 0, 40));

		project(); // проецирование
		ShowCube(pixels, rect.right, rect.bottom); // отображение

		//for(int i = 0; i < 20; i++) circle(pixels,center[0],center[1],300+i,RGB(200-i*5,0,0));

		// ----------------------------------------------------------------------
		// копируем массив пикселей в hdc
		SetDIBitsToDevice(hdc, 0, 0, rect.right, rect.bottom, 0, 0, 0, rect.bottom, pixels, &bmi, DIB_RGB_COLORS);
		// ----------------------------------------------------------------------
		EndPaint(hWnd, &ps);


		break;
	}
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		break;
	
	case WM_ERASEBKGND:
		break;	

	case WM_DESTROY:
		if (pixels)
		{
			delete pixels;
		}

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
