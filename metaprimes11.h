/***************************************************************************
 *   Copyright (C) 2015 by Vladimir Mirnyy, blog.scpp.eu                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the MIT License                                 *
 ***************************************************************************/

#ifndef __metaprimes11_h
#define __metaprimes11_h

/** \file
    \brief Primes generation meta-algorithms using variadic templates
*/

#include <cmath>

#include "metafactorcommon.h"
#include "metaprimescommon.h"

#include "variadic_typelist.h"

// Initial simple and efficient primes generator using formula 6k+r, r = 1,5
// It has shorter compile-times comparing to GenPrimes
template<int Start, int Limit, int K, 
int C = (6*K + 1 <= Limit)>
struct nextPrimesDirect;

template<int Start, int Limit, int K> 
struct nextPrimesDirect<Start, Limit, K, true>
{
  static const int Primecandidate1 = 6*K + 1;
  static const int Primecandidate2 = 6*K + 5;
  static const bool p1 = IsPrime6<CheckSmallPrimes5>::Do<Primecandidate1>::value && (Primecandidate1 > Start);
  static const bool p2 = IsPrime6<CheckSmallPrimes5>::Do<Primecandidate1>::value && (Primecandidate2 <= Limit) && (Primecandidate2 > Start);
  typedef typename nextPrimesDirect<Start, Limit, K+1>::type nextIter;
  typedef typename std::conditional<p1 && p2, 
    typename typelist_cat<typelist<sint<Primecandidate1>, sint<Primecandidate2>>, nextIter>::type, 
    typename std::conditional<p1, typename typelist_cat<sint<Primecandidate1>, nextIter>::type, 
    typename std::conditional<p2, typename typelist_cat<sint<Primecandidate2>, nextIter>::type, nextIter>::type>::type>::type type;
};

template<int Start, int Limit, int K> 
struct nextPrimesDirect<Start, Limit, K, false>
{
  typedef typelist<> type;
};

/////////////////////////////////////////////

template<typename C, bool b, typename Next>
struct addType
{
  typedef Next type;
};

template<typename C, typename Next>
struct addType<C,true,Next>
{
  typedef typename typelist_cat<C,Next>::type type;
};

// Experimental code to achieve shorter compile-times comparing to GenPrimes
template<int Limit, int K = 1, int C = (30*K + 1 <= Limit)>
struct nextPrimesDirect30;

template<int Limit, int K> 
struct nextPrimesDirect30<Limit, K, true>
{
  typedef IsPrime30<CheckSmallPrimes7_29> isPrime;
  static const int_t C1 = 30*K + 1;
  static const int_t C2 = 30*K + 7;
  static const int_t C3 = 30*K + 11;
  static const int_t C4 = 30*K + 13;
  static const int_t C5 = 30*K + 17;
  static const int_t C6 = 30*K + 19;
  static const int_t C7 = 30*K + 23;
  static const int_t C8 = 30*K + 29;
  static const bool p1 = isPrime::Do<C1>::value;
  static const bool p2 = isPrime::Do<C2>::value && (C2 <= Limit);
  static const bool p3 = isPrime::Do<C3>::value && (C3 <= Limit);
  static const bool p4 = isPrime::Do<C4>::value && (C4 <= Limit);
  static const bool p5 = isPrime::Do<C5>::value && (C5 <= Limit);
  static const bool p6 = isPrime::Do<C6>::value && (C6 <= Limit);
  static const bool p7 = isPrime::Do<C7>::value && (C7 <= Limit);
  static const bool p8 = isPrime::Do<C8>::value && (C8 <= Limit);
  typedef typename nextPrimesDirect30<Limit, K+1>::type NextIter;
  
  typedef typename addType<sint<C8>,p8,NextIter>::type R8;
  typedef typename addType<sint<C7>,p7,R8>::type R7;
  typedef typename addType<sint<C6>,p6,R7>::type R6;
  typedef typename addType<sint<C5>,p5,R6>::type R5;
  typedef typename addType<sint<C4>,p4,R5>::type R4;
  typedef typename addType<sint<C3>,p3,R4>::type R3;
  typedef typename addType<sint<C2>,p2,R3>::type R2;
  typedef typename addType<sint<C1>,p1,R2>::type type;
};

template<int Limit, int K> 
struct nextPrimesDirect30<Limit, K, false>
{
  typedef typelist<> type;
};

