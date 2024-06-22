	/*
	- afficheur des mesures de courants et capacité du camping car
	*/

	/*			history
		*	programme d affichage sur lcd 1602 des mesures et bargraphe
		* 	0.0.1 - ecriture initiale base afficheur
		* 	0.1.0 - affichage date, tension, courant, capacite
		* 	0.1.1 - correction init value avant reception
		* 	0.2.0 - ajout barre maj minute et arrivee trame
		* 	0.2.0 - ajout barre maj minute et arrivee trame
		* 	0.3.0 - ajout affichage unités
		* 	0.3.5 - validation ajout bargraph
		* 	0.4.0 - ajout bargraph ah et cumul journaliers
		* 	0.4.1 - selection extinction affichage
		* 	0.4.3 - decalage du zero du bargraph ah
		* 	0.4.4 - ecretage des valeurs bargraph
		* 	0.4.5 - correction 2018 ligne 546
d1		* 	0.4.6 - deplacement procedure serie (16062 bytes )
		* 	0.4.7 - suppression procedures et variables inutiles (16038 bytes ) 
		* 	0.4.8 - correction hour procedure set_Time (16034 bytes )
d		* 	0.4.9 - modif struct date et appel procedure set_Time ( 16136 bytes )
		* 	0.4.10 - correction affichage barre seconde_0 ( 16150 bytes )
		* 	0.4.11 - suppression timer ( (15682 bytes))
		* 	0.4.12 - modif lcd.clear 
		* 	0.4.13 - retour timer (16134 bytes)
		* 	0.4.14 - modif lcd.clear (16176 bytes)
		* 	0.4.15 - retour lcd.clear (16158 bytes)
		* 	0.4.16 - suppression des Serial.print (15338 bytes)
		* 	0.4.17 - modif valeur -> variable locale; ah cj noinit
		* 	0.4.18 - suppression variable name; init ah cj (15328 bytes)
		* 	0.4.19 - init de toutes les variables
		* 	0.4.20 - correction init pointeurs de variables d affichage
		* 	0.5.0  - verification horloge rtc
		* 	0.6.0  - gestion backlight
		* 	0.6.1  - ajout  controle backlight
		* 	0.6.2  - modif  controle backlight digital
		* 	0.6.3  - correction  controle backlight
		* 	0.6.4  - ajout  num_affich 3 = backlight off 
		* 	0.6.5  - correction  num_affich 0-1-2 = backlight on 
		* 	0.6.6  - ajout courant negatif => backlight off (15908 bytes)
		* 	0.7.0  - remplacement de delay() par timer.after()
		* 	0.7.1  - correction affiche_lcd plantage bargraph ah
		* 	0.7.2  - correction longueur buffer +1 null terminated ( 15550 bytes )(reboot present)
		* 	0.7.3  - correction memcpy maintenant + memcpy reception (15522 bytes)
		* 	0.7.4  - modif serialEvent
		* 	0.7.5  - backlight on/off, 	nam_ligne[1] variable
		* 	0.7.6  - modif affiche
		* 	0.7.7  - nettoyage code ( depannage soudure alim )
		* 	0.7.8  - backlight off batterie faible ou 22H ou manu
		* 	0.7.9  - correction fonctionnement autolight/manu
		* 	0.7.10  - deplacement fonction autolight/manu
		* 	0.7.11  - calcul freeram avant modif bargraph
		* 	0.7.12  - retrait freeram , resultat : 654 octets de libre
M		* 	0.7.13  - deplacement affectation variable affichage
		* 	0.8.0	- prepa affich graph reception stat
		* 	0.8.0.1	- prepa affich modif struct
		* 	0.8.0.2	- prepa affich modif struct
		* 	0.8.0.3	- reecriture affiche_lcd
		* 	0.8.0.4 - reecriture affiche_lcd
		* 	0.8.0.5 - renommage variables et structures
		* 	0.8.0.6 - changement structures type variable 
		* 	0.8.0.7 - description d affichage par variable 
		* 	0.8.0.8 - utilisation des variable d affichage en memoire PROGMEM
		* 	0.8.0.9 - debogage affichage
		* 	0.8.0.9 - avant suppression progmem
		* 	0.8.0.10 - modif struct Eglcd ajout separateur correction bug ecran0 ok
		* 	0.8.0.11 - mise en eeprom valeurs eglcd de presentation des ecrans
		* 	0.8.0.12 - modif struct eglcd deplacement position de la donnée
		* 	0.8.1.0 - validation fonctionnemnt params ecrans en eeprom
		* 	0.8.1.1 - modification params ecrans en eeprom via liaison serie
		* 	0.8.2.0 - !dev2:( suppression des const ecrans mise en eeprom deplacée en setup
		* 	0.8.2.1 - correction unité mAh
		* 	0.8.2.2 - modif affich_lcd  pointeur ah & cj
		* 	0.8.2.3 - presentation et debug
d4		* 	0.8.3.0 - utilisation de progmem pour le menu
*/

