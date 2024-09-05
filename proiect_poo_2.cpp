#include<iostream>
#include<cstring>

#define FIXED_VECTOR_SIZE 100000

using uint = unsigned int;

class except
{
    except(char* c)
    {
        code = new char[50];
        strcpy(code,c);
    }
    char* code;
public:
    except() = delete;
    except(const except& e) = default;
    except(except&& e) = default;
    ~except() = default;
    static except invalid_access;
    static except invalid_insertion;
    static except invalid_deletion;
    static except underflow_except;
    static except overflow_except;
    char* what()
    {
        return code;
    }
};

except except::invalid_access("invalid access");
except except::invalid_insertion("invalid insertion");
except except::invalid_deletion("invalid deletion");
except except::underflow_except("underflow except");
except except::overflow_except("overflow except");

template<class T>
class container
{
public:
    container() = default;
    virtual T& at(uint pos) = 0;
    virtual T* begin() = 0;
    virtual T* end() = 0;
    virtual void print() = 0;
    virtual ~container() = default;
};

template<class T>
class seq_alloc_data : public container<T> //date alocate secvential
{
protected:
    T* v;
public:
    seq_alloc_data()
    {
        v = new T[FIXED_VECTOR_SIZE];
    }
    seq_alloc_data(const seq_alloc_data& s)
    {
        v=s.v;
    }
    seq_alloc_data(seq_alloc_data&& s)
    {
        v=s.v;
    }
    virtual T* begin()
    {
        return v;
    }
    virtual T* end()
    {
        return nullptr;
    }
    T& at(uint pos)
    {
        return v[pos];
    }
    ~seq_alloc_data()
    {
        delete v;
    }
};

template<class T>
class non_seq_alloc_data : public container<T> //date alocate non-secvential (liste)
{
    T* internal_vector;
protected:
    class node
    {
    public:
        T value;
        node* next = nullptr;
        node* operator++()
        {
            return next;
        }
    };
    node* head;
    template<class U>
    friend typename non_seq_alloc_data<U>::node* make_node(U val);
public:
    non_seq_alloc_data()
    {
        internal_vector = new T;
        head = new node;
        head = head->next;
    }
    non_seq_alloc_data(const non_seq_alloc_data& s)
    {
        head = s.head;
    }
    non_seq_alloc_data(non_seq_alloc_data&& s)
    {
        head = s.head;
    }
    T& at(uint pos)
    {
        if(head==nullptr)
            throw except::invalid_access;
        node* ptr = new node;
        ptr=head;

        while(pos-- and ptr!=nullptr)
            ptr=ptr->next;
         if(ptr==nullptr)
            throw except::invalid_access;
        return ptr->value;
    }
    uint size()
    {
        uint s = 0;
        node* ptr = new node;

        ptr=head;
        while(ptr!=nullptr)
            ptr=ptr->next,s++;
        return s;
    }
    T* begin()
    {
        node* ptr = new node;

        ptr = head;
        internal_vector = new T[size()];
        for(uint i=0;ptr!=nullptr;i++)
            internal_vector[i]=ptr->value,ptr=ptr->next;
        return internal_vector;
    }
    T* end()
    {
        return internal_vector+size();
    }
    node* nbegin()
    {
        return head;
    }
    node* nend()
    {
        return nullptr;
    }
    void print()
    {
        node* ptr = head;

        while(ptr!=nullptr)
            std::cout<<ptr->value<<" ",ptr=ptr->next;
    }
    ~non_seq_alloc_data()
    {
        delete internal_vector;
        delete head;
    }
};

template<class T>
typename non_seq_alloc_data<T>::node* make_node(T val)
{
    typename non_seq_alloc_data<T>::node* n = new typename non_seq_alloc_data<T>::node;

    n->value=val;
    return n;
}

template<class T>
class stack : private seq_alloc_data<T>
{
    uint s_top;
public:
    stack() : seq_alloc_data<T>()
    {
        s_top=0;
    }
    stack(const stack& s) : seq_alloc_data<T>(s.v)
    {
        s_top=s.s_top;
    }
    stack(stack&& s) : seq_alloc_data<T>(s.v)
    {
        s_top=s.s_top;
    }
    void operator=(stack s)
    {
        s_top=s.s_top;
        seq_alloc_data<T>::v=s.seq_alloc_data<T>::v;
    }
    void print()
    {
        for(uint i=1;i<=s_top;i++)
            std::cout<<seq_alloc_data<T>::v[s_top-i]<<" ";
    }
    void push(T x)
    {
        seq_alloc_data<T>::v[s_top]=x;
        s_top++;
    }
    void pop()
    {
        if(!s_top)
            throw except::underflow_except;
        s_top--;
    }
    T& top()
    {
        return seq_alloc_data<T>::v[s_top];
    }
    bool empty()
    {
        return !s_top;
    }
    uint size()
    {
        return s_top;
    }
};

