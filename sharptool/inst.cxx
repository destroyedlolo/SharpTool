/*
 *	inst.cxx
 *	(c) L.Faillie	2002
 *
 *	CPU instruction
 *
 *	------------- V 2.0 -----------
 *	13/08/2002	Creation
 */

#include "dump.hxx"
#include "inst.hxx"
#include "emul.hxx"

#include <cstdlib>

const struct instruction instructions[]={
	{ "LII", dump_argbyte, emul_lii},	// 00
	{ "LIJ", dump_argbyte, emul_lij},	// 01
	{ "LIA", dump_argbyte, emul_lia},	// 02
	{ "LIB", dump_argbyte, emul_lib},	// 03
	{ "IX",	NULL, emul_ix},		// 04
	{ "DX", NULL, emul_dx},		// 05
	{ "IY", NULL, emul_iy},		// 06
	{ "DY", NULL, emul_dy},		// 07
	{ "MVW", NULL, emul_mvw},	// 08
	{ "EXW", NULL, emul_exw},	// 09
	{ "MVB", NULL, emul_mvb},	// 0A
	{ "EXB", NULL, emul_exb},	// 0B
	{ "ADN", NULL, emul_adn},	// 0C
	{ "SBN", NULL, emul_sbn},	// 0D
	{ "ADW", NULL, emul_adw},	// 0E
	{ "SBW", NULL, emul_sbw},	// 0F

	{ "LIDP", dump_argadr, emul_lidp},	// 10
	{ "LIDL", dump_argbyte, emul_libl},	// 11
	{ "LIP", dump_argreg, emul_lip},	// 12
	{ "LIQ", dump_argreg, emul_liq},	// 13
	{ "ADB", NULL, emul_adb },	// 14
	{ "SBB", NULL, emul_sbb },	// 15
	{ NULL, NULL, emul_panic },		// 16
	{ NULL, NULL, emul_panic },		// 17
	{ "MVWD",	NULL, emul_mvwd },	// 18
	{ "EXWD",	NULL, emul_exwd },	// 19
	{ "MVBD",	NULL, emul_mvbd },	// 1A
	{ "EXBD",	NULL, emul_exbd },	// 1B
	{ "SRW",	NULL, emul_srw },	// 1C
	{ "SLW",	NULL, emul_slw },	// 1D
	{ "FILM",	NULL, emul_film },	// 1E
	{ "FILD",	NULL, emul_fild },	// 1F

	{ "LDP",	NULL, emul_ldp },	// 20
	{ "LDQ",	NULL, emul_ldq },	// 21
	{ "LDR",	NULL, emul_ldr },	// 22
	{ "CLA",	NULL, emul_cla },	// 23
	{ "IXL",	NULL, emul_ixl },	// 24
	{ "DXL",	NULL, emul_dxl },	// 25
	{ "IYS",	NULL, emul_iys },	// 26
	{ "DYS",	NULL, emul_dys },	// 27
	{ "JRNZP",	dump_argpadr, emul_jrnzp },	// 28
	{ "JRNZM",	dump_argmadr, emul_jrnzm },	// 29
	{ "JRNCP",	dump_argpadr, emul_jrncp },	// 2A
	{ "JRNCM",	dump_argmadr, emul_jrncm },	// 2B
	{ "JRP",	dump_argpadr, emul_jrp },	// 2C
	{ "JRM",	dump_argmadr, emul_jrm },	// 2D
	{ NULL,		NULL, emul_panic },		// 2E
	{ "LOOP",	dump_argmadr, emul_loop },	// 2F

	{ "STP",	NULL, emul_stp },	// 30
	{ "STQ",	NULL, emul_stq },	// 31
	{ "STR",	NULL, emul_str },	// 32
	{ "NOP",	NULL, emul_nop },	// 33
	{ "PUSH",	NULL, emul_push },	// 34
	{ "DATA",	NULL, emul_data },	// 35
	{ NULL,		NULL, emul_panic },	// 36
	{ "RTN",	NULL, emul_rtn },	// 37
	{ "JRZP",	dump_argpadr, emul_jrzp },	// 38
	{ "JRZM",	dump_argmadr, emul_jrzm },	// 39
	{ "JRCP",	dump_argpadr, emul_jrcp },	// 3A
	{ "JRCM",	dump_argmadr, emul_jrcm },	// 3B
	{ NULL,		NULL, emul_panic },	// 3C
	{ NULL,		NULL, emul_panic },	// 3D
	{ NULL,		NULL, emul_panic },	// 3E
	{ "illegal",	NULL, emul_illegal },	// 3F

	{ "INCI",	NULL, emul_inci },	// 40
	{ "DECI",	NULL, emul_deci },	// 41
	{ "INCA",	NULL, emul_inca },	// 42
	{ "DECA",	NULL, emul_deca },	// 43
	{ "ADM",	NULL, emul_adm },	// 44
	{ "SBM",	NULL, emul_sbm },	// 45
	{ "ANMA",	NULL, emul_anma },	// 46
	{ "ORMA",	NULL, emul_orma },	// 47
	{ "INCK",	NULL, emul_inck },	// 48
	{ "DECK",	NULL, emul_deck },	// 49
	{ "INCM",	NULL, emul_incm },	// 4A
	{ "DECM",	NULL, emul_decm },	// 4B
	{ "INA",	NULL, emul_ina },	// 4C
	{ "NOPW",	NULL, emul_nopw },	// 4D
	{ "WAIT",	dump_argbyte, emul_wait },	// 4E
	{ "CUP",	NULL, emul_cup },	// 4F

	{ "INCP",	NULL, emul_incp },	// 50
	{ "DECP",	NULL, emul_decp },	// 51
	{ "STD",	NULL, emul_std },	// 52
	{ "MVDM",	NULL, emul_mvdm },	// 53
	{ "READM",	NULL, emul_readm },	// 54
	{ "MVMD",	NULL, emul_mvmd },	// 55
	{ "READ",	NULL, emul_read },	// 56
	{ "LDD",	NULL, emul_ldd },	// 57
	{ "SWP",	NULL, emul_swp },	// 58
	{ "LDM",	NULL, emul_ldm },	// 59
	{ "SL",		NULL, emul_sl },	// 5A
	{ "POP",	NULL, emul_pop },	// 5B
	{ NULL,		NULL, emul_panic },	// 5C
	{ "OUTA",	NULL, emul_outa },	// 5D
	{ NULL,		NULL, emul_panic },	// 5E
	{ "OUTF",	NULL, emul_outf },	// 5F

	{ "ANIM", dump_argbyte, emul_anim },// 60
	{ "ORIM", dump_argbyte, emul_orim },// 61
	{ "TSIM", dump_argbyte, emul_tsim },// 62
	{ "CPIM", dump_argbyte, emul_cpim },// 63
	{ "ANIA", dump_argbyte, emul_ania },// 64
	{ "ORIA", dump_argbyte, emul_oria },// 65
	{ "TSIA", dump_argbyte, emul_tsia },// 66
	{ "CPIA", dump_argbyte, emul_cpia },// 67
	{ NULL,		NULL, emul_panic },	// 68
	{ "CASE2", dump_case2, emul_case2 },	// 69
	{ NULL,		NULL, emul_panic },	// 6A
	{ "TEST",	dump_argbyte, emul_test },	// 6B
	{ NULL,		NULL, emul_panic },	// 6C
	{ NULL,		NULL, emul_panic },	// 6D
	{ NULL,		NULL, emul_panic },	// 6E
	{ "CDN",	NULL, emul_cdn },	// 6F

	{ "ADIM", 	dump_argbyte, emul_adim },	// 70
	{ "SBIM", 	dump_argbyte, emul_sbim },	// 71
	{ NULL,		NULL, emul_panic },		// 72
	{ NULL,		NULL, emul_panic },		// 73
	{ "ADIA", 	dump_argbyte, emul_adia },	// 74
	{ "SBIA", 	dump_argbyte, emul_sbia },	// 75
	{ NULL,		NULL, emul_panic },		// 76
	{ NULL,		NULL, emul_panic },		// 77
	{ "CALL", 	dump_argadr, emul_call },	// 78
	{ "JP", 	dump_argadr, emul_jp },	// 79
	{ "CASE1",	dump_case1, emul_case1},	// 7A
	{ NULL,		NULL, emul_panic },		// 7B
	{ "JPNZ", 	dump_argadr, emul_jpnz },	// 7C
	{ "JPNC", 	dump_argadr, emul_jpnc },	// 7D
	{ "JPZ", 	dump_argadr, emul_jpz },	// 7E
	{ "JPC", 	dump_argadr, emul_jpc },	// 7F

	{ "LP", 	dump_lp, emul_lp },	// 80
	{ "LP", 	dump_lp, emul_lp },	// ?1
	{ "LP", 	dump_lp, emul_lp },	// ?2
	{ "LP", 	dump_lp, emul_lp },	// ?3
	{ "LP", 	dump_lp, emul_lp },	// ?4
	{ "LP", 	dump_lp, emul_lp },	// ?5
	{ "LP", 	dump_lp, emul_lp },	// ?6
	{ "LP", 	dump_lp, emul_lp },	// ?7
	{ "LP", 	dump_lp, emul_lp },	// ?8
	{ "LP", 	dump_lp, emul_lp },	// ?9
	{ "LP", 	dump_lp, emul_lp },	// ?A
	{ "LP", 	dump_lp, emul_lp },	// ?B
	{ "LP", 	dump_lp, emul_lp },	// ?C
	{ "LP", 	dump_lp, emul_lp },	// ?D
	{ "LP", 	dump_lp, emul_lp },	// ?E
	{ "LP", 	dump_lp, emul_lp },	// ?F

	{ "LP", 	dump_lp, emul_lp },	// 90
	{ "LP", 	dump_lp, emul_lp },	// ?1
	{ "LP", 	dump_lp, emul_lp },	// ?2
	{ "LP", 	dump_lp, emul_lp },	// ?3
	{ "LP", 	dump_lp, emul_lp },	// ?4
	{ "LP", 	dump_lp, emul_lp },	// ?5
	{ "LP", 	dump_lp, emul_lp },	// ?6
	{ "LP", 	dump_lp, emul_lp },	// ?7
	{ "LP", 	dump_lp, emul_lp },	// ?8
	{ "LP", 	dump_lp, emul_lp },	// ?9
	{ "LP", 	dump_lp, emul_lp },	// ?A
	{ "LP", 	dump_lp, emul_lp },	// ?B
	{ "LP", 	dump_lp, emul_lp },	// ?C
	{ "LP", 	dump_lp, emul_lp },	// ?D
	{ "LP", 	dump_lp, emul_lp },	// ?E
	{ "LP", 	dump_lp, emul_lp },	// ?F

	{ "LP", 	dump_lp, emul_lp },	// A0
	{ "LP", 	dump_lp, emul_lp },	// ?1
	{ "LP", 	dump_lp, emul_lp },	// ?2
	{ "LP", 	dump_lp, emul_lp },	// ?3
	{ "LP", 	dump_lp, emul_lp },	// ?4
	{ "LP", 	dump_lp, emul_lp },	// ?5
	{ "LP", 	dump_lp, emul_lp },	// ?6
	{ "LP", 	dump_lp, emul_lp },	// ?7
	{ "LP", 	dump_lp, emul_lp },	// ?8
	{ "LP", 	dump_lp, emul_lp },	// ?9
	{ "LP", 	dump_lp, emul_lp },	// ?A
	{ "LP", 	dump_lp, emul_lp },	// ?B
	{ "LP", 	dump_lp, emul_lp },	// ?C
	{ "LP", 	dump_lp, emul_lp },	// ?D
	{ "LP", 	dump_lp, emul_lp },	// ?E
	{ "LP", 	dump_lp, emul_lp },	// ?F

	{ "LP", 	dump_lp, emul_lp },	// B0
	{ "LP", 	dump_lp, emul_lp },	// ?1
	{ "LP", 	dump_lp, emul_lp },	// ?2
	{ "LP", 	dump_lp, emul_lp },	// ?3
	{ "LP", 	dump_lp, emul_lp },	// ?4
	{ "LP", 	dump_lp, emul_lp },	// ?5
	{ "LP", 	dump_lp, emul_lp },	// ?6
	{ "LP", 	dump_lp, emul_lp },	// ?7
	{ "LP", 	dump_lp, emul_lp },	// ?8
	{ "LP", 	dump_lp, emul_lp },	// ?9
	{ "LP", 	dump_lp, emul_lp },	// ?A
	{ "LP", 	dump_lp, emul_lp },	// ?B
	{ "LP", 	dump_lp, emul_lp },	// ?C
	{ "LP", 	dump_lp, emul_lp },	// ?D
	{ "LP", 	dump_lp, emul_lp },	// ?E
	{ "LP", 	dump_lp, emul_lp },	// ?F

	{ "INCJ",	NULL, emul_incj },	// C0
	{ "DECJ",	NULL, emul_decj },	// C1
	{ "INCB",	NULL, emul_incb },	// C2
	{ "DECB",	NULL, emul_decb },	// C3
	{ "ADCM",	NULL, emul_adcm },	// C4
	{ "SBCM",	NULL, emul_sbcm },	// C5
	{ "TSMA",	NULL, emul_tsma },	// C6
	{ "CPMA",	NULL, emul_cpma },	// C7
	{ "INCL",	NULL, emul_incl },	// C8
	{ "DECL",	NULL, emul_decl },	// C9
	{ "INCN",	NULL, emul_incn },	// CA
	{ "DECN",	NULL, emul_decn },	// CB
	{ "INB",	NULL, emul_inb },	// CC
	{ NULL,		NULL, emul_panic },	// CD
	{ "NOPT",	NULL, emul_nopt },	// CE
	{ NULL,		NULL, emul_panic },	// CF

	{ "SC",	NULL, emul_sc },		// D0
	{ "RC",	NULL, emul_rc },		// D1
	{ "SR",	NULL, emul_sr },		// D2
	{ "WRIT",	NULL, emul_write },	// D3
	{ "ANID", dump_argbyte, emul_anid },// D4
	{ "ORID", dump_argbyte, emul_orid },// D5
	{ "TSID", dump_argbyte, emul_tsid },// D6
	{ "CPID", dump_argbyte, emul_cpid },// D7
	{ "LEAVE",	NULL, emul_leave },	// D8
	{ NULL,		NULL, emul_panic },	// D9
	{ "EXAB",	NULL, emul_exab },	// DA
	{ "EXAM",	NULL, emul_exam },	// DB
	{ NULL,		NULL, emul_panic },	// DC
	{ "OUTB",	NULL, emul_outb },	// DD
	{ NULL,		NULL, emul_panic },	// DE
	{ "OUTC",	NULL, emul_outc },	// DF

	{ "CAL", 	dump_cal, emul_cal },	// E0
	{ "CAL", 	dump_cal, emul_cal },	// ?1
	{ "CAL", 	dump_cal, emul_cal },	// ?2
	{ "CAL", 	dump_cal, emul_cal },	// ?3
	{ "CAL", 	dump_cal, emul_cal },	// ?4
	{ "CAL", 	dump_cal, emul_cal },	// ?5
	{ "CAL", 	dump_cal, emul_cal },	// ?6
	{ "CAL", 	dump_cal, emul_cal },	// ?7
	{ "CAL", 	dump_cal, emul_cal },	// ?8
	{ "CAL", 	dump_cal, emul_cal },	// ?9
	{ "CAL", 	dump_cal, emul_cal },	// ?A
	{ "CAL", 	dump_cal, emul_cal },	// ?B
	{ "CAL", 	dump_cal, emul_cal },	// ?C
	{ "CAL", 	dump_cal, emul_cal },	// ?D
	{ "CAL", 	dump_cal, emul_cal },	// ?E
	{ "CAL", 	dump_cal, emul_cal },	// ?F

	{ "CAL", 	dump_cal, emul_cal },	// F0
	{ "CAL", 	dump_cal, emul_cal },	// ?1
	{ "CAL", 	dump_cal, emul_cal },	// ?2
	{ "CAL", 	dump_cal, emul_cal },	// ?3
	{ "CAL", 	dump_cal, emul_cal },	// ?4
	{ "CAL", 	dump_cal, emul_cal },	// ?5
	{ "CAL", 	dump_cal, emul_cal },	// ?6
	{ "CAL", 	dump_cal, emul_cal },	// ?7
	{ "CAL", 	dump_cal, emul_cal },	// ?8
	{ "CAL", 	dump_cal, emul_cal },	// ?9
	{ "CAL", 	dump_cal, emul_cal },	// ?A
	{ "CAL", 	dump_cal, emul_cal },	// ?B
	{ "CAL", 	dump_cal, emul_cal },	// ?C
	{ "CAL", 	dump_cal, emul_cal },	// ?D
	{ "CAL", 	dump_cal, emul_cal },	// ?E
	{ "CAL", 	dump_cal, emul_cal }	// ?F
};