// programme :
const char title[] = "aff_graphe";
const char version[] = "0.8.2.4";

//#include <string.h>

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Timer.h>
#include <RTClib.h>
#include <Time.h>				//http://www.arduino.cc/playground/Code/Time  
#include <Streaming.h>			//http://arduiniana.org/libraries/streaming/
#include <DS1307RTC.h>			//http://github.com/JChristensen/DS3232RTC
#include <LCDMBitmap.h>			// https://bitbucket.org/teckel12/arduino-lcd-bitmap/wiki/Home
#include <EEPROM.h>
#include "EEPROMAnything.h"

using namespace std;

/*	recapitulatif des i/o arduino
	#define rx		D0			// tty
	#define tx		D1			// tty
	#define 		D2			// 
	#define 		D3			// 
	#define 		D4			// lcd
	#define 		D5			// lcd
	#define 		D6			// lcd
	#define 		D7			// lcd
	#define 		D8			// lcd
	#define 		D9			// lcd
	#define 		D10			//	
	#define mosi	D11			// SPI SD	MOSI backlight_Pin
	#define miso	D12			// SPI SD	MISO
	#define sck 	D13			// SPI SD	SCK
	#define 		A0			// keypad
	#define 		A1			// 
	#define 		A2			// 
	#define 		A3			//
	#define sda	A4			// I2C rtc
	#define scl	A5			// I2C rtc
*/

/*-- presentation
 * 2018/07/15	17:20:00,	13769,		0,			236,		193,		100009,			100009,			108173
 * value[0]		value[1]	value[2]	value[3]	value[4]	value[5]	value[6]		value[7]		value[8]
 * "date		hour",		"VBat",		"Inst",		"IBat",		"Ah",		"cumuliahg",	"cumuliah",		"cumuliaht"}
 * affiche=0	namlig0		namlig2					namlig1					namlig3
 * affiche=1	namlig0														namlig1
 * affiche=2	namlig0											namlig1
 *
 * 		afficheur			affiche0 texte		affiche1 graph		affiche2 graph
 *		|----------------|	|----------------|	|----------------|	|----------------|
 *		|namlig0 |namlig2|	|00:00:00| 12421V|	|00:00:00      | |	|00:00:00    |||||
 *		|namlig1 |namlig3|	|  -795mA| 92597%|	| 92597 % Ah |||||	|  -731mA cj |||||
 * 		|----------------|	|----------------|	|----------------|	|----------------|
 *
 */

#define INPUT_SIZE 63						// taille de la variable de message serie
#define BAUD_RATE 19200 
#define btn_Pin  0 			// entrée ana du keypad lcd
#define backlight_Pin  11 	// sortie commande retro eclairage lcd

// Definit des valeurs utilisées par les 5 boutons
#define btnRIGHT 0 
#define btnUP 1 
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

#define FS(x) (__FlashStringHelper*)(x)
const char menu0[]  PROGMEM  = { "help ou ? -> ce menu:\n"
								"d -> debug - reglage:\n"
								"gs,0 -> getscreen screen0 val0 on eeprom:\n"
								"gv,0,0 -> getvalue screen0 val0 on eeprom:\n"
								"ms,0,3,15,5,Ah -> modif screen0,namlig3,cursor,value,unité on eeprom:\n"
								"r -> reset avr:\n"
								"v -> version:"
								};

struct Heure{
	char hour[2];
	char sep0;
	char minute[2];
	char sep1;
	char second[2];
	char end;
} ;

struct Date{
	char year[4];
	char sep0;
	char month[2];
	char sep1;
	char day[2];
	char sep2;
	char hour[2];
	char sep3;
	char minute[2];
	char sep4;
	char second[2];
	char end;
} ;

union Lg2byte{
	byte bval[4] ;
	long lval ; 
} ;

