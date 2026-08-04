#ifndef _MARCOBASE__H_
#define _MARCOBASE__H_

#define  FIX_MARCO(A) A

#define  JOIN(a,b)   a##b

#define  MARCO_JOIN(a, b) JOIN(a, b)

#define  NUM_LIST1 1
#define  NUM_LIST2 1,2
#define  NUM_LIST3 1,2,3
#define  NUM_LIST4 1,2,3,4
#define  NUM_LIST5 1,2,3,4,5
#define  NUM_LIST6 1,2,3,4,5,6
#define  NUM_LIST7 1,2,3,4,5,6,7
#define  NUM_LIST8 1,2,3,4,5,6,7,8
#define  NUM_LIST9 1,2,3,4,5,6,7,8,9
#define  NUM_LIST10 1,2,3,4,5,6,7,8,9,10
#define  NUM_LIST11 1,2,3,4,5,6,7,8,9,10,11
#define  NUM_LIST12 1,2,3,4,5,6,7,8,9,10,11,12
#define  NUM_LIST13 1,2,3,4,5,6,7,8,9,10,11,12,13


#define  LOOP1(X, A) X(A)
#define  LOOP2(X, A, ...) X(A), FIX_MARCO(LOOP1(X, __VA_ARGS__))
#define  LOOP3(X, A, ...) X(A), FIX_MARCO(LOOP2(X, __VA_ARGS__))
#define  LOOP4(X, A, ...) X(A), FIX_MARCO(LOOP3(X, __VA_ARGS__))
#define  LOOP5(X, A, ...) X(A), FIX_MARCO(LOOP4(X,  __VA_ARGS__))
#define  LOOP6(X, A, ...) X(A), FIX_MARCO(LOOP5(X,  __VA_ARGS__))
#define  LOOP7(X, A, ...) X(A), FIX_MARCO(LOOP6(X,  __VA_ARGS__))
#define  LOOP8(X, A, ...) X(A), FIX_MARCO(LOOP7(X,  __VA_ARGS__))
#define  LOOP9(X, A, ...) X(A), FIX_MARCO(LOOP8(X, __VA_ARGS__))
#define  LOOP10(X, A, ...) X(A), FIX_MARCO(LOOP9(X, __VA_ARGS__))
#define  LOOP11(X, A, ...) X(A),FIX_MARCO(LOOP10(X, __VA_ARGS__))
#define  LOOP12(X, A, ...) X(A),FIX_MARCO(LOOP11(X,  __VA_ARGS__))
#define  LOOP13(X, A, ...) X(A),FIX_MARCO(LOOP12(X,  __VA_ARGS__))

#define  KLOOP1(X, A)   LOOP1(X, A)
#define  KLOOP2(X, ...) FIX_MARCO(LOOP2(X, __VA_ARGS__))
#define  KLOOP3(X, ...) FIX_MARCO(LOOP3(X, __VA_ARGS__))
#define  KLOOP4(X, ...) FIX_MARCO(LOOP4(X, __VA_ARGS__))
#define  KLOOP5(X, ...) FIX_MARCO(LOOP5(X,  __VA_ARGS__))
#define  KLOOP6(X, ...) FIX_MARCO(LOOP6(X,  __VA_ARGS__))
#define  KLOOP7(X, ...) FIX_MARCO(LOOP7(X,  __VA_ARGS__))
#define  KLOOP8(X, ...) FIX_MARCO(LOOP8(X,  __VA_ARGS__))
#define  KLOOP9(X, ...) FIX_MARCO(LOOP9(X,  __VA_ARGS__))
#define  KLOOP10(X, ...) FIX_MARCO(LOOP10(X, __VA_ARGS__))
#define  KLOOP11(X, ...) FIX_MARCO(LOOP11(X, __VA_ARGS__))
#define  KLOOP12(X, ...) FIX_MARCO(LOOP12(X,  __VA_ARGS__))
#define  KLOOP13(X, ...) FIX_MARCO(LOOP13(X,  __VA_ARGS__))

#define  BeginLoop(FUC, N)  \
         KLOOP##N(FUC, NUM_LIST##N)


#define  MLOOP1(X, A) X(A);
#define  MLOOP2(X, A, ...) X(A);  FIX_MARCO(MLOOP1(X, __VA_ARGS__))
#define  MLOOP3(X, A, ...) X(A);  FIX_MARCO(MLOOP2(X, __VA_ARGS__))
#define  MLOOP4(X, A, ...) X(A);  FIX_MARCO(MLOOP3(X, __VA_ARGS__))
#define  MLOOP5(X, A, ...) X(A);  FIX_MARCO(MLOOP4(X,  __VA_ARGS__))
#define  MLOOP6(X, A, ...) X(A);  FIX_MARCO(MLOOP5(X,  __VA_ARGS__))
#define  MLOOP7(X, A, ...) X(A);  FIX_MARCO(MLOOP6(X,  __VA_ARGS__))
#define  MLOOP8(X, A, ...) X(A);  FIX_MARCO(MLOOP7(X,  __VA_ARGS__))
#define  MLOOP9(X, A, ...) X(A);  FIX_MARCO(MLOOP8(X, __VA_ARGS__))
#define  MLOOP10(X, A, ...) X(A); FIX_MARCO(MLOOP9(X, __VA_ARGS__))
#define  MLOOP11(X, A, ...) X(A); FIX_MARCO(MLOOP10(X, __VA_ARGS__))
#define  MLOOP12(X, A, ...) X(A); FIX_MARCO(MLOOP11(X,  __VA_ARGS__))
#define  MLOOP13(X, A, ...) X(A); FIX_MARCO(MLOOP12(X,  __VA_ARGS__))

#define  MKLOOP1(X, A)   LOOP1(X, A)
#define  MKLOOP2(X, ...) FIX_MARCO(MLOOP2(X, __VA_ARGS__))
#define  MKLOOP3(X, ...) FIX_MARCO(MLOOP3(X, __VA_ARGS__))
#define  MKLOOP4(X, ...) FIX_MARCO(MLOOP4(X, __VA_ARGS__))
#define  MKLOOP5(X, ...) FIX_MARCO(MLOOP5(X,  __VA_ARGS__))
#define  MKLOOP6(X, ...) FIX_MARCO(MLOOP6(X,  __VA_ARGS__))
#define  MKLOOP7(X, ...) FIX_MARCO(MLOOP7(X,  __VA_ARGS__))
#define  MKLOOP8(X, ...) FIX_MARCO(MLOOP8(X,  __VA_ARGS__))
#define  MKLOOP9(X, ...) FIX_MARCO(MLOOP9(X,  __VA_ARGS__))
#define  MKLOOP10(X, ...) FIX_MARCO(MLOOP10(X, __VA_ARGS__))
#define  MKLOOP11(X, ...) FIX_MARCO(MLOOP11(X, __VA_ARGS__))
#define  MKLOOP12(X, ...) FIX_MARCO(MLOOP12(X,  __VA_ARGS__))
#define  MKLOOP13(X, ...) FIX_MARCO(MLOOP13(X,  __VA_ARGS__))

#define  MBeginLoop(FUC, N)  \
         MKLOOP##N(FUC, NUM_LIST##N)

#define  _DEF_TYPENAME(A) typename T##A
#define  _DEF_T(A) T##A
#endif
