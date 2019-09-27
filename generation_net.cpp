#include <iostream>
#include <fstream>
#include "Net.h"

// имена файлов для вывод элементов сетки
#define FILE_EXPORT_VERTEX "./coordinate.msh"  // координаты вершин
#define FILE_EXPORT_NET_PARAMETRES "./mesh.txt"  // основные параметры сетки
#define FILE_EXPORT_TOPO "./topo.msh"  // топология
#define FILE_EXPORT_EDGE "./edge.msh"  // внешние грани

using namespace std;

int main(int argc, char**argv)
{
  int Nx,Ny,Lx,Ly,K,M;

  ofstream cerrFile("net_err.txt");//откроем файл для вывода
  ofstream out;
  cerr.rdbuf(cerrFile.rdbuf());
  streambuf *cerrbuf =cerr.rdbuf(); //запомним старый буфер
  cerr.rdbuf(out.rdbuf());
  cerr.rdbuf(cerrbuf);

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

  class NetClassNG netNG (Nx, Ny, K, M);


  netNG.ExportMesh(FILE_EXPORT_VERTEX);
  netNG.ExportNetParametres(FILE_EXPORT_NET_PARAMETRES);
  netNG.ExportTopo(FILE_EXPORT_TOPO);
  netNG.ExportEdge(FILE_EXPORT_EDGE);

  netNG.PrintEdgeArray();
  netNG.printTopoElementsNG();

  cerrFile.close();
  return 0;
}