struct T_value{
	uint8_t inamlig;		// index dans la table value
	char unite[4];			// unité de mesure sur l'afficheur
} ;

struct G_value{
	byte *valgraf;	// address de la table graphe
	uint8_t ptitre;			// position du titre sur l'afficheur
	char gtitre[3];			// libellé du graphe
} ;

struct Eglcd{
	char etype;				// etype t ou g affiché ligne 0
	char libelle[17];		// affiché ligne 1 au changement d'ecran
	uint8_t Sep[2];			// separateur ligne 0 & 1
	uint8_t punite[4];			// position de mesure sur l'afficheur
	T_value Tval[4];
	G_value Gval;
} ;

//	definition des constantes

// ces valeurs sont sauvees en EEPROM
const byte EEPROM_ID = 0x93 ;   // used to identify if valid data in EEPROM
//constants used to identify EEPROM addresses
const uint8_t ID_ADDR = 0 ;		// the EEPROM address used to store the ID
const uint8_t ecran_count_addr = 1 ;	// adresse en eeprom du nombre d'ecrans
const uint8_t ecran_addr = 2 ;	// adresse en eeprom de la structure capteur

// Sélection des pins utilisées par le keypad LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7) ;
// Assign the LCD bitmap to "&lcd" set in the previous line and set display location to (12,0)
LCDBitmap bitmap(&lcd, 12, 0);  

const char barre[] = {'|', ':', '.', ' '};

const int timbertimout = 1000 ;
const int aftertimout = 500;
const uint8_t num_affich_count = 3;
Timer timber ;
DS1307RTC rtc;

// definition des Variables :

bool debug = false ;   					// mode debug
int second_0 = -1;					// detection second 00
char** nam_ligne[4] = {}; 			// liste des pointeurs sur les valeurs a afficher stockees dans value
bool stringComplete = false;		// whether the string is complete // "                "
bool affiche = true;				// blocage timer affichage pendant manipulation des boutons
bool decodage = false;				// decodage en cours
int value_count = 0;				// nombre de variables
char** value = {};					// pointeur sur liste indexée des pointeurs de valeurs
char reception[INPUT_SIZE +1] = { "Date,Heure,_,_,attente,_,valeurs,_,_"};	// espace de stockage pour trame complete
char* ptr = reception ;				// pointeur sur reception[]
char* fifo = ptr ; 					// pointeur sur la chaine de reception
bool msg_valid = false ;
char buffer[9] = {};						// buffer Heure
char* btr = buffer ;
byte index[2] = {0, 0};
uint8_t num_affich = 0 ;
byte cj[20] __attribute__ ( (section (".noinit") ) );
byte ah[20] __attribute__ ( (section (".noinit") ) );
byte *pcj = cj;
byte *pah = ah;
bool nortc = true;
bool backlight = true;
bool autolight = true;
Eglcd val_affich ;

//			****	sous programmes		****

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
} // end of freeRam 

// Write any data structure or variable to EEPROM
int EEPROMAnythingWrite(int pos, char *zeichen, int lenge){
	for (int i = 0; i < lenge; i++){
		EEPROM.write(pos + i, *zeichen);
		zeichen++;
	}
	return pos + lenge;
}
 
// Read any data structure or variable from EEPROM
int EEPROMAnythingRead(int pos, char *zeichen, int lenge){
	for (int i = 0; i < lenge; i++){
		*zeichen = EEPROM.read(pos + i);
		zeichen++;
	}
	return pos + lenge;
}


/*void maj_struct(){
  //exemple de mise a jour de structure
	for (int i=0; i < 16;) {
    if (Serial.available()) {
      test1.pt[i++] = Serial.read();
    }
}*/

