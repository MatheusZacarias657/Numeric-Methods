
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>
#include<string.h>
#include <time.h>
#include <conio.h>
#include <stdbool.h>
#include <dos.h>
#include <dir.h>

typedef struct generalInfo{
    float error;
    int k;
    float equation[20];
    float min;
    float max;
}generalInfo;

typedef struct interaction{
    int indice;
    float a;
    float m;
    float b;
    float fA;
    float fM;
    float fB;
    float ba;
    int stop;
}interaction;

float calcEquation (float num);
void header();
void allocateInteraction (interaction **p, int tam);
int calcK (float a, float b, float error);
void generalRegister (generalInfo *p, char *str, int pos);
void calcDichotomy (float initialA, float initialB, float error, interaction *p, int criterion, generalInfo *pGeral);
int verifyInteractions(void);
void registerInteractions(interaction *p, char *str, int pos);
void showInteractions (interaction *p, int criterion);
void SetColor(int ForgC);
void showCursor();
void hideCursor();
void menuOptions(int selec);
int menu(void);
void receiveGeneralInfo (void);

void main (void){

    generalInfo *info = NULL;
    interaction *pInteraction = NULL;
    int criterion, numElements, i, cont=1;

    registerGeneral(&info, 1);
    allocateInteraction(&pInteraction, 1);
    setlocale(LC_ALL, "Portuguese");

    do{
        criterion = menu();

        if (criterion != 4){
            header();
            printf("\n Digite a quantidade de termos: ");
            scanf("%d", &numElements);

            numElements = numElements*2;

            for(i=0; i<numElements; i+=2){
                printf("\n Digite o %d° cojunto de coeficiente e expoente: ", cont);
                scanf("%f;%f", (info+i)->equation, (info+i+1)->equation);
                cont ++;
            }
            printf("\n Digite o valor de A: ");
            scanf("%f", &info->min);
            printf("\n Digite o valor de B: ");
            scanf("%f", &info->max);
            printf("\n Digite o valor do error: ");
            scanf("%f", &info->error);

            if ((calcEquation(info->min)* calcEquation(info->max))> 0){
                continue;
            }

            info->k = calcK(info->min, info->max, info->error);
            generalRegister(info, "wb", 1);
            calcDichotomy(info->min, info->max, info->error, pInteraction, criterion, info);
            showInteractions (pInteraction, criterion);
            getch();
        }

    }while (criterion != 4);


}

float calcEquation (float num){

    //edita a formula
    return (float) ((3*(pow((double) num,5))) - (4*(pow((double) num,3))) + (pow((double) num,2)) -7);
}

int calcK (float a, float b, float error){

    return round((float) ((log ((double) (b-a))-log((double) error))/log (2))) ;
}

void calcDichotomy (float initialA, float initialB, float error, interaction *p, int criterion, generalInfo *pGeral){

    int count = 0;
    bool finaliza = false;

    p->indice = count;
    p->a = initialA;
    p->b = initialB;
    p->m = (p->a + p->b)/2;
    p->fA = calcEquation (p->a);
    p->fB = calcEquation (p->b);
    p->fM = calcEquation (p->m);
    p->ba = p->b - p->a;

    if ((fabs(p->fM) < error) && criterion == 1){
        p->stop = 1;
    }

    if ((fabs (p->ba) < error) && criterion == 2){
        p->stop = 2;
    }

    if ((p->indice == pGeral->k) && criterion == 3){
        p->stop = 3;
    }

    registerInteractions(p, "wb", 1);

    if (p->stop == criterion){
        return;
    }

    while (p->stop != criterion){

        count = verifyInteractions();
        p->indice = count;

        if (((p->fA < 0) && (p->fM > 0)) || ((p->fA > 0) && (p->fM < 0))){
            p->b = p->m;
        }
        else{
            p->a = p->m;
        }

        p->m = (p->a + p->b)/2;
        p->fA = calcEquation (p->a);
        p->fB = calcEquation (p->b);
        p->fM = calcEquation (p->m);
        p->ba = p->b - p->a;


        if ((fabs(p->fM) < error) && criterion == 1){
            p->stop = 1;
        }

        if ((fabs (p->ba) < error) && criterion == 2){
            p->stop = 2;
        }

        if ((p->indice == pGeral->k) && criterion == 3){
            p->stop = 3;
        }

        registerInteractions(p, "ab", 1);
    }

}

