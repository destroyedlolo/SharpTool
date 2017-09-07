/*
 *                  AsmESRH.c
 *              © LFSoft 1991-94
 *
 *  Historique:
 *  xx/xx/xxxx  PréVersion en AmigaBasic
 *  xx/xx/1991  V1.0
 *  21/03/1994  V1.1 Ajout de + & - pour les calculs de labels
 *	13/08/2002	v1.2 Consmetics changes to make it compilable by recent compiler
 *				(GCC) and under unix
 *	17/08/2002	add 'illegal' instruction
 *	22/08/2002	Label file can have blank lines and comments
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* pour le debugage */
/* #define DEBUG */
/*#define DEBUG2*/
#define TLAB

/* quelques constantes */
#define MAX_LINE 255
#define MAXC_LAB 80
#define MAX_LAB 300

#define isSPC(c)    ((c)==' ' || (c)=='\t')
#define isLAB(c)    ((c)=='[' || (c)=='{')

typedef unsigned short Adr;

    FILE *fcode=NULL,*temp=NULL;
    FILE *err=NULL,*flab=NULL,*fcas;
    int numl=0; /* Numéro de la ligne en cours */
    int nl=0; /* Numéro de la ligne BASIC en cours */
    char l[MAX_LINE]; /* derniere ligne lue */
    char ins[MAX_LINE]; /* instruction */
    char pass; /* 1er ou 2em pass (0 ou 1)*/
    Adr arg; /* argument des instructions */
    Adr a; /* @dr pour le LM */
    size_t num; /* numéro de l'instruction */
 /* type du fichier généré */
#define LM 0
#define BASIC 1
    volatile char mode;

/* Les labels compris entre [] sont considérés comme GLOBAux (sauvegardables)
    ceux compris entre {} sont considéré comme LOCAux */

typedef struct {
    char type;          /* GLOBAL ou LOCAL */
    char nom[MAXC_LAB]; /* Nom du label */
    Adr adr;            /* @dresse */
    } Lab ;

#define LOCAL 0
#define GLOBAL 1
#define CHARGER 2

    Lab lab[MAX_LAB]; /* labels */
    char lb[MAXC_LAB]; /* labels scratchs */
    volatile size_t clab=0; /* Numéro du dernier label */

#define CASE1 0x7a
#define CASE2 0x69

typedef struct {
    const char *nom;
    void (*fonct)();
    unsigned char nbre_octets;
    unsigned char code;
} Inst;

/* On prototype ! */
void arg1(),narg(),argadr(),argsadr(),cas(),adrreg(),lp(),cal(),loop(),liab(),lidl(),dc();
char *supSPC();
void Erreur(), Attention(), ferme_fichiers(),lectligne(),sort();
char * lectlab();
Adr valh();
unsigned int chinstr();

