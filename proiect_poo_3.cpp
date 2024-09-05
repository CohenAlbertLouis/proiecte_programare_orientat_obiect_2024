#include<iostream>
#include<cstring>
#include<vector>
#include<list>

enum except_id
{
    invalid_access_id,
    invalid_insertion_id,
    invalid_deletion_id,
    underflow_except_id,
    overflow_except_id,
    invalid_exception_code_id
};

class except //multiton design pattern
{
    except()
    {
        except::except_list[0] = except("invalid access");
        except::except_list[1] = except("invalid insertion");
        except::except_list[2] = except("invalid deletion");
        except::except_list[3] = except("underflow except");
        except::except_list[4] = except("overflow except");
        except::except_list[5] = except("invalid exception code");
    }
    except(char* c)
    {
        code = new char[50];
        strcpy(code,c);
    }
    char* code;
    static except except_list[6];
    static except force_constructor_call;
public:
    except(const except& e) = default;
    except(except&& e) = default;
    ~except() = default;
    void operator=(except e)
    {
        code = new char[50];
        strcpy(code,e.code);
    }
    char* what()
    {
        return code;
    }
    static except get_by_id(except_id e)
    {
        if(e>5)
            throw except::except_list[invalid_exception_code_id];
        return except_list[e];
    }
};

except except::except_list[6];
except except::force_constructor_call  = except();

template<class T>
struct remove_pointer
{
    using type = T;
};

template<class T>
struct remove_pointer<T*>
{
    using type = T;
};

template<class T>
using remove_pointer_t = typename remove_pointer<T>::type;

using namespace std;

template<class T>
class object_pool
{
    list<T> pool;
    static object_pool* inst; //instanta
public:
    static object_pool* get()
    {
        if(inst==nullptr)
            inst = new object_pool;
        return inst;
    }
    T retrieve()
    {
        if(pool.empty())
            return new remove_pointer_t<T>();
        T ptr = pool.front();
        pool.pop_front();
        return ptr;
    }
    void restore(T x)
    {
        x->clear();
        pool.push_back(x);
    }
};

template<class T>
object_pool<T>* object_pool<T>::inst;

int main()
{
    vector<int> v={3,2,3,4,7,-1,-2,5};
    vector<int> v2={6,6,4,-3,4,2,7,8};
    vector<int> v3={2,9,9,0,0,-4,-5,-8};

    object_pool<vector<int>*>* op = object_pool<vector<int>*>::get();

    vector<int>* w1;
    vector<int>* w2;
    vector<int>* w3;

    w1 = op->retrieve();
    w2 = op->retrieve();
    w3 = op->retrieve();

    w1->push_back(5);
    w1->push_back(6);
    w1->push_back(8);
    w1->push_back(12);
    for(auto it = w1->begin();it!=w1->end();it++)
        cout<<*it<<" ";
    cout<<endl;

    op->restore(w1);
    op->restore(w2);
    op->restore(w3);

    except e(except::get_by_id(invalid_access_id));
    except e2 = e;

    try
    {
        throw e;
    }
    catch(except& ex)
    {
        cout<<ex.what();
    }

    return 0;
}
