#include <iostream>
#include "Net.h"


//расчет Nn-Nm, типов прямоугольников
NetClass::NetClass (int Nx_, int Ny_ , int K_, int M_)
{
  Nx=Nx_;
  Ny=Ny_;
  K=K_;
  M=M_;
  Nn=Nx_*Ny_;
  Ncells = (Nx_-1)*(Ny_-1);
  Nk=Ncells/(K_+M_)*K+(((Ncells%(K_+M_))>=K_)?K_:Ncells%(K_+M_));
  Nm=Ncells-Nk;
  Nbf=(Nx_-1)*2+(Ny_-1)*2;
  NFaceBC=(Nx+Ny)*2-4;
}

// номер вершины по известны x y
int NetClass::getVertex(int a, int b)
{
  return (a+Nx*b);
}

// конструктор прямой топологии из данных о сети
TopoClass::TopoClass (class NetClass * net)
{
  net_p=net;
  Ne = net_p->Nk*2+net_p->Nm; // количество элементов топологии
  int JAmax = net_p->Nk*3*2+net_p->Nm*4; //количество вершин

  IA = new int[Ne+1];
  if(!IA)std::cout<<"Ошибка при создании массива IA"<<std::endl;
  JA = new int[JAmax];
  if(!IA)std::cout<<"Ошибка при создании массива JA"<<std::endl;
  IA[0]=0;
  IA[1]=0;

  int base, count = 0; // номер первой вершины очередного элемента в JA, номер очередного элемента топологии
  for (int p=0; p<net_p->Ncells; p++) //пробегаем по всем первичным ячейкам
  {
    int row = p/(net_p->Nx-1); // определяем столбец и колонку для очередной ячейки
    int col = p%(net_p->Nx-1); // он же эквивалент для левого верхнего узла
    if (p%(net_p->K+net_p->M)<net_p->K)  // для ячеек типа к добавить два элемента
    {
//        addTopoElement(p, UP_LEFT);
        base = IA[count];
        JA[base]=net_p->getVertex(col, row);
        JA[base+1]=net_p->getVertex(col+1,row);
        JA[base+2]=net_p->getVertex(col,row+1);
        IA[count+1]=base+3;
        count++;
//        addTopoElement(p, DOWN_RIGHT);
        base = IA[count];
        JA[base]=net_p->getVertex(col+1, row);
        JA[base+1]=net_p->getVertex(col+1,row+1);
        JA[base+2]=net_p->getVertex(col,row+1);
        IA[count+1]=base+3;
        count++;
    }
    else  // ячейки типа М
    {
//        addTopoElement(p, SQUARE);
        base = IA[count];
        JA[base]=net_p->getVertex(col, row);
        JA[base+1]=net_p->getVertex(col+1,row);
        JA[base+2]=net_p->getVertex(col+1,row+1);
        JA[base+3]=net_p->getVertex(col,row+1);
        IA[count+1]=base+4;
        count++;
    }
  }
}

// конструктор обратной топологии на основе прямой
TopoClass::TopoClass (class TopoClass * topo)
{
  net_p=topo->net_p;
  Ne = net_p->Nn;
  int ** temp_node_array; // массив для группировки элементов по вершинам
  temp_node_array = new int * [net_p->Nn]; // количество строк равно количеству элементов
  for (int i=0; i< net_p->Nn; i++) // столбцов по максимуму элементов плюс один на счетчик кол-ва
    temp_node_array[i] = new int [MAX_ELEMENTS_PER_NODE+1];
  // инициализация промежуточного массива в -1
  for (int i=0; i< net_p->Nn; i++)
    for (int j=0; j<MAX_ELEMENTS_PER_NODE+1;j++)
      temp_node_array[i][j]=0;

  // заполнение промежуточного массива первый столбец - кол-во элементов для вершины далее номера элементов топологии
  int node, tmp, counter=0;
  for (int i=0; i< topo->Ne; i++) // i - номер элемента топологии
  {
    for (int j=topo->IA[i];j<topo->IA[i+1];j++)
    {
      node=topo->JA[j];
      tmp = temp_node_array[node][0]; //количество уже записанных элементов для вершины
      temp_node_array[node][tmp+1]=i; // добавляем номер элемента в следующую свободную ячейку для вершины
      temp_node_array[topo->JA[j]][0]++; // увеличиваем счетчик добавленных элементов
      counter++;
    }
  }

  IA = new int [Ne+1];
  JA = new int  [counter];
  // заполнение обратной топологии
  IA[0]=0;
  int base;
  for (int i=0; i<net_p->Nn; i++)
  {
    base = IA[i];
    IA[i+1]=IA[i]+temp_node_array[i][0];
    for (int j=0; j<temp_node_array[i][0]; j++)
      JA[base+j]=temp_node_array[i][j+1];
  }

  // !!!!! УДАЛИТЬ АККУРАТНО ИНАЧЕ УТЕЧКА ПАМЯТИ
  delete [] temp_node_array ;
}

TopoClass::~TopoClass()
{
  delete[] IA;
  delete[] JA;
}

