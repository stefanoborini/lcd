#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm/io.h>

#define PORTADDRESS 0x278 /* per LPT2 0x278 */


#define DATA PORTADDRESS
#define STATUS PORTADDRESS+1
#define CONTROL PORTADDRESS+2
/* conversione da TurboC per Dos a Linux */

#define delay(T) usleep(T*1000)
#define outportb(PORTA,DATO) outb(DATO,PORTA)
#define inportb(PORTA) inb(PORTA)
/*
il pin 1 e' lo strobe. l'lcd esegue sul fronte di discesa
il pin 17 e' lo switch dati/registri ( dati=0 )
*/
char *prima="primariga1234567";
char *seconda="secondariga12345";
char *terza="terzariga1234567";
char *quarta="quartariga123456";

void main (int argc, char *argv[])  {

char init[10];
int count;
int len;


init[0]=0x0f; /* init display */
init[1]=0x38; /* dual line /8 bit*/
init[2]=0x01; /* clear display*/
init[3]=0x0c; /* display on, cursor on, blinking off */

ioperm(PORTADDRESS,3,1); /* in linux e' fondamentale */

/* inizializzazione LCD */
outportb(CONTROL,inportb(CONTROL) & 0xDF); /* reset control port */
outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
outportb(CONTROL,inportb(CONTROL) | 0x08); /* reset pin 17 */

/* nota per Munehiro: il pin 16 non serve piu' a niente...a dire 
il vero anche nell'altro codice non serviva a niente */

for (count=0;count<=3;count++)
 {
 outportb(DATA,init[count]);
 outportb(CONTROL,inportb(CONTROL) & 0xFE); /* set strobe pin 1 */
 delay(40);
 outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
 delay(40);
 }
printf("Invio caratteri della prima riga  : %s\n",prima);
/* invio caratteri della prima riga*/
outportb(CONTROL,inportb(CONTROL) & 0xF7); /* set pin 17 */
len=strlen(prima);
for (count=0;count<len;count++)
 {
 outportb(DATA,prima[count]);
 outportb(CONTROL,inportb(CONTROL) & 0xFE); /* set strobe */
 delay(2);
 outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
 delay(2);
 }
 printf("Invio caratteri della terza riga: %s\n",terza);
 /* invio i caratteri della terza riga */
 len=strlen(terza);
 for (count=0;count<len;count++)
 {
 outportb(DATA,terza[count]);
 outportb(CONTROL,inportb(CONTROL) & 0xFE); /* set strobe */
 delay(2);
 outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
 delay(2);
 }
 
/* invio di caratteri di completamento*/
 printf("Invio caratteri  di completamento\n");
 for (count=0;count<8;count++)
 {
 outportb(DATA,'-');
 outportb(CONTROL,inportb(CONTROL) & 0xFE); /* set strobe */
 delay(2);
 outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
 delay(2);
 }

 /* invio i caratteri della seconda riga */
 printf("Invio caratteri della seconda riga: %s\n",terza);
 len=strlen(seconda);
 for (count=0;count<len;count++)
 {
 outportb(DATA,seconda[count]);
 outportb(CONTROL,inportb(CONTROL) & 0xFE); /* set strobe */
 delay(2);
 outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
 delay(2);
 }
 
/* invio i caratteri della quarta riga */
 printf("Invio caratteri della quarta riga: %s\n",quarta);
 len=strlen(quarta);
 for (count=0;count<len;count++)
 {
 outportb(DATA,quarta[count]);
 outportb(CONTROL,inportb(CONTROL) & 0xFE); /* set strobe */
 delay(2);
 outportb(CONTROL,inportb(CONTROL) | 0x01); /* reset strobe */
 delay(2);
 }
/* RIASSUMENTO, se il messaggio da stampare e' lungo 4 righe

1- inizializzare l'lcd
2- inviare i 16 caratteri della prima riga
3- inviare i 16 caratteri della terza riga
4- inviare 8 caratteri a vuoto
5- inviare i 16 caratteri della seconda riga
6- inviare i 16 caratteri della quarta riga
*/ 

}