Inst inst[]={
    {"LII",arg1,2,0},
    {"LIJ",arg1,2,1},
    {"LIA",arg1,2,2},
    {"LIB",arg1,2,3},
    {"IX",narg,1,4},
    {"DX",narg,1,5},
    {"IY",narg,1,6},
    {"DY",narg,1,7},
    {"MVW",narg,1,8},
    {"EXW",narg,1,9},
    {"MVB",narg,1,0xA},
    {"EXB",narg,1,0xB},
    {"ADN",narg,1,0xC},
    {"SBN",narg,1,0xD},
    {"ADW",narg,1,0xE},
    {"SBW",narg,1,0xF},
    {"LIDP",argadr,3,0x10},
    {"LIDL",lidl,2,0x11},
    {"LIAB",liab,4,0xff},
    {"LIP",adrreg,2,0x12},
    {"LIQ",adrreg,2,0x13},
    {"ADB",narg,1,0x14},
    {"SBB",narg,1,0x15},
    {"MVWD",narg,1,0x18},
    {"EXWD",narg,1,0x19},
    {"MVBD",narg,1,0x1A},
    {"EXBD",narg,1,0x1B},
    {"SRW",narg,1,0x1C},
    {"SLW",narg,1,0x1D},
    {"FILM",narg,1,0x1E},
    {"FILD",narg,1,0x1F},
    {"LDP",narg,1,0x20},
    {"LDQ",narg,1,0x21},
    {"LDR",narg,1,0x22},
    {"CLA",narg,1,0x23},
    {"IXL",narg,1,0x24},
    {"DXL",narg,1,0x25},
    {"IYS",narg,1,0x26},
    {"DYS",narg,1,0x27},
    {"JRNZP",argsadr,2,0x28},
    {"JRNZM",argsadr,2,0x29},
    {"JRNCP",argsadr,2,0x2A},
    {"JRNCM",argsadr,2,0x2B},
    {"JRP",argsadr,2,0x2C},
    {"JRM",argsadr,2,0x2D},
    {"LOOP",loop,2,0x2F},
    {"STP",narg,1,0x30},
    {"STQ",narg,1,0x31},
    {"STR",narg,1,0x32},
    {"PUSH",narg,1,0x34},
    {"DATA",narg,1,0x35},
    {"RTN",narg,1,0x37},
    {"JRZP",argsadr,2,0x38},
    {"JRZM",argsadr,2,0x39},
    {"JRCP",argsadr,2,0x3A},
    {"JRCM",argsadr,2,0x3B},
    {"INCI",narg,1,0x40},
    {"DECI",narg,1,0x41},
    {"INCA",narg,1,0x42},
    {"DECA",narg,1,0x43},
    {"ADM",narg,1,0x44},
    {"SBM",narg,1,0x45},
    {"ANMA",narg,1,0x46},
    {"ORMA",narg,1,0x47},
    {"INCK",narg,1,0x48},
    {"DECK",narg,1,0x49},
    {"INCM",narg,1,0x4A},
    {"DECM",narg,1,0x4B},
    {"INA",narg,1,0x4C},
    {"NOPW",narg,1,0x4D},
    {"WAIT",arg1,2,0x4E},
    {"CUP",narg,1,0x4F},
    {"INCP",narg,1,0x50},
    {"DECP",narg,1,0x51},
    {"STD",narg,1,0x52},
    {"MVDM",narg,1,0x53},
    {"READM",narg,1,0x54},
    {"MVMD",narg,1,0x55},
    {"READ",narg,1,0x56},
    {"LDD",narg,1,0x57},
    {"SWP",narg,1,0x58},
    {"LDM",narg,1,0x59},
    {"SL",narg,1,0x5A},
    {"POP",narg,1,0x5B},
    {"OUTA",narg,1,0x5D},
    {"OUTF",narg,1,0x5F},
    {"ANIM",arg1,2,0x60},
    {"ORIM",arg1,2,0x61},
    {"TSIM",arg1,2,0x62},
    {"CPIM",arg1,2,0x63},
    {"ANIA",arg1,2,0x64},
    {"ORIA",arg1,2,0x65},
    {"TSIA",arg1,2,0x66},
    {"CPIA",arg1,2,0x67},
    {"CASE",cas,0,0xff},
    {"TEST",arg1,2,0x6B},
    {"CDN",narg,1,0x6F},
    {"ADIM",arg1,2,0x70},
    {"SBIM",arg1,2,0x71},
    {"ADIA",arg1,2,0x74},
    {"SBIA",arg1,2,0x75},
    {"CALL",argadr,3,0x78},
    {"JP",argadr,3,0x79},
    {"JPNZ",argadr,3,0x7C},
    {"JPNC",argadr,3,0x7D},
    {"JPZ",argadr,3,0x7E},
    {"JPC",argadr,3,0x7F},
    {"LP",lp,1,0xff},
    {"INCJ",narg,1,0xC0},
    {"DECJ",narg,1,0xC1},
    {"INCB",narg,1,0xC2},
    {"DECB",narg,1,0xC3},
    {"ADCM",narg,1,0xC4},
    {"SBCM",narg,1,0xC5},
    {"TSMA",narg,1,0xC6},
    {"CPMA",narg,1,0xC7},
    {"INCL",narg,1,0xC8},
    {"DECL",narg,1,0xC9},
    {"INCN",narg,1,0xCA},
    {"DECN",narg,1,0xCB},
    {"INB",narg,1,0xCC},
    {"NOPT",narg,1,0xCE},
    {"SC",narg,1,0xD0},
    {"RC",narg,1,0xD1},
    {"SR",narg,1,0xD2},
    {"WRIT",narg,1,0xD3},
    {"ANID",arg1,2,0xD4},
    {"ORID",arg1,2,0xD5},
    {"TSID",arg1,2,0xD6},
    {"CPID",arg1,2,0xD7},
    {"LEAVE",narg,1,0xD8},
    {"EXAB",narg,1,0xDA},
    {"EXAM",narg,1,0xDB},
    {"OUTB",narg,1,0xDD},
    {"OUTC",narg,1,0xDF},
    {"CAL",cal,2,0xff},
    {"DC",dc,0,0xff},
	{"illegal",narg,1,0x3F}
};

