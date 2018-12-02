#include <iostream>
#include "Net.h"

#define getVertex(a,b) (a+Nx*b)

using namespace std;

void printVertexArray (int Vertex [][2]); // печать списка вершин
void printElements (int *, int *); // печать топологии поэлементно
int addElement(int , int); // добавляет элемент топологии в массивы IA JA
// int getVertex (int, int); // возвращает ссылку на номер вершины по известным координатам x y

int Nx, Ny, Lx, Ly;
int K, M;
int Nn, Ne, Ne_actual, Nbf, Ncells, Nk, Nm;//Nn-количество узлов, Ne- количество элементов(треуголбник или четырехугольник), Nbf-количество внешних ребер
//Ncells - количество четерыехугольников, четырехугольников типа K, четырехугольников типа M Ne_actual - количество элементов в IA
int * IA, * JA;

int main()
{
  cout<< "Введите Nx, Ny, Lx, Ly, K, M"<<endl;//M-целые, K-побитые
  cin >> Nx >> Ny >> Lx >> Ly >> K >> M;//добавить проверку

  // предварительные вычисления и инициализация сетки и элементов
  Nn=Nx*Ny;
  Ncells = (Nx-1)*(Ny-1);
  Nk=Ncells/(K+M)*K+(((Ncells%(K+M))>=K)?K:Ncells%(K+M));
  Nm=Ncells-Nk;
  Ne=Nk*2+Nm;
  //cout<<"Количество элементов="<< Ne<<endl;

  // координаты вершин основной сетки - вывести в отдельную функцию
  int ANn[Nn][2];
  for (int i=0; i<Nn; i++) // заполнение координат вершин квадратной сетки
  {
    ANn[i][0]=i%Nx;
    ANn[i][1]=i/Nx;
  }

  // Проверка - вывод координат вершин на печать
  printVertexArray(ANn);

  // заполнение массивов с элементами, вынести в отдельную функцию
  IA = (int *) malloc (sizeof (int)*(Ne+1));//индексный массив
  JA = (int *) malloc (sizeof (int)*(Ne*4));//индексный массив с запасом
  // !!! добавить проверку что выделение памяти прошло успешно
  Ne_actual=0;
  IA[Ne_actual]=0;
  for (int p=0; p<Ncells; p++)
  {
    if (p%(K+M)<K)  // для ячеек типа к
    {
      addElement(p, UP_LEFT);
      addElement(p, DOWN_RIGHT);
    }
    else  // ячейки типа М
      addElement(p, SQUARE);
  }
  // проверка заполнения списка элементов
  printElements (IA,JA);

  //  заполнение массива ребер - вывести в отдельную функцию
  int ANbf[((Nx-1)*(Ny-1)*2)][4];//[0]-количество узлов в грани; [1],[2]-номера узлов; [3]-номер стороны  1|=|3

  //заполняем все верхние внешние ребра; их Nx штук
  int n, m=0, base;
  for(n=0; n<Nx-1; n++)
  {
    ANbf[m][0]=Nx;
    ANbf[m][1]=n;
    ANbf[m][2]=n+1;
    ANbf[m][3]=2;
    m++;
  }
  // правая грань
  for(n=0; n<Ny-1; n++)
  {
    ANbf[m][0]=Ny;
    ANbf[m][1]=(n+1)*Nx-1;
    ANbf[m][2]=(n+2)*Nx-1;
    ANbf[m][3]=3;
    m++;
  }
  // нижнее ребро
  base=Nx*(Ny-1);
  for(n=0; n<Nx-1; n++)
  {
    ANbf[m][0]=Nx;
    ANbf[m][1]=base+n;
    ANbf[m][2]=base+n+1;
    ANbf[m][3]=4;
    m++;
  }
  // левая грань
  for(n=0; n<Ny-1; n++)
  {
    ANbf[m][0]=Ny;
    ANbf[m][1]=Nx*n;
    ANbf[m][2]=Nx*(n+1);
    ANbf[m][3]=1;
    m++;
  }

  // проверка заполнения ребер - удалить
  cout<<"печать ANbf"<<endl;
  for(n=0;n<((Nx-1)+(Ny-1))*2;n++)
  {
    cout<<"n="<<n<<' '<<ANbf[n][0]<<ANbf[n][1]<<ANbf[n][2]<<ANbf[n][3]<<endl;
  }

  /*for(n=0; n<Ny; n++)
  {
    ANbf[n+2*Nx+Ny][0]=2;
    ANbf[n+2*Nx+Ny][1]=;
    ANbf[n+2*Nx+Ny][2]=Nx-n-1;
    ANbf[n+2*Nx+Ny][3]=1;
  }*/
  //i=Lx/Nx;//длина ребра
  //j=Ly/Ny;//длина ребра
  /*for(int j=IA[i]; j<IA[i+1]; j++) //доступ к узлам i-ого элемента
  {
        int in=JA[j];
  }*/
  // cout << i << "    " << j<<endl;
  return 0;
}


// печать координат основной сетки
void printVertexArray (int Vertex [][2])
{
  cout<<"печать массива ANn(для каждого элемента его координаты)"<<endl;
  cout<<" Номер   Х   Y\n";
  for(int i=0; i<Nn; i++)
  {
      cout<<"    "<< i <<"   "<< Vertex[i][0]<<"   "<< Vertex[i][1]<<endl;
  }
}

// печать созданных элементов
void printElements (int * IA, int * JA)
{
  cout<<"печать списка элементов"<<endl;
  cout<<" всего элементов " << Ne << "Nk " << Nk << "Nm" << Nm << endl;
  cout<<"Элемент   №1   №2   №3   №4\n";
  for(int i=0; i<Ne; i++)
  {
      cout<<" "<< i;
      for(int j=IA[i]; j<IA[i+1]; j++) cout<<"   "<< JA[j];
      cout<<endl;
  }
}

// добавление элемента топологии в массивы IA JA, p- номер ячейки по порядку type - тип элемента см h-файл 0
int addElement(int p, int type)
{
  int row, col;
  int JA_new_element_star= IA[Ne_actual]; // индекс в JA с которого будут записываться координаты вершин добавляемого элемента
  row = p/(Nx-1);
  col = p%(Nx-1);
  switch (type)
  {
    case  UP_LEFT: //три вершины
      {
        JA[JA_new_element_star]=getVertex(col, row);
        JA[JA_new_element_star+1]=getVertex(col+1,row);
        JA[JA_new_element_star+2]=getVertex(col,row+1);
        IA[Ne_actual+1]=IA[Ne_actual]+3;
        Ne_actual++;
        return 1;
      }
    case DOWN_RIGHT:
      {
        JA[JA_new_element_star]=getVertex(col+1, row);
        JA[JA_new_element_star+1]=getVertex(col+1,row+1);
        JA[JA_new_element_star+2]=getVertex(col,row+1);
        IA[Ne_actual+1]=IA[Ne_actual]+3;
        Ne_actual++;
        return 1;
      }
    case  SQUARE:
      {
        JA[JA_new_element_star]=getVertex(col, row);
        JA[JA_new_element_star+1]=getVertex(col+1,row);
        JA[JA_new_element_star+2]=getVertex(col+1,row+1);
        JA[JA_new_element_star+3]=getVertex(col,row+1);
        IA[Ne_actual+1]=IA[Ne_actual]+4;
        Ne_actual++;
        return 1;
      }
    return 0;
  }
  return 0;
}

// возвращает номер вершины по координатам
/*int getVertex (int x, int y)
{
    return x+Nx*y;
}*/
