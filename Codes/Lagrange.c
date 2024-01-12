/*
Matheus Henrique Santos Zacarias

O programa recebe quantos pontos o usuário quiser adicionar (através de um 'do-while'), sendo assim
após ser digitado todos os pontos (Adiciona-se cada ponto preciosando "Enter") de x e f(x)
o usuário deve pressionar "ESC". Posteriormente será pedido o valor (ponto) de x que o usuário
quer descobrir o f(x) relativo. O processo para interpolação através do método de Lagrange
inicia-se (vide função calcLagrange). Concluído, apresenta-se na tela do usuário os langValues de Ln(x),
fruto da operação produtória contida dentro do método. concatenando aos outros pontos já cientes é apresentado
o resultado da operação, portanto, f(x) do ponto x informado. Caso o usuário queira, poderá adicionar novos pontos
para x.
*/

/*Bibliotecas utilizadas*/
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

/*Struct para informações dos pontos*/
typedef struct langTable{
    float fx;
    float x;
    bool origin;
}langTable;

/*Protótipo das funções*/
void SetColor(int ForgC);
void showCursor(void);
void header(void);
void hideCursor(void);
void registerLangTable(langTable **p, int tableSize);
void registerFloat(float **p, int tableSize);
float calcLagrange(langTable *origin, float receiveInput, int tableSize);
void sortValues (langTable *origin, int tableSize);
void registerValues(langTable *p, char *str, int pos);
int receiveValues(langTable *valor, int tableSize, bool origin, float min, float max);
void showLangTable(langTable *valor, int tableSize);
void calcPolynomial(langTable *origin, int tableSize);

/*Início da Main*/
void main (void){

    int cont = 1, i, flag;
    float min, max, receiveInput;
    char proceed;

    setlocale(LC_ALL, "Portuguese");
    langTable *langValues = NULL;

    /*Do-While Geral para finalização do programa*/
    do{
        /*Do-While para recebimento dos pontos tabelados*/
        do{
            registerLangTable(&langValues, cont);
            header();

            printf(" Valores tabelados: (utilize , para separar decimais)");
            flag = receiveValues(langValues, cont, false, 0, 0);

            if (flag == 2){
                hideCursor();
                fflush(stdin);
                printf("\n\n O valor digitado já consta na langTable fornecida.");
                printf("\n Pressione ESC para encerrar ou qualquer tecla para tentar novamente");
                proceed = getch();
                continue;
            }
            sortValues(langValues, cont);
            showLangTable(langValues, cont);

            fflush(stdin);
            printf("\n\n\n pressione Esc pra cancelar, Enter para digitar um novo valor ou qualquer tecla para passar para o proximo passo");
            proceed = getch();

            if (((proceed  != 13) && (proceed  != 27)) && cont == 1){
                header();
                printf("\n\n Foi digitado apenas um ponto, favor digite pelo menos 2 pontos.");
                proceed = 13;
                getch();
            }
            cont ++;

        }while((proceed  != 27) && (proceed  == 13));

        if (proceed == 27){
            header();
            printf(" Programa encerrado \n\n\n\n\n\n\n");
            break;
        }

        /*Ponto mínimo e máximo (evitar extrapolação)*/
        min = max = langValues->x;
        for (i=0; i<(cont-1); i++){
            if ((langValues+i)->x > max){
                max = (langValues+i)->x;
            }
            else if ((langValues+i)->x < min){
                min = (langValues+i)->x;
            }
        }

        /*recebe o ponto para ser deduzido, o 'x' desconhecido*/
        do{
            header();
            registerLangTable(&langValues, cont);
            showCursor();
            printf(" Valores a serem interpolados (Intervalo permitido: %.2f - %.2f): ", min, max);
            flag = receiveValues(langValues, cont, true, min, max);

            if (flag != 0){
                hideCursor();
                fflush(stdin);

                if (flag == 1){
                    printf("\n\n Impossível interpolar porque está fora do intevalo.");
                    printf("\n Pressione ESC para encerrar ou qualquer tecla para tentar novamente");
                    proceed = getch();
                    continue;
                }
                else if (flag == 2){
                    printf("\n\n O valor digitado já consta na langTable fornecida ou já foi calculado.");
                    printf("\n Pressione ESC para encerrar ou qualquer tecla para tentar novamente");
                    proceed = getch();
                    continue;
                }

            }

            (langValues+(cont-1))->fx = calcLagrange(langValues, (langValues+(cont-1))->x, cont);
            sortValues(langValues, cont);
            showLangTable(langValues, cont);

            fflush(stdin);
            printf("\n\n\n pressione Enter para digitar um novo valor ou qualquer tecla para exibir os resultados");
            proceed = getch();

            if (proceed == 13){
                cont ++;
            }

        }while(proceed == 13);


        if (flag != 0){
            cont --;
        }

        /*Resultado final*/
        header();
        printf("\t\t\t        Resultados");
        //calcPolynomial(langValues, cont);
        printf("\n Legenda: ");
        SetColor(1);
        printf("\n Pontos interpolados (azul) ");
        SetColor(15);
        printf("\n Pontos tabelados (branco) ");
        printf("\n =========================================================================\n\n");
        showLangTable(langValues, cont);
        printf("\n\n\n Pressione ESC para sair do programa ou qualquer tecla para iniciar uma nova sequencia");
        proceed = getch();
        cont = 1;
        registerValues(langValues, "wb", 1);
        free(&langValues);

    }while(proceed  != 27);

}//Main