char *getlab(lb) /* car un fscanf ne permet pas de lire les
                    labels qui comportent des espaces */
char *lb;
{
    size_t i;

    fgets(lb,MAXC_LAB,temp);
    if ((*lb=',')) strcpy(lb,lb+1);
    if (lb[(i=strlen(lb)-1)]=='\n') lb[i]=0;
    return(lb);
}

Adr adrlab(l)
char *l;
{
    int i;
    for(i=0;i<MAX_LAB;i++)
        if(!strcmp(l,lab[i].nom))
            return(lab[i].adr);
    Erreur("Label inconnu !");
	return NULL; /* never used : juste to avoid a warning */
}

Adr adr(l)
char *l;
{
    int i;

    if(isLAB(*l))
        return(adrlab(strcpy(l,l+1)));
    sscanf(l,"%u",&i);
#ifdef DEBUG
    if(i>0xffff)Erreur("adr()!! Erreur interne au compilateur !! :-(");
#endif
    return((Adr)i);
}

void arg1()
{
    if(pass){
        int i;
        narg();
        fscanf(temp,",%u",&i);
#ifdef DEBUG
        if(i>255) Erreur("arg1()!! Erreur interne au compilateur !! :-(");
#endif
        sort((Adr)i);
    }else{
        if((arg=valh(l))>255)
            Erreur ("Argument incorrect !");
        else
            fprintf(temp,",%d",arg);
    }
}

void narg()
{
    if(pass) sort(inst[num].code);
/* Il n'y a rien a faire donc ... on ne fait rien !*/
}

void arga(f)
FILE *f;
{
    if(isLAB(*l))
        fprintf(f,",[%s",lectlab(l,lb));
    else
        fprintf(f,",%d",valh(l));
}

void argadr()
{
    if(pass){
        Adr arg;
        if(num!=chinstr("CASE")){
            narg();
            getlab(lb);
        }
        if(abs((arg=adr(lb))-a)<256 && num!=chinstr("CALL") && num!=chinstr("CASE"))
            Attention("Optimisation possible :JxP -> JRx");
        if(num==chinstr("CALL") && arg<0x2000)
            Attention("Optimisation possible :CALL -> CAL");
        sort((arg >> 8)& 0xff);
        sort(arg & 0xff);
    }else
        arga(temp);
}

void liab()
{
    if(pass){
        Adr arg;

        getlab(lb);
        arg=adr(lb);
        sort(inst[chinstr("LIB")].code);
        sort((arg >> 8)& 0xff);
        sort(inst[chinstr("LIA")].code);
        sort(arg & 0xff);
    }else
        arga(temp);
}

