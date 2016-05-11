/*

*/

//Inicialização das bibliotecas
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


//Declaração das Structs e outras variaveis globais
typedef struct
{
    int ID, like_count, share_count;
    long views_count;
    char text[30], user[30], coordinates[30], language[30];
}grupo;

grupo post;



typedef struct
{
    char userSec[30];
    int posicSec;
}vetSec;

vetSec vetIndiceSec[1000];



typedef struct
{
    int chaveID;
    int posicReg;
}vet;

vet vetIndice[1000];


FILE *OUT;
FILE *INDICE;
FILE *INDICE_SEC;
FILE *LIST_INVERT;


int contIndice = 0;
int contIndiceSec = 0;





//Inicialização das funções que serão utilizadas
void pesquisaChSecundaria();
void pesquisaChPrimaria();
void swapbubble2(vetSec v[], int i);
void swapbubble(vet v[], int i);
void bubble2(vetSec v[], int qtd);
void bubble(vet v[], int qtd);
void refazerIndiceSecundario();
void refazerIndice();
void compactar();
void alterar();
void inseriravancado();
void remover();
void escritor();
void pesquisar();
void inserir();
void menu();

////////////////////////////////////////////////////////////////////////////////
//Programa Principal
int main()
{
    int inicial, cont, cont1, comparar;
    char auxNome[30], fim;

    //abertura dos arquivos
    OUT = fopen("projeto.dad","r+b");
    if (!OUT) {
        printf("Arquivo inexistente");
        printf("\nCriando arquivo..."); getchar();
        OUT = fopen("projeto.dad","w+b");
        inicial = -1;
        fwrite(&inicial, sizeof(int), 1, OUT); fprintf(OUT, "#");
    }

    INDICE = fopen("indice.dad","r+b");
    if(!INDICE) {
        INDICE = fopen("indice.dad","w+b");
        inicial = 2; //2 = indice desatualizado
        fwrite(&inicial, sizeof(int), 1, INDICE); fprintf(INDICE, "#");
    }

    INDICE_SEC = fopen("indice_sec.dad","r+b");
    if(!INDICE_SEC) {
        INDICE_SEC = fopen("indice_sec.dad", "w+b");
        inicial = 2; //2 = indice desatualizado
        fwrite(&inicial, sizeof(int), 1, INDICE_SEC); fprintf(INDICE_SEC, "#");
    }

    LIST_INVERT = fopen("lista_invertida.dad","r+b");
    if(!LIST_INVERT) {
        LIST_INVERT = fopen("lista_invertida.dad","w+b");
    }


    //--------------------

    //"Transferir" dados do Arquivo de dados para o Vetor(Memoria Principal) - Indice Primario
    rewind(INDICE);
    fread(&inicial, sizeof(int), 1, INDICE);
    if (inicial == 2) {
        refazerIndice();  //Refazer Indice caso o programa tenha sido desligado inesperadamente
    }
    if (inicial == 1) {
        fseek(INDICE, 5, 0);
        fread(&cont, sizeof(int), 1, INDICE);
        fread(&cont1, sizeof(int), 1, INDICE);
        while(cont != 0)
        {
            vetIndice[contIndice].chaveID = cont;
            vetIndice[contIndice].posicReg = cont1;
            contIndice++;
            fread(&cont, sizeof(int), 1, INDICE);
            fread(&cont1, sizeof(int), 1, INDICE);
        }
        rewind(INDICE);
        inicial = 2;
        fwrite(&inicial, sizeof(int), 1, INDICE); //marcando arquivo de indice como desatualizado
    }
    //-------------------

    //"Transferir" dados do Arquivo de dados para o Vetor(Memoria Principal) - Indice Secundario
    rewind(INDICE_SEC);
    fread(&inicial, sizeof(int), 1, INDICE_SEC);
    if (inicial == 2) {
        refazerIndiceSecundario(); // Refazer Indice Secundario caso o programa tenha sido desligado inesperadamente
    }
    if (inicial == 1) {
        fseek(INDICE_SEC, 5, 0);
        fim = getc(INDICE_SEC);
        fseek(INDICE_SEC, -1, 1);
        fread(&auxNome, sizeof(auxNome), 1, INDICE_SEC);
        fread(&cont, sizeof(int), 1, INDICE_SEC);
        comparar = strcmp(auxNome, "fim");
        while (fim != '.')
        {
            strcpy(vetIndiceSec[contIndiceSec].userSec, auxNome);
            vetIndiceSec[contIndiceSec].posicSec = cont;
            contIndiceSec++;
            fim = getc(INDICE_SEC);
            fseek(INDICE_SEC, -1, 1);
            fread(&auxNome, sizeof(auxNome), 1, INDICE_SEC);
            fread(&cont, sizeof(int), 1, INDICE_SEC);
        }
        rewind(INDICE_SEC);
        inicial = 2;
        fwrite(&inicial, sizeof(int), 1, INDICE_SEC); //marcando arquivo de indice secundario como desatualizado
    }
    //-------------------                


    //abertura do menu           
    menu();
    //-------------------


    //salvar vetor de indice no arquivo e fechamento dos mesmos - Indice Primario
    bubble(vetIndice, contIndice);
    fclose(INDICE);
    remove("indice.dad");
    INDICE = fopen("indice.dad", "w+b");
    rewind(INDICE);
    inicial = 1; //1 = indice atualizado
    fwrite(&inicial, sizeof(int), 1, INDICE); fprintf(INDICE,"#");
    for(int i = 0; i < contIndice + 1; i++) {
        if(vetIndice[i].chaveID != 0) {
            fwrite(&vetIndice[i].chaveID, sizeof(int), 1, INDICE);
        }
        if(vetIndice[i].posicReg != 0) {
            fwrite(&vetIndice[i].posicReg, sizeof(int), 1, INDICE);
        }
    }
    inicial = 0;
    fwrite(&inicial, sizeof(int), 1, INDICE);
    //--------------------

    //salvar vetor de indice no arquivo e fechamento dos mesmos - Indice Secundario
    bubble2(vetIndiceSec, contIndiceSec);
    fclose(INDICE_SEC);
    remove("indice_sec.dad");
    INDICE_SEC = fopen("indice_sec.dad", "w+b");
    rewind(INDICE_SEC);
    inicial = 1; //1 = indice atualizado
    fwrite(&inicial, sizeof(int), 1, INDICE_SEC); fprintf(INDICE_SEC, "#");
    for(int i = 0; i < contIndiceSec; i++) {
        fwrite(&vetIndiceSec[i].userSec, sizeof(vetIndiceSec[i].userSec), 1, INDICE_SEC);
        fwrite(&vetIndiceSec[i].posicSec, sizeof(int), 1, INDICE_SEC);
    }
    fim = '.';
    putc(fim, INDICE_SEC);
    //--------------------

    fclose(INDICE_SEC);
    fclose(LIST_INVERT);
    fclose(INDICE);
    fclose(OUT);
    getchar();
    return 0;
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função de pesquisa em chave secundaria
void pesquisaChSecundaria()
{
    int i, j, offset, comparador, ID;
    int k=0;
    char pesquisa[30], carac, numstr[10];

    bubble2(vetIndiceSec, contIndiceSec);
    system("clear");
    printf("\n         Pesquisa (chave secundaria)                   ");

    printf("\n\nInsira o usuario para pesquisa: "); fgets(pesquisa, 30, stdin);

    for(i = 0; i < contIndiceSec +1; i++){
        comparador = strcmp(pesquisa, vetIndiceSec[i].userSec);
        if(comparador == 0) {
            break;
        }
    }

    if(comparador != 0) {
        printf("\n\nPesquisa sem resultados.");
    }

    if(comparador == 0) {
        offset = vetIndiceSec[i].posicSec;
        do
        {
            fseek(LIST_INVERT, offset, 0);
            fread(&ID, sizeof(int), 1, LIST_INVERT);

            for(j = 0; j < contIndice+1; j++){
                if(ID == vetIndice[j].chaveID){
                    break;
                }
            }

            //Escrita do Registro na tela
            if(ID == vetIndice[j].chaveID){
                k=1;
                fseek(OUT, vetIndice[j].posicReg, 0);
                printf("\n\nID: "); escritor();
                printf("\nUser: "); escritor();
                printf("\nText: "); escritor();
                printf("\nLinguagem: "); escritor();
                printf("\nCoordenadas: "); escritor();
                printf("\nLike: "); escritor();
                printf("\nShare: "); escritor();
                printf("\nView: "); escritor();

            }

            fread(&offset, sizeof(int), 1, LIST_INVERT);
        }while(offset != -1);

        if(k == 0) {
            printf("\n\nPesquisa sem resultados.");
        }

    }
    getchar();
}
////////////////////////////////////////////////////////////////////////////////      





////////////////////////////////////////////////////////////////////////////////
//Função de pesquisa em chave primaria
void pesquisaChPrimaria()
{
    int pesquisa, i, offset;
    char carac, numstr[10];

    bubble(vetIndice, contIndice);
    system("clear");
    printf("\n           Pesquisa (chave primaria)                   ");

    printf("\n\nInsira o ID para pesquisa: "); scanf("%d", &pesquisa);

    for(i = 0; i < contIndice+1; i++){
        if(pesquisa == vetIndice[i].chaveID){
            break;
        }
    }

    if(i == (contIndice + 1)) {
        printf("\n\nRegistro Inexistente.");
    }

    //Escrita do Registro na tela
    if(pesquisa == vetIndice[i].chaveID){
        fseek(OUT, vetIndice[i].posicReg, 0);
        printf("\n\nID: "); escritor();
        printf("\nUser: "); escritor();
        printf("\nText: "); escritor();
        printf("\nLinguagem: "); escritor();
        printf("\nCoordenadas: "); escritor();
        printf("\nLike: "); escritor();
        printf("\nShare: "); escritor();
        printf("\nView: "); escritor();

    }

    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Funções para ordenação - codigo retirado de http://pt.wikipedia.org/wiki/Bubble_sort
//ordenação Indice primario

char *itoa(long i, char* s, int dummy_radix) {
    sprintf(s, "%ld", i);
    return s;
}

void swapbubble(vet v[], int i)
{
    vet aux;

    aux = v[i];
    v[i] = v[i+1];
    v[i+1] = aux;
}

void bubble(vet v[], int qtd)
{
    int i;
    int trocou;

    do
    {
        qtd--;
        trocou = 0;

        for(i = 0; i < qtd; i++)
            if(v[i].chaveID > v[i + 1].chaveID)
            {
                swapbubble(v, i);
                trocou = 1;

            }
    }while(trocou);
}

//ordenação Indice secundario
void swapbubble2(vetSec v[], int i)
{
    vetSec aux;

    aux = v[i];
    v[i] = v[i+1];
    v[i+1] = aux;
}

void bubble2(vetSec v[], int qtd)
{
    int i, comp;
    int trocou;

    do
    {
        qtd--;
        trocou = 0;

        for(i = 0; i < qtd; i++) {
            comp = strcmp(v[i].userSec, v[i + 1].userSec);
            if(comp > 0)
            {
                swapbubble2(v, i);
                trocou = 1;

            }
        }
    }while(trocou);
}

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função para refazer o indice secundário.
void refazerIndiceSecundario()
{
    int tam, i, j, offset, offset2, ID, comparador, valor;
    char carac, numstr[10], auxStr[30];

    fclose(LIST_INVERT);
    remove("lista_invertida.dad");
    LIST_INVERT = fopen("lista_invertida.dad", "w+b");

    fseek(OUT, 5, 0);
    tam = getc(OUT);
    carac = getc(OUT);
    while(carac != EOF)
    {
        memset(numstr, '\0', sizeof(numstr));
        memset(auxStr, '\0', sizeof(auxStr));

        i=0;
        if((tam > 5) && (carac != '*'))
        {
            while(carac != '#')
            {
                numstr[i] = carac;
                i++;
                carac = getc(OUT);
            }
            ID = atoi(numstr); //armazeno o ID que será colocado na lista invertida
            j = i;

            i=0;
            carac = getc(OUT);
            while(carac != '#')
            {
                auxStr[i] = carac; //armazeno o dado de chave secundaria
                i++;
                carac = getc(OUT);
            }
            j = j + i;

            for(int k = 0; k < contIndiceSec + 1; k++) { //verifico se a chave já existe
                comparador = strcmp(auxStr, vetIndiceSec[k].userSec);
                if (comparador == 0) {
                    offset = vetIndiceSec[k].posicSec;
                    break;
                }
            }

            if (comparador == 0) {
                fseek(LIST_INVERT, 0, 2);
                offset2 = ftell(LIST_INVERT);
                fwrite(&ID, sizeof(int), 1, LIST_INVERT);
                valor = -1;
                fwrite(&valor, sizeof(int), 1, LIST_INVERT);

                fseek(LIST_INVERT, offset, 0);
                fseek(LIST_INVERT, sizeof(int), 1);
                fread(&offset, sizeof(int), 1, LIST_INVERT);
                while(offset != -1)
                {
                    fseek(LIST_INVERT, offset, 0);
                    fseek(LIST_INVERT, sizeof(int), 1);
                    fread(&offset, sizeof(int), 1, LIST_INVERT);
                }
                fseek(LIST_INVERT, -sizeof(int), 1);
                fwrite(&offset2, sizeof(int), 1, LIST_INVERT);
            } else {
                fseek(LIST_INVERT, 0, 2);
                offset2 = ftell(LIST_INVERT);
                fwrite(&ID, sizeof(int), 1, LIST_INVERT);
                valor = -1;
                fwrite(&valor, sizeof(int), 1, LIST_INVERT);
                strcpy(vetIndiceSec[contIndiceSec].userSec, auxStr);
                vetIndiceSec[contIndiceSec].posicSec = offset2;
                contIndiceSec++;
            }

            fseek(OUT, (tam-(j+2)), 1);
        } else {
            fseek(OUT, (tam - 1), 1);
        }



        tam = getc(OUT);
        carac = getc(OUT);
    }
}







////////////////////////////////////////////////////////////////////////////////     





////////////////////////////////////////////////////////////////////////////////
//Função para refazer o indice primário.
void refazerIndice()
{
    int tam, i, offset;
    char carac, numstr[10];

    fseek(OUT, 5, 0);
    tam = getc(OUT);
    offset = ftell(OUT);
    carac = getc(OUT);
    while(carac != EOF)
    {
        i=0;
        if((tam > 5) && (carac != '*'))
        {
            while(carac != '#')
            {
                numstr[i] = carac;
                i++;
                carac = getc(OUT);
            }

            vetIndice[contIndice].chaveID = atoi(numstr);
            vetIndice[contIndice].posicReg = offset;
            contIndice ++;
        }
        memset(numstr, '\0', sizeof(numstr));

        fseek(OUT, (tam-(i+1)), 1);
        tam = getc(OUT);
        offset = ftell(OUT);
        carac = getc(OUT);
    }
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função para compactar o arquivo
void compactar()
{
    int inicial, tamanho;
    char carac, copiador[100];

    //abrir arquivo auxiliar
    FILE *auxiliar = fopen("auxiliar.dad", "w+b");
    inicial = -1;
    fwrite(&inicial, sizeof(int), 1, auxiliar); fprintf(auxiliar, "#");
    //--------------------

    system("clear");
    fseek(OUT, 5, 0);
    tamanho = getc(OUT);
    carac = getc(OUT);

    while(carac != EOF)
    {
        if ((tamanho < 6) || (carac == '*')) {
            fseek(OUT, (tamanho-1), 1);
            tamanho = getc(OUT);
            carac = getc(OUT);
        } else {
            fseek(OUT, -2, 1);
            fread(&copiador, tamanho+1, 1, OUT);
            fwrite(&copiador, tamanho+1, 1, auxiliar);
            tamanho = getc(OUT);
            carac = getc(OUT);
        }
    }

    fclose(OUT);
    fclose(auxiliar);
    remove("projeto.dad");
    inicial = rename("auxiliar.dad", "projeto.dad");
    OUT = fopen("projeto.dad", "r+b");

    printf("\n\nCompactacao efetuada com sucesso.");

    memset(vetIndice, '\0', sizeof(vetIndice)); //Reorganizando o indice primario
    contIndice = 0;
    refazerIndice();

    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função para alteração dos dados
void alterar()
{
    int alteraID1, alteraID2, tamRegistro, tamRegistro2, tamanho, i, offset, offset2, posic1, posic2;
    char carac, numstr[10];

    tamanho = 0;

    system("clear");
    printf("          Gerenciamento da Disciplina          ");
    printf("\n                   Alteracao                ");

    printf("\n\nInsira o ID para alteracao: "); fgets(numstr, 10, stdin); alteraID1 = atoi(numstr);
    post.ID = alteraID1;

    //Pesquisa do ID
    fseek(OUT, 5, 0);
    alteraID2 = 0;
    tamRegistro = getc(OUT);
    while((alteraID1 != alteraID2) && (tamRegistro != EOF))
    {
        memset(numstr, '\0', sizeof(numstr));
        tamRegistro2 = tamRegistro;
        i=0;
        carac = getc(OUT);
        while(carac != '#')
        {
            numstr[i] = carac;
            i++;
            carac = getc(OUT);
        }
        alteraID2 = atoi(numstr);
        fseek(OUT, (tamRegistro-(i+1)), 1);
        tamRegistro = getc(OUT);
    }
    //--------------------

    //Escrita referente aos dados obtidos
    if ((alteraID1 != alteraID2) && (tamRegistro == -1)) {
        printf("\nID inexistente.");
    } else {
        if ((alteraID1 == alteraID2) && (tamRegistro == -1)) {
            fseek(OUT, -(tamRegistro2), 1);
        } else {
            fseek(OUT, -(tamRegistro2+1), 1);
        }

        posic1 = ftell(OUT) - 1;
        printf("\n\nID: "); escritor();
        printf("\nUser: "); escritor();
        printf("\nText: "); escritor();
        printf("\nLinguagem: "); escritor();
        printf("\nCoordenadas: "); escritor();
        printf("\nLike: "); escritor();
        printf("\nShare: "); escritor();
        printf("\nView: "); escritor();

    }
    posic2 = ftell(OUT);
    //--------------------

    //Se existir registro, solicitar novos dados
    if ((alteraID1 != alteraID2) && (tamRegistro == -1)) {
        printf("\nRefaca o procedimento.");
    } else {
        printf("\n\nReescreve todos os seguintes campos, efetuando as alteracoes que desejar:");
        printf("\nUser: "); fgets(post.user, 30, stdin);
        printf("\nText: "); fgets(post.text, 30, stdin);
        printf("\nCoordenadas: "); fgets(post.coordinates, 30, stdin);
        printf("\nLinguagem: "); fgets(post.language, 30, stdin);
        printf("\n\nID: "); fgets(numstr, 10, stdin); post.like_count = atoi(numstr);
        printf("\n\nID: "); fgets(numstr, 10, stdin); post.share_count = atoi(numstr);
        printf("\n\nID: "); fgets(numstr, 10, stdin); post.views_count = atoi(numstr);

        //Calcular o tamanho do registro com os novos dados
        itoa(post.ID,numstr,10); tamanho = tamanho + strlen(numstr) + 1;
        tamanho = tamanho + strlen(post.user) + 1;
        tamanho = tamanho + strlen(post.text) + 1;
        tamanho = tamanho + strlen(post.coordinates) + 1;
        tamanho = tamanho + strlen(post.language) + 1;
        itoa(post.like_count,numstr,10); tamanho + strlen(numstr) + 1;
        itoa(post.share_count,numstr,10); tamanho + strlen(numstr) + 1;
        itoa(post.views_count,numstr,10); tamanho + strlen(numstr) + 1;

        //Tentamos inserir. Se o novo registro for maior que o anterior, remover e inserir novamente
        fseek(OUT, -(posic2 - posic1), 1);

        if (tamanho > tamRegistro2) {
            //remover
            //ver posição do registro e inserir o final da lista(-1)
            offset = ftell(OUT);
            putc(tamRegistro2, OUT);
            fprintf(OUT, "*");
            offset2 = -1;
            fwrite(&offset2, sizeof(int), 1, OUT);

            //procurar o ultimo registro vazio e indicar qual será o proximo vazio
            rewind(OUT);
            fread(&offset2, sizeof(int), 1, OUT);
            while (offset2 != -1)
            {
                fseek(OUT, (offset2+2), 0);
                fread(&offset2, sizeof(int), 1, OUT);
            }
            fseek(OUT, -4, 1);
            fwrite(&offset, sizeof(int), 1, OUT);
            //--------------------

            //inserir
            fseek(OUT, 0, 2);
            putc(tamanho, OUT);
            //alterar registro no arquivo de indice primario
            i = 0;
            while(alteraID1 != vetIndice[i].chaveID)
            {
                i++;
            }
            vetIndice[i].posicReg = ftell(OUT);
            fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);
            //--------------------

        } else {
            putc(tamRegistro2, OUT);
            fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);
            /*
               Neste caso, irá ocorrer problema de fragmentação interna.
               A fragmentação interna não será retirada pela compactação.
            */
        }
    }

    memset(vetIndiceSec, '\0', sizeof(vetIndiceSec));
    contIndiceSec = 0;
    refazerIndiceSecundario();

    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função de Inserção Avançada de posts
void inserirAvancado()
{
    char simNao, numstr[10];
    int tamanho1=0, tamanho2, offset, offset2, offset3, offset4, comparador, valor;

    system("clear");
    printf("\n               Insercao Avancada                ");


    //Inserção dos dados na estrutura
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.ID = atoi(numstr);
    printf("User: "); fgets(post.user, 30, stdin);
    printf("Text: "); fgets(post.text, 30, stdin);
    printf("Coordenadas: "); fgets(post.coordinates, 30, stdin);
    printf("\nLinguagem: "); fgets(post.language, 30, stdin);
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.like_count = atoi(numstr);
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.share_count = atoi(numstr);
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.views_count = atoi(numstr);

    //Verificar tamanho do novo registro
    itoa(post.ID,numstr,10); tamanho1 = tamanho1 + strlen(numstr) + 1;
    tamanho1 = tamanho1 + strlen(post.user) + 1;
    tamanho1 = tamanho1 + strlen(post.text) + 1;
    tamanho1 = tamanho1 + strlen(post.coordinates) + 1;
    tamanho1 = tamanho1 + strlen(post.language) + 1;
    itoa(post.like_count,numstr,10); tamanho1 + strlen(numstr) + 1;
    itoa(post.share_count,numstr,10); tamanho1 + strlen(numstr) + 1;
    itoa(post.views_count,numstr,10); tamanho1 + strlen(numstr) + 1;

    //Inserção do novo Registro
    rewind(OUT);
    offset2 = ftell(OUT);
    fread(&offset, sizeof(int), 1, OUT);
    if (offset == -1) {
        fseek(OUT, 0, 2);
        putc(tamanho1, OUT);
        //salvando registro no arquivo de indice primario
        vetIndice[contIndice].chaveID = post.ID;
        vetIndice[contIndice].posicReg = ftell(OUT);
        contIndice ++;
        //--------------------

        //salvando registro no arquivo de indice secundario
        for(int k = 0; k < contIndiceSec + 1; k++) { //verifico se a chave já existe
            comparador = strcmp(post.user, vetIndiceSec[k].userSec);
            if (comparador == 0) {
                offset3 = vetIndiceSec[k].posicSec;
                break;
            }
        }

        if (comparador == 0) {
            fseek(LIST_INVERT, 0, 2);
            offset4 = ftell(LIST_INVERT);
            fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
            valor = -1;
            fwrite(&valor, sizeof(int), 1, LIST_INVERT);

            fseek(LIST_INVERT, offset3, 0);
            fseek(LIST_INVERT, sizeof(int), 1);
            fread(&offset3, sizeof(int), 1, LIST_INVERT);
            while(offset3 != -1)
            {
                fseek(LIST_INVERT, offset, 0);
                fseek(LIST_INVERT, sizeof(int), 1);
                fread(&offset3, sizeof(int), 1, LIST_INVERT);
            }
            fseek(LIST_INVERT, -sizeof(int), 1);
            fwrite(&offset4, sizeof(int), 1, LIST_INVERT);
        } else {
            fseek(LIST_INVERT, 0, 2);
            offset4 = ftell(LIST_INVERT);
            fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
            valor = -1;
            fwrite(&valor, sizeof(int), 1, LIST_INVERT);
            strcpy(vetIndiceSec[contIndiceSec].userSec, post.user);
            vetIndiceSec[contIndiceSec].posicSec = offset4;
            contIndiceSec++;
        }
        //--------------------

        fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);
    } else {
        fseek(OUT, offset, 0);
        tamanho2 = getc(OUT);
        while((tamanho2 < tamanho1) && (offset != -1))
        {
            fseek(OUT, 1, 1);
            offset2 = ftell(OUT);
            fread(&offset, sizeof(int), 1, OUT);
            fseek(OUT, offset, 0);
            tamanho2 = getc(OUT);
        }


        if ((offset == -1) && (tamanho2 < tamanho1)) {
            fseek(OUT, 0, 2);
            putc(tamanho1, OUT);
            //salvando registro no arquivo de indice primario
            vetIndice[contIndice].chaveID = post.ID;
            vetIndice[contIndice].posicReg = ftell(OUT);
            contIndice ++;
            //--------------------

            //salvando registro no arquivo de indice secundario
            for(int k = 0; k < contIndiceSec + 1; k++) { //verifico se a chave já existe
                comparador = strcmp(post.user, vetIndiceSec[k].userSec);
                if (comparador == 0) {
                    offset3 = vetIndiceSec[k].posicSec;
                    break;
                }
            }

            if (comparador == 0) {
                fseek(LIST_INVERT, 0, 2);
                offset4 = ftell(LIST_INVERT);
                fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
                valor = -1;
                fwrite(&valor, sizeof(int), 1, LIST_INVERT);

                fseek(LIST_INVERT, offset, 0);
                fseek(LIST_INVERT, sizeof(int), 1);
                fread(&offset3, sizeof(int), 1, LIST_INVERT);
                while(offset3 != -1)
                {
                    fseek(LIST_INVERT, offset3, 0);
                    fseek(LIST_INVERT, sizeof(int), 1);
                    fread(&offset3, sizeof(int), 1, LIST_INVERT);
                }
                fseek(LIST_INVERT, -sizeof(int), 1);
                fwrite(&offset4, sizeof(int), 1, LIST_INVERT);
            } else {
                fseek(LIST_INVERT, 0, 2);
                offset4 = ftell(LIST_INVERT);
                fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
                valor = -1;
                fwrite(&valor, sizeof(int), 1, LIST_INVERT);
                strcpy(vetIndiceSec[contIndiceSec].userSec, post.user);
                vetIndiceSec[contIndiceSec].posicSec = offset4;
                contIndiceSec++;
            }
            //--------------------

            fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);
        } else {
            if (tamanho2 == tamanho1) {
                fseek(OUT, 1, 1);
                fread(&offset, sizeof(int), 1, OUT);
                fseek(OUT, -(sizeof(int)+2), 1);
                putc(tamanho1, OUT);
                //salvando registro no arquivo de indice primario
                vetIndice[contIndice].chaveID = post.ID;
                vetIndice[contIndice].posicReg = ftell(OUT);
                contIndice ++;
                //--------------------

                //salvando registro no arquivo de indice secundario
                for(int k = 0; k < contIndiceSec + 1; k++) { //verifico se a chave já existe
                    comparador = strcmp(post.user, vetIndiceSec[k].userSec);
                    if (comparador == 0) {
                        offset3 = vetIndiceSec[k].posicSec;
                        break;
                    }
                }

                if (comparador == 0) {
                    fseek(LIST_INVERT, 0, 2);
                    offset4 = ftell(LIST_INVERT);
                    fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
                    valor = -1;
                    fwrite(&valor, sizeof(int), 1, LIST_INVERT);

                    fseek(LIST_INVERT, offset, 0);
                    fseek(LIST_INVERT, sizeof(int), 1);
                    fread(&offset3, sizeof(int), 1, LIST_INVERT);
                    while(offset3 != -1)
                    {
                        fseek(LIST_INVERT, offset3, 0);
                        fseek(LIST_INVERT, sizeof(int), 1);
                        fread(&offset3, sizeof(int), 1, LIST_INVERT);
                    }
                    fseek(LIST_INVERT, -sizeof(int), 1);
                    fwrite(&offset4, sizeof(int), 1, LIST_INVERT);
                } else {
                    fseek(LIST_INVERT, 0, 2);
                    offset4 = ftell(LIST_INVERT);
                    fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
                    valor = -1;
                    fwrite(&valor, sizeof(int), 1, LIST_INVERT);
                    strcpy(vetIndiceSec[contIndiceSec].userSec, post.user);
                    vetIndiceSec[contIndiceSec].posicSec = offset4;
                    contIndiceSec++;
                }
                //--------------------


                fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);
                fseek(OUT, offset2, 0);
                fwrite(&offset, sizeof(int), 1, OUT);
            }
            if (tamanho2 > tamanho1) {
                fseek(OUT, 1, 1);
                fread(&offset, sizeof(int), 1, OUT);
                fseek(OUT, -(sizeof(int)+2), 1);
                putc(tamanho1, OUT);
                //salvando registro no arquivo de indice primario
                vetIndice[contIndice].chaveID = post.ID;
                vetIndice[contIndice].posicReg = ftell(OUT);
                contIndice ++;
                //--------------------

                //salvando registro no arquivo de indice secundario
                for(int k = 0; k < contIndiceSec + 1; k++) { //verifico se a chave já existe
                    comparador = strcmp(post.user, vetIndiceSec[k].userSec);
                    if (comparador == 0) {
                        offset3 = vetIndiceSec[k].posicSec;
                        break;
                    }
                }

                if (comparador == 0) {
                    fseek(LIST_INVERT, 0, 2);
                    offset4 = ftell(LIST_INVERT);
                    fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
                    valor = -1;
                    fwrite(&valor, sizeof(int), 1, LIST_INVERT);

                    fseek(LIST_INVERT, offset, 0);
                    fseek(LIST_INVERT, sizeof(int), 1);
                    fread(&offset3, sizeof(int), 1, LIST_INVERT);
                    while(offset3 != -1)
                    {
                        fseek(LIST_INVERT, offset3, 0);
                        fseek(LIST_INVERT, sizeof(int), 1);
                        fread(&offset3, sizeof(int), 1, LIST_INVERT);
                    }
                    fseek(LIST_INVERT, -sizeof(int), 1);
                    fwrite(&offset4, sizeof(int), 1, LIST_INVERT);
                } else {
                    fseek(LIST_INVERT, 0, 2);
                    offset4 = ftell(LIST_INVERT);
                    fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
                    valor = -1;
                    fwrite(&valor, sizeof(int), 1, LIST_INVERT);
                    strcpy(vetIndiceSec[contIndiceSec].userSec, post.user);
                    vetIndiceSec[contIndiceSec].posicSec = offset4;
                    contIndiceSec++;
                }
                //--------------------

                fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);

                tamanho2 = tamanho2 - tamanho1;
                if (tamanho2 < 6) {
                    putc((tamanho2-1), OUT);
                    fseek(OUT, offset2, 0);
                    fwrite(&offset, sizeof(int), 1, OUT);
                } else {
                    putc((tamanho2-1), OUT);
                    fprintf(OUT, "*");
                    fwrite(&offset, sizeof(int), 1, OUT);
                    fseek(OUT, -(sizeof(int)+2), 1);
                    offset = ftell(OUT);
                    fseek(OUT, offset2, 0);
                    fwrite(&offset, sizeof(int), 1, OUT);
                }
            }
        }
    }

    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função para Remoção de um registro