template<class T,class U>
struct pair
{
    T first;
    U second;
    pair() = default;
    pair(T f,U s)
    {
        first=f;
        second=s;
    }
    pair(const pair& p)
    {
        first=p.first;
        second=p.second;
    }
    pair(pair&& p)
    {
        first=p.first;
        second=p.second;
    }
    void operator=(pair p)
    {
        first=p.first;
        second=p.second;
    }
    ~pair() = default;
};

template<class T>
class vector : public seq_alloc_data<T>
{
    uint top;
    template<class U,class ...args>
    pair<T*,uint> vectorise(pair<T*,uint> p,U x,args... pargs)
    {
        p.first[p.second]=x;
        p.second++;
        return vectorise(p,pargs...);
    }
    template<class U>
    pair<T*,uint> vectorise(pair<T*,uint> p,U x)
    {
        p.first[p.second]=x;
        p.second++;
        return p;
    }
public:
    vector() : seq_alloc_data<T>()
    {
        top=0;
    }
    vector(const vector& vec) : seq_alloc_data<T>(vec.v)
    {
        top=vec.top;
    }
    vector(vector&& vec) : seq_alloc_data<T>(vec.v)
    {
        top=vec.top;
    }
    vector(T* vec,uint n)
    {
        for(uint i=0;i!=n;i++)
            seq_alloc_data<T>::v=vec[i];
    }
    template<class ...args>
    vector(args... pargs)
    {
        pair<T*,uint> p(new T[FIXED_VECTOR_SIZE],0);

        auto u = vectorise(p,pargs...);

        seq_alloc_data<T>::v = u.first;
        top = u.second;
    }
    void operator=(vector vec)
    {
        top=vec.top;
        seq_alloc_data<T>::v=vec.seq_alloc_data<T>::v;
    }
    T& operator[](uint pos)
    {
        if(pos>FIXED_VECTOR_SIZE)
            throw except::invalid_access;
        top=pos>top ? pos:top;
        return seq_alloc_data<T>::at(pos);
    }
    T* end() override
    {
        return seq_alloc_data<T>::v+top;
    }
    void push_back(T x)
    {
        seq_alloc_data<T>::v[top]=x;
        top++;
        if(top==FIXED_VECTOR_SIZE)
            throw except::overflow_except;
    }
    void push_front(T x)
    {
        for(uint i=top;i!=0;i--)
            seq_alloc_data<T>::v[i]=seq_alloc_data<T>::v[i-1];
        seq_alloc_data<T>::v[0]=x;
        top++;
        if(top==FIXED_VECTOR_SIZE)
            throw except::overflow_except;
    }
    void pop_back()
    {
        if(!top)
            throw except::underflow_except;
        top--;
    }
    void clear()
    {
        for(uint i=0;i<top;i++)
            seq_alloc_data<T>::v[i]=0;
    }
    void clear_all()
    {
        for(uint i=0;i<FIXED_VECTOR_SIZE;i++)
            seq_alloc_data<T>::v[i]=0;
    }
    uint size()
    {
        return top;
    }
    void print()
    {
        for(uint i=0;i<top;i++)
            std::cout<<seq_alloc_data<T>::v[i]<<" ";
    }
    bool empty()
    {
        return !top;
    }
};

