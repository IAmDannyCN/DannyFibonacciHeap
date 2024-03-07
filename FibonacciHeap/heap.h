#ifndef HEAP_H
#define HEAP_H

#include <bits/stdc++.h>
#include <QMessageBox>
#include <QFile>
#include <QIODevice>
#include <QFileDialog>


using namespace std;

#define index xedni
#define x1 xxx
#define y1 yyy

class NODE
{
public:
    NODE(int _val=0, bool _marked=0, int _x=0, int _y=0, bool _isMinRoot=0);
    int val;
    bool marked;
    int x, y;
    bool isMinRoot;
};
class NODES
{
public:
    NODES();
    vector<NODE> nodeList;
    int maxx, maxy;
    void AddNode(NODE tmp);
    void clear();
};

class LINE
{
public:
    LINE(int _x1=0, int _y1=0, int _x2=0, int _y2=0);
    int x1, y1, x2, y2;
};
class LINES
{
public:
    LINES();
    vector<LINE> lineList;
    void AddLine(LINE tmp);
    void clear();
};

class FibonacciHeap
{
public:
    class Node
    {
    public:
        Node(int _val, bool _isroot, bool _marked=0);
        //~Node();
        int val;
        bool marked, isroot;
        vector<Node*> son;
        Node* lnext, *rnext, *fa;
        int xpos, ypos;
    };
    map<int, set<Node*> > mp;

private:
    Node* MinRoot;
    int potential;

    //************************************************************* ROOT
    void InsertRoot(Node* NewRoot);
    void UpdateMin(Node* cur);
    void RemoveRoot(Node* ToDel);
    void SwapRoot(Node* x, Node* y);

    //************************************************************* NODE
    void AddSon(Node* fa, Node* son);
    void RemoveSon(Node* fa, Node* son);
    void PointwiseUnion(Node** rank, Node* x, Node* y);
    void _Check(Node* cur, int depth);
    int CalcPos(Node* cur, NODES &nodes, LINES &lines, int xbase, int ybase); // return the right-most position of this subtree

public:
    FibonacciHeap();
    ~FibonacciHeap();

    //************************************************************* OPERATION
    void Insert(int val);
    void DeleteMin();
    void DecreaseKey(int val, int delta);
    void Delete(int val);
    //FibonacciHeap* Union(FibonacciHeap *x, FibonacciHeap *y);
    void Union(FibonacciHeap* &x, FibonacciHeap* &y);
    void Check();
    void GetPaint(NODES &nodes, LINES &lines);
    bool isEmpty();

    Node* GetMinRoot();
    void SetMinRoot(Node* _MinRoot);
    int GetPotential();
};

void Insert(int heapid, int val);
void DeleteMin(int heapid);
void DecreaseKey(int heapid, int val, int delta);
void Delete(int heapid, int val);
void Union(int heapid1, int heapid2);
string GetPaint(NODES &nodes, LINES &lines);

void ClearHeap();
void LoadHeap(QTextStream in);
void SaveHeap(QTextStream out);

#endif // HEAP_H