void showInteractions (interaction *p, int criterion){

    int i, j, tamanho;
    FILE *fptr=NULL;

    setlocale(LC_ALL, "Portuguese");
    header();
    tamanho = verifyInteractions();

    if((fptr=fopen("Interacoes.bin","rb"))==NULL){
        printf("\nErro ao abrir o arquivo");
    }
    else{

        receiveGeneralInfo();

        fseek(fptr,(tamanho-1)*sizeof(interaction),0);
        fread(p,sizeof(interaction),1,fptr);

        SetColor(4);
        printf(" Zero Real: %.3f", p->m);

        if (criterion == 1){
           printf("\n Erro Aproximado: +/- %.3f", fabs(p->fM));
        }
        else if (criterion == 2){
           printf("\n Erro Aproximado: +/- %.3f", fabs(p->ba));
        }
        else{
            printf("\n Erro por f(m): +/- %.3f", fabs(p->fM));
            printf("\n Erro por b-a: +/- %.3f", fabs(p->ba));
        }

        SetColor(15);
        printf("\n =========================================================================\n");
        printf("\n\t\t\tInterações Feitas\n");

        if (criterion == 1){
            printf("\n        i   |    a    |    m    |   b   |   f(a)   |   f(m)   |   f(b)   ");
        }
        else{
            printf("\n   i   |   a   |   m   |   b   |   f(a)   |   f(m)   |   f(b)   |   b-a   ");
        }

        for(i=0;i<tamanho;i++){

            fseek(fptr,i*sizeof(interaction),0);
            fread(p,sizeof(interaction),1,fptr);

            if (criterion == 1){
                printf("\n        %d   | %.3f  | %.3f  | %.3f  |   %.3f   |   %.3f   |   %.3f    ", p->indice, p->a, p->m, p->b, p->fA, p->fM, p->fB);
            }
            else{
                printf("\n   %d  | %.3f  | %.3f  | %.3f  |   %.3f   |   %.3f   |   %.3f   |   %.3f", p->indice, p->a, p->m, p->b, p->fA, p->fM, p->fB, p->ba);
            }
        }
        fclose(fptr);
    }
}

void receiveGeneralInfo (void){
    FILE *fptr=NULL;
    generalInfo *p = NULL;
    registerGeneral(&p, 1);

    if((fptr=fopen("generalInfo.bin","rb"))==NULL){
        printf("\nErro ao abrir o arquivo");
    }
    else{
        fread(p,sizeof(generalInfo),1,fptr);
        printf("\n Equação: %s", p->equation);
        printf("\n Valor de A inicial: %.3f", p->min);
        printf("\n Valor de B inicial: %.3f", p->max);
        printf("\n Valor do error: %.3f", p->error);
        printf("\n Valor do k: %d", p->k);
        printf("\n ---------------------------------------\n");

        fclose(fptr);
    }
}

int verifyInteractions(void){

    long int cont = 0;
    FILE *fptr = NULL;
    if ((fptr = fopen("Interacoes.bin", "rb")) == NULL)
        return cont;
    else{
        fseek(fptr, 0, 2);
        cont = ftell(fptr) / sizeof(interaction);
        fclose(fptr);
        return cont;
    }
}

void allocateInteraction(interaction **p, int tam){

    if ((*p = (interaction *)realloc(*p, tam * sizeof(interaction))) == NULL){
        printf("Erro ao alocar. \nEncerrando...");
        exit(1);
    }
}