// Returns true, if Candidate is not divisible by any entry of TrialList
template<int_t Candidate, typename TrialList, bool doExit = false>
struct isNotDivisibleByListMembers;

template<int_t Candidate, typename H, typename ...Tail>
struct isNotDivisibleByListMembers<Candidate,typelist<H,Tail...>,false>
{
  static const bool value = (Candidate % H::value != 0) && 
         isNotDivisibleByListMembers<Candidate,typelist<Tail...>,(H::value*H::value >= Candidate)>::value;
};

template<int_t Candidate>
struct isNotDivisibleByListMembers<Candidate,typelist<>,false>
{
  static const bool value = true;
};

template<int_t Candidate, typename H, typename ...Tail>
struct isNotDivisibleByListMembers<Candidate,typelist<H,Tail...>,true>
{
  static const bool value = true;
};

template<int_t Candidate>
struct isNotDivisibleByListMembers<Candidate,typelist<>,true>
{
  static const bool value = true;
};

// Generate all primes less equal Limit with a constant Q in the formula Q*K+R
// Second exit criteria: (K+1 == InitRList::Head::value)
template<int_t Limit, int_t Q, int_t K, typename InitRList, typename CheckFactors,
         typename RemindersList = InitRList, bool doExit = false>
struct GenPrimesFormula;

// Next loop step over RList
template<int_t Limit, int_t Q, int_t K, typename InitRList, typename CheckFactors, typename H, typename ...Tail>
struct GenPrimesFormula<Limit,Q,K,InitRList,CheckFactors,typelist<H,Tail...>,false>
{
  static const int_t candidate = Q*K + H::value;
  typedef typename GenPrimesFormula<Limit, Q, K, InitRList, CheckFactors, typelist<Tail...>, (candidate > Limit)>::type nextIter;
  typedef typename std::conditional<(candidate <= Limit && isNotDivisibleByListMembers<candidate, CheckFactors>::value), 
          typename typelist_cat<sint<candidate>,nextIter>::type, nextIter>::type type;
};

// Next loop step over K
template<int_t Limit, int_t Q, int_t K, typename Head, typename ...List, typename CheckFactors>
struct GenPrimesFormula<Limit,Q,K,typelist<Head,List...>,CheckFactors,typelist<>,false>
{
  typedef typelist<Head,List...> InitRList;
  static const int_t candidate = Q*(K+1) + 1;
  typedef typename GenPrimesFormula<Limit, Q, K+1, InitRList, CheckFactors, InitRList, (K+1 == Head::value)>::type nextIter;
  typedef typename std::conditional<(candidate <= Limit && (K+1 < Head::value) && isNotDivisibleByListMembers<candidate, CheckFactors>::value), 
          typename typelist_cat<sint<candidate>,nextIter>::type, nextIter>::type type;
};

template<int_t Limit, int_t Q, int_t K, typename InitRList, typename CheckFactors, typename H, typename ...Tail>
struct GenPrimesFormula<Limit,Q,K,InitRList,CheckFactors,typelist<H,Tail...>,true>
{
  typedef typelist<> type;
};

template<int_t Limit, int_t Q, int_t K, typename InitRList, typename CheckFactors>
struct GenPrimesFormula<Limit,Q,K,InitRList,CheckFactors,typelist<>,true>
{
  typedef typelist<> type;
};



template<int_t Q, typename List, int_t N>
struct FilterRemindersList;

template<int_t Q, typename H, typename ...Tail, int_t N>
struct FilterRemindersList<Q, typelist<H, Tail...>, N>
{
  static const bool C = (N*H::value < Q);
  typedef FilterRemindersList<Q,typelist<Tail...>,N> Next;
  typedef typename std::conditional<C, typename typelist_cat<H,typename Next::ExcludedPrimes>::type, typelist<>>::type ExcludedPrimes;
  typedef typename std::conditional<C, typename Next::CheckPrimes, typelist<H, Tail...>>::type CheckPrimes;
};

template<int_t Q, int_t N>
struct FilterRemindersList<Q, typelist<>, N>
{
  typedef typelist<> ExcludedPrimes;
  typedef typelist<> CheckPrimes;
};


template<typename List, int_t N>
struct EliminateNonPrimesLoop;

template<typename H, typename ...Tail, int_t N>
struct EliminateNonPrimesLoop<typelist<H, Tail...>, N>
{
  typedef typename EliminateNonPrimesLoop<typelist<Tail...>,N>::type Next;
  typedef typename std::conditional<(H::value%N != 0), typename typelist_cat<H,Next>::type, Next>::type type;
};