int set_Time(){		//set rtc time
	/*check for input to set the RTC, minimum length is 12, i.e. yy,m,d,h,m,s
	note that the tmElements_t Year member is an offset from 1970,
	but the RTC wants the last two digits of the calendar year.
	use the convenience macros from Time.h to do the conversions.*/
	Date Aujourdhui = {};
	time_t t = 0;
	tmElements_t tm;
	memcpy ( Aujourdhui.year, reception, 20 );
	Aujourdhui.end = '\0';
	Aujourdhui.sep4 = '\0';
	Aujourdhui.sep3 = '\0';
	Aujourdhui.sep2 = '\0';
	Aujourdhui.sep1 = '\0';
	Aujourdhui.sep0 = '\0';
	tm.Hour = atoi(Aujourdhui.hour);
	tm.Minute = atoi(Aujourdhui.minute);
	tm.Second = atoi(Aujourdhui.second);
	tm.Day = atoi(Aujourdhui.day);
	tm.Month = atoi(Aujourdhui.month);
	tm.Year = y2kYearToTm(atoi(Aujourdhui.year));
	t = makeTime(tm);
	if (RTC.chipPresent()) {
		rtc.read(tm);
		time_t u = makeTime(tm);
		if (u != t){
			//use the time_t value to ensure correct weekday is set
			rtc.set(t);
			setTime(t);
		}
		nortc = false;
	}
	else{
		nortc = true;
	}
	if( strcmp(Aujourdhui.minute, "00" ) == 0){
		return atoi(Aujourdhui.hour);
	}
	else{
		return -1;
	}

} // end of set_Time

int get_time0(){

/**set_rtcTime();**/
	Heure Maintenant = {};
	memcpy ( Maintenant.hour, value[1], 8 );
	Maintenant.end = '\0';
	Maintenant.sep0 = '\0';
	Maintenant.sep1 = '\0';
	if( strcmp(Maintenant.minute, "00" ) == 0){
		return atoi(Maintenant.hour);
	}
	else{
		return -1;
	}
}// end of get_time0

void do_at_time (int hour){
	Lg2byte Valeur = {} ; 
	if(hour == 6){		//enregistrement de la valeur cd capacite a 6h du matin
		// rotation des Valeurs de cj perte Valeur de gauche
		for (byte x=1; x<=19; x++) {
			cj[x-1] = cj[x];
		}
		Valeur.lval = atol(value[6]) * 16 /100000;
		if (Valeur.lval > 15){
			cj[19] = 15;
		}
		if (Valeur.lval < 0){
			cj[19] = 0;
		}
		else{
			cj[19] = Valeur.bval[0];
		}
		for (byte x=1; x<=19; x++) {
			ah[x-1] = ah[x];
		}
		ah[19] = 1;
	}
	if(hour < 2 or hour >= 6){		//enregistrement de la valeur cd capacite a 6h du matin
		// rotation des valeurs de cj perte valeur de gauche
		for (byte x=1; x<=19; x++) {
			ah[x-1] = ah[x];
		}
		Valeur.lval = ((atol(value[4])*1.5)/1000)+8;
		if (Valeur.lval > 16){
			ah[19] = 16;
		}
		if (Valeur.lval < 0){
			ah[19] = 0;
		}
		else{
			ah[19] = Valeur.bval[0];
		}
	}
	if ( autolight){
		if(atoi(value[4]) > 0){		// courant paneau solaire > 0 affichage allumé
			backlight = HIGH;
		}
		else {
			if (hour >= 23 or hour < 8){		// extinction après 23h jusqu'a 8h
				backlight = LOW;
			}
		}
	}
}	// end of set_stat

void doafter (){
	affiche = true ;
}// end of do_after

void affiche_lcd (){

	if (affiche){	// pas de reception serie ni d'appui sur un bouton
		lcd.clear();
		bitmap.clear();
		// selection affiche text ou graph
		if (val_affich.etype == 'g'){
			bitmap.clear_text();
			lcd.setCursor(val_affich.Gval.ptitre,1) ;
			lcd.print(val_affich.Gval.gtitre) ;
			bitmap.barGraph(20, val_affich.Gval.valgraf, ON, UPDATE);
		}
		else if (val_affich.etype == 't'){
			for(int i=0;i<=1;i++){
				if (val_affich.punite[i] <= 9){
					// si position unité namlig0 ou namlig1 >> 8 => namlig2 ou namlig3 n'existe pas
					// sinon affichage de name_ligne[2 & 3] => i+2
					nam_ligne[i+2] = &value[val_affich.Tval[i+2].inamlig] ;
					// affichage unité
					lcd.setCursor(val_affich.punite[i+2],i) ;
					lcd.print(val_affich.Tval[i+2].unite) ;
					// affichage valeur
					lcd.setCursor(val_affich.punite[i+2] - strlen(*nam_ligne[i+2]),i) ;
					lcd.print(*nam_ligne[i + 2]) ;
				}
			}
		}
		// dans tous les cas affichage de name_ligne[0 & 1]
		for(int i=0;i<=1;i++){
			if ( i == 0 and !nortc){					// pas de rtc affichage Heure trame
				nam_ligne[i] = &btr;	// selection Heure rtc
			}
			else{
				nam_ligne[i] = &value[val_affich.Tval[i].inamlig] ;
			}
			// affichage unité
			lcd.setCursor(val_affich.punite[i],i) ;
			lcd.print(val_affich.Tval[i].unite) ;
			// affichage valeur
			lcd.setCursor(val_affich.punite[i] - strlen(*nam_ligne[i]),i) ;
			lcd.print(*nam_ligne[i]) ;
			// affichage du separateur
			lcd.setCursor(val_affich.Sep[i],i) ;
			lcd.print(barre[index[i]]);
		}
		if(debug){
			Serial << F("freeRam affiche:") << freeRam() << endl;
		}
	}
}	// end of affiche_lcd