void remover()
{
    int removaID1, removaID2, tamRegistro, tamRegistro2, i, offset1, offset2;
    char carac, numstr[10];

    system("clear");
    printf("\n                    Remocao                   ");

    printf("\n\nInsira o ID para remocao: "); fgets(numstr, 10, stdin); removaID1 = atoi(numstr);

    //Pesquisa do ID para ser removido
    fseek(OUT, 5, 0);
    removaID2 = 0;
    tamRegistro = getc(OUT);
    while((removaID1 != removaID2) && (tamRegistro != EOF))
    {
        memset(numstr, '\0', sizeof(numstr));
        tamRegistro2 = tamRegistro;
        i=0;
        carac = getc(OUT);
        while(carac != '#')
        {
            numstr[i] = carac;
            i++;
            carac = getc(OUT);
        }
        removaID2 = atoi(numstr);
        fseek(OUT, (tamRegistro-(i+1)), 1);
        tamRegistro = getc(OUT);
    }
    //--------------------

    //Removendo dados - Colocando informações no registro removido
    if ((removaID1 != removaID2) && (tamRegistro == -1)) {
        printf("\nID inexistente. Tente novamente.");
    } else {
        if ((removaID1 == removaID2) && (tamRegistro == -1)) {
            fseek(OUT, -((tamRegistro2)+1), 1);
        } else {
            fseek(OUT, -((tamRegistro2)+2), 1);
        }

        //ver posição do registro e inserir o final da lista(-1)
        offset1 = ftell(OUT);
        putc(tamRegistro2, OUT);
        fprintf(OUT, "*");
        offset2 = -1;
        fwrite(&offset2, sizeof(int), 1, OUT);
        printf("\nRegistro deletado.");

        //procurar o ultimo registro vazio e indicar qual será o proximo vazio
        rewind(OUT);
        fread(&offset2, sizeof(int), 1, OUT);
        while (offset2 != -1)
        {
            fseek(OUT, (offset2+2), 0);
            fread(&offset2, sizeof(int), 1, OUT);
        }
        fseek(OUT, -4, 1);
        fwrite(&offset1, sizeof(int), 1, OUT);

        //deletar registro do arquivo de indices
        i = 0;
        while(removaID1 != vetIndice[i].chaveID)
        {
            i++;
        }
        vetIndice[i].chaveID = 0;
        vetIndice[i].posicReg = 0;

    }


    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função para efetuar a impressão na tela
void escritor()
{
    char carac;

    carac = fgetc(OUT);
    while((carac != '#') && (carac != EOF))
    {
        printf("%c", carac);
        carac = fgetc(OUT);
    }
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função de Pesquisa de posts
void pesquisar()
{
    int pesquisaID1, pesquisaID2, tamRegistro, tamRegistro2, i;
    char carac, numstr[10];

    system("clear");
    printf("          Gerenciamento da Disciplina          ");
    printf("\n                    Pesquisa                   ");

    printf("\n\nInsira o ID para pesquisa: "); fgets(numstr, 10, stdin); pesquisaID1 = atoi(numstr);

    //Pesquisa do ID
    fseek(OUT, 5, 0);
    pesquisaID2 = 0;
    tamRegistro = getc(OUT);
    while((pesquisaID1 != pesquisaID2) && (tamRegistro != EOF))
    {
        memset(numstr, '\0', sizeof(numstr));
        tamRegistro2 = tamRegistro;
        i=0;
        carac = getc(OUT);
        while(carac != '#')
        {
            numstr[i] = carac;
            i++;
            carac = getc(OUT);
        }
        pesquisaID2 = atoi(numstr);
        fseek(OUT, (tamRegistro-(i+1)), 1);
        tamRegistro = getc(OUT);
    }
    //--------------------

    //Escrita referente aos dados obtidos
    if ((pesquisaID1 != pesquisaID2) && (tamRegistro == -1)) {
        printf("\nID inexistente. Procure novamente.");
    } else {
        if ((pesquisaID1 == pesquisaID2) && (tamRegistro == -1)) {
            fseek(OUT, -(tamRegistro2), 1);
        } else {
            fseek(OUT, -(tamRegistro2+1), 1);
        }

        printf("\n\nID: "); escritor();
        printf("\nUser: "); escritor();
        printf("\nText: "); escritor();
        printf("\nLinguagem: "); escritor();
        printf("\nCoordenadas: "); escritor();
        printf("\nLike: "); escritor();
        printf("\nShare: "); escritor();
        printf("\nView: "); escritor();


    }
    //--------------------

    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função de Inserção Simples de posts
void inserir()
{
    char simNao, numstr[10];
    int tamanho=0, comparador, offset, offset2, valor;

    system("clear");
    printf("          Gerenciamento da Disciplina          ");
    printf("\n                    Insercao                   ");


    //Inserção dos dados na estrutura
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.ID = atoi(numstr);
    printf("User: "); fgets(post.user, 30, stdin);
    printf("Text: "); fgets(post.text, 30, stdin);
    printf("Coordenadas: "); fgets(post.coordinates, 30, stdin);
    printf("Linguagem: "); fgets(post.language, 30, stdin);
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.like_count = atoi(numstr);
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.share_count = atoi(numstr);
    printf("\n\nID: "); fgets(numstr, 10, stdin); post.views_count = atoi(numstr);

    //Gravação no arquivo
    fseek(OUT, 0, 2);
    itoa(post.ID,numstr,10); tamanho = tamanho + strlen(numstr) + 1;
    tamanho = tamanho + strlen(post.user) + 1;
    tamanho = tamanho + strlen(post.text) + 1;
    tamanho = tamanho + strlen(post.coordinates) + 1;
    tamanho = tamanho + strlen(post.language) + 1;
    itoa(post.like_count,numstr,10); tamanho + strlen(numstr) + 1;
    itoa(post.share_count,numstr,10); tamanho + strlen(numstr) + 1;
    itoa(post.views_count,numstr,10); tamanho + strlen(numstr) + 1;
    putc(tamanho, OUT);
    //salvando registro no arquivo de indice primario
    vetIndice[contIndice].chaveID = post.ID;
    vetIndice[contIndice].posicReg = ftell(OUT);
    contIndice ++;
    //--------------------

    //salvando registro no arquivo de indice secundario
    for(int k = 0; k < contIndiceSec + 1; k++) { //verifico se a chave já existe
        comparador = strcmp(post.user, vetIndiceSec[k].userSec);
        if (comparador == 0) {
            offset = vetIndiceSec[k].posicSec;
            break;
        }
    }

    if (comparador == 0) {
        fseek(LIST_INVERT, 0, 2);
        offset2 = ftell(LIST_INVERT);
        fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
        valor = -1;
        fwrite(&valor, sizeof(int), 1, LIST_INVERT);

        fseek(LIST_INVERT, offset, 0);
        fseek(LIST_INVERT, sizeof(int), 1);
        fread(&offset, sizeof(int), 1, LIST_INVERT);
        while(offset != -1)
        {
            fseek(LIST_INVERT, offset, 0);
            fseek(LIST_INVERT, sizeof(int), 1);
            fread(&offset, sizeof(int), 1, LIST_INVERT);
        }
        fseek(LIST_INVERT, -sizeof(int), 1);
        fwrite(&offset2, sizeof(int), 1, LIST_INVERT);
    } else {
        fseek(LIST_INVERT, 0, 2);
        offset2 = ftell(LIST_INVERT);
        fwrite(&post.ID, sizeof(int), 1, LIST_INVERT);
        valor = -1;
        fwrite(&valor, sizeof(int), 1, LIST_INVERT);
        strcpy(vetIndiceSec[contIndiceSec].userSec, post.user);
        vetIndiceSec[contIndiceSec].posicSec = offset2;
        contIndiceSec++;
    }
    //--------------------


    fprintf(OUT, "%d#%s#%s", post.ID, post.user, post.text);
    //-------------------

    getchar();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//Função para exibir tela de Menu
void menu()
{
    char op;

    do {
        system("clear");
        printf("\n\n1. Inserir post (Modo Simples)");
        printf("\n2. Inserir post (Modo Avancado)");
        printf("\n3. Alterar dados post");
        printf("\n4. Remover post");
        printf("\n5. Pesquisar post (Modo Basico)");
        printf("\n6. Compactar");
        printf("\n7. Pesquisa post (Chave Primaria)");
        printf("\n8. Pesquisa post (Chave Secundaria)");
        printf("\n0. Sair");
        printf("\n\nOpcao: "); op = getchar();
        getchar();

        switch (op)
        {
            case '1':
                inserir();
                break;
            case '2':
                inserirAvancado();
                break;
            case '3':
                alterar();
                break;
            case '4':
                remover();
                break;
            case '5':
                pesquisar();
                break;
            case '6':
                compactar();
                break;
            case '7':
                pesquisaChPrimaria();
                break;
            case '8':
                pesquisaChSecundaria();
                break;
            case '0':
                printf("\n\nEncerrando o programa...");
                break;
            default:
                printf("\nOpcao invalida");
                getchar();
        }

    } while (op != '0');
}
////////////////////////////////////////////////////////////////////////////////         
     
     
     
