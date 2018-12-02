#include <iostream>
#include <fstream>
#include "Net.h"
// тестовый комментарий ветка Алиса1
#define getVertex(a,b) (a+Nx*b)

using namespace std;

void printVertexArray (int Vertex [][2]); // печать списка вершин
void printVertexArray_file (int Vertex [][2]);
void printElements (int *, int *); // печать топологии поэлементно
int addElement(int , int); // добавляет элемент топологии в массивы IA JA
//int getVertex (int, int); // возвращает ссылку на номер вершины по известным координатам x y
int mas_init (int*, int);//инициализация массива -1
int print (int*, int);//печать массива заданной длины в cerr
int print_cout (int *tmp, int length);//в cout

ofstream out, out_mesh, out_coord, out_topo, out_bctopo;//поток для записи

int Nx, Ny, Lx, Ly;
int K, M;
int Nn, Ne, Ne_actual, Nbf, Ncells, Nk, Nm;//Nn-количество узлов, Ne- количество элементов(треуголбник или четырехугольник), Nbf-количество внешних ребер
//Ncells - количество четерыехугольников, четырехугольников типа K, четырехугольников типа M Ne_actual - количество элементов в IA
int * IA, * JA;

int * RIA, *RJA;//обратная топология

int main(int argc, char**argv)
{
  //вывод ошибок в файл
  ofstream cerrFile("out_err.txt");//откроем файл для вывода
  cerr.rdbuf(cerrFile.rdbuf());
  //streambuf *cerrbuf =cerr.rdbuf(); //запомним старый буфер
  //cerr.rdbuf(out.rdbuf());
  //cerr.rdbuf(cerrbuf);

  //для записи в файл
  out.open("./exp.txt");
  out_mesh.open("./mesh.txt");//содержит Nn, Ne, Nbf
  out_coord.open("./coordinate.txt");//координаты узлов, Nn строк по 3 значения НАДО ФОРМАТ MSH
  out_topo.open("./topo.msh");//список элементов, Nе строк, в строках первое значение - число узлов в элементе, затем номера узлов
  out_bctopo.open("./bctopo.msh");//список внешних граней, Nbf строк, в строках первое значение - число узлов в грани, затем номера узлов, затем метка граничной поверхности. поверхности нумеровать можно так: левая граница 1, правая 2, верхняя 3, нижняя 4.

  //ввод данных
  if(argc==0)
  {
    cout<< "Введите Nx, Ny, Lx, Ly, K, M"<<endl;//M-целые, K-побитые
    cin >> Nx >> Ny >> Lx >> Ly >> K >> M;//добавить проверку
  }
  else
  {
    ifstream input(argv[1]);//открываем файл на чтение
    if(input.is_open())
      cerr<<"file is open"<<endl;
    input>>Nx>>Ny>>Lx>>Ly>>K>>M;
    //cout<<Nx<<Ny << Lx << Ly << K << M;//добавить проверку
    input.close();
  }


  cerr<<"ОК"<<endl;


  // предварительные вычисления и инициализация сетки и элементов
  Nn=Nx*Ny;
  Ncells = (Nx-1)*(Ny-1);
  Nk=Ncells/(K+M)*K+(((Ncells%(K+M))>=K)?K:Ncells%(K+M));
  Nm=Ncells-Nk;
  Ne=Nk*2+Nm;
  Nbf=(Nx-1)*2+(Ny-1)*2;
  //cout<<"Количество элементов="<< Ne<<endl;

  out_mesh<<"Nn="<<Nn<<endl<<"Ne="<<Ne<<endl<<"Nbf="<<Nbf<<endl;

  // координаты вершин основной сетки - вывести в отдельную функцию
  int ANn[Nn][2];
  for (int i=0; i<Nn; i++) // заполнение координат вершин квадратной сетки
  {
    ANn[i][0]=i%Nx;
    ANn[i][1]=i/Nx;
  }

  // Проверка - вывод координат вершин на печать
  //printVertexArray(ANn);
  printVertexArray_file (ANn);


  // заполнение массивов с элементами, вынести в отдельную функцию
  IA = (int *) malloc (sizeof (int)*(Ne+1));//индексный массив
  mas_init(IA, Ne+1);
  JA = (int *) malloc (sizeof (int)*(Ne*4));//индексный массив с запасом
  mas_init(JA, Ne*4);
  //print_cout(IA, Ne+1);
  //print_cout(JA, Ne*4);
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

  cout<<"ПЕЧАТАЕМ IA и JA"<<endl;
  print_cout(IA, Ne+1);
  print_cout(JA, Ne*4);


  //обратная топология
  RIA = (int *) malloc (sizeof (int)*(Nn+1));//индексный массив(номера вершин)
  RJA = (int *) malloc (sizeof (int)*(Nn*5));//индексный массив с запасом(каким элементам принадлежит)
  mas_init(RIA, Nn+1);
  mas_init(RJA, Nn*5);

  int k;
  int cur_ria=0, cur_rja=0;
  for (int i=0; i<Nn; i++)
  {
    cerr<<"i="<<i<<endl;
    RIA[cur_ria]=cur_rja;
    cerr<<"cur_ria="<<cur_ria<<endl;
    cerr<<"cur_rja="<<cur_rja<<endl;

    int my_tmp=-1;
    k=0;
    while(JA[k]!=-1)
    {
      cerr<<"JA[k]="<<JA[k]<<endl;
      if(JA[k]==i)//нашли вершину в JA
      {
        cerr<<"заходим в if"<<endl;
        //тут посчитать номер элемента
        int j=0;
        cerr<<"k="<<k<<"IA[j]="<<IA[j]<<"IA[j+1]="<<IA[j+1]<<endl;
        while (!((IA[j]<=k)&&(k<IA[j+1])))
        {
          cerr<<"in WHILE"<<endl;
          //cerr<<"j="<<j<<"IA[j]="<<IA[j]<<"JA[j]="<<JA[j]<<endl;
          j++;
        }
        my_tmp=j;//номер элемента
        RJA[cur_rja]=my_tmp;
        cerr<<"номер элемента my_tmp="<<my_tmp<<"выходим из if"<<endl;

        cur_rja++;
      }
      k++;
    }
    cur_ria++;
  }

  cerr<<"печать RIA и RJA"<<endl;
  print(RIA, Nn+1);
  print(RJA, Nn*5);
  cerr<<"конец печати"<<endl;


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

  cerrFile.close();

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

// печать в файл координат основной сетки
void printVertexArray_file (int Vertex [][2])
{
  //out_coord<<"печать массива ANn(для каждого элемента его координаты)"<<endl;
  //out_coord<<" Номер   Х   Y\n";
  for(int i=0; i<Nn; i++)
  {
      out_coord<<"    "<< i <<"   "<< Vertex[i][0]<<"   "<< Vertex[i][1]<<endl;
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

int mas_init (int *tmp, int length)
{
  for (int t=0; t<length; t++)
    tmp[t]=-1;
  return 0;
}

int print (int *tmp, int length)//печать ошибок в файл
{
  cerr<<"печать массива"<<endl;
  for (int t=0; t<length; t++)
    cerr<<tmp[t]<<endl;
  return 0;
}

int print_cout (int *tmp, int length)//печать в стандартный поток
{
  cout<<"печать массива"<<endl;
  for (int t=0; t<length; t++)
    cout<<tmp[t]<<endl;
  return 0;
}

// возвращает номер вершины по координатам
/*int getVertex (int x, int y)
{
    return x+Nx*y;
}*/