void key_pressed(int lcd_key){
	affiche = false ;
	static byte sel_ligne   = 0 ;
	static byte indx_ligne[2] = {0, 2} ;
	switch (lcd_key) {              // Selon le bouton choisi
		case btnRIGHT:				// incrementation de selection de variable
			{
				indx_ligne[sel_ligne] ++ ;
				if (indx_ligne[sel_ligne] > value_count -1){
					indx_ligne[sel_ligne] = 0 ;
				}
				lcd.setCursor(0,sel_ligne) ;
				lcd.print("                ") ;
				lcd.setCursor(0,sel_ligne) ;
				lcd.print(value[indx_ligne[sel_ligne]]) ;
				int afterEvent = timber.after(aftertimout, doafter);
				break ;
			}
		case btnLEFT:				// decrementation de selection de variable
			{
				indx_ligne[sel_ligne] -- ;
				if (indx_ligne[sel_ligne] < 0){
					indx_ligne[sel_ligne] = value_count -1 ;
				}
				lcd.setCursor(0,sel_ligne) ;
				lcd.print("                ") ;
				lcd.setCursor(0,sel_ligne) ;
				lcd.print(value[indx_ligne[sel_ligne]]) ;
				int afterEvent = timber.after(aftertimout, doafter);
				break ;
			}
		case btnUP:					// selection ligne lcd 0
			{
				sel_ligne = 0 ;
				backlight = true;
				autolight = !autolight;
				if(autolight){
					lcd.setCursor(0,0) ;
					lcd.print("autolight ON    ") ;
					lcd.setCursor(0,1) ;
					lcd.print("                ") ;
				}
				else{
					lcd.setCursor(0,0) ;
					lcd.print("autolight OFF   ") ;
					lcd.setCursor(0,1) ;
					lcd.print("                ") ;
				}
				int afterEvent = timber.after(aftertimout, doafter);
				break ;
			}
		case btnDOWN:					// selection ligne lcd 1
			{
				sel_ligne = 1 ;
				backlight = false;
				autolight = !autolight;
				if(!autolight){
					lcd.setCursor(0,0) ;
					lcd.print("autolight OFF   ") ;
					lcd.setCursor(0,1) ;
					lcd.print("                ") ;
				}
				else{
					lcd.setCursor(0,0) ;
					lcd.print("autolight ON    ") ;
					lcd.setCursor(0,1) ;
					lcd.print("                ") ;
				}
				int afterEvent = timber.after(aftertimout, doafter);
				break ;
			}
		case btnSELECT:					// mise en memoire des pointeurs selectionnes
			{
				num_affich += 1;
				if(num_affich > num_affich_count){
					num_affich = 0;
				}
				//Serial << F("num_affich: ") << num_affich << endl;
				if(num_affich == num_affich_count ){
					bitmap.clear();
					lcd.noDisplay();
					backlight = LOW;
					int afterEvent = timber.after(aftertimout, doafter);
				}
				else {
					int addr = ecran_addr + (sizeof(val_affich) * num_affich);
					int rc = EEPROM_readAnything(addr, val_affich);
					lcd.display();
					lcd.setCursor(0,0) ;
					if (val_affich.etype == 't'){
						lcd.print(F("Numerique       ") );
					}
					else{
						lcd.print("Graphique       ") ;
					}
					lcd.setCursor(0,1) ;
					lcd.print(val_affich.libelle) ;
					backlight = HIGH;
					int afterEvent = timber.after(aftertimout, doafter);
				}
			}
	}
}		// end of key_pressed

