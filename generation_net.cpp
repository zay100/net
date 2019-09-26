#include <iostream>
#include <fstream>
#include "Net.h"
// тестовый комментарий ветка Алиса1
#define getVertex(a,b) (a+Nx*(b))

#define FILE_EXPORT_VERTEX "./coordinate.msh"

using namespace std;

void printVertexArray (class VertexClass *); // печать списка вершин
void printVertexArray_file (class VertexClass *);
void printEdgeArray ( class EdgeClass *);
void printElements (int *, int *); // печать топологии поэлементно
void printTopoElements (TopoClass *);
void printNeighbors (class NNadjClass * );
int addElement(int , int); // добавляет элемент топологии в массивы IA JA
int mas_init (int*, int);//инициализация массива -1
int print (int*, int);//печать массива заданной длины в cerr
int print_cout (int *tmp, int length);//в cout



int Nx, Ny, Lx, Ly;
int K, M;
int Nn, Ne, Ne_actual, Nbf, Ncells, Nk, Nm;//Nn-количество узлов, Ne- количество элементов(треуголбник или четырехугольник), Nbf-количество внешних ребер
//Ncells - количество четерыехугольников, четырехугольников типа K, четырехугольников типа M Ne_actual - количество элементов в IA
int * IA, * JA;
int * RIA, *RJA;//обратная топология

int main(int argc, char**argv)
{
  const char * filename;
  ofstream out, out_mesh, out_topo, out_bctopo;//поток для записи
  ofstream cerrFile("out_err.txt");//откроем файл для вывода
  cerr.rdbuf(cerrFile.rdbuf());
  streambuf *cerrbuf =cerr.rdbuf(); //запомним старый буфер
  cerr.rdbuf(out.rdbuf());
  cerr.rdbuf(cerrbuf);
  //для записи в файл
  //out.open("./exp.txt");
  out_mesh.open("./mesh.txt");//содержит Nn, Ne, Nbf
  //out_coord.open("./coordinate.msh");//координаты узлов, Nn строк по 3 значения НАДО ФОРМАТ MSH
  out_topo.open("./topo.msh");//список элементов, Nе строк, в строках первое значение - число узлов в элементе, затем номера узлов
  out_bctopo.open("./bctopo.msh");//список внешних граней, Nbf строк, в строках первое значение - число узлов в грани, затем номера узлов, затем метка граничной поверхности. поверхности нумеровать можно так: левая граница 1, правая 2, верхняя 3, нижняя 4.

  //ввод данных
  if(argc==1)
  {

    cout<< "Введите Nx, Ny, Lx, Ly, K, M"<<endl;//M-целые, K-побитые
    cin >> Nx >> Ny >> Lx >> Ly >> K >> M;//добавить проверку
  }
  else
  {
    ifstream input(argv[1]);//открываем файл на чтение
    if(input.is_open()) {
      cout<<"file is open"<<endl;
      input>>Nx>>Ny>>Lx>>Ly>>K>>M;
    //cout<<Nx<<Ny << Lx << Ly << K << M;//добавить проверку
      input.close();
      }
    else {
      cout<<"Невозможно открыть файл \n";
      return (0);
    }
  }
  cerr<<"ОК"<<endl;

  // генерерация сетки элементов, топологий и тп через конструкторы порядок важен
  // class NetClass net (Nx, Ny, K, M);
/*  class CellClass cells (&net);
  net.cell_p=&cells;
  class VertexClass vertex (&net);
  net.vertex_p=&vertex;
  class EdgeClass edge (&net);
  class TopoClass topo (& net);
  class TopoClass r_topo (& topo);
  class NNadjClass neighbors (&net);
*/
  class NetClassNG netNG (Nx, Ny, K, M);


/*

  //ВЫВОД В ФАЙЛ ПЕРЕДЕЛАТЬ НА ФУНКЦИИ
  // вывод в файл параметров сетки
  out_mesh<<"NumCoords 2\nNn= "<<net.Nn<<endl<<"Nt= "<<net.Nn<<endl<<"NFaceBC= "<<net.Nbf<<endl;
  out_mesh.close();
  // вывод в файл массива вершин
  for(int i=0; i<vertex.number_; i++)
  {
      out_coord<< i <<" "<< vertex.ANn[i][0]<<" "<< vertex.ANn[i][1]<<endl;
  }
  out_coord.close();
  // вывод в файл топологии
  for(int i=0; i<topo.Ne; i++)
  {
      out_topo<< i<< " ";
      for(int j=topo.IA[i]; j<topo.IA[i+1]; j++) out_topo<<" "<< topo.JA[j];
      out_topo<<endl;
  }
  out_topo.close();
  // вывод в файл внешних граней
  for(int n=0;n<edge.number;n++)
  {
    out_bctopo<<edge.ANbf[n][0]<<" "<<edge.ANbf[n][1]<<" "<<edge.ANbf[n][2]<<" "<<edge.ANbf[n][3]<<endl;
  }
  out_bctopo.close();



  // Проверочная печать на экран
  printNeighbors (&neighbors);
  printTopoElements (&topo);
  printVertexArray ( &vertex );
  printEdgeArray ( &edge);
  printTopoElements (&r_topo);
*/
  filename = FILE_EXPORT_VERTEX;
  netNG.ExportMesh(filename);


  netNG.PrintEdgeArray();
  netNG.printTopoElementsNG();

  cerrFile.close();
  return 0;
}

