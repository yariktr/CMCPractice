#include <iostream>
#include <vector>
class CSet
{
    private:
    std::vector <int> els;
    unsigned size;
    public:
    CSet(): size(0) { }
 
    void print()
    {
        std::cout<<'[';
        for (int el : els) std::cout<<el<<',';
        std::cout<<']';
    }
    
    void include(int el)
    {
        els.push_back(el);
        size++;
    }
    
    void exclude(int el)
    {
        for (int i=0; i<size; i++)
        {
            if (els[i] == el)
            {
            els.erase(els.begin()+i);
            size--;
            }
        }
    }
    
    bool contains(int el) const
    {
        for (int x : els)
        {
            if (x==el) return true;
        }
        return false;
    }
    
    const friend CSet operator+(const CSet& set1, const CSet& set2)
    {
        CSet result_set;
        for (int x : set1.els)
        {
            if (!result_set.contains(x)) result_set.include(x);
        }
        for (int x : set2.els)
        {
            if (!result_set.contains(x)) result_set.include(x);
        }
        return result_set;
    }
    
    const friend CSet operator-(const CSet& set1, const CSet& set2)
    {
        CSet result_set;
        for (int x : set1.els)
        {
            if (!set2.contains(x)) result_set.include(x);
        }
        return result_set;
    }

    const friend CSet operator*(const CSet& set1, const CSet& set2)
    {
        CSet result_set;
        for (int x : set1.els)
        {
            if (set2.contains(x)) result_set.include(x);
        }
        return result_set;
    }

    int operator[](unsigned index)
    {
        return els.at(index);
    }
    
    inline unsigned length() const { return size; }
 
};

int main()
{
    CSet set1, set2, result_set;
    set1.include(1); set1.include(2); set1.include(3);
    set2.include(1); set2.include(2); set2.include(3); set2.exclude(3); //добавили и удалили 3, в итоге получили мн-во (1,2)
    std::cout<<"Вы в меню программы\nМножество 1:\n";
    set1.print();
    std::cout<<"\nМножество 2:\n";
    set2.print();
    std::cout<<"\nОбъединение множеств:\n";
    result_set = set1 + set2;
    result_set.print();
    std::cout<<"\nПересечение множеств:\n";
    result_set = set1 * set2;
    result_set.print();
    std::cout<<"\nРазность множеств:\n";
    result_set = set1 - set2;
    result_set.print();

    return 0;
}
