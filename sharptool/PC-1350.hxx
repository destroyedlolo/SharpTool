/*
 * PC-1350.hxx
 *
 *	cpu redefinition for PC-1350
 *
 *	14/08/2002	Creation
 */

#ifndef PC1350_HXX
#define PC1350_HXX

#include "emul.hxx"

	/* Japanees version of PC-1350 */
struct pc1350K : virtual public CPU {
	pc1350K() : CPU() { type = "1350K"; };

	virtual void gui();
	virtual void refresh_display();
	virtual void updatevideo(unsigned short int);
	virtual void LCDOff();
	virtual unsigned char customkey(long int);

	virtual unsigned char ina();
	virtual unsigned char inb();
	virtual unsigned char read(unsigned short);
	virtual unsigned char pread(unsigned short);
	virtual void write(unsigned short, unsigned char);

	virtual void patch();
	bool illegal();
};

	/* International version of PC-1350 */
struct pc1350 : virtual public pc1350K {
	pc1350() : pc1350K() { type = "1350"; };

	virtual unsigned char ina();
};

#endif