// печать координат основной сетки
void printVertexArray (VertexClass* vertex)
{
  cout<<"печать массива ANn(для каждого элемента его координаты)"<<endl;
  cout<<" Номер   Х   Y\n";
  for(int i=0; i<vertex->number_; i++)
  {
      cout<<"    "<< i <<"   "<< vertex->ANn[i][0]<<"   "<< vertex->ANn[i][1]<<endl;
  }
}

// печать массива граней
void printEdgeArray (EdgeClass* edge)
{
  cout<<"печать ANbf"<<endl;
  for(int n=0;n<edge->number;n++)
  {
    cout<<"n="<<n<<' '<<edge->ANbf[n][0]<<" "<<edge->ANbf[n][1]<<" "<<edge->ANbf[n][2]<<" "<<edge->ANbf[n][3]<<endl;
  }
}



// печать в файл координат основной сетки
/*
void printVertexArray_file (VertexClass* vertex)
{
  //out_coord<<"печать массива ANn(для каждого элемента его координаты)"<<endl;
  //out_coord<<" Номер   Х   Y\n";
  for(int i=0; i<vertex->number_; i++)
  {
      out_coord<< i <<" "<< vertex->ANn[i][0]<<" "<< vertex->ANn[i][1]<<endl;
  }
}
*/
// печать массива элементов топологии
void printTopoElements (TopoClass * topo)
{
  cout<<"печать списка элементов из класса топологии"<<endl;
  cout<<" всего элементов " << Ne << " Nk " << Nk << " Nm" << Nm << endl;
  cout<<"Элемент  №1   №2   №3   №4\n";
  for(int i=0; i<topo->Ne; i++)
  {
      cout<<" "<< i<< "     ";
      for(int j=topo->IA[i]; j<topo->IA[i+1]; j++) cout<<"    "<< topo->JA[j];
      cout<<endl;
  }
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

// печать элементов массива соседей
void printNeighbors (class NNadjClass * neighbors)
{
  cout << "Печать массива соседей" << endl;
  cout<<" Количество связей ="<<neighbors->nnadj_number<<endl;
  int base;
  for (int i=0; i<neighbors->net_p->Nn; i++)
  {
    cout << i<< " ";
    for (int j=neighbors->IA[i]; j<neighbors->IA[i+1]; j++)  cout<<" "<< neighbors->JA[j];
    cout << endl;
  }
}
