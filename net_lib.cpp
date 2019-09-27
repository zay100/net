#include <iostream>
#include <fstream>
#include "Net.h"

using namespace std;



NetClassNG::NetClassNG (int Nx_, int Ny_ , int K_, int M_) : VertexClassNG (Nx_, Ny_), CellClassNG (Nx_, Ny_, K_, M_), EdgeClassNG (Nx_, Ny_)
{
//  Nx=Nx_;
//  Ny=Ny_;
//  K=K_;
//  M=M_;
  //Nn=Nx_*Ny_;
  //Nk=Ncells/(K_+M_)*K+(((Ncells%(K_+M_))>=K_)?K_:Ncells%(K_+M_));
  //Nm=Ncells-Nk;
  Nbf=(Nx_-1)*2+(Ny_-1)*2;
  NFaceBC=(Nx+Ny)*2-4;
  InitTopo (K_, M_, Ncells, Nx, Nn, this);
};

// возвращает тип ячeйки номеру узла и направлению на ячейку от узла, перегружена
int NetClassNG::GetPrimaryCellType (int node, int direction)
{
  int cell_number;
  int x=VertexClassNG::GetVertexX(node);
  int y=VertexClassNG::GetVertexY(node);

  switch (direction)
  {
    case NW:
        if (x==0 || y==0) return NO_ELEMENT;
        cell_number = (y-1)*(Nx-1)+x-1;
        return GetPrimaryCellType (cell_number);
    case NE:
        if (x==Nx || y==0) return NO_ELEMENT;
        cell_number = (y-1)*(Nx-1)+x;
        return GetPrimaryCellType (cell_number);
    case SE:
        if (x==Nx || y==Ny) return NO_ELEMENT;
        cell_number = y*(Nx-1)+x;
        return GetPrimaryCellType (cell_number);
        break;
    case SW:
        if (x==0 || y==Ny) return NO_ELEMENT;
        cell_number = y*(Nx-1)+x-1;
        return GetPrimaryCellType (cell_number);
    default:
        return NO_ELEMENT;
  }

}

// конструктор класс внешних граней
EdgeClassNG::EdgeClassNG (int Nx, int Ny)
{

  EdgeNumbers = ((Nx-1)+(Ny-1))*2;
  NFaceBC = (Nx+Ny-2)*2;
  Nbf = (Nx+Ny-2)*2;
  ANbf = new int * [EdgeNumbers];  // выделение памяти для массива ребер граней
  for (int i=0; i< EdgeNumbers; i++)
    ANbf[i] = new int [4];

  int n, m=0;
  // верхняя грань
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
  // нижняя грань
  int base=Nx*(Ny-1);
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
}

void EdgeClassNG::PrintEdgeArray()
{
  cout<<"печать массива граней из объекта NetClassNG"<<endl;
  for(int n=0;n<EdgeNumbers;n++)
  {
    cout<<"n="<<n<<' '<<ANbf[n][0]<<" "<<ANbf[n][1]<<" "<<ANbf[n][2]<<" "<<ANbf[n][3]<<endl;
  }


}

