#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define pipe "|"

void concatena(char *primeiro, char *segundo);
//void escreve_campo(char *str, FILE *reg);
void menu(int opcao, FILE *arq, FILE *reg);
void importacao(FILE *arq, FILE *reg);
//void insercao(FILE *reg);
void remocao(FILE *reg);
int readfield(FILE *arq, char str[]);
int busca_rem(FILE *reg, int num_insc);

int main(){
    long pos;
    char ch;
    int opcao;
    FILE *arq, *reg;

    printf("===Menu==\nEscolha a opcao:\n1-Importacao\n2-Insercao\n3-Remocao\n0-Sair\n=> ");
    scanf("%i", &opcao);
    fflush(stdin);
    //menu(opcao, arq, reg); //passa a opção escolhida e dois ponteiros para o arquivo original dados inline e para o arquivo de registro

    arq = fopen("Dados-inline.txt", "r");
    reg = fopen("Arq-reg.txt", "w");

    importacao(arq, reg);

    fseek(reg, 3, SEEK_SET);
    pos = ftell(reg);
    printf("pos = %ld\n", pos);
    fflush(stdin);
    fscanf(reg, "%c", &ch);
    printf("seek set = %c\n", ch);
    fclose(arq);
    fclose(reg);
}
/*
void menu(int opcao, FILE *arq, FILE *reg){
  while(opcao != '0'){
    switch (opcao) {
      case 1:
        importacao(arq, reg);
        break;
      case 2:
        insercao(reg);
        break;
      case 3:
        remocao(reg);
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
int readfield(FILE *arq, char str[]){  //le o arquivo dados inline e determina o tamanho do campo entre os pipes e salva o que foi lido na string str e retorna o tamanho do campo
    int i;
    char ch;

    i=0;
    ch = fgetc(arq);

    while (ch != '|' && ch != EOF){
        str[i]=ch;
        i++;
        ch=fgetc(arq);
    }

    str[i] = '\0';

    if(feof(arq))
        return 0;
    else
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
    char numero[20], nome[50], curso[25], buffer[100];
    int tam_campo, num_insc;
    float nota;
    if(arq == NULL){
        arq = fopen("Dados-inline.txt", "r");
    }

    if(reg == NULL){
        reg = fopen("Arq-reg.txt", "w");
    } else{
        fclose(reg);
        reg = fopen("Arq-reg.txt", "w");
    }


    fwrite("-1", sizeof(int), 1, reg); //escreve a LED no inicio do arquivo


    fscanf(arq, "%d", &num_insc);

    tam_campo = num_insc;

    buffer[0]='\0';

    while (tam_campo > 0) {   //transcreve o arquivo dados inline para o arquivo de registro
        fseek(arq, 1, SEEK_CUR);

        sprintf(numero,"%d", num_insc);

        tam_campo = readfield(arq, nome);
        tam_campo = readfield(arq, curso);

        fscanf(arq, "%f", &nota);

        concatena(buffer, numero);
        concatena(buffer, nome);
        concatena(buffer, curso);
        sprintf(numero, "%f", nota);
        concatena(buffer, numero);

        buffer[strlen(buffer)] = '\0';

        nota=0;
        curso[0]= '\0';
        nome[0]='\0';
        numero[0]='\0';


        tam_campo = strlen(buffer);


        fwrite(tam_campo, sizeof(int), 1, reg);

        fputs(buffer, reg);

        buffer[0] = '\0';

        num_insc = 0;

        fseek(arq, 1, SEEK_CUR);

        fscanf(arq, "%d", &num_insc);
        tam_campo = num_insc;
    }

}

int busca_rem(FILE *reg, int num_insc){
    int num, pos=4, find = 0, tam;

    fseek(reg, 4, SEEK_SET);
    fscanf(reg, "%d", &tam);
    fscanf(reg, "%d", &num);

    while (find == 0 && !feof(reg)){
        if(num == num_insc){
            find = 1;
        } else{
            fseek(reg, -4, SEEK_CUR);

            fseek(reg, tam, SEEK_CUR);
            pos = pos+tam;
        }
    }

    if(find == 1){
        return pos;
    } else{
        printf("\nO candidato com o numero de inscricao %d, nao se encontra nesse arquivo", num_insc);
        return 0;
    }

}

int busca_insercao(FILE *reg, int tam){
    int led, fit, tam_campo;

    rewind(reg);

    fscanf(reg, "%d", &led);

    while(led != -1){
        fseek(reg, -4, SEEK_CUR);
        fscanf(reg, "%d", &tam_campo);

        if(tam_campo >= tam){
            fit = 1;
        } else{
            fscanf(reg, "%d", &led);
        }
    }

    if(fit)
        return led;
    else
        return 0;
}

void insercao(FILE *reg){
    int num, tam, posicao, led;
    char nome[30], curso[20], buffer[100], num_aux[15];
    float nota;

    printf("\nDigite o numero de inscricao: ");
    scanf("%d", &num);

    printf("\nDigite o nome: ");
    scanf("%s", nome);

    printf("\nDigite o nome do curso: ");
    scanf("%s", curso);

    printf("\nDigite a nota: ");
    scanf("%f", &nota);

    sprintf(num_aux, "%d", num);

    concatena(buffer, num);
    concatena(buffer, curso);

    sprintf(num_aux,"%f", nota);
    concatena(buffer, num_aux);

    tam = strlen(buffer);

    posicao = busca_insercao(reg,tam);

    if(posicao == 0){
        fseek(reg, 0, SEEK_END);
        fwrite(tam, sizeof(int), 1, reg);
        fputs(buffer, reg);
    }else {
        rewind(reg);
        fscanf(reg, "%d", &led);

        while (led != -1 || led != posicao) {
            fseek(reg, led, SEEK_SET);

        }
    }
}


void remocao(FILE *reg){
    int num_insc, pos, tam, led, anterior;

    printf("\nDigite o numero de inscricao do candidato que deseja remover: ");
    scanf("%d", &num_insc);

    pos = busca_rem(reg, num_insc);

    rewind(reg);
    fscanf(reg, "%d", &led);

    while (led != -1){
        anterior = led;
        fseek(reg, led, SEEK_SET);
        fscanf(reg, "%d", &led);
    }

    fseek(reg, led, SEEK_SET);
    fwrite("-1", sizeof(int), 1, reg);

    fseek(reg, anterior, SEEK_SET);
    fwrite(led, sizeof(int), 1, reg);
}