void argsadr()
{
    if(pass){
        Adr arg;
        char x[16];
        strcpy(x,inst[num].nom);

        getlab(lb);
        if(isLAB(*lb)){
            if(x[strlen(x)-1]=='M'){
                if((arg=adrlab(strcpy(lb,lb+1)))>a+1){
                    Attention("JRxM -> JRxP");
                    x[strlen(x)-1]='P';
                    num=chinstr(x);
                    arg-=a+1;
                }else
                    arg=a+1-arg;
            }else {
                if((arg=adrlab(strcpy(lb,lb+1)))<a+1){
                    Attention("JRxP -> JRxM");
                    x[strlen(x)-1]='M';
                    num=chinstr(x);
                    arg=a+1-arg;
                }else
                    arg-=a+1;
            }
        }else
            arg=adr(lb);
        if(arg>255)Erreur("Les sauts relatifs doivent etre < a 255");
        narg();
        sort(arg);
    }else{
       if(isLAB(*l))
            fprintf(temp,",[%s",lectlab(l,lb));
        else
            if((arg=valh(l))>255)
                Erreur ("argument incorrect !!");
            else
                fprintf(temp,",%d",arg);
    }
}

void lidl()
{
    if(pass){
        getlab(lb);
        narg();
        sort(adr(lb)& 0xff);
    }else
        argsadr();
}

void loop()
{

    if(pass){
        Adr arg;
        narg();
        getlab(lb);
        if(isLAB(*lb)){
            if((arg=adrlab(strcpy(lb,lb+1)))>a)
                Erreur("Un LOOP ne peut pas sauter en avant");
            arg=a-arg;
        }else
            arg=adr(lb);
        if(arg>255)Erreur("Les sauts relatifs doivent etre < a 255");
        sort((char)arg);
    } else argsadr();
}

void cas()
{
    if(pass){
        int i,nbre,ch;

        sort(CASE1);
        getlab(lb);
        fscanf(temp,"%d",&nbre);
        sort(nbre);
        argadr();
        sort(CASE2);
        for(i=0;i<nbre;i++){
            fscanf(temp,"%d,%d",&numl,&ch);
            getlab(lb);
            sort(ch);
            argadr();
        }
        fscanf(temp,"%d",&numl);
        getlab(lb);
        argadr();
    }else{
        short i,nbre=0;
        char c;

        if((fcas=fopen("T:case","w+"))==0)
            Erreur("Impossible d'ouvrir \"T:case\"\n");
        argadr();/* Adr de retour */
        fputc('\n',temp);
        do {
            lectligne();
            if(!strcmp(l,";fini"))
                Erreur("Le programme fini au milieu d'un CASE !");
            if(isLAB(*l))
                Erreur("Définition d'un label au milieu d'un CASE");
            strcpy(ins,l);
            if((i=strcspn(ins,", \t"))){
                ins[i]=0;
                supSPC(i,l);
            } else
                Erreur("ligne incomplete!");
            fprintf(fcas,"%d",numl);
            if(strcasecmp(ins,"ELSE")){
                if((arg=valh(ins))>255)
                    Erreur("argument incorrect !!");
                fprintf(fcas,",%d",arg);
                arga(fcas);
                fputc('\n',fcas);
                if(++nbre>255)
					 Erreur("argument incorrect !!");
            }else{
                arga(fcas);
                fputc('\n',fcas);
                break;
            }
        }while(1);
#ifdef DEBUG2
        printf("case :%d choix luent\n",nbre);
#endif
        fprintf(temp,"%d\n",nbre);
        fseek(fcas,0,0);
        while(!feof(fcas))
            if(isprint(c=fgetc(fcas))||iscntrl(c)) fputc(c,temp);
        fclose(fcas);
        a+=7+3*nbre;
    }
}