template<int_t N>
struct EliminateNonPrimesLoop<typelist<>,N>
{
  typedef typelist<> type;
};


template<typename List, typename PossibleFactors>
struct EliminateNonPrimes;

template<typename List, typename H, typename ...Tail>
struct EliminateNonPrimes<List, typelist<H, Tail...> >
{
  typedef typename EliminateNonPrimesLoop<List,H::value>::type NewList;
  typedef typename EliminateNonPrimes<NewList,typelist<Tail...>>::type type;
};

template<typename List>
struct EliminateNonPrimes<List, typelist<>>
{
  typedef List type;
};


template<typename T>
struct GetFirstNumberInList
{
  static const int_t value = T::Head::value;
};

template<>
struct GetFirstNumberInList<typelist<>>
{
  static const int_t value = 1;
};

template<int_t Limit, int_t StartQ=6, 
typename StartRList = typelist<sint<5>>, bool doExit = false>
struct GenPrimes;

template<int_t Limit, int_t Q, typename RList>
struct GenPrimes<Limit,Q,RList,false>
{
  static const int_t NextQ = Q*RList::Head::value;
  typedef FilterRemindersList<NextQ,typename RList::Tail,GetFirstNumberInList<typename RList::Tail>::value> Filter;
  typedef typename typelist_cat<typename RList::Head, typename Filter::CheckPrimes>::type PrimesToCheck;
  typedef typename typelist_cat<sint<Q+1>,   // Q+1 is always prime and should not be checked
    typename GenPrimesFormula<(NextQ<Limit) ? NextQ : Limit,Q,1,RList,PrimesToCheck>::type>::type NewList;
  typedef typename EliminateNonPrimes<NewList,typename Filter::ExcludedPrimes>::type PrimesList;
  typedef typename typelist_cat<typename RList::Tail, NewList>::type NextRList;
  typedef typename GenPrimes<Limit, NextQ, NextRList, (NextQ+1 > Limit)>::type NextIter;
  typedef typename typelist_cat<PrimesList,NextIter>::type type;
};

template<int_t Limit, int_t Q, typename RList>
struct GenPrimes<Limit,Q,RList,true>
{
  typedef typelist<> type;
};

//////////////////////////////////////////////////////////////////////////////////////////

// Generate all primes less equal Limit with a constant Q in the formula Q*K+R
// Similar to GenPrimesFormula, but without second exit condition
template<int_t Limit, int_t Q, int_t K, typename InitRList, 
         typename CheckPrimeCandidate = IsPrime6<>,
         typename RList = InitRList, bool doExit = false>
struct GeneratePrimes;

template<int_t Limit, int_t Q, int_t K, typename InitRList, typename H, typename ...Tail, typename CheckPrimeCandidate>
struct GeneratePrimes<Limit,Q,K,InitRList,CheckPrimeCandidate,typelist<H,Tail...>,false>
{
  static const int_t candidate = Q*K + H::value;
  typedef typename GeneratePrimes<Limit, Q, K, InitRList, CheckPrimeCandidate, Tail..., (candidate > Limit)>::type nextIter;
  typedef typename std::conditional<(candidate <= Limit && CheckPrimeCandidate::template Do<candidate>::value), 
          typename typelist_cat<sint<candidate>,nextIter>::type, nextIter>::type type;
};

template<int_t Limit, int_t Q, int_t K, typename InitRList, typename CheckPrimeCandidate>
struct GeneratePrimes<Limit,Q,K,InitRList,CheckPrimeCandidate,typelist<>,false>
: public GeneratePrimes<Limit, Q, K+1, InitRList> {};

template<int_t Limit, int_t Q, int_t K, typename InitRList, typename H, typename ...Tail, typename CheckPrimeCandidate>
struct GeneratePrimes<Limit,Q,K,InitRList,CheckPrimeCandidate,typelist<H,Tail...>,true>
{ typedef typelist<> type; };

template<int_t Limit, int_t Q, int_t K, typename InitRList, typename CheckPrimeCandidate>
struct GeneratePrimes<Limit,Q,K,InitRList,CheckPrimeCandidate,typelist<>,true>
{ typedef typelist<> type; };


typedef typelist<sint<2>,sint<3>,sint<5>> typelist235;