// инициализация массивов топологии из данных о сети
void NetClassNG::InitTopo ( int K, int M, int Ncells, int Nx , int Nn, class NetClassNG * net_p)
{
//заполнение прямой топологии
  int Nk=Ncells/(K+M)*K+(((Ncells%(K+M))>=K)?K:Ncells%(K+M));
  int Nm=Ncells-Nk;

  NeTopo = Nk*2+Nm; // количество элементов топологии
  NnTopo = Nn;
  int JAmax = Nk*3*2+Nm*4; //количество вершин

  IAtopo = new int[NeTopo+1];
  if(!IAtopo) cout<<"Ошибка при создании массива IAtopo"<<endl;
  JAtopo = new int[JAmax];
  if(!IAtopo) cout<<"Ошибка при создании массива JAtopo"<<endl;
  IAtopo[0]=0;
  IAtopo[1]=0;

  int base, count = 0; // номер первой вершины очередного элемента в JA, номер очередного элемента топологии
  for (int p=0; p<Ncells; p++) //пробегаем по всем первичным ячейкам
  {
    int row = p/(Nx-1); // определяем столбец и колонку для очередной ячейки
    int col = p%(Nx-1); // он же эквивалент для левого верхнего узла
    if (p%(K+M)<K)  // для ячеек типа к добавить два элемента МОЖНО ЗАМЕНИТЬ НА GetPrimaryCellType
    {
//        addTopoElement(p, UP_LEFT);
        base = IAtopo[count];
        JAtopo[base]=GetVertexNG(col, row);
        JAtopo[base+1]=GetVertexNG(col+1,row);
        JAtopo[base+2]=GetVertexNG(col,row+1);
        IAtopo[count+1]=base+3;
        count++;
//        addTopoElement(p, DOWN_RIGHT);
        base = IAtopo[count];
        JAtopo[base]=GetVertexNG(col+1, row);
        JAtopo[base+1]=GetVertexNG(col+1,row+1);
        JAtopo[base+2]=GetVertexNG(col,row+1);
        IAtopo[count+1]=base+3;
        count++;
    }
    else  // ячейки типа М
    {
//        addTopoElement(p, SQUARE);
        base = IAtopo[count];
        JAtopo[base]=GetVertexNG(col, row);
        JAtopo[base+1]=GetVertexNG(col+1,row);
        JAtopo[base+2]=GetVertexNG(col+1,row+1);
        JAtopo[base+3]=GetVertexNG(col,row+1);
        IAtopo[count+1]=base+4;
        count++;
    }
  }

// заполнение обратной топологии

  int ** temp_node_array; // массив для группировки элементов по вершинам
  temp_node_array = new int * [Nn]; // количество строк равно количеству элементов
  for (int i=0; i< Nn; i++) // столбцов по максимуму элементов плюс один на счетчик кол-ва
    temp_node_array[i] = new int [MAX_ELEMENTS_PER_NODE+1];
  // инициализация промежуточного массива в 0, ВОЗМОЖНО ЛИШНЕЕ
  for (int i=0; i< Nn; i++)
    for (int j=0; j<MAX_ELEMENTS_PER_NODE+1;j++)
      temp_node_array[i][j]=0;

  // заполнение промежуточного массива первый столбец - кол-во элементов для вершины далее номера элементов топологии
  int node, tmp, counter=0;
  for (int i=0; i< NeTopo; i++) // i - номер элемента топологии
  {
  // ПОСМОТРЕТЬ НА ВОЗМОЖНОСТЬ ОПТИМИЗАЦИИ
    for (int j=IAtopo[i];j<IAtopo[i+1];j++)
    {
      node=JAtopo[j];
      tmp = temp_node_array[node][0]; //количество уже записанных элементов для вершины
      temp_node_array[node][tmp+1]=i; // добавляем номер элемента в следующую свободную ячейку для вершины
      temp_node_array[node][0]++; // увеличиваем счетчик добавленных элементов
      counter++;
    }
  }
  IAtopoRevert = new int [Nn+1];
  JAtopoRevert = new int  [counter+1];
  // заполнение обратной топологии
  IAtopoRevert[0]=0;

  for (int i=0; i<Nn; i++)
  {
    base = IAtopoRevert[i];
    IAtopoRevert[i+1]=IAtopoRevert[i]+temp_node_array[i][0];
    for (int j=0; j<temp_node_array[i][0]; j++)
      JAtopoRevert[base+j]=temp_node_array[i][j+1];
  }

  // !!!!! УДАЛИТЬ АККУРАТНО ИНАЧЕ УТЕЧКА ПАМЯТИ
  delete [] temp_node_array ;

  // заполнение таблицы связанных узлов
  IAadj = new int [Nn+1];
  JAadj = new int [Nn*MAX_RIBS_PER_NODE];  //оценка сверху, можно лучше
  IAadj[0]=IAadj[1]=0;
  int m; // можно использовать ранее использованную переменную

  for (int i=0; i<Nn; i++) // перебираем все точки
  {
    m=0;
    base = IAadj[i];
    if ((i%Nx)<(Nx-1))//ребро справа
    {
      JAadj[base+m]=i+1;
      m++;
    }
    if ((i%Nx)!=0)//ребро слева
    {
      JAadj[base+m]=i-1;
      m++;
    }
    if (i>=Nx)//ребро сверху и верхние диагонали
    {
      JAadj[base+m]=i-Nx;
      m++;
      if ((net_p->GetVertexX(i)<Nx-1) && (net_p->GetPrimaryCellType(i, NE)==DIAG_ASCEND))
      {
        JAadj[base+m]=i-Nx+1;
        m++;
      }
      if ((net_p->GetVertexX(i)>0) && (net_p->GetPrimaryCellType(i, NW)==DIAG_DESCEND))
      {
        JAadj[base+m]=i-Nx-1;
        m++;
      }
    }
    if (i < Nn - Nx)//ребро вниз и нижние диагонали
    {
      JAadj[base+m]=i+net_p->Nx;
      m++;
      if ((net_p->GetVertexX(i)<Nx-1) && (net_p->GetPrimaryCellType(i, SE)==DIAG_DESCEND))
      {
        JAadj[base+m]=i+Nx+1;
        m++;
      }
      if ((net_p->GetVertexX(i)>0) && (net_p->GetPrimaryCellType(i, SW)==DIAG_ASCEND))
      {
        JAadj[base+m]=i+Nx-1;
        m++;
      }
    }
    IAadj[i+1]=base+m;
  }
  NnAdj=IAadj[Nn];

}

