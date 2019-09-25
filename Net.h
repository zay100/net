
// виды фигур для элементов топологии - треугольники и прямоугольник или направление первичного элемента от вершины
#define UP_LEFT 1
#define UP_RIGHT 2
#define DOWN_LEFT 3
#define DOWN_RIGHT 4
#define SQUARE 5

// тип разделения прямогольника первичной сетки - два вида диагонали или без разделения
#define NO_DIAG 1
#define DIAG_ASCEND 2
#define DIAG_DESCEND 3
#define NO_ELEMENT -1  // значение не определено, к примеру элемент не существует

// тип первичой ячейки сетки УДАЛИТЬ ИСПОЛЬЗОВАНИЕ
#define CELL_TYPE_K 1
#define CELL_TYPE_M 2

// направления ячеейки от вершины
#define NW 1
#define NE 2
#define SE 3
#define SW 4

#define ERROR -1

#define MAX_ELEMENTS_PER_NODE 8 //максимальное количество элементов в которых может содержаться узел
#define MAX_RIBS_PER_NODE 8 //максимальное количество ребер в которых может содержаться узел

// класс ячеек основной сетки



class EdgeClassNG
{
public:
  int EdgeNumbers; // количество ребер на гранях
  int ** ANbf; // ?? номера двух вершин и номер грания с левой=1 по часовой

  EdgeClassNG (int , int);
  ~EdgeClassNG(){};

  void PrintEdgeArray();

};

class TopoClassNG
{
public:

  int NeTopo; // общее число элементов в топологии
  int NnTopo; // количество узлов в топологии
  // хранение прямой топологии
  int * IAtopo;
  int * JAtopo;
  // хранение обратной топологии
  int * IAtopoRevert;
  int * JAtopoRevert;

  void printTopoElementsNG ();

  TopoClassNG (){}; // конструктор по умолчанию
  ~TopoClassNG(){};
};


class NetClassNG : public EdgeClassNG, public TopoClassNG
{
  public:
  // int type_k; // для данного варианта всегда  такая / диагональ
  int Nx,Ny,Lx,Ly,K,M; // параметры для построения сетки
  int Nn,Nbf,Ncells,Nk,Nm; // число узлов, ребер, прямоугольных ячеек, ячеек типа K, M
  int ANn; //Проверерить где используется, при необходимости удалить
  int NFaceBC;  // количество узлов на внешней грани - разобраться

  int GetPrimaryCellType (int i) { return (i%(K+M)<K)?DIAG_ASCEND:NO_DIAG;} //возвращает тип ячейки по номеру ячейки
  int GetPrimaryCellType (int node, int direction); // возвращает тип ячeйки номеру узла и направлению на ячейку от узла
  int GetVertexX(int i) {return ((i<0)||(i>=Nn))?ERROR:i%Nx;} //Получить номер столбца узла в сетке по номеру узла
  int GetVertexY(int i) {return ((i<0)||(i>=Nn))?ERROR:i/Nx;} //Получить номер строки узла в сетке по номеру узла
  int GetVertexNG (int a, int b) {return (a+Nx*b);} // возврат номера узла по его положению на сетке

  void InitTopo (int K, int M, int Nk, int Nm, int Ncells, int Nx, int Nn);

  NetClassNG (int Nx_, int Ny_ , int K_ , int M_ );

  // int getVertex(int a, int b); // номер вершины по известны x y

};










//класс сетки
class NetClass
{
  public:
  // const static int type_k=DIAG_ASCEND; // для данного варианта всегда  такая / диагональ
  int Nx,Ny,Lx,Ly,K,M; // параметры для построения сетки
  int Nn,Nbf,Ncells,Nk,Nm; // число узлов, ребер, прямоугольных ячеек, ячеек типа K, M
  int ANn;
  int NFaceBC;  //?? количество узлов на внешней грани - разобраться

  class VertexClass * vertex_p; // ссылка на массив узлов
  class CellClass * cell_p; // массив ячеек
  //class _CellClass _Cells;
  class EdgeClass * edge;

  NetClass (int Nx_, int Ny_ , int K_ , int M_ );

  int getVertex(int a, int b); // номер вершины по известны x y

};


class EdgeClass
{
public:
  int number; // количество ребер на гранях
  int ** ANbf; // ?? номера двух вершин и номер грания с левой=1 по часовой
  class NetClass * net_p;
  EdgeClass (class NetClass *);
  ~EdgeClass();
};


// класс элементов топологии - хранит элементы с координатами вершин
class TopoClass
{
public:
  int Ne; // общее число элементов в топологии
  int * IA;
  int * JA;
  class NetClass *net_p;

  TopoClass (class NetClass * net); // конструктор прямой топологии из данных о сети
  TopoClass (class TopoClass * topo); // конструктор обратной топологии на основе прямой
  ~TopoClass();
};

// класс внешних граней
/* class EdgeClass
{
public:
  int number; // количество ребер на гранях
  int ** ANbf; // ?? номера двух вершин и номер грания с левой=1 по часовой
  class NetClass * net_p;
  EdgeClass (class NetClass * net);
  ~EdgeClass();
};
*/
// класс координат вершин по первоначальной сетке
class VertexClass
{
public:
  class NetClass * net_p;
  int number_; //количество вершин
  int ** ANn; // массив координат

  int GetX(int i);
  int GetY(int i);
  VertexClass (class NetClass * net); // стандартный конструктор
  ~VertexClass();
};

// класс ячеек основной сетки
class CellClass
{
public:
  class NetClass * net_p;
  int number_; //количество ячеек
  int * ANcell; // массив со значениями типа ячейки K или М
  int GetType (int i); //возвращает тип ячейки по номеру ячейки
  int GetType (int node, int direction); // возвращает тип ячeйки номеру узла и направлению на ячейку от узла
  CellClass (class NetClass * net); // стандартный конструктор
  ~CellClass();
};

// класс соседей - связей между узлами
class NNadjClass
{
public:
  class NetClass * net_p;
  int * IA; // количество соседей для узла
  int * JA; // номера соседних узлов
  int nnadj_number;

  NNadjClass (class NetClass * net_);
  ~NNadjClass();
};
