/*! \file DES.h
//
//	Header File : DES.h
//
*/
#ifndef		DES_H
#define		DES_H

#include <stdlib.h>

#define DES_LITTLE_ENDIAN

/*************** Macros ***************************************************/
////////	rotate by using shift operations	////////
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (unsigned int)((x) << (n)) | (unsigned int)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (unsigned int)((x) >> (n)) | (unsigned int)((x) << (32-(n))) )
#endif

#define DES_KEY			32
#define DES_NO_ROUNDS	16

#define	ROUNDKEY_SSE	128 // 32 * 4	
#define	SP_SIZE			8
#define	SP_EXPAND_SIZE	4096

#define IP(l,r) {						\
	unsigned int tt;							\
	PERM_OP(r,l,tt, 4,0x0f0f0f0f);		\
	PERM_OP(l,r,tt,16,0x0000ffff);		\
	PERM_OP(r,l,tt, 2,0x33333333);		\
	PERM_OP(l,r,tt, 8,0x00ff00ff);		\
	PERM_OP(r,l,tt, 1,0x55555555);		\
}

#define HPERM_OP(a,t,n,m) {				\
	(t) = ((((a)<<(16-(n)))^(a))&(m));	\
	(a) = (a) ^ (t) ^ (t >> (16-(n)));	\
}

#define PERM_OP(a,b,t,n,m) {			\
	(t) = ( ( ( (a) >> (n) ) ^(b) )&(m));		\
	(b) ^= (t);							\
	(a) ^= ((t)<<(n));					\
}

#define FP(l,r) {						\
	unsigned int tt;							\
	PERM_OP(l,r,tt, 1,0x55555555);		\
	PERM_OP(r,l,tt, 8,0x00ff00ff);		\
	PERM_OP(l,r,tt, 2,0x33333333);		\
	PERM_OP(r,l,tt,16,0x0000ffff);		\
	PERM_OP(l,r,tt, 4,0x0f0f0f0f);		\
}

//	MACROS for ENCRYPTION - DES Round Function
#define DES_ENCRYPT(LL,R,S) {				\
	u = R ^ _roundKey[S  ];				\
	t = R ^ _roundKey[S+1];				\
	t = ROTR_DWORD(t,4);					\
	LL ^= des_SPtrans[0][(u>> 2)&0x3f]		\
		^ des_SPtrans[2][(u>>10)&0x3f]		\
		^ des_SPtrans[4][(u>>18)&0x3f]		\
		^ des_SPtrans[6][(u>>26)&0x3f]		\
		^ des_SPtrans[1][(t>> 2)&0x3f]		\
		^ des_SPtrans[3][(t>>10)&0x3f]		\
		^ des_SPtrans[5][(t>>18)&0x3f]		\
		^ des_SPtrans[7][(t>>26)&0x3f];		\
}

//!	DES Class
/*!
*/
class DES
{

//	Constructor & Destructor
//
public:
	DES();
	virtual ~DES();

//	Public Method
//
public:
	// 암호/복호화 하기 전에 반드시 key를 설정하여야 한다.
	void		setKey( unsigned long long key );

	// 파일에서 파일로 암호/복호화 하여 저장한다. 
	// readAll은 true일 경우 메모리에 데이터를 전부 올린 후 암호/복호화 하고 false 일 경우 8 byte씩 읽어서 암호/복호화 한다.
	//bool		encrypt( const char* inFileName, const char* outFileName, bool readAll = false );
	//bool		decrypt( const char* inFileName, const char* outFileName, bool readAll = false );
	
	// 메모리에 있는 데이터를 암호/복호화 한다.
	void		encrypt( unsigned char* data, int size );
	void		decrypt( unsigned char* data, int size );
	

//	Private Methods
//
public:
	void		encrypt( unsigned char* data );
	void		decrypt( unsigned char* data );

//	Virtual Method
//
protected:

//	Attributes
//
private:
	unsigned int		_roundKey[DES_KEY];

};

#endif
