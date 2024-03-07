#include "heap.h"

#define bug cout<<"bug "<<__LINE__<<endl

const int inf=1e9+7;
const int MAXR=1050;

using namespace std;

NODE::NODE(int _val, bool _marked, int _x, int _y, bool _isMinRoot)
{
    val = _val, marked =  _marked, x = _x, y = _y, isMinRoot = _isMinRoot;
}

NODES::NODES()
{
    nodeList.clear();
    maxx = maxy = 0;
}

void NODES::AddNode(NODE tmp)
{
    nodeList.push_back(tmp);
    maxx = max(maxx, tmp.x);
    maxy = max(maxy, tmp.y);
}

void NODES::clear()
{
    nodeList.clear();
    maxx = maxy = 0;
}

LINE::LINE(int _x1, int _y1, int _x2, int _y2)
{
    x1 = _x1, y1 = _y1;
    x2 = _x2, y2 = _y2;
}

LINES::LINES()
{
    lineList.clear();
}

void LINES::AddLine(LINE tmp)
{
    lineList.push_back(tmp);
}

void LINES::clear()
{
    lineList.clear();
}

FibonacciHeap::Node::Node(int _val, bool _isroot, bool _marked)
{
    val = _val;
    isroot = _isroot;
    marked = _marked;
    son.clear();
    lnext = rnext = fa = nullptr;
    xpos = ypos = 0;
}

// FibonacciHeap::Node::~Node()
// {
//     for(Node* curson: son)
//         delete curson;
// }

//************************************************************* ROOT
void FibonacciHeap::InsertRoot(Node* NewRoot)
{
    assert(NewRoot->fa == nullptr);
    assert(NewRoot->isroot);

    if(MinRoot == nullptr)
    {
        MinRoot = NewRoot;
        NewRoot->lnext = NewRoot->rnext = NewRoot;
        return ;
    }

    Node* oriL = MinRoot->lnext;

    oriL->rnext = NewRoot;
    MinRoot->lnext = NewRoot;
    NewRoot->lnext = oriL;
    NewRoot->rnext = MinRoot;

    if(NewRoot->val < MinRoot->val)
        MinRoot = NewRoot;
}

void FibonacciHeap::UpdateMin(Node* cur)
{
    assert(cur->fa == nullptr);
    assert(cur->isroot);

    Node* start = cur;
    int MIN = cur->val;
    MinRoot = cur;
    cur = cur->rnext;

    while(cur != start)
    {
        if(cur->val < MIN)
        {
            MIN = cur->val;
            MinRoot = cur;
        }
        cur = cur->rnext;
    }
}

void FibonacciHeap::RemoveRoot(Node* ToDel)
{
    assert(ToDel->fa == nullptr);
    assert(ToDel->isroot);

    if(ToDel->lnext == ToDel)
    {
        MinRoot = nullptr;
        return ;
    }
    Node *oriL = ToDel->lnext, *oriR = ToDel->rnext;
    oriL->rnext = oriR;
    oriR->lnext = oriL;
    ToDel->lnext = ToDel->rnext = nullptr;

    if(ToDel == MinRoot)
        UpdateMin(oriL);
}

void FibonacciHeap::SwapRoot(Node* x, Node* y)
{
    assert(x->isroot == 1);
    assert(x->fa == nullptr);
    assert(y->isroot == 1);
    assert(y->fa == nullptr);

    Node* orixl = x->lnext, *orixr = x->rnext;
    Node* oriyl = y->lnext, *oriyr = y->rnext;

    x->lnext = oriyl, x->rnext = oriyr;
    y->lnext = orixl, y->rnext = orixr;
}

//************************************************************* NODE
void FibonacciHeap::AddSon(Node* fa, Node* son)
{
    assert(son->fa == nullptr);

    fa->son.push_back(son);
    son->fa = fa;
    son->isroot = 0;
}

void FibonacciHeap::RemoveSon(Node* fa, Node* son)
{
    auto sonit = find(fa->son.begin(), fa->son.end(), son);
    assert(sonit != fa->son.end());

    fa->son.erase(sonit);
    son->fa = nullptr;
}

void FibonacciHeap::PointwiseUnion(Node** rank, Node* x, Node* y)
{
    assert(x->son.size() == y->son.size());

    int orirank = x->son.size();

    Node* fa;

    if((y->val < x->val) || (y->val == x->val && y == MinRoot))
    {
        RemoveRoot(x);
        AddSon(y, x);
        fa = y;
    }
    else
    {
        RemoveRoot(y);
        AddSon(x, y);
        fa = x;
    }

    rank[orirank] = nullptr;

    if(rank[orirank + 1] == nullptr)
        rank[orirank + 1] = fa;
    else
        PointwiseUnion(rank, fa, rank[orirank + 1]);
}

