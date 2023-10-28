#include <iostream>
#include "matrix.h"
#include <assert.h>
#include <list>
#include "new.h"


#define TEST 0
using namespace std;

int main()
{
   if (TEST)
   {


    Matrix<int,-1>matrix;
    cout<<"matrix size must be=0 :"<<matrix.size()<<endl;


    auto a=matrix[0][0];
    cout<<"a must be -1 :"<<a<<endl;
    cout<<"matrix size must be=0 :"<<matrix.size()<<endl;

    matrix[100][100]=314;
    auto b=matrix[100][100];
    cout<<"b must be 314: "<<b<<endl;
    cout<<"matrix size must be 1 :"<<matrix.size()<<endl;
    cout<<"matrix[100][100] must be 314: "<< matrix[100][100]<<endl;
    a=b+6;
    cout<<"a must be 320: "<<a<<endl;
    matrix[0][0]=matrix[100][100]+6;
    cout<<"matrix[0][0] must be 320: "<<matrix[0][0]<<endl;
    cout<<"matrix size must be 2 :"<<matrix.size()<<endl;
    matrix[0][0]=-1;
    cout<<"matrix size must be 1 :"<<matrix.size()<<endl;


    Matrix<int,-1,3>matrix3;
    matrix3[5][5][5]=5;
    cout<<"matrix3[5][5][5] must be 5 :"<<matrix3[5][5][5]<<endl;


    matrix3[7][2][1]=26;
    matrix3[4][1][24]=8;
    for (auto x: matrix3)
    {
        for (auto y: x.first) cout<<y<<" ";
        cout<<" -> "<<x.second;
        cout<<endl;

    }
    cout<<"------end TEST"<<endl<<endl;

   }

   //Подготовка матрицы
   Matrix<int,0>matrix;
   for (int i=0; i<10; i++)
       for (int j=0; j<10; j++)
       {
           if (i==j) matrix[i][j]=i;
           if (i+j==9) matrix[i][j]=j;
       }

   //Вывод на экран части матрицы без применения итераторов

   for (int i=1; i<9; i++)
   {
       for (int j=1; j<9; j++)
       {
           cout<<matrix[i][j]<<" ";
       }
       cout<<endl;
   }

   cout<<"matrix size = "<<matrix.size()<<endl;

   //Применение итераторов (вывод всех занятых ячеек на экран)

   for (auto x: matrix)
   {
       for (auto y: x.first) cout<<y<<" ";
       cout<<" -> "<<x.second;
       cout<<endl;

   }





    return 0;
}
