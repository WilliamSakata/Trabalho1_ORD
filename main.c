#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define pipe "|"

void concatena(char *primeiro, char *segundo);
//void escreve_campo(char *str, FILE *reg);
void menu(int opcao, FILE *arq, FILE *reg);
void importacao(FILE *arq, FILE *reg);
//void insercao(FILE *arq);
//void remocao(FILE *arq);
int readfield(FILE *arq, FILE *reg, char str[]);

int main(){
    int opcao;
    FILE *arq, *reg;

    printf("===Menu==\nEscolha a opcao:\n1-Importacao\n2-Insercao\n3-Remocao\n0-Sair\n=> ");
    scanf("%i", &opcao);

    //menu(opcao, arq, reg); //passa a opção escolhida e dois ponteiros para o arquivo original dados inline e para o arquivo de registro

    arq = fopen("Dados-inline.txt", "r");
    reg = fopen("Arq-reg.txt", "w");

    importacao(arq, reg);
}
/*
void menu(int opcao, FILE *arq, FILE *reg){
  while(opcao != '0'){
    switch (opcao) {
      case 1:
        importacao(arq, reg);
        break;
      case 2:
        insercao(arq, reg);
        break;
      case 3:
        remocao(arq, reg);
        break;
      default:
        fclose(arq);
        fclose(reg);
        break;
      printf("===Menu==\nEscolha a opcao:\n1-Importacao\n2-Insercao\n3-Remocao\n0-Sair\n=> ");
    }
  }
}
*/
int readfield(FILE *arq, FILE *reg, char str[]){  //le o arquivo dados inline e determina o tamanho do campo entre os pipes e salva o que foi lido na string str e retorna o tamanho do campo
    int i;
    char ch;

    i=0;
    ch = fgetc(arq);

    while (ch != pipe && ch != EOF){
        str[i]=ch;
        i++;
        ch=fgetc(arq);
    }

    str[i] = '\0';

    return i;
}
/*
void escreve_campo(char *str, FILE *reg){
  fputs(str, reg);
  fputc(pipe, reg);
}*/

void concatena(char *primeiro, char *segundo){
    strcat(primeiro, segundo);
    strcat(primeiro, pipe);
}

void importacao(FILE *arq, FILE *reg){
    char num_insc[10], nome[50], curso[25], nota[5], buffer[100];
    int tam_campo;

    if(arq == NULL){
        arq = fopen("Dados-inline.txt", "r");
    }

    if(reg == NULL){
        reg = fopen("Arq-reg.txt", "w");
    }
    else{
        fclose(reg);
        fopen("Arq-reg.txt", "w");
    }

    fwrite('0', sizeof(int), 1, reg); //escreve a LED no inicio do arquivo

    tam_campo = readfield(arq, reg, num_insc);

    while (tam_campo > 0) {   //transcreve o arquivo dados inline para o arquivo de registro

        tam_campo = readfield(arq, reg, nome);
        tam_campo = readfield(arq, reg, curso);
        tam_campo = readfield(arq, reg, nota);

        concatena(buffer, num_insc);
        concatena(buffer, nome);
        concatena(buffer, curso);
        concatena(buffer, nota);

        tam_campo = srtlen(buffer);

        fwrite(&tam_campo, sizeof(int), 1, reg);

        fputs(buffer, reg);

        tam_campo = readfield(arq, reg, num_insc);
    }

}
/*
void insercao(FILE *arq, FILE *reg){

}

void remocao(FILE *arq, FILE *reg){

}
*/