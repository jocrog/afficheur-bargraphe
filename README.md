    // Module ecran LCD 2 lignes x 16 caracteres
    // Clavier 5 touches
    // DFRobot : LCD Keypad Shield

    // affichage des donnees au format csv provenant de la liaison serie i_acquisition

    /*-- presentation
     * 2018/07/15	17:20:00,	13769,		0,			236,		193,		100009,			100009,			108173
     * value[0]		value[1]	value[2]	value[3]	value[4]	value[5]	value[6]		value[7]		value[8]
     * "date		hour",		"VBat",		"Inst",		"IBat",		"Ah",		"cumuliahg",	"cumuliah",		"cumuliaht"}
     * affiche=0	namlig0		namlig2					namlig1					namlig3
     * affiche=1	namlig0														namlig1
     * affiche=2	namlig0											namlig1
     *
     * 		afficheur			       affiche0 texte		   affiche1 graph		   affiche2 graph
     *		|----------------|	|----------------|	|----------------|	|----------------|
     *		|namlig0 |namlig2|	|00:00:00| 12421V|	|00:00:00      | |	|00:00:00    |||||
     *		|namlig1 |namlig3|	|  -795mA| 92597%|	| 92597 % Ah |||||	|  -731mA cj |||||
    * 		|----------------|	|----------------|	|----------------|	|----------------|
