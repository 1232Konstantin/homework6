#ifndef MATRIX_H
#define MATRIX_H
#include <list>
#include <set>
#include <iostream>


/*!
 * \brief Класс хранитель элемента данных класса Matrix
 */

template<typename T>
class Point
{
    T m_value;
    int m_index;
    public:
    Point() =default;
    Point (const T &value, int index) : m_value(value), m_index(index) {}
    void setValue(T& value) {m_value=value;}
    T& value() {return m_value;}
    int index() {return m_index;}

    operator T(){return m_value;} //операторы для корректного функционирования квадратных скобочек класса Matrix
    Point& operator=(const T t) {m_value=t; return (*this);} //операторы для корректного функционирования квадратных скобочек класса Matrix

};


/*!
 * \brief Итератор std::list<T> расширенный проверкой на валидность
 */
template <typename T>
class extended_list_operator
{
    typename std::list<T>::iterator m_current;
    typename std::list<T>::iterator m_end;
    mutable bool m_always_not_valid=true; //если установить этот параметр, то итератор всегда будет не валидным
public:

    extended_list_operator(){}
    extended_list_operator(typename std::list<T>::iterator&& begin, typename std::list<T>::iterator&& end): m_current(begin), m_end(end), m_always_not_valid(false) {};
    extended_list_operator(const extended_list_operator<T>& other)
    {
        m_current=other.m_current;
        m_end=other.m_end;
        m_always_not_valid=other.m_always_not_valid;
    }
    extended_list_operator(const extended_list_operator<T>&& other)
    {
        m_current=std::move(other.m_current);
        m_end=std::move(other.m_end);
        m_always_not_valid=other.m_always_not_valid;
        other.m_always_not_valid=true;
    }

    typename std::list<T>::iterator& get_wrapped_iterator() {return m_current;}
    void operator++(){++m_current;}
    void operator++(int){m_current++;}
    T& operator*(){return *m_current;}
    bool isValid(){ return ( (m_always_not_valid)? false : (m_current!=m_end)? true :false );  }
    bool operator==(extended_list_operator& other) {return m_current==other.m_current;}
    bool operator!=(extended_list_operator& other) {return m_current!=other.m_current;}
    void operator=(extended_list_operator other)
    {
        m_current=other.m_current;
        m_end=other.m_end;
        m_always_not_valid=other.m_always_not_valid;
    }
};


/*!
 * \brief Абстрактный интерфейсный класс, необходимый для реализации механизма итерации по Matrix
 */
template <typename T>
struct Interface
{
    std::list<int> indexes; //список индексов для вычисления индексов элемента матрицы
    virtual extended_list_operator<Point<T>> get_iterator() =0; //запросить итератор
    virtual void fill_queue(std::list<Interface<T>*>& list) =0; //заполнение очереди на разбор данных внутри итератора
    virtual ~Interface()=default;

};

/*!
 * \brief Класс итератора предназначенный для итерации внутри иерархической структуры N мерной матрицы
 * \details Поддерживает синтаксис for( auto x: matrix) , возвращает std::pair первым элементом которой является лист индексов std::list<int>, 
 * \details вторым элементом пары является значение внутри адресованной ячейки
 */

template <typename T>
struct MyIterator
{
    std::list<Interface<T>*> m_queue;
    extended_list_operator<Point<T>> m_current;
    std::list<int> m_current_indexes;

    MyIterator(){};
    MyIterator(Interface<T>* first)
    {
        first->fill_queue(m_queue);
        operator++();
    }

    MyIterator(const MyIterator<T>& other)
    {
        m_queue=other.m_queue;
        m_current=other.m_current;
        m_current_indexes=other.m_current_indexes;
    }

    MyIterator& operator++()
    {
        if (m_current.isValid())
        {
            m_current++; //если есть текущий валидный итератор, пробуем сместиться на следующий
        }
        if (!m_current.isValid()) //если следующий уже не валидный
        {
            bool stop=false;
            while(!stop)
            {
                if (!m_queue.empty()) //проверяем пуста ли очередь
                {
                    auto temp=m_queue.front();
                    m_queue.pop_front();
                    m_current=temp->get_iterator();
                    m_current_indexes=temp->indexes;
                    if (!m_current.isValid())
                    {
                        temp->fill_queue(m_queue);
                    }
                    else stop=true;
                }
                else stop=true;
            }
        }
        return (*this);
    }

    MyIterator& operator++(int) {return operator++();}


    bool isValid(){return m_current.isValid();}
    std::pair<std::list<int>, T> operator*()
    {
        std::list<int> indexes=m_current_indexes;
        indexes.push_back((*m_current).index());
        return std::make_pair(indexes, (*m_current).value());
    }
    bool operator==(MyIterator<T>& other)
    {
        if ((!m_current.isValid()) && (!other.m_current.isValid())) return true;
        return (m_current_indexes==other.m_current_indexes);
    }
    bool operator!=(MyIterator<T>& other) {return !(operator==(other));}

};



/*!
 * \brief Вспомогательная функция поиска внутри листа std::list<Point<T>>& list
 */
template<typename T>
Point<T>* find(std::list<Point<T>>& list, int t)
{
      if (list.empty()) return nullptr;
      for (auto it=list.begin(); it!=list.end(); it++)
      {
          if ((*it).index()==t)
          {
              return &(*it);
          }
      }
      return nullptr;
}