/*
void calcPolynomial(langTable *origin, int tableSize){

    int i,j = 0, cont = 0;
    float *coeficientes = NULL, *aux = NULL, denominator=1, px=0, ln;

    for (i=0;i<tableSize;i++){
        if ((origin+i)->origin == false){
            cont ++;
        }
    }

    langTable *valor = NULL;
    registerLangTable(&valor, cont);

    registerFloat(&coeficientes, cont);
    registerFloat(&aux, cont);

    for (i=0;i<tableSize;i++){
        if ((origin+i)->origin == false){
            (valor+j)->fx = (origin+i)->fx;
            (valor+j)->x = (origin+i)->x;
            j++;
        }
    }
    j=0;

    if (cont >= 3){
        for(i=0; i<cont; i++){
            denominator=1;
            for(j=0; j<cont; j++){
                if(j!=i){
                    denominator=denominator*((valor+i)->x-(valor+j)->x);
                }
            }
            ln=(((valor+i)->fx)/denominator);
            *coeficientes = *coeficientes +ln;
        }

    }


} */

/*Procura na Struct o ponto deduzido para destaca-lo
e mostra todos os pontos tabelados*/
void showLangTable(langTable *valor, int tableSize){
    int i;

    hideCursor();
    for (i=0; i<tableSize; i++){
        SetColor(15);
        printf("\n |", (valor+i)->x);
        if ((valor+i)->origin == false){
            printf(" X: %.2f", (valor+i)->x);
            printf("\n | f(%.2f): %.2f",(valor+i)->x, (valor+i)->fx);
        }
        else{

            SetColor(1);
            printf(" X: %.2f", (valor+i)->x);
            SetColor(15);
            printf("\n |", (valor+i)->x);
            SetColor(1);
            printf(" f(%.2f): %.2f",(valor+i)->x, (valor+i)->fx);
        }

        SetColor(15);
        printf("\n +-----------------");
    }
}//pontoInterpolacao

/*Recebe os langValues tabelados que o usuário já possui*/
int receiveValues(langTable *valor, int tableSize, bool origin, float min, float max){

    int i, zeros= 0, j;
    float receiveInput;
    int flag = 0;
    showCursor();
    printf("\n\n Digite o valor de x: ");
    scanf("%f", &receiveInput);



    for (i=0; i<(tableSize); i++){
        if (receiveInput == (valor+i)->x){

            if (receiveInput == 0){
                for (j=0; j<(tableSize); j++){
                    if ((valor+i)->x == 0){
                        zeros ++;
                    }
                }

                if (zeros > 1){
                    return 2;
                    break;
                }
            }
            else{
                return 2;
                break;
            }
        }
    }

    if (origin == false){
        (valor+(tableSize-1))->x = receiveInput;
        printf("\n Digite o valor de f(x): ");
        scanf("%f", &(valor+(tableSize-1))->fx);
        (valor+(tableSize-1))->origin = false;
    }
    else{

        //Verifica se o x está dentro do intervalo permitido
        if (receiveInput < min || receiveInput> max){
            return 1;
        }

        /*Verifica se o ponto digitado já foi acrescido na langTable*/
        (valor+(tableSize-1))->x = receiveInput;
        (valor+(tableSize-1))->origin = true;
    }
    return flag;

}//receiveValues