void FibonacciHeap::_Check(Node* cur, int depth)
{
    assert(cur->val >= MinRoot->val);

#ifdef DEBUG
    for(int i = 1; i < depth; i++) std::cout << "    ";
    std::cout << "(" << cur->val << ", " << cur->marked << ") : ";
    for(Node* son: cur->son) std::cout << son->val <<" "; std::cout << std::endl;
#endif

    potential += 2 * (cur->marked);

    for(Node* son: cur->son)
    {
        assert(son->fa == cur);
        assert(son->val >= cur->val);
        _Check(son, depth + 1);
    }
}

int FibonacciHeap::CalcPos(Node* cur, NODES &nodes, LINES &lines, int xbase, int ybase) // return the right-most position of this subtree
{
    int SPACE = 20;

    cur->ypos = ybase;
    if(cur->son.size() == 0)
    {
        cur->xpos = xbase;
        nodes.AddNode(NODE(cur->val, cur->marked, cur->xpos, cur->ypos, cur==MinRoot));
        return cur->xpos;
    }
    int Nextxpos = xbase;
    for(Node* son: cur->son)
    {
        Nextxpos = SPACE + CalcPos(son, nodes, lines, Nextxpos, ybase + SPACE);
    }

    int RightMostxpos = Nextxpos - SPACE;
    cur->xpos = (xbase + RightMostxpos) / 2;
    nodes.AddNode(NODE(cur->val, cur->marked, cur->xpos, cur->ypos, cur==MinRoot));

    for(Node* son: cur->son)
    {
        lines.AddLine(LINE(cur->xpos, cur->ypos, son->xpos, son->ypos));
    }

    return RightMostxpos;
}

FibonacciHeap::FibonacciHeap()
{
    MinRoot = nullptr;
    mp.clear();
}

FibonacciHeap::~FibonacciHeap()
{
    mp.clear();
    Node* cur = MinRoot->rnext;
    while(cur != MinRoot)
    {
        Node* nxt = cur->rnext;
        delete cur;
        cur = nxt;
    }
    delete MinRoot;
}

//************************************************************* OPERATION
void FibonacciHeap::Insert(int val)
{
    Node* cur = new Node(val, 1);
    InsertRoot(cur);
    mp[val].insert(cur);
}

void FibonacciHeap::DeleteMin()
{
    if(MinRoot == nullptr)
    {
        //std::cout << "Heap is empty!" << std::endl;
        throw string("Heap is empty.\n");
        return ;
    }

    Node* old = MinRoot;
    RemoveRoot(old);

    for(Node* cur: old->son)
    {
        cur->isroot = 1;
        cur->fa = nullptr;
        InsertRoot(cur);
    }

    mp[old->val].erase(old);
    delete old;

    if(MinRoot == nullptr)
        return ;

    Node** rank = new Node*[MAXR]; for(int i = 0; i < MAXR; i++) rank[i] = nullptr;

    vector<Node*> roots;
    roots.push_back(MinRoot);
    Node* _cur = MinRoot->rnext;
    while(_cur != MinRoot)
    {
        roots.push_back(_cur);
        _cur = _cur->rnext;
    }

    for(Node* cur: roots)
    {
        int currank = cur->son.size();
        if(rank[currank] == nullptr)
        {
            rank[currank] = cur;
            continue;
        }
        PointwiseUnion(rank, cur, rank[currank]);
    }
}

void FibonacciHeap::DecreaseKey(int val, int delta)
{
    if(mp[val].empty())
    {
        //std::cout << "Key not found!" << std::endl;
        throw string("Key not found.\n");
        return ;
    }

    Node* cur = *mp[val].begin();
    mp[val].erase(cur);
    cur->val -= delta;
    mp[val-delta].insert(cur);

    if(cur->isroot)
    {
        cur->marked = 0;
        if(cur->val < MinRoot->val)
            MinRoot = cur;
        return ;
    }

    Node* p = cur->fa;
    while(1)
    {
        RemoveSon(p, cur);
        cur->isroot = 1;
        cur->marked = 0;
        InsertRoot(cur);

        if(p->isroot)
            break;

        if(!p->marked)
        {
            p->marked = 1;
            break;
        }

        cur = p;
        p = cur->fa;
    }
}

