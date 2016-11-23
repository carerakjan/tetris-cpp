/*
Випадкові фігурки тетраміно падають зверху в прямокутний стакан (матрицю) визначеної ширини і висоти 
(наприклад, 10х20 клітинок).

Під час польоту гравець може переміщувати фігуру вліво, вправо по горизонталі, а також вниз, прискорюючи падіння 
(за допомогою клавіш управління курсором). На 12 балів реалізувати також поворот фігури. 
Кожна фігурка летить зверху, поки не наштовхнеться на іншу фігурку або не впаде на дно матриці. Якщо при цьому заповниться 
горизонтальний ряд, тоді він пропадає і все, що було його вище, опускається на 1 клітинку. Гра закінчується, коли нова 
фігурка не може поміститись в матрицю. 
Гравець отримує очки за кожну фігурку, тому його задача — заповнювати ряди, не заповнюючи саму матрицю (стакан) як можна 
довше, щоб таким чином отримати як можна більше очків. Сам підрахунок очків в різних версіях «Тетріса» різна. Очки можуть 
нараховуватись за прибрані лінії, за скинуті фігурки, за перехід на нову швидкість і т.д.

Орієнтовна структура класів:
----------------------------
	1) Point, який описує окрему точку матриці 
	2) Matrix (матриця), яка має певний розмір, а також управляє фігурами, підраховує очки, перемальовує саме поле 
	   тетріса тощо. Він також може містити орієнтовно наступні методи: 
			- Конструктор, який по бажанню може містити темп гри, ініціалізує очки нулем тощо. 
			- Draw або Fill, який дозволяє розташовувати фігуру на полі матриці. При першому виклику цього метода, 
			  буде відображатись весь тетріс. 
			- Метод генерування фігури. 
			- Move, який дозволяє переміщувати фігуру вліво/вправо. 
			- Rotate - метод повороту фігури. 
			- Метод падіння фігури 
			- Метод перевірки на заповнення горизонтального ряду в тих місцях, де розміщується щойно впавша фігура. 
	3) Shape (фігура), який дозволяє згенерувати рандомну фігуру. Можна також створити окремі класи для різних типів фігур. 

Передбачити також можливість збереження гри в файл та завантаження її з файла. 
*/

#include <iostream>
#include <fstream>
#include <conio.h>
#include <Windows.h>
#include <process.h>
#include <ctime>
using namespace std;


enum ConsoleColor
{
                Black         = 0,
                Blue          = 1,
                Green         = 2,
                Cyan          = 3,
                Red           = 4,
                Magenta       = 5,
                Brown         = 6,
                LightGray     = 7,
                DarkGray      = 8,
                LightBlue     = 9,
                LightGreen    = 10,
                LightCyan     = 11,
                LightRed      = 12,
                LightMagenta  = 13,
                Yellow        = 14,
                White         = 15
};

const char BACKGROUND = 176;
const char SHAPE = 254;

void SetColor(int text, int background)
{
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}
 
void SetColor(int text, ConsoleColor background)
{
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

class Point
{
protected:
	unsigned x;
	unsigned y;
public:
	Point(): x(0), y(0) {}
	Point(unsigned x, unsigned y): x(x), y(y) {}
	void set_x(unsigned x) { this->x = x; }
	void set_y(unsigned y) { this->y = y; }
	unsigned get_x() { return this->x; }
	unsigned get_y() { return this->y; }
};
//////////////////////////////////////////////////////////////////////////////////////
class Matrix: public Point
{
	//T - тетрис с рамочкой 12x22
	//F - рабочее поле 10x20
	//S - фигура
protected:
	unsigned row;
	unsigned col;
	unsigned score;
	char ** M;
public:

//------------------------------- конструкторы ------------------------------------------

	Matrix(): row(1), col(1), score(0), Point()
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				M[i][j] = BACKGROUND;
			}
		}
	}
	Matrix(const unsigned r, const unsigned c): row(r), col(c), score(0), Point()
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				M[i][j] = BACKGROUND;
			}
		}
	}
	Matrix(const Matrix & mtrx): row(mtrx.row), col(mtrx.col)
	{
		cout << "Copy constr!\n";
		M = new char * [this->row];
		
		for (int i=0; i<this->row; i++)
		{
			M[i] = new char [this->col];
			for (int j=0; j<this->col; j++)
				this->M[i][j] = mtrx.M[i][j];
		}		
	}
	/*~Matrix()
	{
		cout << "Destruct!\n";
		for (int i=0; i<this->col; i++)
			delete M[i];
		delete [] M;
	}*/