// Lecture des  boutons (tous regroupés sur le Pin Analog A0)
int read_LCD_buttons(){

	int adc_key_in  = 0 ;

	adc_key_in = analogRead(btn_Pin) ;      // Lit la valeur depuis le sensor sur A0
	// centré sur ces valeurs : 0, 144, 329, 504, 741
	// on ajoute environ 50 à ces valeurs et on vérifie si on est proche
	if (adc_key_in > 1000) return btnNONE ; // On définit ça en première option pour raison de rapidité car option la plus probable

	if (adc_key_in < 50)   return btnRIGHT ; 
	if (adc_key_in < 195)  return btnUP ;
	if (adc_key_in < 380)  return btnDOWN ;
	if (adc_key_in < 555)  return btnLEFT ;
	if (adc_key_in < 790)  return btnSELECT ;   
	return btnNONE ;  // Sinon, on n'a aucun bouton de sélectionné (par défaut)

}	// end of read_LCD_buttons

int dtmsplit(char *str, const char *delim, char ***array, int *length ) {

	int i=0;
	char *token;
	char **res = (char **) malloc(0 * sizeof(char *));

	/* get the first token */
	token = strtok(str, delim);
	while( token != NULL ) {
		res = (char **) realloc(res, (i + 1) * sizeof(char *));
		res[i] = token;
		i++;
		token = strtok(NULL, delim);
	}
	*array = res;
	*length = i ;
	return 1;

}	// end of dtmsplit

bool decode(){
	
	decodage = true ;
	bool msg_valid = false;
	free (value) ;
	int c = dtmsplit(ptr, "\r\t\n, ", &value, &value_count);
	msg_valid = true ;
	fifo = ptr ;

	#if defined DEBUG
		Serial << F("Debug { ") ;
		for(int i = 0; i <= value_count -1; i++){
			Serial << i << F(":") << value[i] << "; ";
		}
		Serial << F( "}" ) << endl;
	#endif

	decodage = false ;
	stringComplete = false;
	return msg_valid ;
	
}	// end of decode