void adrreg()
{
    if(pass){
        int i;
        narg();
        fscanf(temp,",%u",&i);
#ifdef DEBUG
        if(i>0x5f) Erreur("adrreg()!! Erreur interne au compilateur !! :-(");
#endif
        sort((unsigned short)i);
    }else{
       if((arg=valh(l))>0x5f)
            Erreur ("argument incorrect !!");
        if(num==chinstr("LIP") && arg<0x40)
            Attention("Optimisation possible : LIP ->LP");
        fprintf(temp,",%d",arg);
    }
}

void lp()
{
    if(pass){
        int i;
        fscanf(temp,",%u",&i);
#ifdef DEBUG
        if(i>0x3f) Erreur("lp()!! Erreur interne au compilateur !! :-(");
#endif
        sort((unsigned short)i+0x80);
    }else{
        if((arg=valh(l))>0x3f)
            Erreur("Argument >&3f trops grand, transformer le LP en LIP");
        fprintf(temp,",%d",arg);
    }
}

void cal()
{
    if(pass){
        Adr arg;
        getlab(lb);
        if((arg=adr(lb))>0x1fff)
            Erreur ("argument incorrect !!\n[0mTranformer le [4mCAL[0m en [4mCALL[0m!");
        sort(((arg >> 8)& 0xff)+0xe0);
        sort(arg & 0xff);
    }else{
        if(isLAB(*l))
            fprintf(temp,",[%s",lectlab(l,lb));
        else
            if((arg=valh(l))>0x1fff)
                Erreur ("argument incorrect !!\n[0mTranformer le [4mCAL[0m en [4mCALL[0m!");
            else
                fprintf(temp,",%d",arg);
    }
}

void dc()
{
    size_t i,j;
    if(pass){
        getlab(lb);
        switch (*lb) {
            case '\'':
                sort(lb[1]);
                break;
            case '"':
                i=strlen(strcpy(lb,lb+1))-1;
                for(j=0;j<i;j++)
                    sort(lb[j]);
                break;
            default :
                { Adr arg;
                arg=valh(lb);
                sort(arg & 0xff); /* poids faible en premier */
                if (arg>255)sort((arg >> 8)& 0xff);
                }
        }
    }else{
        switch (*l) {
            case '\'':
                if(l[2]!='\'')Erreur ("argument incorrect !!");
                fprintf(temp,",'%c",l[1]);
                a++;
                break;
            case '"':
                if(!(i=strcspn(strcpy(l,l+1),"\"")))
                    Erreur("argument incorrect !!");
                l[i]=0;
                if((i=strlen(l))>MAXC_LAB-3)
                    Erreur("La chaîne est trops grande");
                a+=i;
                fprintf(temp,",\"%s",l);
                break;
            default :
                { Adr arg;
                arg=valh(l);
                if(arg<256) a++; else a+=2;
                fprintf(temp,",%d",arg);
                }
        }
    }
}

unsigned int chinstr(ins)
char ins[];
{
    int i;
    int n=-1;

    for(i=0;(i<(int)(sizeof(inst)/sizeof(Inst)))&&(n==-1);i++)
        if (!strcasecmp(ins,inst[i].nom))
            n=i;
    return (n);
}

void Erreur(const char *msg)
{
    if(numl){
        if (err) fprintf(err,"Erreur ligne %d : %s\n",numl,msg);
        printf("\n[0;33mErreur ligne %d :",numl);
        }
    else{
        if (err) fputs(msg,err);
        puts("[0;33m");
        }

    printf("%s[0m\n",msg);
    ferme_fichiers();
    exit(1);
}

void Attention(const char *msg)
{
    if(numl){
        if (err) fprintf(err,"ligne %d : %s\n",numl,msg);
        printf("\n[0;33mLigne %d :",numl);
        }
    else {
        if (err) fputs(msg,err);
        puts("[0;33m");
        }

    printf("%s[0m\n",msg);
}


