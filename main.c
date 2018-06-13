#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio_ext.h>

#define pipe "|"

void concatena(char *primeiro, char *segundo);
void menu(int opcao, FILE *arq, FILE **reg);
void importacao(FILE *arq, FILE *reg);
void insercao(FILE *reg);
void remocao(FILE *reg);
int readfield(FILE *arq, char str[]);
int busca_rem(FILE *reg, char *num_insc);

int main(){
    long pos;
    char ch;
    int opcao;
    FILE *arq, *reg;

    printf("===Menu==\nEscolha a opcao:\n1-Importacao\n2-Insercao\n3-Remocao\n0-Sair\n=> ");
    scanf("%i", &opcao);
    fflush(stdin);

    arq = fopen("Dados-inline.txt", "r");
    reg = fopen("Arq-reg.txt", "w");

    menu(opcao, arq, &reg); //passa a opção escolhida e dois ponteiros para o arquivo original dados inline e para o arquivo de registro

    fclose(arq);
    fclose(reg);
}

void menu(int opcao, FILE *arq, FILE **reg){
  while(opcao != 0){
    switch (opcao) {
      case 1:
        importacao(arq, *reg);
        *reg = fopen("Arq-reg.txt", "rb+");
        break;
      case 2:
        insercao(*reg);
        break;
      case 3:
        remocao(*reg);
        break;
      default:
        fclose(arq);
        fclose(*reg);
        break;
    }
    printf("===Menu==\nEscolha a opcao:\n1-Importacao\n2-Insercao\n3-Remocao\n0-Sair\n=> ");
    scanf("%d", &opcao);
    fflush(stdin);
  }
}

int readfield(FILE *arq, char str[]){  //le o arquivo dados inline e determina o tamanho do campo salva o que foi lido na string str e retorna o tamanho do campo
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

void concatena(char *primeiro, char *segundo){      //concatena duas strings e coloca um pipe no final
    strcat(primeiro, segundo);
    strcat(primeiro, pipe);
}

void importacao(FILE *arq, FILE *reg){      //função de importação, pega do arquivo dados-inline e joga no arq-reg no formato pra leitura
    char numero[20], nome[50], curso[25], buffer[100];
    int tam_campo, num_insc, led = -1;
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


    fwrite(&led, sizeof(int), 1, reg); //escreve a LED no inicio do arquivo


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

        sprintf(numero, "%f", nota);    //converte o float pra string

        concatena(buffer, numero);

        buffer[strlen(buffer)] = '\0';  //zera o buffer

        nota=0;             //zera as variaveis
        curso[0]= '\0';
        nome[0]='\0';
        numero[0]='\0';     //pra colocar no arquivo reg como numero legivel


        tam_campo = strlen(buffer);     //pega o tamanho do campo


        fwrite(&tam_campo, sizeof(int), 1, reg);     //escreve o tamanho do campo no arquivo de registro

        fputs(buffer, reg);     //escreve o buffer

        buffer[0] = '\0';       //zera o buffer

        num_insc = 0;

        fseek(arq, 1, SEEK_CUR);        //precisa por que se nao pega o pipe

        fscanf(arq, "%d", &num_insc);
        tam_campo = num_insc;
    }
    fclose(reg);
    printf("\n\n\n\n\n\n\n\n\n\n");

}

int read_rec(FILE * reg, char * buffer){
    int tam;

    if (fread(&tam, sizeof(int), 1, reg) == 0) {
        return 0;
    }
    fread(buffer, tam, 1, reg);
    buffer[tam] = '\0';
    return tam;
}

int busca_rem(FILE *reg, char *num_insc){ //função de busca para remover um registro
    int  pos=4, find = 0, tam, i=0;
    char ch, num_str[10], num[20];
    char buffer[250];
    char *field;

    fseek(reg, 4, SEEK_SET);    //faz o seek pra 4 posicao por causa que os primeiros 4 bytes são da led

    while (find == 0 && (tam = read_rec(reg, buffer) > 0)){
        printf("\nEntrou while");
        field = strtok(buffer, pipe);


        if (strcmp(field, num_insc) == 0){
            find = 1;
        } else{
            fseek(reg, -sizeof(field), SEEK_CUR);
            fseek(reg, tam, SEEK_CUR);
            pos = pos+tam;
            tam = read_rec(reg, buffer);
        }
        printf("\nTa no while busca rem");
    }

//    fread(&tam, sizeof(int), 1, reg);
//
//    ch = fgetc(reg);
//
//    while(ch != '|'){  //como o num de insc está no formato de string, tem que fazer isso p/ converter p/ int
//        num_str[i]=ch;
//        i++;
//        ch = fgetc(reg);
//    }
//
//    num = atoi(num_str);
//
//
//    while (find == 0 && !feof(reg)){    //enquanto nao achar o numero de inscrição certo e nao chegar no fim do arquivo repete
//        i=0;
//        if(num == num_insc){
//            find = 1;
//        } else{
//            fseek(reg, -sizeof(num_str), SEEK_CUR);   //tem que fazer seek de -4 pra dar certo o seek, se não, ele vai estar 4 posicoes pra frente
//
//            fseek(reg, tam, SEEK_CUR);
//
//            pos = pos+tam;      //incrementa a posição com o tamanho do campo
//            fscanf(reg, "%d", &tam);
//
//            ch = fgetc(reg);
//
//            while(ch != '|'){
//                num_str[i]=ch;
//                i++;
//                ch = fgetc(reg);
//            }
//
//            num = atoi(num_str);
//        }
//    }

    if(find == 1){
        printf("\nAchou pos: %d", pos);
        return pos;
    } else{
        printf("\nO candidato com o numero de inscricao %s, nao se encontra nesse arquivo", num_insc);
        return 0;
    }

}