void FibonacciHeap::Delete(int val)
{
    try {
        DecreaseKey(val, inf);
    } catch(const string& message) {
        throw message;
    }
    DeleteMin();
}

/*
FibonacciHeap* FibonacciHeap::Union(FibonacciHeap *x, FibonacciHeap *y)
{
    if(x->MinRoot == nullptr)
        return y;
    if(y->MinRoot == nullptr)
        return x;

    if(x->MinRoot->val > y->MinRoot->val)
        swap(x, y);

    bool xsingle = x->MinRoot->rnext == x->MinRoot;
    bool ysingle = y->MinRoot->lnext == y->MinRoot;
    if(xsingle && ysingle)
    {
        x->MinRoot->rnext = x->MinRoot->lnext = y->MinRoot;
        y->MinRoot->rnext = y->MinRoot->lnext = x->MinRoot;
        for(auto &i: y->mp)
            x->mp[i.first].insert(i.second.begin(), i.second.end());
        y->MinRoot = nullptr;
        y->mp.clear();
        return x;
    }
    if(xsingle)
    {
        x->MinRoot->lnext = y->MinRoot->lnext;
        y->MinRoot->lnext->rnext = x->MinRoot;
        x->MinRoot->rnext = y->MinRoot;
        y->MinRoot->lnext = x->MinRoot;
        for(auto &i: y->mp)
            x->mp[i.first].insert(i.second.begin(), i.second.end());
        y->MinRoot = nullptr;
        y->mp.clear();
        return x;
    }
    if(ysingle)
    {
        y->MinRoot->lnext = x->MinRoot->lnext;
        x->MinRoot->lnext->rnext = y->MinRoot;
        y->MinRoot->rnext = x->MinRoot;
        x->MinRoot->lnext = y->MinRoot;
        for(auto &i: y->mp)
            x->mp[i.first].insert(i.second.begin(), i.second.end());
        y->MinRoot = nullptr;
        y->mp.clear();
        return x;
    }

    x->MinRoot->rnext->rnext = y->MinRoot->lnext;
    y->MinRoot->lnext->lnext = x->MinRoot->rnext;
    x->MinRoot->rnext = y->MinRoot;
    y->MinRoot->lnext = x->MinRoot;

    for(auto &i: y->mp)
        x->mp[i.first].insert(i.second.begin(), i.second.end());

    y->MinRoot = nullptr;
    y->mp.clear();

    return x;
}
*/
void FibonacciHeap::Union(FibonacciHeap* &x, FibonacciHeap* &y)
{
    FibonacciHeap* newheap = new FibonacciHeap;
    for(auto &i: x->mp)
        newheap->mp[i.first].insert(i.second.begin(), i.second.end());
    for(auto &i: y->mp)
        newheap->mp[i.first].insert(i.second.begin(), i.second.end());
    newheap->MinRoot = (x->MinRoot->val < y->MinRoot->val)? x->MinRoot : y->MinRoot;

    bool xsingle = (x->MinRoot->rnext == x->MinRoot);
    bool ysingle = (y->MinRoot->lnext == y->MinRoot);
    if(xsingle && ysingle)
    {
        x->MinRoot->rnext = x->MinRoot->lnext = y->MinRoot;
        y->MinRoot->rnext = y->MinRoot->lnext = x->MinRoot;
    }
    else if(xsingle)
    {
        x->MinRoot->lnext = y->MinRoot->lnext;
        y->MinRoot->lnext->rnext = x->MinRoot;
        x->MinRoot->rnext = y->MinRoot;
        y->MinRoot->lnext = x->MinRoot;
    }
    else if(ysingle)
    {
        y->MinRoot->lnext = x->MinRoot->lnext;
        x->MinRoot->lnext->rnext = y->MinRoot;
        y->MinRoot->rnext = x->MinRoot;
        x->MinRoot->lnext = y->MinRoot;
    }
    else
    {
        x->MinRoot->rnext->lnext = y->MinRoot->lnext;
        y->MinRoot->lnext->rnext = x->MinRoot->rnext;
        x->MinRoot->rnext = y->MinRoot;
        y->MinRoot->lnext = x->MinRoot;
    }

    x = newheap;
    y = new FibonacciHeap;
}