//------------------------------- проверка соприкосновения фигуры с препятствием ------------------------------------------

	bool check_move_down(Matrix * mtrx)
	{
		bool flag = false;

		int r = mtrx->get_row();
		int c = mtrx->get_col();
		//-------------------------- вниз ------------------------------			
		for (int j=mtrx->y, m=0; j<mtrx->y+c, m<c; j++, m++)
		{
			flag = false;
			if (r == 4 && c == 1 || r == 1 && c == 4 || r == 2 && c == 2)
			{
				if (mtrx->M[r-1][m] == SHAPE && this->M[mtrx->x+r][j] == SHAPE)
				{
					flag = true;
					break;
				}
			}
			if (r == 3 && c == 2)
			{
				if (mtrx->M[r-1][m] == SHAPE && this->M[mtrx->x+r][j] == SHAPE ||
					mtrx->M[r-2][m] == SHAPE && mtrx->M[r-1][m] == BACKGROUND && this->M[mtrx->x+r-1][j] == SHAPE ||
					mtrx->M[r-3][m] == SHAPE && mtrx->M[r-2][m] == BACKGROUND && mtrx->M[r-1][m] == BACKGROUND && this->M[mtrx->x+r-2][j] == SHAPE)
				{
					flag = true;
					break;
				}
			}
			if (r ==2 && c== 3)
			{
				if (mtrx->M[r-1][m] == SHAPE && this->M[mtrx->x+r][j] == SHAPE ||
					mtrx->M[r-2][m] == SHAPE && mtrx->M[r-1][m] == BACKGROUND && this->M[mtrx->x+r-1][j] == SHAPE)
				{
					flag = true;
					break;
				}
			}
		}
		return flag;
	}

	bool check_move_right(Matrix * mtrx)
	{
		bool flag = false;

		int r = mtrx->get_row();
		int c = mtrx->get_col();
		
		//-------------------------- вправо ------------------------------
		for (int i=mtrx->x, n=0; i<mtrx->x+r, n<r; i++, n++)
		{
			flag = false;
			if (r == 4 && c == 1 || r == 1 && c == 4 || r == 2 && c == 2)
			{
				if (mtrx->M[n][c-1] == SHAPE && this->M[i][mtrx->y+c] == SHAPE)
				{
					flag = true;
					break;
				}
			}
			
			if (r == 3 && c == 2)
			{
				if (mtrx->M[n][c-1] == SHAPE && this->M[i][mtrx->y+c] == SHAPE ||
					mtrx->M[n][c-2] == SHAPE && mtrx->M[n][c-1] == BACKGROUND && this->M[i][mtrx->y+c-1] == SHAPE)
				{
					flag = true;
					break;
				}
			}
			if (r ==2 && c== 3)
			{
				if (mtrx->M[n][c-1] == SHAPE && this->M[i][mtrx->y+c] == SHAPE ||
					mtrx->M[n][c-2] == SHAPE && mtrx->M[n][c-1] == BACKGROUND && this->M[i][mtrx->y+c-1] == SHAPE ||
					mtrx->M[n][c-3] == SHAPE && mtrx->M[n][c-2] == BACKGROUND && mtrx->M[n][c-1] == BACKGROUND && this->M[i][mtrx->y+c-2] == SHAPE)
				{
					flag = true;
					break;
				}
			}
		}
		return flag;
	}

	bool check_move_left(Matrix * mtrx)
	{
		bool flag = false;

		int r = mtrx->get_row();
		int c = mtrx->get_col();
		
		//-------------------------- влево ------------------------------
		for (int i=mtrx->x, n=0; i<mtrx->x+r, n<r; i++, n++)
		{
			flag = false;
			if (r == 4 && c == 1 || r == 1 && c == 4 || r == 2 && c == 2)
			{
				if (mtrx->M[n][0] == SHAPE && this->M[i][mtrx->y-1] == SHAPE)
				{
					flag = true;
					break;
				}
			}
			
			if (r == 3 && c == 2)
			{
				if (mtrx->M[n][0] == SHAPE && this->M[i][mtrx->y-1] == SHAPE ||
					mtrx->M[n][1] == SHAPE && mtrx->M[n][0] == BACKGROUND && this->M[i][mtrx->y] == SHAPE)
				{
					flag = true;
					break;
				}
			}
			if (r ==2 && c== 3)
			{
				if (mtrx->M[n][0] == SHAPE && this->M[i][mtrx->y-1] == SHAPE ||
					mtrx->M[n][1] == SHAPE && mtrx->M[n][0] == BACKGROUND && this->M[i][mtrx->y] == SHAPE ||
					mtrx->M[n][2] == SHAPE && mtrx->M[n][1] == BACKGROUND && mtrx->M[n][0] == BACKGROUND && this->M[i][mtrx->y+1] == SHAPE)
				{
					flag = true;
					break;
				}
			}
		}
		return flag;
	}

