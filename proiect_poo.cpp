#include<iostream>
#include<cstdlib>

using namespace std;
using uint = unsigned;

#define DYN_ALLOC_ARR_SIZE 100000

template<class T>
class dyn_alloc_arr //array unidimensional alocat dinamic
{
    T* v;
    uint size;
    void dyn_alloc(uint newsize)
    {
        size=2*newsize;
        v=(T*)realloc(v,size*sizeof(T));
    }
public:
    dyn_alloc_arr() //construire fara parametrii
    {
        v=(T*)calloc(DYN_ALLOC_ARR_SIZE,sizeof(T));
        size=DYN_ALLOC_ARR_SIZE;
    }
    dyn_alloc_arr(const dyn_alloc_arr& d) //copy constr
    {
        size=d.size;
        v=(T*)calloc(size,sizeof(T));
        for(uint i=0;i<size;i++)
            v[i]=d.v[i];
    }
    dyn_alloc_arr(dyn_alloc_arr&& d) //move constr
    {
        size=d.size;
        v=d.v;
    }
    ~dyn_alloc_arr()
    {
        free(v);
    }
    T& at(uint pos)
    {
        if(pos>size)
            dyn_alloc(pos);
        return v[pos];
    }
};

template<class T>
class vector : public dyn_alloc_arr<T>
{
    uint top;
public:
    vector() : dyn_alloc_arr<T>()
    {
        top=0;
    }
    vector(const vector& v) : dyn_alloc_arr<T>(v)
    {
        top=v.top;
    }
    vector(vector&& v) : dyn_alloc_arr<T>(v)
    {
        top=v.top;
    }
    T& operator[](uint i)
    {
        top=i>=top ? i+1:top;
        return this->at(i);
    }
    bool empty()
    {
        return !top;
    }
    uint size()
    {
        return top;
    }
    void push_back(T x)
    {
        this->at(top)=x;
        top++;
    }
    void push_front(T x)
    {
        for(uint i=top;i;i--)
            this->at(i)=this->at(i+1);
        this->at(0)=x;
        top++;
    }
    void operator=(vector<T> v)
    {
        uint max_top = top>v.top ? top:v.top;
        for(uint i=0;i<max_top;i++)
            this->at(i)=v[i];
        top=max_top;
    }
    T* begin()
    {
        return &(this->at(0));
    }
    T* end()
    {
        return &(this->at(top));
    }
};

template<class T>
void swap(vector<T>& v,vector<T>& v2)
{
    vector<T> vaux = v;

    v=v2;
    v2=vaux;
}

template<class T>
class stack : private dyn_alloc_arr<T>
{
    uint stack_top;
public:
    stack() : dyn_alloc_arr<T>()
    {
        stack_top=0;
    }
    stack(const stack& s) : dyn_alloc_arr<T>(s)
    {
        stack_top=s.stack_top;
    }
    stack(stack&& s) : dyn_alloc_arr<T>(s)
    {
        stack_top=s.stack_top;
    }
    void push(T x)
    {
        this->at(stack_top)=x;
        stack_top++;
    }
    void pop()
    {
        if(!stack_top)
            throw "stiva este goala";
        stack_top--;
    }
    T top()
    {
        return this->at(stack_top-1);
    }
    uint size()
    {
        return stack_top;
    }
    bool empty()
    {
        return !stack_top;
    }
};

int main()
{
    vector<int> v;

    v[0]=3;

    vector<int> w(v);

    cout<<w[0]<<endl;
    cout<<v.size()<<endl;
    v.push_back(2);
    v.push_back(6);
    for(auto* x=v.begin();x!=v.end();x++)
        cout<<*x<<endl;
    cout<<v.size();

    stack<int> s;

    s.push(5);
    s.push(7);
    s.push(3);
    while(!s.empty())
        cout<<s.top()<<endl,s.pop();
    try
    {
        s.pop();
    }
    catch(const char* e)
    {
        cout<<e;
    }

    return 0;
}