int help(argc,argv)
/* Affiche une aide pour :
    - pas d'option;
    - option ?; ( Ne marche pas toujours avec Csh )
    - option -h ou -H;
*/
int argc;
char *argv[];
{
    if(argc>1){
        if(argv[1][0]=='?' || (argv[1][0]=='-' && tolower(argv[1][1])=='h'))
            return(1);
        else
            return(0);
        } else return(1);

}

void ouvre_fichiers(in,out)
/* Ouvre les fichiers */
char *in,*out;
{
    char ferr[255];

    if ((fcode=fopen(in,"r"))==0)
        Erreur("Erreur d'ouverture pour le fichier d'entrée\n");
    if (strlen(out)>250)
        Erreur("Le fichier de sortie a un nom trops grand\n");
    else
        if((err=fopen(strcat(strcpy(ferr,out),".err"),"w"))==0)
            Erreur("Erreur d'ouverture pour le fichier d'Erreurs !\n");
    if((temp=fopen("T:temp_asmEsr_H","w+"))==0)
        Erreur("Impossible d'ouvrir \"T:temp_asmEsr_H\"\n");
    if((flab=fopen(strcat(strcpy(ferr,out),".lab"),"w"))==0)
            Erreur("Erreur d'ouverture pour le fichier de labels !\n");
}

void ferme_fichiers()
/* fermeture des fichiers */
{
    if(fcode)   fclose(fcode);
    if(err)     fclose(err);
    if(temp)    fclose(temp);
    if(fcas)    fclose(fcas);
    if(flab)    fclose(flab);
}

char *supSPC(pos,l)
size_t pos;
char *l;
{
    char l2[MAX_LINE];
    size_t p=pos;

    strncpy(l2,l,MAX_LINE);
/*    if ((pos = (pos<0)?0:pos)<strlen(l2)) */
	if (pos<strlen(l2))        
		while (isSPC(l2[p]) && l2[p]!=0)
            p++;
    else {
        *l2=0;p=0;
    }
    strcpy(l,&l2[p]);
    return(l);
}

void pre()
/* Préprocesseur ( enfin presque ) */
{
        size_t i;

        strcpy(ins,l+1);
        if((i=strcspn(ins," \t"))) ins[i]=0;
        if (!strcasecmp("DEFLAB",ins)){
                supSPC(7,l);
                if(!isLAB(*l))
                        Erreur("#DEFLAB doit définir un ... label !");
        if (!(clab<MAX_LAB)) {
                printf ("[33mlimite [4m%d[0;33m labels",MAX_LAB);
            Erreur ("Trops de labels definis");
        }
        lab[clab].type=(*l=='[')?GLOBAL:LOCAL;
        strcpy(lab[clab].nom,lectlab(l,lb));
        lab[clab++].adr=valh(l);
    } else if (!strcasecmp("LOADLAB",ins)){
        FILE *ll;
        supSPC(0,strcpy(l,l+8));
        if((i=strcspn(l," \t;"))) l[i]=0;
        if(!(ll=fopen(l,"r")))   Erreur("Impossible d'ouvrir le fichier");
        while(!feof(ll)){
            if (!(clab<MAX_LAB)) {
                printf ("[33mlimite [4m%d[0;33m labels",MAX_LAB);
                Erreur ("Trops de labels definis");
            }
            fgets(l,MAX_LINE,ll);
            if(feof(ll))break;
			if(*l=='\n')
				continue;
			if(*l=='#' || *l==';')
				continue;
            lab[clab].adr=valh(l);
            strcpy(lab[clab].nom,lectlab(strpbrk(l,"["),lb));
            lab[clab++].type=CHARGER;
        }
        fclose(ll);
    } else Erreur ("Instruction préprocesseur inconnue !");
}