//------------------------------- проверка на переполнение "стакана" ------------------------------------------

	bool check_full_glass(Matrix * mtrx)
	{
		bool flag = false;
		if (mtrx->col > mtrx->row)
		{
			for (int i=3, n=0; i<3 + mtrx->col, n<mtrx->col; i++, n++)
			{
				if (M[mtrx->row - 1][i] == SHAPE && mtrx->M[mtrx->row - 1][n] == SHAPE)
				{
					flag = true;
					break;
				}
			}
		}
		return flag;
	}


//------------------------------- отрисовка фигуры на поле ------------------------------------------

	void draw_shape(Matrix * mtrx, const unsigned x, const unsigned y)
	{
		int r = mtrx->get_row();
		int c = mtrx->get_col();
						
		for (int i=mtrx->x, n=0; i<mtrx->x+r, n<r; i++, n++)
			for (int j=mtrx->y, m=0; j<mtrx->y+c, m<c; j++, m++)
			{
				if (mtrx->M[n][m] == SHAPE)
					this->M[i][j] = BACKGROUND;
			}
		
		
		mtrx->x = this->x + x;
		mtrx->y = this->y + y;


		for (int i=mtrx->x, n=0; i<mtrx->x+r, n<r; i++, n++)
			for (int j=mtrx->y, m=0; j<mtrx->y+c, m<c; j++, m++)
			{
				if (mtrx->M[n][m] == SHAPE)
					this->M[i][j] = mtrx->M[n][m];
			}
	}

//------------------------------- отрисовка поля ------------------------------------------

	void draw_glass(Matrix * mtrx, const unsigned x, const unsigned y)
	{
		int r = mtrx->get_row();
		int c = mtrx->get_col();
						
		mtrx->x = this->x + x;
		mtrx->y = this->y + y;


		for (int i=mtrx->x, n=0; i<mtrx->x+r, n<r; i++, n++)
			for (int j=mtrx->y, m=0; j<mtrx->y+c, m<c; j++, m++)
			{
				this->M[i][j] = mtrx->M[n][m];
			}
	}
//------------------------------- проверка заполнения горизонтального ряда ------------------------------------------

	void delete_full_row()
	{
		int count = 0;
		int temp_row = 0;
		int temp_score = 0;
		bool flag = false;
		char ** temp_M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			temp_M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				temp_M[i][j] = BACKGROUND;
			}
		}
		
		while(1)
		{
			//------------------ поиск нужного ряда ------------
			for (int i=0; i<row; i++)
			{
				temp_row = 0;
				count = 0;
				for (int j=0; j<col; j++)
				{
					if (M[i][j] == SHAPE)
					{
						++count;
						if (count == 10)
						{
							temp_row = i;
							++score;
							break;
						}
					}
				}
				if (temp_row > 0)
				{
					break;
				}
			}
			
			if (temp_row == 0)
			{
				break;
			}
			//----------------------- удаление ряда --------------------
			for (int i=row-1; i>=temp_row+1; i--)
			{
				for (int j=0; j<col; j++)
				{
					temp_M[i][j] = M[i][j];
				}
			}
			for (int i=temp_row; i>0; i--)
			{
				for (int j=0; j<col; j++)
				{
					temp_M[i][j] = M[i-1][j];
				}
			}
			//------------------- перезаливаем матрицу --------------
			for (int i=0; i<row; i++)
			{
				for (int j=0; j<col; j++)
				{
					M[i][j] = temp_M[i][j];
				}
			}
		}
		this->score = score;

		for (int i=0; i<row; i++)
			delete temp_M[i];
		delete [] temp_M;
	}
//------------------------------- проверка соприкосновения при повороте ------------------------------------------

	bool check_rotate(Matrix * mtrx)
	{
		bool flag = false;
		if (mtrx->col > mtrx->row)
		{
			for (int i=mtrx->y, n=0; i<mtrx->y + mtrx->row, n<mtrx->row; i++, n++)
			{
				if (M[mtrx->x + mtrx->col - 1][i] == SHAPE && mtrx->M[mtrx->row - 1 - n][mtrx->col - 1] == SHAPE)
				{
					flag = true;
					break;
				}
			}
		}
		return flag;
	}