void FibonacciHeap::Check()
{
    if(MinRoot == nullptr)
    {
        potential = 0;
        return ;
    }

#ifdef DEBUG
    std::cout << "MinRoot: " << MinRoot->val << std::endl;
#endif

    potential = 1;

    vector<Node*> tor, tol;
    tor.clear(); tol.clear();
    for(Node* cur = MinRoot->rnext; cur != MinRoot; cur = cur->rnext)
        tor.push_back(cur);
    for(Node* cur = MinRoot->lnext; cur != MinRoot; cur = cur->lnext)
        tol.push_back(cur);
    reverse(tor.begin(), tor.end());
    assert(tor.size() == tol.size());
    for(int i = 0; i < (int)tor.size(); i++)
        assert(tol[i] == tor[i]);

    potential += tor.size();

    Node* cur = MinRoot;

    while(1)
    {
        _Check(cur, 1);
        cur = cur->rnext;
        if(cur == MinRoot)
            break;
    }
}

void FibonacciHeap::GetPaint(NODES &nodes, LINES &lines)
{
    if(MinRoot == nullptr)
        return ;

    Node* cur = MinRoot;
    do
    {
        int basex = nodes.maxx;
        CalcPos(cur, nodes, lines, basex + 20, 10);
        if(cur != MinRoot && cur->lnext != cur)
        {
            lines.AddLine(LINE(cur->xpos, cur->ypos, cur->lnext->xpos, cur->lnext->ypos));
        }
        cur = cur->rnext;
    }while(cur != MinRoot);
}

bool FibonacciHeap::isEmpty()
{
    return MinRoot == nullptr;
}

FibonacciHeap::Node* FibonacciHeap::GetMinRoot()
{
    return MinRoot;
}

void FibonacciHeap::SetMinRoot(FibonacciHeap::Node* _MinRoot)
{
    MinRoot = _MinRoot;
}

int FibonacciHeap::GetPotential()
{
    return potential;
}

vector<FibonacciHeap*> heaps;

//************************************************************* MODEL API

void Insert(int heapid, int val)
{
    if(heapid <= 0 || heapid > (int)heaps.size() + 1)
    {
        //std::cout << "Invalid heapID" << std::endl;
        throw string("Invalid HeapID");
        return ;
    }
    if(heapid == (int)heaps.size() + 1)
        heaps.push_back(new FibonacciHeap);

    heaps[heapid - 1]->Insert(val);
}

void DeleteMin(int heapid)
{
    if(heapid <= 0 || heapid > (int)heaps.size())
    {
        //std::cout << "Invalid heapID" << std::endl;
        throw string("Invalid HeapID.");
        return ;
    }

    try {
        heaps[heapid - 1]->DeleteMin();
    } catch(const string& message) {
        throw message;
    }
}

void DecreaseKey(int heapid, int val, int delta)
{
    if(heapid <= 0 || heapid > (int)heaps.size())
    {
        //std::cout << "Invalid heapID" << std::endl;
        throw string("Invalid HeapID.");
        return ;
    }
    if(delta < 0)
    {
        throw string("Para 2 should be non negative under DecreaseKey");
        return ;
    }

    try {
        heaps[heapid - 1]->DecreaseKey(val, delta);
    } catch(const string& message) {
        throw message;
    }
}

void Delete(int heapid, int val)
{
    if(heapid <= 0 || heapid > (int)heaps.size())
    {
        //std::cout << "Invalid heapID" << std::endl;
        throw string("Invalid HeapID.");
        return ;
    }

    try {
        heaps[heapid - 1]->Delete(val);
    } catch(const string& message) {
        throw message;
    }
}

void Union(int heapid1, int heapid2)
{
    if(heapid1 <= 0 || heapid1 > (int)heaps.size())
    {
        //std::cout << "Invalid heapID" << std::endl;
        throw string("Invalid HeapID.");
        return ;
    }
    if(heapid2 <= 0 || heapid2 > (int)heaps.size())
    {
        //std::cout << "Invalid heapID" << std::endl;
        throw string("Invalid HeapID.");
        return ;
    }
    heaps[heapid1 - 1]->Union(heaps[heapid1 - 1], heaps[heapid2 - 1]);
}

string GetPaint(NODES &nodes, LINES &lines)
{
    string potentialinfo = "";

    vector<FibonacciHeap*> newheaps;

    int index = 0;
    for(FibonacciHeap* cur: heaps)
    {
        if(!cur->isEmpty())
        {
            newheaps.push_back(cur);
            cur->Check();
            potentialinfo += "Heap No." + to_string(++index) + " has potential " + to_string(cur->GetPotential()) + ".\n";
        }
    }
    heaps = newheaps;

    for(FibonacciHeap* heap: heaps)
    {
        heap->GetPaint(nodes, lines);
    }

    return potentialinfo;
}

