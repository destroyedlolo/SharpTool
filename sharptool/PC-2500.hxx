/*
 * PC-2500.hxx
 *
 *	cpu redefinition for PC-2500
 *
 *	19/10/2010	Creation
 */

#ifndef PC2500_HXX
#define PC2500_HXX

#include "PC-1350.hxx"

	/* Japanees version of PC-2500 */
struct pc2500K : virtual public pc1350K {
	pc2500K() : CPU() { type = "2500K"; };

	virtual unsigned char customkey(long int);
	virtual bool numericKP(){ return true; };

	virtual unsigned char ina();
	virtual unsigned char read(unsigned short);
	virtual unsigned char pread(unsigned short);
	virtual void patch();
};

	/* International version of PC-1350 */
struct pc2500 : virtual public pc2500K {
	pc2500() : CPU() { type = "2500"; };
};

#endif