//------------------------------- поворот фигуры ------------------------------------------

	void rotate_shape(Matrix * mtrx)
	{
		int tr = mtrx->get_row();
		int tc = mtrx->get_col();
		for (int i=mtrx->x, n=0; i<mtrx->x+tr, n<tr; i++, n++)
		{	
			for (int j=mtrx->y, m=0; j<mtrx->y+tc, m<tc; j++, m++)
			{
				if (mtrx->M[n][m] == SHAPE)
				{
					this->M[i][j] = BACKGROUND;
				}
			}
		}

		Matrix _temp(tc,tr);
		for (int i=0; i<_temp.row; i++)
		{
			for (int j=0; j<_temp.col; j++)
			{
				_temp.M[i][j] = mtrx->M[_temp.col - 1 - j][i];
			}
		}
		mtrx->row = _temp.row;
		mtrx->col = _temp.col;
		
		tr = mtrx->get_row();
		tc = mtrx->get_col();

		mtrx->M = new char * [tr];
		for (int i=0; i<tr; i++)
		{
			mtrx->M[i] = new char [tc];
			for (int j=0; j<tc; j++)
			{
				mtrx->M[i][j] = _temp.M[i][j];
			}
		}

		for (int i=mtrx->x, n=0; i<mtrx->x+tr, n<tr; i++, n++)
		{
			for (int j=mtrx->y, m=0; j<mtrx->y+tc, m<tc; j++, m++)
			{
				if (this->M[i][j] == SHAPE && mtrx->M[n][m] == BACKGROUND)
				{
					continue;
				}
				else
				{
					this->M[i][j] = mtrx->M[n][m];
				}
			}
		}
	}

//------------------------------- "сетеры" ------------------------------------------

	void set_mtrx(const char ch)
	{
		for (int i=0; i<row; i++)
		{
			for (int j=0; j<col; j++)
			{
				M[i][j] = ch;
			}
		}
	}

	void set_score(const unsigned sc) { this->score = sc; }
	void set_row(const unsigned r) { this->row = r; }
	void set_col(const unsigned c) { this->col = c; }

	void creat_new_mtrx(const unsigned r, const unsigned c)
	{
		M = new char * [r];
		for (int i=0; i<r; i++)
		{	
			M[i] = new char [c];
		}
	}

//------------------------------- "гетеры" ------------------------------------------

	unsigned get_row() { return this->row; }
	unsigned get_col() { return this->col; }
	unsigned get_score() { return this->score; }
	char ** get_mtrx() { return this->M; }

//------------------------------- сброс координат ------------------------------------------	
	
	void clear_point() { this->x = 0; this->y = 0; }

//------------------------------- сброс очков ------------------------------------------	
	
	void clear_score() { this->score = 0; }

//------------------------------- обычная печать матрицы ------------------------------------------
	
	void PrintMtrx()
	{
		for (int i=0; i<row; i++)
		{
			for (int j=0; j<col; j++)
				cout << M[i][j];
			cout << "\n";
		}
	}

//------------------------------- печать цветной матрицы ------------------------------------------

	void c_PrintMtrx()
	{
		for (int i=0; i<row; i++)
		{
			for (int j=0; j<col; j++)
			{
				if (j != col-1)
				{
					SetColor(Yellow, Black);
					cout << M[i][j];
				}
				else
				{
					SetColor(Yellow, Black);
					cout << M[i][j];
				}
			}
			cout << endl;
		}
		SetColor(White, Black);
	}

//------------------------------- очистка экрана и печать цветной матрицы ------------------------------------------

	void clear_mtrx()
	{
		system("cls");
		c_PrintMtrx();
	}

//------------------------------- вернуть объект класса ------------------------------------------

	Matrix * get_obj_mtrx() { return this; }

//------------------------------- на всякий случай :) для копирования матрицы ------------------------------------------

	Matrix * operator = (const Matrix * mtrx)
	{
		row = mtrx->row;
		col = mtrx->col;
		M = new char * [this->row];
		
		for (int i=0; i<this->row; i++)
		{
			M[i] = new char [this->col];
			for (int j=0; j<this->col; j++)
				this->M[i][j] = mtrx->M[i][j];
		}

		return this;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////
class Shape: public Matrix
{
public:
	Shape(unsigned r, unsigned c): Matrix(r, c)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				M[i][j] = BACKGROUND;
			}
		}
	}
};
////////////////////////////////////////////////////////////////////////////////////////////
class A: public virtual Shape //линия
{
public:
	A(): Shape(1,4)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				M[i][j] = SHAPE;
			}
		}
	}
};