template<class T>
class list : public non_seq_alloc_data<T>
{
    using elem = typename non_seq_alloc_data<T>::node;
public:
    list() : non_seq_alloc_data<T>()
    {

    }
    list(const list& s) : non_seq_alloc_data<T>(s)
    {

    }
    list(list&& s) : non_seq_alloc_data<T>(s)
    {

    }
    list(typename non_seq_alloc_data<T>::node* n) : non_seq_alloc_data<T>()
    {
        non_seq_alloc_data<T>::head=n;
    }
    void operator=(list l)
    {
        non_seq_alloc_data<T>::head=l.non_seq_alloc_data<T>::head;
    }
    void push_front(T v)
    {
        elem* ptr = new elem;

        ptr->value=v;
        ptr->next=non_seq_alloc_data<T>::head;
        non_seq_alloc_data<T>::head=ptr;
    }
    void push_back(T v)
    {
        if(non_seq_alloc_data<T>::head==nullptr)
        {
            push_front(v);
            return;
        }
        elem* ptr = new elem;
        ptr=non_seq_alloc_data<T>::head;

        while(ptr->next!=nullptr)
            ptr=ptr->next;
        ptr->next = new elem;
        (ptr->next)->value = v;
    }
    T& operator[](uint pos)
    {
        if(non_seq_alloc_data<T>::head==nullptr)
            throw except::invalid_access;
        elem* ptr = new elem;
        ptr=non_seq_alloc_data<T>::head;

        while(pos-- and ptr!=nullptr)
            ptr=ptr->next;
        if(ptr==nullptr)
            throw except::invalid_access;
        return ptr->value;
    }
    elem* operator()(uint pos)
    {
        if(non_seq_alloc_data<T>::head==nullptr)
            throw except::invalid_access;
        elem* ptr = new elem;
        ptr=non_seq_alloc_data<T>::head;

        while(pos-- and ptr!=nullptr)
            ptr=ptr->next;
        if(ptr==nullptr)
            throw except::invalid_access;
        return ptr;
    }
    void insert(elem* n,T val)
    {
        if(non_seq_alloc_data<T>::head==nullptr)
            throw except::invalid_access;
        elem* ptr = new elem;
        ptr=non_seq_alloc_data<T>::head;

        while(ptr!=n and ptr!=nullptr)
            ptr=ptr->next;
        if(ptr==nullptr)
            throw except::invalid_insertion;
        elem* p = new elem;
        p=ptr->next;
        ptr->next = new elem;
        (ptr->next)->value=val;
        (ptr->next)->next=p;
    }
    void erase(elem* n)
    {
        elem* ptr = new elem;
        ptr=non_seq_alloc_data<T>::head;

        if(ptr==nullptr)
            throw except::invalid_deletion;
        while(ptr->next!=n and ptr!=nullptr)
            ptr=ptr->next;
        if(ptr==nullptr)
            throw except::invalid_deletion;
        ptr->next=ptr->next->next;
    }
    void pop_front()
    {
        if(non_seq_alloc_data<T>::head==nullptr)
            throw except::invalid_deletion;
        non_seq_alloc_data<T>::head=non_seq_alloc_data<T>::head->next;
    }
    void pop_back()
    {
        elem* p = new elem;

        if(non_seq_alloc_data<T>::head==nullptr)
            throw except::invalid_deletion;
        p=non_seq_alloc_data<T>::head;
        while(p->next->next!=nullptr)
            p=p->next;
        p->next=nullptr;
    }
};

using namespace std;

int main()
{
    stack<int> s;

    s.push(5);
    s.push(3);
    s.pop();
    s.print();

    vector<int> v;

    v.push_back(6);
    v.push_front(4);
    v.push_back(5);
    v.pop_back();
    v.print();

    auto x = make_node(5);

    cout<<endl<<x->value<<endl;

    //non_seq_alloc_data<int> n;
    list<int> l(x);

    l.push_back(5);
    l.push_back(12);
    l.push_back(4);
    l.push_back(7);
    cout<<l[3]<<endl;
    l.print();
    cout<<endl;

    try
    {
        l.erase(l(6));
    }
    catch(except e)
    {
        cout<<e.what()<<endl;
    }

    vector<int> v0(3,4,5,6,7,7,2,-1,4);
    container<int>* c = dynamic_cast<container<int>*>(&v0);

    c->print();
    cout<<endl;
    c->at(12)=4;
    for(auto i=c->begin();i!=c->end();i++)
        cout<<*i<<" ";

    list<int> l0;

    l0.push_front(6);
    l0.push_front(7);
    l0.push_front(4);
    l0.push_front(5);
    l0.push_front(-1);
    l0.push_front(7);

    non_seq_alloc_data<int>* nsad = dynamic_cast<non_seq_alloc_data<int>*>(&l0);
    container<int>* cl = dynamic_cast<container<int>*>(nsad);

    cout<<endl;
    nsad->print();
    cout<<endl;
    for(auto i=cl->begin();i!=cl->end();i++)
        cout<<*i<<" ";

    return 0;
}