template<int_t Limit>
struct GeneratePrimesF1
{
  typedef typelist<sint<5>> List1;
  typedef typelist<sint<1>,sint<5>> RList;
  
//   typedef typename GeneratePrimes<Limit,6,1,RList,IsPrime6<CheckSmallPrimes5> >::type PrimesList;
  typedef typename GeneratePrimes<Limit,6,1,RList,IsPrime30<CheckSmallPrimes13_29> >::type PrimesList;
  typedef typename typelist_cat<typelist235,PrimesList>::type type;
};


template<int_t Limit>
struct GeneratePrimesF1direct
{
  typedef typename nextPrimesDirect<0,Limit,1>::type PrimesList;
  typedef typename typelist_cat<typelist235,PrimesList>::type type;
};


template<int_t Limit>
struct GeneratePrimesF2
{
  typedef typelist<sint<7>,sint<11>,sint<13>,sint<17>,sint<19>,sint<23>,sint<29>> List1;
  typedef typename typelist_cat<sint<1>,List1>::type RList;
  
//   typedef typename GeneratePrimes<Limit,30,1,RList,IsPrime6<CheckNone> >::type PrimesList;
  typedef typename GeneratePrimes<Limit,30,1,RList,IsPrime30<CheckNone> >::type PrimesList;
  typedef typename typelist_cat<typelist235,List1>::type List2;
  typedef typename typelist_cat<List2, PrimesList>::type type;
};


template<int_t Limit>
struct GeneratePrimesF2direct
{
  typedef typelist<sint<2>,sint<3>,sint<5>,sint<7>,sint<11>,sint<13>,sint<17>,sint<19>,sint<23>,sint<29>> List1;
  typedef typename nextPrimesDirect30<Limit>::type PrimesList;
  typedef typename typelist_cat<List1, PrimesList>::type type;
};


template<int_t Limit>
struct GeneratePrimesF3
{
  typedef typelist<sint<11>,sint<13>,sint<17>,sint<19>,sint<23>,sint<29>> List1;
  // Generate list containing primes and divisible by 11 and 13 
  // The formula 30k+r is used, but from generated numbers only divisible by 7 are eliminated
  typedef typename typelist_cat<typelist<sint<1>,sint<7>>,List1>::type RemindersListForFormula30k;
  typedef typename GenPrimesFormula<210,30,1,RemindersListForFormula30k,typelist<sint<7>>>::type NewList;
  typedef typename typelist_cat<typename typelist_cat<typelist<sint<1>>,List1>::type, NewList>::type RemindersListForFormula210k;
  typedef typename EliminateNonPrimes<NewList,typelist<sint<11>,sint<13>>>::type PList;

  // Creates a list of primes starting with 211 and not exceeding Limit
//   typedef typename GeneratePrimes<Limit,210,1,RList,IsPrime6<CheckNone> >::type PrimesList;
  typedef typename GeneratePrimes<Limit,210,1,RemindersListForFormula210k,IsPrime30<CheckSmallPrimes11_29> >::type PrimesList;
  
  // Add primes less than 211
  typedef typename typelist_cat<typelist<sint<2>,sint<3>,sint<5>,sint<7>>,List1>::type List2;
  typedef typename typelist_cat<List2, PList>::type List3;
  typedef typename typelist_cat<List3, PrimesList>::type type;
};


template<typename List>
struct CheckPrimesListAtRunTime;

template<typename H, typename ...Tail>
struct CheckPrimesListAtRunTime<typelist<H,Tail...>>
{
  static bool apply() 
  {
    if (H::value > 2 && H::value % 2 == 0) {
      std::cout << ">>> " << H::value << std::endl;
      return false;
    }
    if (H::value > 3 && H::value % 3 == 0) {
      std::cout << ">>> " << H::value << std::endl;
      return false;
    }
    if (H::value > 5 && H::value % 5 == 0) {
      std::cout << ">>> " << H::value << std::endl;
      return false;
    }
    int k = 1;
    while (6*k+1 <= (int)floor(sqrt((double)H::value))) {
      if (H::value % (6*k+1) == 0) {
	std::cout << ">>> " << H::value << std::endl;
	return false;
      }
      if (H::value % (6*k+5) == 0) {
	std::cout << ">>> " << H::value << std::endl;
	return false;
      }
      k++;
    }
    return CheckPrimesListAtRunTime<typelist<Tail...>>::apply();
  }
};

template<>
struct CheckPrimesListAtRunTime<typelist<>>
{
  static bool apply() { return true; }
};

#endif /*__metaprimes_h*/