/*!
 * \brief Класс 1-мерная матрица
 * \detailsКласс Matrix это бесконечная (ограниченная только размером индекса) N мерная разреженная матрица элементов типа Т с дефолтным значением DEFAULT_VAL
 * \detailsдобавление элементов в матрицу осуществляется выражением matrix[i1][i2]...[iN] = value
 * \detailsдопускаются присвоения вида: T val=matrix[i1][i2]...[iN] , а также вида (matrix[i1][i2]...[iN]=val1)=val2
 * \detailsВ матрице должны храниться только занятые элементы, значение которых != DEFAULT_VAL. Количество таких элементов должно определяться методом size()
 * \detailsПрисвоение элементу матрицы значения DEFAULT_VAL должно освобождать ячейку
 * \detailsПри запросе данных из свободной ячейки должно возвращаться значение  DEFAULT_VAL

 * \detailsОсобенности реализации: поскольку требуется обеспечить выражения вида  (matrix[i1][i2]...[iN]=DEFAULT_VAL)=val2,
 * \detailsочистка ячеек с дефолтными значениями осуществляется в процессе подсчета size() или при получении итератора для обхода матрицы
 * \detailsТип Т должен допускать копирование
 */
template <typename T, T DEFAULT_VAL, int N=2>
class Matrix : public Interface<T>
{
    using Type=Matrix<T, DEFAULT_VAL, N-1>;
    std::list<Point<Type>> m_list;

public:
    using Iterator= MyIterator<T>;

    Matrix() =default;

    void operator=(const Matrix<T,DEFAULT_VAL,N>& other)
    {
        m_list=other.m_list;
    }


    //оператор []
    template <typename Head>
    Type& operator[](Head firstArg)
    {

        //проверим тип индекса, он не может быть произвольным
        static_assert(std::is_same<int, Head>::value, "Element indexes must have type int");

        Point<Type>* point=find(m_list, firstArg);
        if (point)  //Элемент существует
        {
            return point->value();
        }
        else //Элемента с таким значением нет
        {
            auto temp_t=Type(); //Создаем матрицу размерности N-1
            auto temp_p=Point<Type>(temp_t, firstArg); //создаем на ее основе элемент для помещения в list
            m_list.push_back(temp_p); //размещаем
            return m_list.back().value();
        }
    }

    void clear()
    {
        m_list.clear();
    }

    size_t size()
    {
        size_t result=0;
        for (auto &x: m_list) result+=x.value().size();
        return result;
    }

    extended_list_operator<Point<T>> get_iterator() override
    {
        return extended_list_operator<Point<T>>();
    }

    virtual void fill_queue(std::list<Interface<T>*>& list) override
    {

        for (auto &x: m_list)
        {
            Interface<T>* temp=x.value().get_address();
            temp->indexes=Interface<T>::indexes;
            temp->indexes.push_back(x.index());
            list.push_back(temp);
        }

    }

    MyIterator<T> begin()
    {
        size(); //для удаления ячеек с дефолтными занчениями
        MyIterator<T> it(this);
        return it;
    }

    MyIterator<T> end()
    {
        MyIterator<T> it;
        return it;
    }

    //Запросить собственный адрес у матрицы
    Interface<T>* get_address()
    {
        return this;
    }
};



/*!
 * \brief Класс 1-мерная матрица
 */

template <typename T, T DEFAULT_VAL>
class Matrix<T, DEFAULT_VAL, 1> : public Interface<T>
{
    using Type=T;
    std::list<Point<Type>> m_list;

public:
    using iterator=MyIterator<T>;


    Matrix() =default;

    void operator=(const Matrix<T,DEFAULT_VAL,1>& other)
    {
        m_list=other.m_list;
    }


    //оператор []
    template <typename Head>
    Point<T>& operator[](Head firstArg)
    {
        //проверим тип индекса, он не может быть произвольным
        static_assert( std::is_same<int, Head>::value, "Element indexes must have type int");
       Point<Type>* point=find(m_list, firstArg);
       if (point)  //Элемент существует
        {
            return *point;
        }
        else //Элемента с таким значением нет
        {
            auto temp_p=Point<Type>(DEFAULT_VAL, firstArg); //создаем
            m_list.push_back(temp_p); //размещаем
            return m_list.back();
        }
    }


    size_t size()
    {
        //Сборка мусора при измерении размера
        for (auto it=m_list.begin(); it!=m_list.end(); it++)
        {
            if ((*it).value()==DEFAULT_VAL)
            {
                m_list.erase(it);
                it=m_list.begin();
            }
        }
        return m_list.size();
    }

    void clear()
    {
        m_list.clear();
        typename std::list<T>::iterator it;
    }

    extended_list_operator<Point<T>> get_iterator() override
    {
        return extended_list_operator<Point<T>>(m_list.begin(),m_list.end());
    }

    virtual void fill_queue(std::list<Interface<T>*>& list) override {}

    MyIterator<T> begin()
    {
        MyIterator<T> it(this);
        return it;
    }

    MyIterator<T> end()
    {
        MyIterator<T> it;
        return it;
    }

   //Запросить собственный адрес у матрицы
    Interface<T>* get_address()
    {
        return this;
    }
};




#endif // MATRIX_H