/*Realiza a dedução do ponto através da formula de Lagrange*/
float calcLagrange(langTable *origin, float receiveInput, int tableSize){

    int i,j = 0, cont = 0;
    float numerator=1, denominator=1, px=0, ln;

    for (i=0;i<tableSize;i++){
        if ((origin+i)->origin == false){
            cont ++;
        }
    }

    langTable *valor = NULL;
    registerLangTable(&valor, cont);

    for (i=0;i<tableSize;i++){
        if ((origin+i)->origin == false){
            (valor+j)->fx = (origin+i)->fx;
            (valor+j)->x = (origin+i)->x;
            j++;
        }
    }

    j=0;

    for(i=0; i<cont; i++){
        numerator=1;
        denominator=1;
        for(j=0; j<cont; j++){
            if(j!=i){
                numerator=numerator*(receiveInput-(valor+j)->x);
                denominator=denominator*((valor+i)->x-(valor+j)->x);
            }
        }
        ln=(numerator/denominator);
        px=px+((numerator/denominator)*(valor+i)->fx);
    }
    return px;
}//calcLagrange

/*Armazena os langValues obtidos dentro de um arquivo .bin*/
void registerValues(langTable *p, char *str, int pos){

    FILE *fptr = NULL;

    if ((fptr = fopen("Tabela.bin", str)) == NULL)
        printf("\nErro ao abrir o arquivo");
    else{
        if (strcmp(str, "rb+") == 0)
            fseek(fptr, pos * sizeof(langTable), 0);
        fwrite(p, sizeof(langTable), 1, fptr);
    }
    fclose(fptr);
}//registerValues

/*Organiza sempre mantendo em ordem crescente na Struct em relação ao 'x'*/
void sortValues (langTable *origin, int tableSize){

    int i, j;
    float auxX, auxFx;
    bool auxOrigin;

    for (i = 0; i < tableSize; i++){
        for (j = 0; j < tableSize; j++){

            if ((origin+i)->x < (origin+j)->x){
                auxX = (origin+i)->x;
                auxFx = (origin+i)->fx;
                auxOrigin = (origin+i)->origin;

                (origin+i)->x = (origin+j)->x;
                (origin+i)->fx = (origin+j)->fx;
                (origin+i)->origin = (origin+j)->origin;

                (origin+j)->x = auxX;
                (origin+j)->fx = auxFx;
                (origin+j)->origin = auxOrigin;
            }
        }
    }
}//sortValues

/*Alocação dinâmica para a Struct*/
void registerLangTable(langTable **p, int tableSize){

    if ((*p = (langTable *)realloc(*p, tableSize * sizeof(langTable))) == NULL){
        printf("\n\n Erro ao alocar. \n Encerrando...");
        exit(1);
    }
}//registerLangTable

void registerFloat(float **p, int tableSize){

    if ((*p = (float *)realloc(*p, tableSize * sizeof(float))) == NULL){
        printf("Erro ao alocar. \nEncerrando...");
        exit(1);
    }
}//registerLangTable

/*Função com o cabeçalho*/
void header(void){

    setlocale(LC_ALL, "Portuguese");
    system("cls");
    printf("\n\t\t\tMétodos Númericos: Lagrange");
	printf("\n =========================================================================\n\n");
}//header

/*Função para utilizar as cores em C*/
void SetColor(int ForgC){

    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
    }
    return;
}//SetColor

/*Função para remover o cursor*/
void hideCursor(void){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 5;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}//hideCursor

/*Função para mostrar o cursor*/
void showCursor(void){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 5;
   info.bVisible = TRUE;
   SetConsoleCursorInfo(consoleHandle, &info);
}//mostrarCursor