void lectligne()
/* Lecture d'une ligne */
{
    int i; /* Scratch */

    while (!feof(fcode)){
        fgets(l,MAX_LINE,fcode);
        numl++;
        if (*l){
            if (l[(i=strlen(l)-1)]=='\n') l[i]=0;
            if (isSPC(*l)){
                supSPC(0,l);
            }
        if (*l=='#') {pre(); *l=0;}
        if (*l!=';' && *l!=0 ) break;
/* Bug : si la derniere ligne ne comporte pas de CR, elle est éffacée */
        }
    }
    if (feof(fcode)) strcpy(l,";fini");
#ifdef DEBUG2
    printf(" lectline() renvoit %s\n",l);
#endif
}

Adr valh(v)
/* atoi(v) avec convertion hexa */
char v[];
{
    int i;

    supSPC(0,v);
    if(!*v) Erreur("Il manque l'argument");

    if (*v=='&' || *v=='$')
        i=1;
    else
        i=0;
    if(sscanf(&v[i],(i)?strdup("%x"):strdup("%d"),&i)<0) Erreur ("l'argument doit etre numérique");
    if (i<0 || i>0xffff)
                Erreur ("l'argument n'est pas une @dr. valide !");
    return((Adr)i);
}

char * lectlab(l,lb)
/* lecture d'un label ,=> lb =label, ret * sur lb, le label est suprimé de l*/
char *l,*lb;
{
    char *i;

    if ((i=strpbrk(l,(*l=='[')?strdup("]"):strdup("}")))==0){
        if (*l=='[')
            Erreur ("']' pas trouvé dans la déclaration d'un label");
        else
            Erreur ("'}' pas trouvé dans la déclaration d'un label");
	}
    if(i==l+1)
        Erreur ("Label nul {} ou [] interdit !");
    if(i>l+MAXC_LAB)
        Erreur ("Label trops grand !");
    strncpy(lb,l+1,i-l-1);
    lb[i-l-1]=0;
    strcpy(l,i+1);
    return(lb);
}

char *ucase(c)
char *c;
{
    char *x=c;
    while(*x){
        *x=toupper(*x);
        x++;
    }
    return(c);
}

void sort(code)
unsigned char code;
{
    static unsigned char cs=0;

    if(mode==LM){
        char t[10],*pt;
        if (!*l){
            sprintf(l,"0000%x ",a);
/*            strins(l,"0000");*/
            pt=l+strlen(l)-5;
            strcpy(l,pt);
            cs=0;
        }
        sprintf(t,"00%x",code);
/*        strins(t,"00");  */
        pt=t+strlen(t)-2;
        strcat(l,pt);
        cs+=code;
        a++;
        if(strlen(l)>20) {
            sprintf(t,"00%x",cs);
/*            strins(t,"00");  */
            pt=t+strlen(t)-2;
            strcat(strcat(l,":"),pt);
            fprintf(fcode,"%s\n",ucase(l));
#ifdef DEBUG2
            printf("%s\n",l);
#endif
            *l=0;cs=0;
        }
    }else {
        char t[10];
        if (!*l)
            sprintf(l,"%d POKE &%x",nl+=10,a);
        sprintf(t,",&%x",code);
        strcat(l,t);
        a++;
        if(strlen(l)>70) {
            fprintf(fcode,"%s\n",ucase(l));
            *l=0;
#ifdef DEBUG2
            printf("%s\n",l);
#endif
        }
    }
}