class B: public virtual Shape //куб
{
public:
	B(): Shape(2,2)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				M[i][j] = SHAPE;
			}
		}
	}
};

class C: public virtual Shape //Г-образная фигура 
{
public:
	C(): Shape(2,3)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				if(i==0 && (j==1 || j==2))
					M[i][j] = BACKGROUND;
				else
					M[i][j] = SHAPE;
			}
		}
	}
};

class D: public virtual Shape //Г-образная фигура в обратную сторону
{
public:
	D(): Shape(2,3)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				if(i==0 && (j==0 || j==1))
					M[i][j] = BACKGROUND;
				else
					M[i][j] = SHAPE;
			}
		}
	}
};
class E: public virtual Shape //фигура в виде "ступеньки" вправо
{
public:
	E(): Shape(2,3)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				if(i==0 && j==0 || i==row-1 && j==col-1)
					M[i][j] = BACKGROUND;
				else
					M[i][j] = SHAPE;
			}
		}
	}
};
class H: public virtual Shape //фигура в виде "ступеньки" вплево
{
public:
	H(): Shape(2,3)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				if(i==1 && j==0 || i==0 && j==col-1)
					M[i][j] = BACKGROUND;
				else
					M[i][j] = SHAPE;
			}
		}
	}
};
class G: public virtual Shape //фигура в виде "пьедестала"
{
public:
	G(): Shape(2,3)
	{
		M = new char * [row];
		for (int i=0; i<row; i++)
		{	
			M[i] = new char [col];
			for (int j=0; j<col; j++)
			{
				if(i==0 && (j==0 || j==col-1))
					M[i][j] = BACKGROUND;
				else
					M[i][j] = SHAPE;
			}
		}
	}
};

//
//---------------------------------- entry point ----------------------------------------------
//
/*Критические секции обеспечивают синхронизацию подобно мьютексам, 
  система будет "справедлива" ко всем потокам*/

CRITICAL_SECTION cs; //критическая секция 

int i_input = 0;
int menu_input = 0;
int num = 0;
int flag = 0;
int ix=0, iy=0;
bool check = false;

enum keys{UP=72, DOWN=80, LEFT=75, RIGHT=77, SAVE=115, LOAD=108, EXIT=120, NEW=110};

Shape * shape[7];
Matrix * F = new Matrix(20,10); //поле
Matrix * T = new Matrix(22,12); //тетрис

