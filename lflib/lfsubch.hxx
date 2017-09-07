/****************************************************************************
				Structure des sous chaine de caracteres

				(c)1997 - 2000 Laurent FAILLIE

		Cette classe implemente la gestion des sous chaines de caracteres

	28/10/2000 - All definition in this file (no library need) + C++ conformances
	15/10/2010 - Remove gnuisses

 ****************************************************************************/

#ifndef LFSUBCH_H
#define LFSUBCH_H 2

/*
 * Because of a brocken include file in Hp-Ux 10.20 when compiling C++ program
 * with Gcc 2.95+
 */
#ifdef __hpux__
#       include <sys/sigevent.h>
#endif

#include <cstdlib>    // Pour malloc()/free()
#include <cstring>    // Fonctions de chaines standard du C
#include <cctype>

class lfsubch {
private:
		void libcp(){ if(cp){ free(cp); cp=NULL;} };

protected:
		char *org;      // pointeur vers le debut de la chaine (originale)
		size_t lg;      // Longueur de la chaine a prendre en compte (0 si la chaine entiere est a prendre en compte)

public:
		char *cp;       // Copie (malloc()) pour l'operateur *

		lfsubch() { org=NULL; lg=0; cp=NULL; };
		lfsubch(char *x, size_t l=0){ org=x; lg=l; cp=NULL; };
		lfsubch( lfsubch &x ){ org=x.org; lg=x.lg; cp=NULL; };
		lfsubch( lfsubch, size_t );

		~lfsubch(){ libcp(); };

		char * operator *(); // Renvoie une copy de travail de la sous chaine (valide jusqu'au prochain appel de cette operateur
	bool operator==( lfsubch & );
	bool operator==( const char * );
	bool operator!=( lfsubch &x ){ return !operator==(x);};
	bool operator!=( const char *x ){ return !operator==(x);};

	char operator[](size_t);

	size_t index(const char, const size_t debut=0); // renvoie l'index de la premier occurence de 'car' dans la sous chaine (-1 si il n'y en a pas)
	unsigned long int hash(); // Calcul le code hash pour une chaine
	unsigned long int ihash(); 	// Calcul le code hash pour une chaine (insensitif)

	void supcrlf(); // Supprime un eventuel [CR]/LF qui se trouverait en fin de ligne.
		// /!\ La chaine original EST MODIFIEE
		// /!\ Ne tient absolument pas compte que c'est une sous-chaine

	size_t strlen();

	friend class lfchaine;
};

inline lfsubch::lfsubch(lfsubch x, size_t alg){
	org = x.org;
	cp=NULL;
	lg = x.lg<alg ? x.lg:alg;
}

inline char * lfsubch::operator *(){
	libcp();

	if(org){
		if(lg){
			if((cp = (char *)malloc(lg+1))){
				strncpy(cp,org,lg);
				cp[lg]=0;
			}
		} else
			return org;
	}
	return cp; // OK car libcp() a mis cp a NULL
}

inline size_t lfsubch::index(const char x, const size_t debut){
	register size_t i;

	if(!org)
		return (size_t) -1;

	if(!*org)
		return (size_t) -1;

	for(i=debut; (lg ? i < lg : 1) && org[i]; i++)
		if(org[i] == x) return i;

	return (size_t)-1;
}

inline unsigned long int lfsubch::hash(){
	register unsigned long int h;
	register size_t idx;

	for(idx=h=0; (lg ? idx < lg : 1) && org[idx]; idx++)
		h = h*10 + org[idx];

	return h;
}

inline unsigned long int lfsubch::ihash(){
	register unsigned long int h;
	register size_t idx;

	for(idx=h=0; (lg ? idx < lg : 1) && org[idx]; idx++)
		h = h*10 + toupper(org[idx]);

	return h;
}

inline bool lfsubch::operator==(lfsubch &x){
	register size_t idx;

	if(strlen() != x.strlen()) // Les chaine n'ont pas les memes longueurs
		return false;

	for( idx=0; lg ? idx < lg : 1; idx++){
		if(!org[idx] || !x.org[idx]) // Fin d'une des 2 chaines
			return (!(org[idx] | x.org[idx])); // identique uniquement si les 2 sont terminees
		if(org[idx] != x.org[idx]) return false;
	}


	return true;
}

inline bool lfsubch::operator==(const char * x){
	register size_t idx;

	for( idx=0; lg ? idx<lg : 1; idx++){
		if(!org[idx] || !x[idx]) // Fin d'une des 2 chaines
			return (!(org[idx] | x[idx]));
		if(org[idx] != x[idx]) return false;
	}

	if(x[idx]) // La chaine de comparaison n'est pas finie
		return false;

	return true;
}

inline void lfsubch::supcrlf(){
/* Supprime des eventuel [cr]/lf qui se trouveraient en fin de ligne.
 * NOTE: Rien est fait pour les ^Z present en fin de fichier provenant
 *  d'OS complettement debille (oui, oui, c'est bien le MS-DOS)
 * 28/02/1997: Vi est encore plus bete (heu non, c'est pas possible) car
 *  il transforme les CR en ^M (normal) mais ne les remet pas en CR lors
 *  de la sauvegarde...
 *
 * NOTE: Si un jour cette fonction teste la longueur de la sous chaine,
 *  il faudra modifier la methode supcrlf() de la classe chaine.
 */
	if(org){
		switch(size_t fin=::strlen(org)){
		case 0: break;
		case 1:
			if(*org == '\n')
				*org=0;
			break;
		default:
			if(org[fin-1]=='\n'){
				if(org[fin-2]==0x0d) // Peut etre un CR
					org[fin-2]=0;
				else if(fin>2){ // Peut etre un '^M'
					if(org[fin-3]=='^' && org[fin-2]=='M')
						org[fin-3]=0;
					else
						org[fin-1]=0;
				} else
					org[fin-1]=0;
			}
		}
	}
}

inline size_t lfsubch::strlen(){
/* Calcul la longueur de la chaine.
 * Si 'lg' est nul ou si la chaine est plus courte, la longueur physique est renvoye
 * sinon c'est lg
 */
	if(org){
		size_t i;

		for(i=0; (lg ? i<lg : 1) && org[i]; i++);

		return i;
	} else
		return 0;
}

inline char lfsubch::operator[](size_t x){
/* Retourne la valeur du 'x'ieme caractere de la sous chaine.
 * Note: Seule la fin de chaine par 'lg' est testee. Donc pour une chaine
 * C normale (qui se termine par un '\0'), il est possible d'explorer plus
 * loin que la fin reel de la chaine...
 */
	if(lg) if(x>lg) // Apres la fin de la sous chaine
		return 0;

	return org[x];
}

#endif