int main(argc,argv)
int argc;
char *argv[];
{
    unsigned int i; /* scratch */
    int out;

        puts("\n\n\n                           [33;41m   AsmEsr_H   [0m");;
        puts("                          [33;42m © LF soft 1991-2002 [0m\n");

        puts("\n                                V 1.2\n\n\n");

    if(help(argc,argv)){
            puts("Usage :[0;33m AsmEsr_H [-b] source dest [0;3m");
            puts("avec [0;33m source [0;3m : nom du fichier source.");
            puts("     [0;33m dest [0;3m : nom du fichier destination.\n");
            puts(" Ce programme est un assembleur pour les Sharps à cpu [0mESR_H[3m.");
            puts("Avec le [0;33m-b[0;3m, il genere un fichier basic (POKE) ,sinon un fichier 'binaire' est\nproduit.");
            exit(0);
    }

    mode=LM;
    switch (argc){
        case 3:
            ouvre_fichiers(argv[1],argv[2]);
            out=2;
            break;
        case 4:
            if (strcmp(argv[1],"-b"))
                Erreur(" Arguments incorrects !\n[0mTapper [1mAsmEsr_H -h [0mpour de l'aide !!");
            else {
#ifdef DEBUG2
    puts("mode BASIC");
#endif
                mode = BASIC;
                ouvre_fichiers(argv[2],argv[3]);
                out=3;
                }
            break;
        default :
            Erreur(" Arguments incorrects !\n[0mTapper [1masm -h [0mpour de l'aide !!");
        }

    lectligne();

    if(strncasecmp(l,"ORG",3)) Erreur("Il manque l'@dr de départ (ORG) !");

    supSPC(4,l);
    a=valh(l);
    printf("[33;4mDepart en &%x[0m\n",a);
    fprintf(temp,"%d\n",a);

    while(strcmp(l,";fini")){

        lectligne();

        if(isLAB(*l)){
            if (!(clab<MAX_LAB)) {
                printf ("[33mlimite [4m%d[0;33m labels",MAX_LAB);
                Erreur ("Trops de labels definis");
            }
            lab[clab].type=(*l=='[')?GLOBAL:LOCAL;
            strcpy(lab[clab].nom,lectlab(l,lb));
            lab[clab++].adr=a;
        }

        supSPC(0,l);
        if (*l && *l!=';'){
            fprintf(temp,"%d,",numl);
            strcpy(ins,l);
            if((i=strcspn(ins," \t"))){
                ins[i]=0;
                supSPC(i,l);
            } else *l=0;
#ifdef DEBUG2
        printf("instr \"%s\" lue\n",ins);
#endif
        if((num=chinstr(ins))!=(unsigned int)-1){
            fprintf(temp,"%d",num);
            a+=inst[num].nbre_octets;
            (inst[num].fonct)();
            if(num!=chinstr("CASE"))
                fputc('\n',temp);
            } else Erreur("Instruction inconnue");
        }
    }

    puts("[33;4m Labels définis :[0m");
    for(i=0;i<clab;i++)
        switch(lab[i].type){
            case GLOBAL :
                printf("%s =>&%x\n",lab[i].nom,lab[i].adr);
                break;
#ifdef TLAB
            case LOCAL :
                printf("[3mlocal[0m %s =>&%x\n",lab[i].nom,lab[i].adr);
                break;
            case CHARGER :
                printf("[3mcharger[0m %s =>&%x\n",lab[i].nom,lab[i].adr);
                break;
#endif
        }
    fseek(temp,0,0);
    fclose(fcode);
    if ((fcode=fopen(argv[out],"w"))==0)
        Erreur("Erreur d'ouverture pour le fichier de sortie\n");

    fscanf(temp,"%d",&i);
    a=(Adr)i;
#ifdef DEBUG2
    printf("Passe 2 :&%x\n",i);
#endif
    pass=1;*l=0;
    while(!feof(temp)){
        fscanf(temp,"%d,%d",&numl,&num);
        if(feof(temp)) break;
#ifdef DEBUG
        if(num>sizeof(inst)/sizeof(Inst))
            Erreur("main()!! Erreur interne au compilateur !! :-(");
#endif
        (inst[num].fonct)();
    }

    numl=0;
    sprintf(lb,"1er @dr libre : &%x",a);
    Attention(lb);

    if (mode==LM)
        while(*l) sort(0);
    else if(*l) fputs(ucase(strcat(l,"\n")),fcode);

/* Sauvegarde des labels */
    for(i=0;i<clab;i++)
        if (lab[i].type==GLOBAL)
            fprintf(flab,"&%x [%s]\n",lab[i].adr,lab[i].nom);

    ferme_fichiers();
    exit(0);
}
