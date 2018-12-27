#ifndef __CUSTOMERPARA_H
#define __CUSTOMERPARA_H

//Digital front plus 0b means it is binary
//Digital front plus 0 means it is octonary 
//Digital front plus 0x means it is hexadecimal

//Digital front plus none means it is decimal 


/************生产用代码****************/
#define	L508003BC			5180030203
#define	L508003BD			5180030204
#define	L508010JB			5180101002

#define	L5180030A			5180030001
#define	L5180030B			5180030002
#define	L5180030C			5180030003
#define	L5180030D			5180030004
#define	L5180030E			5180030005
#define	L5180030F			5180030006
#define	L5180030G			5180030007
#define	L5180030H			5180030008

#define	L5180100A			5180100001
#define	L5180100B			5180100002
#define	L5180100C			5180100003
#define	L5180110A			5180110001
#define	L5180130A			5180130001


/************样品用代码****************/
#define	L51810300			5181030000
#define	L51810400			5181040000
#define	L51810500			5181050000
#define	L51810600			5181060000
#define	L51810700			5181070000
#define	L51811200			5181120000
/*****************test************************/
#define	L10800001           1080000100
//*****************************************************

#define	CumstomerNum			1080000100


/***************NEW***************************/
#if(L5180030A == CumstomerNum)
        #include	"CustomerL5180030A.h"
#elif(L5180030B == CumstomerNum)
        #include	"CustomerL5180030B.h"
#elif(L5180030C == CumstomerNum)
        #include	"CustomerL5180030C.h"
#elif(L5180030D == CumstomerNum)
        #include	"CustomerL5180030D.h"
#elif(L5180030E == CumstomerNum)
        #include	"CustomerL5180030E.h"
#elif(L5180030F == CumstomerNum)
        #include	"CustomerL5180030F.h"
#elif(L5180030G == CumstomerNum)
        #include	"CustomerL5180030G.h"
#elif(L5180030H == CumstomerNum)
        #include	"CustomerL5180030H.h"


#elif(L5180100A == CumstomerNum)
        #include	"CustomerL5180100A.h"
#elif(L5180100B == CumstomerNum)
        #include	"CustomerL5180100B.h"
#elif(L5180100C == CumstomerNum)
        #include	"CustomerL5180100C.h"
#elif(L5180110A == CumstomerNum)
        #include	"CustomerL5180110A.h"
#elif(L5180130A == CumstomerNum)
	    #include	"CustomerL5180130A.h"

/******************Sample*******************/

#elif(L51810300 == CumstomerNum)
        #include	"CustomerL51810300.h"
#elif(L51810400 == CumstomerNum)
        #include	"CustomerL51810400.h"
#elif(L51810500 == CumstomerNum)
        #include	"CustomerL51810500.h"
#elif(L51810600 == CumstomerNum)
        #include	"CustomerL51810600.h"
#elif(L51810700 == CumstomerNum)
        #include	"CustomerL51810700.h"
#elif(L51811200 == CumstomerNum)
        #include	"CustomerL51811200.h"

/*****************OLD************************/

#elif(L508003BC == CumstomerNum)
        #include	"CustomerL508003BC.h"
#elif(L508003BD == CumstomerNum)
        #include	"CustomerL508003BD.h"
#elif(L508010JB == CumstomerNum)
        #include	"CustomerL508010JB.h"
/*****************test************************/
#elif(L10800001 == CumstomerNum)
        #include	"CustomerL10800001.h"

#else
	#error	"没有载入合适的客户档案.h文件"
#endif	



#endif