void auto_move_down(void* pParams);
void draw_tetris(Matrix * T);
void game_over(Matrix * F);
void menu(Matrix * F);
void tetris(Matrix * T, Matrix * F);
void save_in_file(Matrix * F, Shape * shape, int num);
void load_from_file(Matrix * F, Shape * shape, int &ix, int &yx, int & num);
void main()
{
	srand(time(NULL));
	
	shape[0] = new A;
	shape[1] = new B;
	shape[2] = new C;
	shape[3] = new D;
	shape[4] = new E;
	shape[5] = new H;
	shape[6] = new G;
	
	//MENU
	menu(F);
	tetris(T, F);
	Sleep(5000);
	//очистка поля
	F->set_mtrx(BACKGROUND);
	//сброс координат поля
	F->clear_point();
	
	while(1)
	{  
		flag = 0;
		//генерация фигуры	
		num = rand()%7;
		//сброс координат фигуры
		shape[num]->clear_point();
		//сброс координат поля
		F->clear_point();
		//проверка ряда
		F->delete_full_row();
		//проверка на переполнение
		if (F->check_full_glass(shape[num]->get_obj_mtrx()) == true)
		{
			//GAME OVER
			game_over(F);
			tetris(T, F);

			Sleep(1000);
			
			//MENU
			menu(F);
			tetris(T, F);
			Sleep(3000);
			
			//очистка поля
			F->set_mtrx(BACKGROUND);
			//сброс координат поля
			F->clear_point();
		}
		//прорисовка фигуры на поле
		F->draw_shape(shape[num]->get_obj_mtrx(), 0, 3);
		//вывод на экран
		tetris(T, F);
		//вывод координат фигуры
		cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
		//вывод очков
		cout << "SCORE: " << F->get_score() << endl;
		
		InitializeCriticalSection(&cs);
		_beginthread(auto_move_down, 0, NULL);
		
		while(1)
		{  			
			if (flag == 1)
			{
				break;
			}
			else
			{
				
				if (_kbhit())
				{
					i_input = _getch();
					switch (i_input)
					{
					case LEFT:
						if (shape[num]->get_y() == 0 || shape[num]->get_x() + shape[num]->get_row() - 1 == 19 ||
							F->check_move_left(shape[num]->get_obj_mtrx()) == true)
						{
							break;
						}
						else
						{
							EnterCriticalSection(&cs);
							F->clear_point();
							F->draw_shape(shape[num]->get_obj_mtrx(), shape[num]->get_x(), shape[num]->get_y()-1);
							tetris(T, F);
							cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
							cout << "SCORE: " << F->get_score() << endl;
							cout << i_input;
							LeaveCriticalSection(&cs);
							break;
						}
					case RIGHT:
						if (shape[num]->get_y() + shape[num]->get_col() - 1 == 9 ||
							shape[num]->get_x() + shape[num]->get_row() - 1 == 19 ||
							F->check_move_right(shape[num]->get_obj_mtrx()) == true)
						{	
							break;
						}
						else
						{
							EnterCriticalSection(&cs);
							F->clear_point();
							F->draw_shape(shape[num]->get_obj_mtrx(), shape[num]->get_x(), shape[num]->get_y()+1);
							tetris(T, F);
							cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
							cout << "SCORE: " << F->get_score() << endl;
							LeaveCriticalSection(&cs);
							break;
						}	
					case DOWN:
						while(1)
						{
							EnterCriticalSection(&cs);
							if (shape[num]->get_x() + shape[num]->get_row() - 1 == 19 ||
								F->check_move_down(shape[num]->get_obj_mtrx()) == true)
							{	
								flag = 1;
								break;
							}
							else
							{
								F->clear_point();
								F->draw_shape(shape[num]->get_obj_mtrx(), shape[num]->get_x()+1, shape[num]->get_y());
								tetris(T, F);
								cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
								cout << "SCORE: " << F->get_score() << endl;
							}
							LeaveCriticalSection(&cs);
						}
						break;
					case UP:
						if (shape[num]->get_col() > shape[num]->get_row())
						{
							if (shape[num]->get_x() + shape[num]->get_col() - 1 > 19 ||
								F->check_rotate(shape[num]->get_obj_mtrx()) == true ||
								F->check_move_down(shape[num]->get_obj_mtrx()) == true)
							{
								break;
							}
							else
							{
								EnterCriticalSection(&cs);
								F->rotate_shape(shape[num]->get_obj_mtrx());
								F->clear_point();
								F->draw_shape(shape[num]->get_obj_mtrx(), shape[num]->get_x(), shape[num]->get_y());
								tetris(T, F);
								cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
								cout << "SCORE: " << F->get_score() << endl;
								LeaveCriticalSection(&cs);
								break;
								
							}
						}
						if (shape[num]->get_col() < shape[num]->get_row())
						{
							if (shape[num]->get_y() + shape[num]->get_row() - 1 > 9 ||
								F->check_rotate(shape[num]->get_obj_mtrx()) == true ||
								F->check_move_left(shape[num]->get_obj_mtrx()) == true ||
								F->check_move_right(shape[num]->get_obj_mtrx()) == true)
							{
								break;
							}
							else
							{	
								EnterCriticalSection(&cs);
								F->rotate_shape(shape[num]->get_obj_mtrx());
								F->clear_point();
								F->draw_shape(shape[num]->get_obj_mtrx(), shape[num]->get_x(), shape[num]->get_y());
								tetris(T, F);
								cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
								cout << "SCORE: " << F->get_score() << endl;
								LeaveCriticalSection(&cs);
								break;
							}
						}
					case EXIT:
						system("cls");
						exit(0);
					case NEW:
						EnterCriticalSection(&cs);
						F->clear_point();
						F->clear_score();
						F->set_mtrx(BACKGROUND);
						shape[num]->clear_point();
						num = rand()%7;
						F->draw_shape(shape[num]->get_obj_mtrx(), 0, 3);
						tetris(T, F);
						cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
						cout << "SCORE: " << F->get_score() << endl;
						LeaveCriticalSection(&cs);
						break;
					case SAVE:
						EnterCriticalSection(&cs);
						save_in_file(F, shape[num], num);
						LeaveCriticalSection(&cs);
						break;
					case LOAD:
						EnterCriticalSection(&cs);
						load_from_file(F, shape[num], ix, iy, num);
						F->clear_point();
						F->draw_shape(shape[num]->get_obj_mtrx(), ix, iy);
						tetris(T, F);
						cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
						cout << "SCORE: " << F->get_score() << endl;
						LeaveCriticalSection(&cs);
						break;
					default:
						i_input = 0;
						break;
					}
				}
				
			}
			
		}
	}
}
void tetris(Matrix * T, Matrix * F)
{
	//прорисовка поля с фигурой в тетрис
	T->draw_glass(F,1,1);
	//прорисовка рамки тетриса
	draw_tetris(T);
	//вывод на экран
	T->clear_mtrx();
}
//------------------------------- MENU ------------------------------------------
void menu(Matrix * F)
{
	F->set_mtrx(BACKGROUND);
	F->get_mtrx()[1][2] = 'T';F->get_mtrx()[1][3] = 'E';F->get_mtrx()[1][4] = 'T';F->get_mtrx()[1][5] = 'R';F->get_mtrx()[1][6] = 'I';F->get_mtrx()[1][7] = 'S';
	F->get_mtrx()[3][1] = 'H';F->get_mtrx()[3][2] = 'O';F->get_mtrx()[3][3] = 'T';F->get_mtrx()[3][4] = ' ';F->get_mtrx()[3][5] = 'K';F->get_mtrx()[3][6] = 'E';F->get_mtrx()[3][7] = 'Y';F->get_mtrx()[3][8] = ':';
	
	F->get_mtrx()[4][1] = 'N';F->get_mtrx()[4][2] = ' ';F->get_mtrx()[4][3] = '-';F->get_mtrx()[4][4] = ' ';F->get_mtrx()[4][5] = 'n';F->get_mtrx()[4][6] = 'e';F->get_mtrx()[4][7] = 'w';F->get_mtrx()[4][8] = ' ';
	F->get_mtrx()[5][1] = 'S';F->get_mtrx()[5][2] = ' ';F->get_mtrx()[5][3] = '-';F->get_mtrx()[5][4] = ' ';F->get_mtrx()[5][5] = 's';F->get_mtrx()[5][6] = 'a';F->get_mtrx()[5][7] = 'v';F->get_mtrx()[5][8] = 'e';
	F->get_mtrx()[6][1] = 'L';F->get_mtrx()[6][2] = ' ';F->get_mtrx()[6][3] = '-';F->get_mtrx()[6][4] = ' ';F->get_mtrx()[6][5] = 'l';F->get_mtrx()[6][6] = 'o';F->get_mtrx()[6][7] = 'a';F->get_mtrx()[6][8] = 'd';
	F->get_mtrx()[7][1] = 'X';F->get_mtrx()[7][2] = ' ';F->get_mtrx()[7][3] = '-';F->get_mtrx()[7][4] = ' ';F->get_mtrx()[7][5] = 'e';F->get_mtrx()[7][6] = 'x';F->get_mtrx()[7][7] = 'i';F->get_mtrx()[7][8] = 't';
	
	F->get_mtrx()[9][1] = 'C';F->get_mtrx()[9][2] = 'O';F->get_mtrx()[9][3] = 'N';F->get_mtrx()[9][4] = 'T';F->get_mtrx()[9][5] = 'R';F->get_mtrx()[9][6] = 'O';F->get_mtrx()[9][7] = 'L';F->get_mtrx()[9][8] = ':';

	F->get_mtrx()[10][1] = 27;F->get_mtrx()[10][2] = ' ';F->get_mtrx()[10][3] = '-';F->get_mtrx()[10][4] = ' ';F->get_mtrx()[10][5] = 'l';F->get_mtrx()[10][6] = 'e';F->get_mtrx()[10][7] = 'f';F->get_mtrx()[10][8] = 't';
	F->get_mtrx()[11][1] = 26;F->get_mtrx()[11][2] = ' ';F->get_mtrx()[11][3] = '-';F->get_mtrx()[11][4] = ' ';F->get_mtrx()[11][5] = 'r';F->get_mtrx()[11][6] = 'i';F->get_mtrx()[11][7] = 'g';F->get_mtrx()[11][8] = 'h';
	F->get_mtrx()[12][1] = 24;F->get_mtrx()[12][2] = ' ';F->get_mtrx()[12][3] = '-';F->get_mtrx()[12][4] = ' ';F->get_mtrx()[12][5] = 'r';F->get_mtrx()[12][6] = 'o';F->get_mtrx()[12][7] = 't';F->get_mtrx()[12][8] = 'a';
	F->get_mtrx()[13][1] = 25;F->get_mtrx()[13][2] = ' ';F->get_mtrx()[13][3] = '-';F->get_mtrx()[13][4] = ' ';F->get_mtrx()[13][5] = 'd';F->get_mtrx()[13][6] = 'o';F->get_mtrx()[13][7] = 'w';F->get_mtrx()[13][8] = 'n';

}
//------------------------------- GAME OVER ------------------------------------------
void game_over(Matrix * F)
{
	F->clear_score();
	//очистка поля
	F->set_mtrx(BACKGROUND);
	F->get_mtrx()[2][3] = 'G';F->get_mtrx()[2][4] = 'A';F->get_mtrx()[2][5] = 'M';F->get_mtrx()[2][6] = 'E';
	F->get_mtrx()[3][3] = 'O';F->get_mtrx()[3][4] = 'V';F->get_mtrx()[3][5] = 'E';F->get_mtrx()[3][6] = 'R';
}
//------------------------------- рисуем рамку тетриса ------------------------------------------
void draw_tetris(Matrix * T)
{
	(T->get_mtrx())[0][0] = 218;
	(T->get_mtrx())[0][T->get_col()-1] = 191;
	(T->get_mtrx())[T->get_row()-1][0] = 192;
	(T->get_mtrx())[T->get_row()-1][T->get_col()-1] = 217;

	for (int i=0; i<T->get_row(); i++)
		for (int j=0; j<T->get_col(); j++)
		{
			if ((j == 0 || j == T->get_col()-1) && (i>0 && i<T->get_row()-1))
				(T->get_mtrx())[i][j] = 179;
			if ((i == 0 || i == T->get_row()-1) && (j>0 && j<T->get_col()-1))
				(T->get_mtrx())[i][j] = 196;
		}
}
//------------------------------- авто-падение фигуры ------------------------------------------
void auto_move_down(void* pParams)
{
    
	while(1)
	{
		EnterCriticalSection(&cs);
		if (shape[num]->get_x() + shape[num]->get_row() - 1 == 19 ||
			F->check_move_down(shape[num]->get_obj_mtrx()) == true)
		{	
			flag = 1;
			break;
		}
		else
		{
			F->clear_point();
			F->draw_shape(shape[num]->get_obj_mtrx(), shape[num]->get_x()+1, shape[num]->get_y());
			T->draw_glass(F,1,1);
			draw_tetris(T);
			T->clear_mtrx();
			cout << "(" << shape[num]->get_x() << ", " << shape[num]->get_y() << ")\n";
			cout << "SCORE: " << F->get_score() << endl;
		}
		LeaveCriticalSection(&cs);
		Sleep(300);
	}
}
//---------------------------------- запись в файл -------------------------------------------------
void save_in_file(Matrix * F, Shape * shape, int num)
{
	ofstream outF("save.txt", ios::out);
	if (outF)
	{
		outF << F->get_score() << '\n'
			 << num << '\n'
			 << shape->get_x() << ' ' << shape->get_y() << '\n'
			 << shape->get_row() << ' ' << shape->get_col() << '\n';
		
		for (int i=0; i<F->get_row(); i++)
		{
			for(int j=0; j<F->get_col(); j++)
			{
				outF << F->get_mtrx()[i][j];
			}
			outF << "\n";
		}
	}
	outF.close();
}
//---------------------------------- загрузка из файла -------------------------------------------------
void load_from_file(Matrix * F, Shape * shape,  int &ix, int &yx, int & num)
{
	int score = 0;
	int shape_r = 0;
	int shape_c = 0;
	EnterCriticalSection(&cs);
	ifstream inF("save.txt", ios::in); 
	if (inF)
	{
		inF >> score;
		F->set_score(score);
		inF >> num;
		inF >> ix;
		inF >> iy;
		inF >> shape_r;
		shape->set_row(shape_r);
		inF >> shape_c;
		shape->set_col(shape_c);
		
		for (int i = 0; i<F->get_row(); i++)
		{
			for (int j = 0; j<F->get_col(); j++)
			{
				inF >> F->get_mtrx()[i][j];
			}
		}

		shape->creat_new_mtrx(shape_r, shape_c);
		for (int i = ix, n=0; i<ix+shape_r, n<shape_r; i++, n++)
		{
			for (int j = iy, m=0; j<iy+shape_c, m<shape_c; j++, m++)
			{
				shape->get_mtrx()[n][m] = F->get_mtrx()[i][j];
				F->get_mtrx()[i][j] = BACKGROUND;
			}
		}
	}
	else
	{
		cout << "No such file...\n";
	}
	
	inF.close();
	LeaveCriticalSection(&cs);
}