int digitalClockDisplay(char *btr){

	tmElements_t tm;
	
	if (rtc.read(tm)) {
		sprintf(btr, "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
		nortc = false;
	}
	else{
		nortc = true;
	}
	// changement du caractere de separation Heure | tension sur l afficheur
	if (tm.Second == 0 and second_0 != 0){
		index[0] +=1;
		if(index[0] > 3){
			index[0] = 0;
		}
	}
	second_0 = tm.Second;
	return tm.Second;
	
}	// end of digitalClockDisplay

void select_input_serial(){
	//selectionner action en fonction de la trame en reception
	// action, address,  params
	// ms	, ecran, punité, variable, unité
	if (strlen(value[0]) < 8){
		if ( strcmp(value[0], "gs") == 0){		// modif screen: affich eeprom screen
			int indx = atoi(value[1]);
			if (indx <= 2){
				char tab[]={"\t|"};
				int addr = ecran_addr + (sizeof(val_affich) * indx);
				int rc = EEPROM_readAnything(addr, val_affich);
				Serial << F("ecran:\t|type:\t|separateur ligne 0:\t|separateur ligne 1:\t|")<< endl;
				Serial<<indx<<tab<<val_affich.etype<<tab<<val_affich.Sep[0]<<tab<<val_affich.Sep[1]<< endl;
			}
		}
		if ( strcmp(value[0], "gv") == 0){		// modif screen: affich eeprom screen
			int ecran = atoi(value[1]);
			if (ecran <= 2){
				char tab[]={"\t|"};
				int indxval = atoi(value[2]);
				int ligne = indxval % 2;
				int addr = ecran_addr + (sizeof(val_affich) * ecran);
				int rc = EEPROM_readAnything(addr, val_affich);
				//char temp[56]; // = {"ecran:\t|ligne:\t|leftcursor:\t|variable:\t|value:\t|unité:\t|"};
				//sprintf(temp, "ecran:\t|ligne:\t|leftcursor:\t|variable:\t|value:\t|unité:\t|");
				Serial << F("ecran:\t|ligne:\t|leftcursor:\t|variable:\t|value:\t|unité:\t|")<< endl;
				//Serial.println(F(temp));
				/*Serial<< F("ecran:\t|")<< F("ligne:\t|")<< F("leftcursor:\t|")<< F("variable:\t|")<< F("value:\t|")
					<< F("unité:\t|")<<endl;*/
				Serial<< ecran<<tab<<ligne<<tab<<val_affich.punite[indxval]<<'\t'<<tab<<indxval<<'\t'<<tab
				<<val_affich.Tval[indxval].inamlig<<tab<<val_affich.Tval[indxval].unite<<tab<<endl;

			}
		}
		if ( strcmp(value[0], "help") == 0 or strcmp(value[0], "?") == 0){
			Serial.println(FS(menu0));
			/*Serial << F("help ou ? -> ce menu:") << endl;
			Serial << F("d -> debug - reglage:") << endl;
			Serial << F("gs,0 -> getscreen screen0 val0 on eeprom:") << endl;
			Serial << F("gv,0,0 -> getvalue screen0 val0 on eeprom:") << endl;
			Serial << F("ms,0,3,15,5,Ah -> modif screen0,namlig3,cursor,value,unité on eeprom:") << endl;
			//Serial << F("syy,m,d,h,m,s -> mise a l heure:") << endl;
			Serial << F("r -> reset avr:") << endl;
			Serial << F("v -> version:") << endl;
			*/

		}
		if ( strcmp(value[0], "d") == 0){
				debug = !debug ;
				Serial << F("debug = ") <<  debug;
		}
		if ( strcmp(value[0], "r") == 0){
			resetFunc();
		}
		if ( strcmp(value[0], "v") == 0){
			Serial << title<<  F(" ") << version<< endl;
		}
		if(debug){
			Serial << F("freeRam select_input_serial: ") << freeRam() << endl;
		}
		affiche = true;
	}
	else{		//	longeur == 10 champ date trame de mesures
		;
	}
	
}	// end of select_input_serial

//			****	fin des sous programmes		****

void setup() {

	// initialize serial:
	Serial.begin(BAUD_RATE) ;
	pinMode(backlight_Pin , OUTPUT);
	digitalWrite(backlight_Pin, HIGH); 
	// Lance la librarie (16 caractères x 2 lignes)
	lcd.begin(16, 2) ;
	bitmap.begin();
	// This creates a big "X" using the LCDBitmap to show the inverse and move functions
	for (byte x=0; x<=4; x++) {
		bitmap.line(x, BITMAP_H-1, x+BITMAP_H-1, 0, ON, NO_UPDATE);  // Draw a line, but don't upDate the bitmap display
		bitmap.line(x, 0, x+BITMAP_H-1, BITMAP_H-1, ON, NO_UPDATE);  // Draw a line, but don't upDate the bitmap display
	}
	bitmap.update();  // Now that everything is written to the bitmap, show the changes
	lcd.setCursor(0,0) ; //En haut à gauche (numéroté depuis 0)
	lcd.print(title) ; // Afficher un petit message
	lcd.setCursor(0,1) ;   //Ligne du bas
	lcd.print(version) ;   //intro de 5 secondes-
	Serial << title<<  F(" ") << version<< endl;
	delay(500);
	bitmap.rectFill(0, 0, 19, 15, ON, UPDATE);
	delay(500);
	bitmap.rectFill(2, 2, 17, 13, OFF, UPDATE);
	delay(500);
	bitmap.rectFill(4, 4, 15, 11, ON, UPDATE);
	delay(500);
	bitmap.rect(6, 6, 13, 9, OFF, UPDATE);
	delay(500);
	stringComplete = true;
	lcd.clear();
	bitmap.clear();
	// configuration valeur initiales si elle sont à 0
	for (byte x=0; x<=19; x++) {
		if(ah[x] == 0 or ah[x] > 15){
			ah[x] = 1;
		}
	}
	for (byte x=0; x<=19; x++) {
		if(cj[x] == 0 or cj[x] > 15){
			cj[x] = 1;
		}
	}
	byte id = EEPROM.read(ID_ADDR);
	Serial << F("EEPROM_ID attendu = ") << EEPROM_ID << F(" EEPROM_ID lu = ") << id << endl;
	if(id == 255){
		id = EEPROM.read(0);
		Serial << F("EEPROM_ID attendu = ") << EEPROM_ID << F(" EEPROM_ID lu = ") << id << endl;
	}
	if(id == EEPROM_ID){
		Serial << F("EEPROM_ID correct") << endl;
		int addr = ecran_addr;
		// Read structure (struct) from EEPROM
		int rc = EEPROM_readAnything(addr, val_affich);
	}
	else {
		Serial << F("erreur lecture EEPROM_ID = ") << id << endl;
		EEPROM.write(ID_ADDR, EEPROM_ID);
		//const T_value t_date = {0, ""};		// date
		T_value t_rien = {16, ""};
		T_value t_heure = {1, ""};		// heure
		T_value t_Vbat = {2, "V"};		// tension batterie
		T_value t_Ibat = {4, "mA"};		// courant instantanné
		T_value t_Ah = {5, "mAh"};			// courant en Aheure
		T_value t_cumuliahg = {6, "%"};	//	cumul moyenne glissante
		G_value g_ah = {pah, 9, "Ah"};	//	graph journée courant
		G_value g_cj = {pcj, 9, "cj"};	//	graph journée cumul

		Eglcd Ecran0_default = {'t', "Heure V A charge", 8, 8, 8, 6, 15, 15, t_heure, t_Ibat, t_Vbat, t_cumuliahg, g_ah};	//	ecran numerique
		Eglcd Ecran1_default = {'g', "Courant 24 H    ", 8, 8, 8, 5, 15, 15, t_heure, t_Ah, t_rien, t_rien, g_ah};	//	ecran graph Ah journée
		Eglcd Ecran2_default = {'g', "Cumul 20 J      ", 8, 8, 8, 6, 15, 15, t_heure, t_Ibat, t_rien, t_rien, g_cj};	//	ecran graph cumul 20 jours

		int addr = ecran_addr;
		int rc = EEPROM_writeAnything(addr, Ecran0_default);
		Serial << F("copie EEPROM valeurs Ecran0_default ") << rc << " octets"<< endl;
		addr = addr + sizeof(Ecran0_default);
		rc = EEPROM_writeAnything(addr, Ecran1_default);
		Serial << F("copie EEPROM valeurs Ecran1_default ") << rc << " octets"<< endl;
		addr = addr + sizeof(Ecran1_default);
		rc = EEPROM_writeAnything(addr, Ecran2_default);
		Serial << F("copie EEPROM valeurs Ecran2_default ") << rc << " octets"<< endl;
		addr = addr + sizeof(Ecran2_default);
		memcpy ( &val_affich, &Ecran0_default, sizeof(Ecran0_default) );
		Serial << F("freeRam config ecran:") << freeRam() << endl;
	}
	
	timber.every(timbertimout, affiche_lcd) ; //raffraichissement du lcd
	Serial << F("freeRam après config:") << freeRam() << endl;

}	// end of setup

 void loop() {

	byte lcd_key    = 0 ;
	// affichage de 20 bars/24Heures -> Heures non affichées entre 2h et 6h
	// enregistrement capacite mini a 6h (UT +2; equinoxe 8h-20h; solstice 6h-22h)
	int hour = -1;
	if (stringComplete) {
		if (!decodage){  //	test pour ne pas boucler dans le decodage en cours
			msg_valid = decode();
			select_input_serial();
			hour = set_Time();
			if(hour != -1){
				do_at_time(hour);
			}
			affiche = true;
			if(debug){
				Serial << F("freeRam stringComplete:") << freeRam() << endl;
			}
		}
	}
	else  if (affiche){		// s'il n'y a pas d'attente after event
		lcd_key = read_LCD_buttons() ;  // Lire le bouton appuyé sur afficheur lcd
		if (lcd_key != btnNONE) {
			key_pressed(lcd_key);
		}
	}
	digitalWrite(backlight_Pin, backlight); 
	int rc = digitalClockDisplay(buffer) ;
	timber.update() ;	// mise à jour du timer

}	// end of loop

void serialEvent() {
	if ( Serial.available()) {		// remplissage tampon minimum Date et Heure avant traitement de trame
		affiche = false ;
		while ( Serial.available()) {
			// get the buffer:
			*fifo = Serial.read() ;
			if(*fifo == '\n'){
				*fifo = '\0' ;
				fifo = ptr;
				stringComplete = true ;
				// changement du caractere de separation ligne1 sur l afficheur
				index[1] +=1;
				if(index[1] > 3){
					index[1] = 0;
				}
			}
			else{
				fifo ++ ;
				if ( fifo >= (ptr + INPUT_SIZE)){
					fifo = ptr ;
					while (Serial.available()){
						Serial.read();
					}
				}
			}
		}
	}

}