void registerGeneral(generalInfo **p, int tam){

    if ((*p = (generalInfo *)realloc(*p, tam * sizeof(generalInfo))) == NULL){
        printf("Erro ao alocar. \nEncerrando...");
        exit(1);
    }
}

void registerInteractions(interaction *p, char *str, int pos){

    FILE *fptr = NULL;

    if ((fptr = fopen("Interacoes.bin", str)) == NULL)
        printf("\nErro ao abrir o arquivo");
    else{
        if (strcmp(str, "rb+") == 0)
            fseek(fptr, pos * sizeof(generalInfo), 0);
        fwrite(p, sizeof(generalInfo), 1, fptr);
    }
    fclose(fptr);
}

void generalRegister (generalInfo *p, char *str, int pos){

    FILE *fptr = NULL;

    if ((fptr = fopen("generalInfo.bin", str)) == NULL)
        printf("\nErro ao abrir o arquivo");
    else{
        if (strcmp(str, "rb+") == 0)
            fseek(fptr, pos * sizeof(generalInfo), 0);
        fwrite(p, sizeof(generalInfo), 1, fptr);
    }
    fclose(fptr);
}


void header(){

    setlocale(LC_ALL, "Portuguese");
    system("cls");
    printf("\n\t\t\tMétodos Númericos: Dicotomia");
	printf("\n =========================================================================\n");

}

int menu(void){

    fflush(stdin);
	setlocale(LC_ALL, "Portuguese");
	int navigator=1, receiveUserInput;

    do{
        header();
        printf("  Use w para ir para cima e s para baixo \n");
        hideCursor();
        menuOptions(navigator);
        fflush(stdin);
        receiveUserInput = getch();
        receiveUserInput = toupper(receiveUserInput);

        if (receiveUserInput == 87){
            if(navigator == 1){
                navigator =4;
            }
            else{
                navigator --;
            }
        }

        if (receiveUserInput == 83){
            if(navigator == 4){
                navigator =1;
            }
            else{
                navigator ++;
            }
        }

    }while (receiveUserInput != 13);

    showCursor();
    if (navigator == 4){
        hideCursor();
        header(0);
        printf("\n Até a próxima \n");
    }

    return navigator;
}

void menuOptions(int selec){

    switch(selec){
        case 1:
        printf("\n  >Aplicar em função do f(m)");
        printf("\n   Aplicar em função de b-a");
        printf("\n   Aplicar em função do k");
        printf("\n   Sair");
        break;
        case 2:
        printf("\n   Aplicar em função do f(m)");
        printf("\n  >Aplicar em função de b-a");
        printf("\n   Aplicar em função do k");
        printf("\n   Sair");
        break;
        case 3:
        printf("\n   Aplicar em função do f(m)");
        printf("\n   Aplicar em função de b-a");
        printf("\n  >Aplicar em função do k");
        printf("\n   Sair");
        break;
        case 4:
        printf("\n   Aplicar em função do f(m)");
        printf("\n   Aplicar em função de b-a");
        printf("\n   Aplicar em função do k");
        printf("\n  >Sair");
        break;
        default:
        break;
    }

}

void SetColor(int ForgC){

    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
    }
    return;
}

void hideCursor(){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 5;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

void showCursor(){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 5;
   info.bVisible = TRUE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

/*
void calculaTabela(){
    if (criterion == 1){
        printf("\n    +-------+-------+-------+-------+----------+----------+----------+");
        printf("\n    |                  .::Tabela de Interações::.                    |");
        printf("\n    +-------+-------+-------+-------+----------+----------+----------+");
        printf("\n    |   i   |   a   |   m   |   b   |   f(a)   |   f(m)   |   f(b)   |");
        printf("\n    +-------+-------+-------+-------+----------+----------+----------+");
        printf("\n    |   %d  | %.3f  | %.3f  | %.3f  |   %.3f   |   %.3f   |   %.3f   |", p->indice, p->a, p->m, p->b, p->fA, p->fM, p->fB);
        printf("\n    +-------+-------+-------+-------+----------+----------+----------+");
    }
}
*/
