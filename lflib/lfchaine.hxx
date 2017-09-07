/***************************************************************************
		Structure pour stocker dynamiqyement des chaines

		   (c)1997 - 2010 Laurent FAILLIE 

	Comme la class 'string' de la librairie des compilo C++ mais en
	suppriment tous ce qui fait grossir l'executable...
	
	28/10/2000 - All definitions in this file (no library need) + C++ conformances
	15/10/2010 - Remove gnuisses

***************************************************************************/

#ifndef LFCHAINE_H
#define LFCHAINE_H 2

#include <lflib/lfsubch.hxx>

class lfchaine {
private:
	void libptr(){ if(ptr){ free(ptr); ptr=NULL;} lg=0; };
	size_t increment;	// increment lors des allocations

protected:
	char *ptr;	// pointeur sur le buffer alloue
	size_t lg;	// Longueur du buffer alloue (par forcement celle de la chaine)

public:
	lfchaine() { ptr=NULL; libptr(); setinc(); };
	lfchaine( const char *, size_t l=0 );
	lfchaine( lfsubch &, size_t l=0 );
	lfchaine( lfchaine & );

	inline size_t setinc( size_t i=0); // change la valeur de l'increment
	size_t alloue( size_t, bool force=false ); // Change la taille du buffer
	char *operator *() { return ptr; }; // /!\ n'est valable que tant que la chaine n'est pas modifiee

	bool operator==(const char *);
	bool operator==(lfchaine &);
	bool operator!=(const char *x){ return !operator==(x); };
	bool operator!=(lfchaine &x){ return !operator==(x); };

	lfchaine &operator=(const char *);
	lfchaine &operator +=(const char *);
	lfchaine &operator +=(const char );
	lfchaine &operator +=(lfsubch &);
	
	size_t index(const char, const size_t debut=0); // renvoie l'index de la premier occurence de 'car' dans la sous chaine (-1 s'il n'y en a pas)

	void supcrlf(); // Supprime un eventuel CR/LF qui se trouverait en fin de chaine
	unsigned long int hash(); // Calcul du code hash pour cette chaine
	unsigned long int ihash();	// Calcul du code hash pour cette chaine (insensitif)

	lfchaine &toupper();	// Passe la chaine en uppercase
	lfchaine &tolower();	// Passe la chaine en lowercase
	size_t taille(){ return lg; }; // Retourne la taille du buffer
};

inline size_t lfchaine::setinc( size_t i ){
/* Change la valeur de l'increment lors des reallocations
 * -> i: Nouvelle increment (si 0, utilise la valeur par defaut).
 * <- retourn la valeur precedente de l'increment
 */
	size_t ans=increment;
	if(i)
		increment = i;
	else 
		increment = 8; // Valeur par defaut

	return ans;
}

inline void lfchaine::supcrlf(){
	lfsubch(ptr).supcrlf();
}

inline unsigned long int lfchaine::hash(){
	return lfsubch(ptr).hash();
}

inline unsigned long int lfchaine::ihash(){
	return lfsubch(ptr).ihash();
}

inline lfchaine::lfchaine(const char *x, size_t l){
/* Constructeur d'une nouvelle chaine, a partir d'une chaine C. 
 * -> x: Chaine a copier
 * -> l: Longueur du buffer a allouer (si > a la longueur de x)
 */
	ptr=NULL; libptr(); setinc();

	if(x){
		size_t len = strlen(x)+1;
		if(l > len)
			len = l;

		alloue( len );

		strcpy(ptr,x);
	}
}

inline lfchaine::lfchaine(lfsubch &x, size_t l){
/* Constructeur a partir d'une sous chaine.
 */
	ptr=NULL; libptr(); setinc();
	
	if(x.org){ // Si la sous chaine est valide
		size_t len = x.strlen()+1;
		if(l > len)
			len = l;

		alloue( len );

		strcpy(ptr,*x); // Comme il a alloue au minimum la taille de la sous chaine, on peut la copier
	} else if(l){
		alloue(l);
		*ptr=0;
	}
}

inline lfchaine::lfchaine(lfchaine &x){
/* Constructeur de copie 
 */
	ptr=NULL; libptr(); setinc();

	if(x.ptr){
		alloue(x.lg);

		strcpy(ptr,x.ptr);
	}
}

inline size_t lfchaine::alloue( size_t taille, bool force ){
/* alloue ou realloue le buffer.
 *	-> taille : nouvelle taille pour le buffer.
 *		si NUL, rien n'ai fait (ne fait que retourner la taille actuelle)
 *		/!\ Il s'agit de la taille du buffer, son dernier caractere sera toujours force a 0.
 *	-> force : si 'faux', le buffer n'est pas modifiee si la taille demandee est inferieur a la taille actuelle.
 *		si 'vrais', la taille peut etre diminuee (avec le dernier caractere mis a 0).
 *	<- retourne la taille precedente.
 */
	size_t r= lg;

	if(taille)
		if(taille > lg || force){
			ptr= (char *)realloc(ptr,taille);
			lg=taille;
			ptr[taille-1]=0;
		}

	return r;
}

inline bool lfchaine::operator==(const char *x){
/* teste si 2 chaines sont identiques
 */
	if(ptr){
		if(!x) 
			return false;	// L'autre chaine est vide mais pas celle-ci
		else
			return !strcmp(ptr,x);
	} else {
		if(!x)
			return true;	// Les 2 chaines sont vide
		else
			return false;	// Cette chaine est vide mais pas l'autre
	}
}

inline bool lfchaine::operator==(lfchaine &x){
/* teste avec une sous chaine
 */
	if(ptr)
		return x==ptr;
	else
		if(x.ptr)
			return false;
		else
			return true; // les 2 chaines sont vides...
}

inline size_t lfchaine::index(const char x, const size_t debut){
/* Attention, aucun test n'est fait pour savoir si 'debut' est en dehors de la chaine.
 */
	register size_t i;

	if(!ptr) return (size_t)-1;

	for(i=debut; ptr[i]; i++)
		if(ptr[i] == x) return i;

	return (size_t)-1;
}

inline lfchaine &lfchaine::operator=(const char *x){
	libptr(); setinc();

	if(x){
		size_t len = strlen(x)+1;

		alloue( len );

		strcpy(ptr,x);
	}
	
	return(*this);
}

inline lfchaine &lfchaine::operator +=(const char *x){
	if(!ptr){
		size_t l=strlen(x);
		alloue(l+1);
		strcpy(ptr,x);
	} else {		
		size_t l=strlen(x), l1=strlen(ptr);
		if(l+l1>=lg){
			alloue( (l+l1+1 > lg+increment) ? l+l1+1 : lg+increment); // Augmente la taille du buffer
		}
		strcat(ptr,x);
	}
	return *this;
}

inline lfchaine &lfchaine::operator +=(const char x){
	if(!ptr){
		alloue( (increment > 2) ? increment : 2);
		*ptr=x;
		ptr[1]=0;
	} else {
		size_t l=strlen(ptr);
		if(l+2>lg)
			alloue(lg+increment);
		ptr[l]=x;
		ptr[l+1]=0;
	}

	return *this;
}

inline lfchaine &lfchaine::operator +=(lfsubch &x){
	return operator += (*x);
}

inline lfchaine &lfchaine::toupper(){
	if(ptr)
		for(register char *x=ptr; *x; x++) *x = ::toupper(*x);

	return *this;
}

inline lfchaine &lfchaine::tolower(){
	if(ptr)
		for(register char *x=ptr; *x; x++) *x = ::tolower(*x);

	return *this;
}
#endif