int busca_insercao(FILE *reg, int tam){     //faz a busca na led pra achar um espaço para inserir no arquivo reg
    int led, fit, tam_campo;

    printf("\nEntrou busca insercao");
    rewind(reg);    //volta p/ o inicio do arquivo reg



    fscanf(reg, "%d", &led);    //pega o primeiro byteoffset da led

    while(led != -1){       //enquanto não for o final da led
        fseek(reg, led, SEEK_SET);  //faz o seek pro byteoffset da led
        fscanf(reg, "%d", &tam_campo);  //pega o tamanho do campo

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

void insercao(FILE *reg){       //função de inserção de registros no arquivo reg
    int tam, posicao=0, led, anterior, num;
    char nome[30], curso[30], buffer[200], num_aux[8];
    float nota;

    nome[0]='\0';
    curso[0]='\0';
    buffer[0]='\0';
    num_aux[0]='\0';

    __fpurge(stdin);
    printf("\nDigite o numero de inscricao: ");
    scanf("%d", &num);
    snprintf(num_aux,8 , "%d", num);    //converte o numero de inscricao de int para string

    printf("\nDigite o nome: ");
    __fpurge(stdin);
    fgets(nome, 30, stdin);


    printf("\nDigite o nome do curso: ");
    fgets(curso, 30, stdin);
    __fpurge(stdin);

    printf("\nDigite a nota: ");
    scanf("%f", &nota);
    fflush(stdin);

    concatena(buffer, num_aux);

    concatena(buffer, curso);

    snprintf(num_aux,8 ,"%.3f", nota);    //converte a nota de float p/ string

    concatena(buffer, num_aux);

    tam = strlen(buffer);

    posicao = busca_insercao(reg,tam);


    if(posicao == 0){       //se não tem lugar disponivel na led, coloca o novo registro no final
        fseek(reg, 0, SEEK_END);
        fwrite(&tam, sizeof(int), 1, reg);
        fputs(buffer, reg);
    }else {
        rewind(reg);
        fscanf(reg, "%d", &led);    //arrumando a led

        while (led != -1 || led != posicao) {   //enquanto nao chegou o final da led e nao chegou na posicao faz o seek p/ o byteoffset da led e salva o anterior
            fseek(reg, led, SEEK_SET);
            anterior = led;
            fseek(reg, 4, SEEK_CUR);
            fscanf(reg, "%d", &led);
        }

        fseek(reg, led, SEEK_SET);
        fscanf(reg, "%d", &led);
        fseek(reg, -4, SEEK_CUR);   //tem que voltar 4 bytes pois tem que colocar o tamanho do campo

        fwrite(&tam, sizeof(int), 1, reg);
        fputs(buffer, reg);

        fseek(reg, led+4, SEEK_SET);       //seek de led + 4 pra arrumar o -4 anterior
        fscanf(reg, "%d", &led);
        fwrite(&anterior, sizeof(int), 1, reg);
        anterior = led;

        while (led != -1){      //arruma o resto da led ate o final
            fseek(reg, led, SEEK_SET);
            fscanf(reg, "%d", &led);
            fwrite(&anterior, sizeof(int), 1, reg);
            anterior = led;
        }
    }
}


void remocao(FILE *reg){
    int pos, tam, led, anterior;
    char num_insc[20];

    __fpurge(stdin);

    num_insc[0]='\0';

    printf("\nDigite o numero de inscricao do candidato que deseja remover: ");
    scanf("%s", num_insc);

    //fgets(num_insc, 20, stdin);

    pos = busca_rem(reg, num_insc);     //pega o byteoffset do registro

    if(pos != 0) {
        rewind(reg);
        fscanf(reg, "%d", &led);

        if (led == -1) {        //tratamento da primeira remoção
            fwrite(&pos, sizeof(int), 1, reg);   //escreve o byteoffset do registro removido
            fseek(reg, pos, SEEK_SET);   //faz o seek p/ o byteoffset do registro e coloca -1 na led
            fwrite("-1", sizeof(int), 1, reg);
        } else {
            while (led != -1) {
                anterior = led;
                fseek(reg, led, SEEK_SET);
                fscanf(reg, "%d", &led);
            }

            fseek(reg, led, SEEK_SET);
            fwrite("-1", sizeof(int), 1, reg);

            fseek(reg, anterior, SEEK_SET);
            fwrite(&led, sizeof(int), 1, reg);
        }
    }
}