// класс внешних граней
EdgeClass::EdgeClass (class NetClass * net)
{
  net_p = net;
  number = ((net->Nx-1)+(net->Ny-1))*2;
  ANbf = new int * [number];  // выделение памяти для массива ребер граней
  for (int i=0; i< number; i++)
    ANbf[i] = new int [4];

  int n, m=0;
  // верхняя грань
  for(n=0; n<net_p->Nx-1; n++)
  {
      ANbf[m][0]=net_p->Nx;
      ANbf[m][1]=n;
      ANbf[m][2]=n+1;
      ANbf[m][3]=2;
      m++;
  }
  // правая грань
  for(n=0; n<net_p->Ny-1; n++)
  {
      ANbf[m][0]=net_p->Ny;
      ANbf[m][1]=(n+1)*net_p->Nx-1;
      ANbf[m][2]=(n+2)*net_p->Nx-1;
      ANbf[m][3]=3;
      m++;
  }
  // нижняя грань
  int base=net_p->Nx*(net_p->Ny-1);
  for(n=0; n<net_p->Nx-1; n++)
  {
      ANbf[m][0]=net_p->Nx;
      ANbf[m][1]=base+n;
      ANbf[m][2]=base+n+1;
      ANbf[m][3]=4;
      m++;
  }
  // левая грань
  for(n=0; n<net_p->Ny-1; n++)
  {
      ANbf[m][0]=net_p->Ny;
      ANbf[m][1]=net_p->Nx*n;
      ANbf[m][2]=net_p->Nx*(n+1);
      ANbf[m][3]=1;
      m++;
  }
}

EdgeClass::~EdgeClass()
{
  // добавить уничтожение массива, если будут динамически создаваться/переписываться
}

// класс координат вершин по первоначальной сетке
int VertexClass::GetX(int i)
{
  return ((i<0)||(i>=number_))?ERROR:ANn[i][0];
}

int VertexClass::GetY(int i)
{
  return (i<0||i>=number_)?ERROR:ANn[i][1];
}

VertexClass::VertexClass (class NetClass * net) // стандартный конструктор
{
  net_p = net;
  net_p->vertex_p = this;
  number_ = net->Nn;
  ANn=new int * [number_];
  for (int i=0; i<number_; i++) // создание двумерного массива нужного размера
  {
	     ANn[i] = new int [2];
	     if (!ANn[i]) std::cout<<"ошибка создания массива"<<std::endl;
  }
  for (int i=0; i<net->Nn; i++) // заполнение координат вершин квадратной сетки
  {
    ANn[i][0]=i%net->Nx;
    ANn[i][1]=i/net->Nx;
  }
}

VertexClass::~VertexClass()
{
  // добавить уничтожение массива
}

//возвращает тип ячейки по номеру ячейки
int CellClass::GetType (int i)
{
  if (i<0||i>=number_) return NO_ELEMENT;
  return ANcell[i];
}

// возвращает тип ячeйки номеру узла и направлению на ячейку от узла
int CellClass::GetType (int node, int direction)
{
    int cell_number;
    class VertexClass * vertex_p= net_p->vertex_p;

    int x=vertex_p->GetX(node);
    int y=vertex_p->GetY(node);
    int Nx=net_p->Nx;
    int Ny=net_p->Nx;
    switch (direction)
    {
      case NW:
          if (x==0 || y==0) return NO_ELEMENT;
          cell_number = (y-1)*(Nx-1)+x-1;
          return GetType (cell_number);
      case NE:
          if (x==Nx || y==0) return NO_ELEMENT;
          cell_number = (y-1)*(Nx-1)+x;
          return GetType (cell_number);
      case SE:
          if (x==Nx || y==Ny) return NO_ELEMENT;
          cell_number = y*(Nx-1)+x;
          return GetType (cell_number);
          break;
      case SW:
          if (x==0 || y==Ny) return NO_ELEMENT;
          cell_number = y*(Nx-1)+x-1;
          return GetType (cell_number);
      default:
          return NO_ELEMENT;
    }
}

// стандартный конструктор
CellClass::CellClass (class NetClass * net)
{
  net_p = net;
  number_ = (net->Nx-1)*(net->Ny-1);
  ANcell=new int [number_];
  for (int i=0; i<number_; i++) // заполнение вектора типом ячеек
  {
	     ANcell[i] = (i%(net_p->K+net_p->M)<net_p->K)?DIAG_ASCEND:NO_DIAG;
	}
}

CellClass::~CellClass()
{
  // добавить уничтожение массива
}

NNadjClass::NNadjClass (class NetClass * net_)
{
  net_p=net_;
  class CellClass * cell_p = net_p->cell_p; // локальное переопределение для читабельности
  nnadj_number=net_p->Nn*8; // с запасом, оценить точнее
  IA = new int [net_p->Nn+1];
  JA = new int [nnadj_number];
  IA[0]=IA[1]=0;
  int m=0, base;
  for (int i=0; i<net_p->Nn; i++) // перебираем все точки
  {
    m=0;
    base = IA[i];
    if ((i%net_p->Nx)<(net_p->Nx-1))//ребро справа
    {
      JA[base+m]=i+1;
      m++;
    }
    if ((i%net_p->Nx)!=0)//ребро слева
    {
      JA[base+m]=i-1;
      m++;
    }
    if (i>=net_p->Nx)//ребро сверху и верхние диагонали
    {
      JA[base+m]=i-net_p->Nx;
      m++;
      if (cell_p->GetType(i, NW)==DIAG_DESCEND)
      {
        JA[base+m]=i-net_p->Nx-1;
        m++;
      }
      if (cell_p->GetType(i, NE)==DIAG_ASCEND)
      {
        JA[base+m]=i-net_p->Nx+1;
        m++;
      }
    }
    if (i < net_p->Nn - net_p->Nx)//ребро снизу и нижние диагонали
    {
      JA[base+m]=i+net_p->Nx;
      m++;
      if (cell_p->GetType(i, SW)==DIAG_ASCEND)
      {
        JA[base+m]=i+net_p->Nx-1;
        m++;
      }
      if (cell_p->GetType(i, SE)==DIAG_DESCEND)
      {
        JA[base+m]=i+net_p->Nx+1;
        m++;
      }
    }
    IA[i+1]=base+m;
  }
  nnadj_number=IA[net_p->Nn];
}

NNadjClass::~NNadjClass()
{
  delete[] IA;
  delete[] JA;
}
