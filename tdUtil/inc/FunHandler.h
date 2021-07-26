#ifndef __FUNHANDLER_H_
#define __FUNHANDLER_H_
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <functional>
// template<typename Tobject, typename Tparam01,typename Tparam02>
// class FunHandler
// {
// private:
//     typedef void (Tobject::*CbFun)(Tparam01*,Tparam02);
//     CbFun       pCbFun;     //回调函数指针
//     Tobject*    m_pInstance;    //调用对象
//     /* data */
// public:
//     void set(Tobject *pInstance, CbFun pFun);
//     bool exec(Tparam01 *pParam01,Tparam02 pParam02);
// };

// //设置调用对象及其回调函数
// template<typename Tobject, typename Tparam01,typename Tparam02>
// void FunHandler<Tobject, Tparam01,Tparam02>::set(Tobject *pInstance , CbFun pFun)
// {
//     m_pInstance = pInstance;
//     pCbFun = pFun;
// }
 
// //调用回调函数
// template<typename Tobject, typename Tparam01,typename Tparam02>
// bool FunHandler<Tobject, Tparam01,Tparam02>::exec(Tparam01 *pParam01,Tparam02 pParam02)
// {
//     (m_pInstance->*pCbFun)(pParam01,pParam02);
//     return true;
// }

typedef std::function<void(uint8_t *data,int size)> FunHandler;

#endif