void ClearHeap()
{
    for(FibonacciHeap* heap: heaps)
        delete heap;
    heaps.clear();
}

void LoadHeap(QTextStream in)
{
    unsigned long long hash = 0;

    vector<FibonacciHeap*> tmpheaps;
    int heapscnt = 0;
    in >> heapscnt;
    hash = hash*31 + heapscnt;

    while(heapscnt--)
    {
        FibonacciHeap* curheap = new FibonacciHeap;

        int n, m, rootnum;
        map<int, FibonacciHeap::Node*> mp;
        in >> n >> rootnum;
        hash = hash*31 + n;
        hash = hash*31 + rootnum;

        for(int i = 1; i <= n; i++)
        {
            int val, marked;//, xpos, ypos;
            in >> val >> marked;// >> xpos >> ypos;
            hash = hash*31 + val;
            hash = hash*31 + marked;
            //hash = hash*31 + xpos;
            //hash = hash*31 + ypos;

            FibonacciHeap::Node* curnode = new FibonacciHeap::Node(val, i<=rootnum, marked);
            //curnode->xpos = xpos, curnode->ypos = ypos;
            mp[i] = curnode;

            curheap->mp[val].insert(curnode);
        }

        curheap->SetMinRoot(mp[1]);
        for(int i = 1; i <= rootnum; i++)
        {
            mp[i]->rnext = mp[(i == rootnum)?(1):(i + 1)];
            mp[i]->lnext = mp[(i == 1)?(rootnum):(i - 1)];
        }

        in >> m;
        hash = hash*31 + m;

        while(m--)
        {
            int fa, son;
            in >> fa >> son;
            hash = hash*31 + fa;
            hash = hash*31 + son;
            mp[fa]->son.push_back(mp[son]);
            mp[son]->fa = mp[fa];
        }

        tmpheaps.push_back(curheap);
    }

    unsigned long long std_hash;
    in >> std_hash;
    if(hash != std_hash)
        throw std::string("File broken. Cannot Load.");

    heaps.clear();
    for(FibonacciHeap* heap: tmpheaps)
        heaps.push_back(heap);
}

void Save_DFS(FibonacciHeap::Node* cur, int &cnt, map<int, FibonacciHeap::Node*> &mp, vector<pair<int, int>> &relation, int faid)
{
    int curid;
    if(faid <= 0)
    {
        curid = -faid;
    }
    else
    {
        curid = ++cnt;
        relation.push_back(make_pair(faid, cnt));
        mp[curid] = cur;
    }
    for(FibonacciHeap::Node* son: cur->son)
    {
        Save_DFS(son, cnt, mp, relation, curid);
    }
}

void SaveHeap(QTextStream out)
{
    unsigned long long hash = 0;
    out << heaps.size() << '\n';
    hash = hash*31 + heaps.size();
    for(FibonacciHeap* heap: heaps)
    {
        int cnt = 0;
        map<int, FibonacciHeap::Node*> mp;
        vector<pair<int, int>> relation; // (fa, son)

        FibonacciHeap::Node* cur = heap->GetMinRoot();
        do
        {
            mp[++cnt] = cur;
            cur = cur->rnext;
        }while(cur != heap->GetMinRoot());
        int rootnum = cnt;

        for(int i = 1; i <= rootnum; i++)
        {
            Save_DFS(mp[i], cnt, mp, relation, -i);
        }

        out << cnt <<" "<< rootnum << '\n';
        hash = hash*31 + cnt;
        hash = hash*31 + rootnum;
        for(int i = 1; i <= cnt; i++)
        {
            FibonacciHeap::Node* cur = mp[i];
            out << cur->val <<" "<< cur->marked << '\n';// <<" "<< cur->xpos <<" "<< cur->ypos << '\n';
            hash = hash*31 + cur->val;
            hash = hash*31 + cur->marked;
            //hash = hash*31 + cur->xpos;
            //hash = hash*31 + cur->ypos;
        }

        out << relation.size() << '\n';
        hash = hash*31 + relation.size();
        for(pair<int, int> rel: relation)
        {
            out << rel.first <<" "<< rel.second << '\n';
            hash = hash*31 + rel.first;
            hash = hash*31 + rel.second;
        }
    }
    out << hash << '\n';
}
