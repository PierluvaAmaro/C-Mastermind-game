/*
 ============================================================================
 Name        : Mastermind.c
 Author      : Pierluca Amato
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//definizione dei parametri di gioco predefiniti
#define MAXLUNCODICE 10
#define MAXSIMBOLI 9
#define MAXTENTATIVI 15

//definizione strutture
//definizione strutture
typedef struct{
	int attempts; //numero di tentativi
	int codelen; //lunghezza del codice
	int symbols; //numero di simboli
	int dual; //presenza di doppie
}settings;

typedef struct{
	int dimensione;
	int valori[MAXLUNCODICE];
}tentativo;

typedef struct{
	int black;
	int white;
	int win;
}risposta;

typedef struct{
	settings impostazioni;
	risposta risposte;
	tentativo tentativi[MAXTENTATIVI];
	tentativo codicesegreto;
}partita;

void gotoxy(int x, int y) {
	COORD CursorPos = {x, y};
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, CursorPos);
}

void setgameoutline(){
	int contcol;
	int contraw;

	//set della riga alta del contorno
	contcol = 0;
	while(contcol<80){
		printf("-");
		contcol++;
	}

	//set della colonna sinistra
	contraw = 0;
	while(contraw < 25){
		printf("\n|");
		contraw++;
	}

	printf("\n");

	//set della riga bassa del contorno
	contcol = 0;
	while(contcol<80){
		printf("-");
		contcol++;
	}

	//set della colonna destra contorno
	contraw = 1;
	while(contraw<=25){
		gotoxy(79,contraw);
		printf("|");
		contraw++;
	}
}

void setgamemenu(){
	gotoxy(2,3);
	printf("1)Gioca");
	gotoxy(2,4);
	printf("2)Gestisci impostazioni");
	gotoxy(2,5);
	printf("3)Gestisci salvataggio");
	gotoxy(2,6);
	printf("4)Come si gioca");
	gotoxy(2,7);
	printf("0)Esci dal gioco");
}

void setimpmenu(){
		gotoxy(2,3);
		printf("1)Lunghezza codice");
		gotoxy(2,4);
		printf("2)Tentativi");
		gotoxy(2,5);
		printf("3)Simboli");
		gotoxy(2,6);
		printf("4)Doppie");
		gotoxy(2,7);
		printf("5)Impostazioni attuali");
		gotoxy(2,8);
		printf("0)Menu principale");
}

int inputc(int min, int max) {
    char input[30];  // Adjust the size as needed
    int choose;
    int cont = 0;

    do {
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &choose) != 1 || choose < min || choose > max) {
            cont++;

            if (cont == 5) {
                printf("| Troppi tentativi. Uscita...\n");
                sleep(2);
                choose = 0;
            }

            if(cont!=5){
            	printf("| Reinserire il valore: ");
            }
        }
    } while ((choose < min || choose > max) && (cont != 5));

    return choose;
}

void systempause(){
	gotoxy(0,27);
	system("pause");
}

void defaultimp(partita* game){
	game->impostazioni.attempts = 15;
	game->impostazioni.codelen = 4;
	game->impostazioni.symbols = 9;
	game->impostazioni.dual = 0; //presenza di doppie si
}

const char* showbooldoppi(partita* game)
{
    char* bool = (char*)malloc(3);
    if (bool == NULL) {
        // Gestione dell'errore se l'allocazione di memoria fallisce
        return NULL;
    }

    if (game->impostazioni.dual == 1) {
        strcpy(bool, "SI");
    } else {
        strcpy(bool, "NO");
    }

    return bool;
}


void showimp(partita* game){
    printf("\n| Impostazioni attuali");
    printf("\n| Tentativi -> %d", game->impostazioni.attempts);
    printf("\n| Lunghezza codice -> %d", game->impostazioni.codelen);
    printf("\n| Simboli fino a -> %d", game->impostazioni.symbols);

    const char* boolean = showbooldoppi(game);  //Passa l'indirizzo della struttura
    if (boolean != NULL) {
        printf("\n| Doppie -> %s", boolean);
        free((void*)boolean);  //Dealloco la memoria allocata dinamicamente
    } else {
        //Gestione dell'errore se showbooldoppi restituisce NULL
        printf("\n| Errore nella visualizzazione delle doppie.");
    }
}

void randomcode(partita* game) {
    srand(time(NULL));

    if (game->impostazioni.dual == 1) {
        // Genera il codice con doppie
        game->codicesegreto.dimensione = game->impostazioni.codelen;
        int i = 0;
        while (i < game->codicesegreto.dimensione) {
            game->codicesegreto.valori[i] = rand() % (game->impostazioni.symbols + 1);
            ++i;
        }
    } else {
        // Genera il codice senza doppie
        game->codicesegreto.dimensione = game->impostazioni.codelen;
        int i = 0;
        int symbolsAvailable[MAXSIMBOLI + 1];  // Array per tracciare la disponibilità di ciascun simbolo

        // Inizializza l'array di disponibilità
        i = 0;
        while (i <= game->impostazioni.symbols) {
            symbolsAvailable[i] = 1;  // Tutti i simboli sono inizialmente disponibili
            ++i;
        }

        i = 0;
        while (i < game->codicesegreto.dimensione) {
            int temp = rand() % (game->impostazioni.symbols + 1);

            // Controlla se il simbolo è disponibile
            if (symbolsAvailable[temp] == 1) {
                game->codicesegreto.valori[i] = temp;
                symbolsAvailable[temp] = 0;  // Imposta il simbolo come non più disponibile
                ++i;
            }
        }
    }
}

void printcode(partita* game) {
    int i = 0;
    while (i < game->codicesegreto.dimensione) {
        printf("%d", game->codicesegreto.valori[i]);
        ++i;
    }
    printf("\n");
}

void printfakecode(partita* game){
	printf("\033[0;32m");
	int cont = 0;
	while(cont<game->impostazioni.codelen){
		printf("*");
		cont++;
	}
	printf("\033[0m");
}

void setattempt(partita* game, int tentativo){
	int cont = 0;
	while(cont<game->tentativi[tentativo].dimensione){
		printf("\n| qual e' la %da cifra del codice?: ", cont+1);
		game->tentativi[tentativo].valori[cont] = inputc(0,game->impostazioni.symbols);
		cont++;
	}
}

void printattempt(partita* game, int tentativo){
	int i = 0;
	while (i < game->tentativi[tentativo].dimensione){
		printf("%d", game->tentativi[tentativo].valori[i]);
	    ++i;
	}
}

void valutatentativo(partita* game, int tentativo) {
    int whitepeg = 0;
    int blackpeg = 0;
    int i = 0, j;

    // Reset counters
    game->risposte.black = 0;
    game->risposte.white = 0;

    // Count black pegs
    while (i < game->codicesegreto.dimensione) {
        if (game->tentativi[tentativo].valori[i] == game->codicesegreto.valori[i]) {
            blackpeg++;
        }
        i++;
    }

    i = 0;

    // Count white pegs
    while (i < game->codicesegreto.dimensione) {
        j = 0;
        while (j < game->codicesegreto.dimensione) {
            if (i != j &&
                game->tentativi[tentativo].valori[i] == game->codicesegreto.valori[j] &&
                game->tentativi[tentativo].valori[i] != game->codicesegreto.valori[i]) {
                whitepeg++;
            }
            j++;
        }
        i++;
    }
    // Update counters in the game structure
    game->risposte.black = blackpeg;
    game->risposte.white = whitepeg;

    // Check for a win
    if (blackpeg == game->codicesegreto.dimensione) {
        game->risposte.win = 1;
    }
}

void resetrisposta(partita* game){
	game->risposte.win = 0;
}

void discovercode(partita* game){
	printf("\033[0;32m");
	int cont = 0;
	int colonna = 27;
	int riga = 3;
	while(cont<game->codicesegreto.dimensione){
		gotoxy(colonna,riga);
		printf("%d",game->codicesegreto.valori[cont]);
		cont++;
		colonna++;
	}
	printf("\033[0m");
}

void setgameoutline2(){
	int contcol;
		int contraw;

		//set della riga alta del contorno
		contcol = 0;
		while(contcol<80){
			printf("-");
			contcol++;
		}

		//set della colonna sinistra
		contraw = 0;
		while(contraw < 25){
			printf("\n|");
			contraw++;
		}

		printf("\n");

		//set della colonna destra contorno
		contraw = 1;
		while(contraw<=25){
			gotoxy(79,contraw);
			printf("|");
			contraw++;
		}
}

void clear(int type){
	if(type==1){
		system("cls"); //codice ANSI che pulisce lo schermo;
		setgameoutline();
	}else{
		system("cls"); //codice ANSI che pulisce lo schermo;
		setgameoutline2();
	}
}


//INIZIO DEL MAIN
int main(void){
/*
**************
*	//numero max colonne = 80 (X)	   *
*	//numero max righe = 25 (Y)		   * //NOTAZIONI
*	//gotoColonneRighe			       *
**************
*/

	//ALLOCO DINAMICAMENTE I PUNTATORI
	partita* mygame = (partita*)malloc(sizeof(partita));
		if (mygame == NULL){
		printf("Errore interno al programma.\n");
		return 1;
}

	//assegnazione delle impostazioni di default
	defaultimp(mygame);
	int selezioneout;
	int selezionein;
	int selezioneingame;

	do{
		//pulizia iniziale dello schermo e set outline
		clear(1);

		//set della scritta iniziale
		gotoxy(35,1);
		printf("MASTERMIND");

		//set menu iniziale
		setgamemenu();

		//*************INIZIO FASE SCELTA **************//
			gotoxy(2,8);
			printf("Inserire un valore tra 0 e 4: ");
			selezioneout = inputc(0,4); //funzione input controllato con inserimento integrato

			do{
			//selezione GIOCA

			if(selezioneout == 1){
				do{
					resetrisposta(mygame);
					clear(2);
					gotoxy(35,1);
					printf("START PLAY");
					printf("\n| Genero il codice segreto...");
					randomcode(mygame);
					printf("\n| Codice segreto generato: ");
					printfakecode(mygame);
					//printcode(mygame);
					printf("\n| 1)Prova a indovinare");
					printf(" -- ");
					printf("0)Torna al menu principale: ");
					selezionein = inputc(0,1);
					if(selezionein == 1){
						int tentativoattuale = 0;
						selezioneingame = 1;
						while(tentativoattuale !=mygame->impostazioni.attempts && selezioneingame!=0){
							printf("\n| TENTATIVO NUMERO %d/%d",(tentativoattuale+1),mygame->impostazioni.attempts);
							mygame->tentativi[tentativoattuale].dimensione = mygame->impostazioni.codelen;
							setattempt(mygame, tentativoattuale);
							printf("\n| il tuo codice: ");
							printattempt(mygame,tentativoattuale);
							valutatentativo(mygame,tentativoattuale); //valutazione del tentativo
							if(mygame->risposte.win == 1){
								printf("\n| Complimenti, hai indovinato il codice!");
								printf("\n| Vai a vedere il codice segreto coperto sopra!");
								printf("\n| Ritorno al menu di gioco tra 7 secondi");
								discovercode(mygame);
								selezioneingame = 0;
								sleep(7);
							}else{
								printf("\n| Numeri corretti in posizione corretta: %d",mygame->risposte.black);
								printf("\n| Numeri corretti in posizione sbagliata: %d",mygame->risposte.white);
								printf("\n| 1)Prova a indovinare");
								printf(" -- ");
								printf("0)Abbandona: ");
								selezioneingame = inputc(0,1);
							}
							tentativoattuale++;
						}
						if(selezioneingame==0 && mygame->risposte.win!=1){
							printf("\n| Che fai, ti arrendi gia'...");
							sleep(2);
						}
						if((tentativoattuale)+1>mygame->impostazioni.attempts && mygame->risposte.win != 1){
							printf("\n\n| Che peccato, tentativi terminati...");
							sleep(2);
						}
					}
				}while(selezionein!=0);
			}

			//selezione GESTISCI IMPOSTAZIONI
			else if(selezioneout == 2){
				int change;
				clear(2);
				gotoxy(34,1);
				printf("IMPOSTAZIONI");
				setimpmenu();
				do{
					printf("\n| Inserire un valore tra 0 e 5: ");
					selezionein = inputc(0,5);
					if(selezionein==1){
						printf("\n| inserire lunghezza codice(2-10): ");
						change = inputc(2,MAXLUNCODICE);
						mygame->impostazioni.codelen = change;
						printf("\n| lunghezza codice settata con successo a %d!", mygame->impostazioni.codelen);
					}

					if(selezionein==2){
						printf("\n| inserire numero tentativi(1-15): ");
						change = inputc(1,MAXTENTATIVI);
						mygame->impostazioni.attempts = change;
						printf("\n| Numero tentativi settato con successo a %d!", mygame->impostazioni.attempts);
					}

					if(selezionein==3){
						printf("\n| giocare con simboli fino a(1-%d): ", MAXSIMBOLI);
						change = inputc(1,MAXSIMBOLI);
						mygame->impostazioni.symbols = change;
						printf("\n| Numero simboli settato con successo fino a %d!", mygame->impostazioni.symbols);
					}

					if(selezionein==4){
						printf("\n| inserire presenza doppie(1=SI,0=NO): ");
						change = inputc(0,1);
						mygame->impostazioni.dual = change;
						if(change == 1){
							printf("\n| presenza doppi settata con successo a SI!");
						}else{
							printf("\n| presenza doppi settata con successo a NO!");
						}
					}

					if(selezionein==5){
						showimp(mygame);
					}
				}while(selezionein!=0);
			}

			else if(selezioneout == 3){
				//Salvataggio da implementare...;
				clear(1);
				gotoxy(37.5,1);
				printf("SALVATAGGIO");
				printf("\n| Salvataggio da implementare...");
				printf("\n| Premere 0 per tornare indietro: ");
				selezionein = inputc(0,0);
			}

			else if(selezioneout == 4){
				clear(1);
				gotoxy(37.5,1);
				printf("GUIDA");
				printf("\n| Mastermind e' un gioco da tavolo in cui un giocatore, il");
				printf("\n| decodificatore, deve indovinare il codice segreto composto");
				printf("\n| dal codificatore, il suo avversario. Nella versione originale");
				printf("\n| di mastermind, il codice segreto e' di quattro cifre e il");
				printf("\n| codificatore ha a disposizione, per comporlo, le dieci cifre");
				printf("\n| del sistema decimale standard (0,1,2,3,4,5,6,7,8,9).");
				printf("\n| Regolamento: ");
				printf("\n| Dopo che il codificatore ha composto il codice, il");
				printf("\n| decodificatore fa il suo primo tentativo, cercando di");
				printf("\n| indovinare il codice. Il codificatore, appena il suo");
				printf("\n| avversario ha completato il tentativo, fornisce degli aiuti");
				printf("\n| comunicando:");
				printf("\n| -Il numero di cifre giuste al posto giusto, cioe' le cifre");
				printf("\n| del tentativo che sono effettivamente presenti nel codice");
				printf("\n| al posto tentato, con pioli neri.");
				printf("\n| -Il numero di cifre giuste al posto sbagliato, cioe' le cifre");
				printf("\n| del tentativo che sono effettivamente presenti nel codice,");
				printf("\n| ma non al posto tentato, con pioli bianchi.");
				printf("\n| Non bisogna comunicare quali cifre sono giuste o sbagliate");
				printf("\n| ma solo quante. Se il decodificatore riesce ad indovinare");
				printf("\n| il codice entro il numero di tentativi predeterminati");
				printf("\n| (solitamente i tentativi sono 9 allora quest'ultimo vince");
				printf("\n| la partita, altrimenti vince il codificatore.");
				printf("\n| Premere 0 per tornare indietro: ");
				selezionein = inputc(0,0);
			}
		}while(selezionein!=0 && selezioneout != 0);
	}while(selezioneout!=0);
	systempause();
	return 0;
}