void TopoClassNG::printTopoElementsNG ()
{
  cout<<"\nПечать списка элементов топологии класса NetClassNG"<<endl;
  cout<<"Всего элементов топологии "<< NeTopo<<endl;
    cout<<"Элемент             Узлы\n";
  for(int i=0; i<NeTopo; i++)
  {
      cout<<"Element "<< i<< "   Nodes ";
      for(int j=IAtopo[i]; j<IAtopo[i+1]; j++) cout<<"    "<< JAtopo[j];
      cout<<endl;
  }

  cout<<"\nПечать обратно топологии класса NetClassNG"<<endl;
  cout<<"Всего узлов "<< NnTopo<<endl;
    cout<<"Узел            Элементы топологии\n";
  for(int i=0; i<NnTopo; i++)
  {
      cout<<"Node "<< i<< " Elements ";
      for(int j=IAtopoRevert[i]; j<IAtopoRevert[i+1]; j++) cout<<"    "<< JAtopoRevert[j];
      cout<<endl;
  }

  cout<<"\nПечать списка ребер NetClassNG"<<endl;
  cout<<"Всего ребер "<< NnAdj<<endl;
    cout<<"Узел                     Смежные узлы\n";
  for(int i=0; i<NnTopo; i++)
  {
      cout<<"Node "<< i<< " Adjusted nodes  ";
      for(int j=IAadj[i]; j<IAadj[i+1]; j++) cout<<"    "<< JAadj[j];
      cout<<endl;
  }

}

VertexClassNG::VertexClassNG ( int Nx_, int Ny_) // стандартный конструктор
{
  Nx= Nx_;
  Ny= Ny_;
  Nn= Nx *Ny;
  ANn=new int * [Nn];
  for (int i=0; i<Nn; i++) // создание двумерного массива нужного размера
  {
	     ANn[i] = new int [2];
	     if (!ANn[i]) std::cout<<"ошибка создания массива"<<std::endl;
  }
  for (int i=0; i<Nn; i++) // заполнение координат вершин квадратной сетки
  {
    ANn[i][0]=i%Nx;
    ANn[i][1]=i/Nx;
  }
}

CellClassNG::CellClassNG (int Nx, int Ny, int K, int M)
{
  NxCells = Nx - 1;
  NyCells = Ny - 1;
  Ncells = NxCells * NyCells;
  ANcell=new int [Ncells];  // добавить проверку на ошибку
  for (int i=0; i<Ncells; i++) // заполнение вектора типом ячеек СПЕЦИФИЧНО ДЛЯ ВАРИАНТА ЗАДАНИЯ
  {
	     ANcell[i] = (i%(K+M)<K)?DIAG_ASCEND:NO_DIAG;
	}
}

void VertexClassNG::ExportMesh (const char * name)
{
  ofstream fs;

  // добавить проверки на правильность
  fs.open(name);
  for (int i=0; i<Nn; i++)
    fs << i <<" "<< ANn[i][0] << " " << ANn[i][1]<< endl;
  fs.close();
  return;
}

void NetClassNG::ExportNetParametres (const char * name)
{
  ofstream fs;

  // добавить проверки на правильность
  fs.open(name);
  fs <<"NumCoords 2\nNn= "<<Nn<<endl<<"Netopo= "<<NeTopo<<endl<<"NFaceBC= "<<Nbf<<endl;
  fs.close();
  return;
}

void TopoClassNG::ExportTopo (const char * name)
{
  ofstream fs;

  // добавить проверки на правильность
  fs.open(name);
  for(int i=0; i<NeTopo; i++)
  {
      fs<< i<< " ";
      for(int j=IAtopo[i]; j<IAtopo[i+1]; j++) fs<<" "<< JAtopo[j];
      fs<<endl;
  }
  fs.close();
  return;
}

void EdgeClassNG::ExportEdge (const char * name)
{
  ofstream fs;

  // добавить проверки на правильность
  fs.open(name);
  for(int n=0;n<EdgeNumbers;n++)
  {
    fs<<ANbf[n][0]<<" "<<ANbf[n][1]<<" "<<ANbf[n][2]<<" "<<ANbf[n][3]<<endl;
  }
  fs.close();
  return;
}
