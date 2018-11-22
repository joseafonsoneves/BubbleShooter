/*
----------------------------- Trabalho intermédio -----------------------------
  Autores
    João Moura - ist189678
    José Neves - ist189683

  Para compilar: gcc bolhas.c -g -I/usr/include/SDL2 -D_REENTRANT -Wall
                 -pedantic -std=c99 -L/usr/lib/i386-linux-gnu -lm -lSDL2
                 -lSDL2_gfx -o bolhas
-------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL.h>
#include <time.h>

#define L_ECRA_PADRAO 1366
#define H_ECRA_PADRAO 768
#define H_CABECALHO_JANELA 50
#define NUM_MAX_COL 30
#define NUM_MIN_COL 13
#define H_MAX 23
#define H_MIN 5
#define VAL_MAX_RBOL 25
#define VAL_MIN_RBOL 10
#define VAL_MAX_DL 25
#define H_PIX_MIN 200

#define L_JANELA 0
#define H_JANELA 1
#define R_BOLHA 2
#define DL_COLISAO 3
#define NL_INICIAL 4
#define NJ_GERACAO 5

#define V_LOGICOS 0
#define VARS 1
#define P_MAT_MOVER 2
#define P_RATO 3
#define FIM 0
#define EXISTE_B_MOVER 1
#define GANHOU 2
#define JOGADAS 0
#define PONTUACAO 1
#define AP_VERDE 4
#define AP_BRANCO 5
#define AP_VERMELHO 6
#define AP_ROXO 7
#define AP_AMARELO 8
#define AP_CASTANHO 9
#define AP_CYAN 10
#define AP_AZUL 11
#define AP_PRETO 12
#define ESPECIFICAS 13
#define A_MOVER 0
#define ATUAL 1
#define SEGUINTE 2

#define VERDE { 89, 223,  55}
#define BRANCO {255, 255, 255}
#define VERMELHO {247,  53,  32}
#define ROXO {153,   0, 150}
#define AMARELO {245, 252,   7}
#define CASTANHO {166,  83,   0}
#define CYAN {  0, 255, 255}
#define AZUL { 67,  47, 249}
#define PRETO {  0,   0,   0}

#define PONTUACAO_MAX 9999990
#define A_RATO 2

#define DIM_MAX_NOME 10

SDL_Window* g_pWindow = NULL;
SDL_Renderer* g_pRenderer = NULL;

void ver_leitura(int i, float configs[]){
  //Aumenta a rapidez com que se pode alterar os valores limite de cada condição
  int num_max_col=NUM_MAX_COL, num_min_col=NUM_MIN_COL;
  int h_max=H_MAX, h_min=H_MIN;
  int val_max_rbol=VAL_MAX_RBOL, val_min_rbol=VAL_MIN_RBOL;
  int val_max_dl=VAL_MAX_DL;

  if(i!=6){
    printf("Abertura malsucedida do ficheiro de configuração!\n\n");
    exit(1);
  }
  //Verifica o número de variáveis lidas
  if(configs[L_JANELA]>NUM_MAX_COL || configs[L_JANELA]<NUM_MIN_COL){
    printf("Altere o número de colunas para estar compreendido ");
    printf("entre %d e %d inclusive.\n\n", num_min_col, num_max_col);
    exit(2);
  }
  //Verifica o número de colunas de bolhas
  if(configs[H_JANELA]>H_MAX || configs[H_JANELA]<H_MIN){
    printf("Altere o número de linhas para estar compreendido entre ");
    printf("%d e %d inclusive.\n\n", h_min, h_max);
    exit(3);
  }
  //Verifica o número de linhas de bolhas
  if(configs[2]>VAL_MAX_RBOL || configs[2]<VAL_MIN_RBOL){
    printf("Altere o raio para estar compreendido entre ");
    printf("%d e %d inclusive.\n\n", val_min_rbol, val_max_rbol);
    exit(4);
  }
  //Verifica o valor do raio de cada bolha
  if(configs[3]>=VAL_MAX_DL  || configs[3]<=0){
    printf("Altere a distância para avaliação da colisão para estar ");
    printf("compreendida entre 0 e %d.\n\n", val_max_dl);
    exit(5);
  }
  //Verifica o valor de distância para avaliação da colisão
  if(configs[4]==0 || configs[4]>=configs[H_JANELA]-3){
    printf("Altere o número inicial de linhas para ser maior que a diferença");
    printf(" entre o número de linhas e 3.\n\n");
    exit(6);
  }
  //Verifica o número de linhas iniciais com bolhas
  if(configs[L_JANELA]*configs[2]*2>L_ECRA_PADRAO ||
     configs[H_JANELA]*configs[2]*2>H_ECRA_PADRAO-H_CABECALHO_JANELA ||
     configs[H_JANELA]*configs[2]*2<H_PIX_MIN){
    printf("Relação raio-linhas e/ou raio-colunas inválida\n\n");
    exit(7);
  }
  //Verfica o tamanha em píxeis da janela
  if(configs[L_JANELA]-(int)configs[L_JANELA]!=0 ||
     configs[H_JANELA]-(int)configs[H_JANELA]!=0 ||
     configs[2]-(int)configs[2]!=0 || configs[4]-(int)configs[4]!=0 ||
     configs[5]-(int)configs[5]!=0){
    printf("À exceção da distância de avaliação da colisão todos os valores");
    printf(" no ficheiro de configuração devem ser inteiros!\n\n");
    exit(8);
  }
  //Verfica se as variáveis são inteiras ou decimais
}
void ler_config(float configs[]){
  FILE* pFILE_r_config;
  char linha[100];
  int i=0;

  pFILE_r_config=fopen("config.txt", "r");

  if(pFILE_r_config==NULL){
    printf("Impossível abrir o ficheiro de cofiguração!");
    exit(0);
  }

  while(fgets(linha, 100, pFILE_r_config)!=NULL){
    int conta_espacos;

    for(conta_espacos=0; isspace(linha[conta_espacos]); conta_espacos++);
    //Conta o número de espaços numa linha até ao primeiro carater não espaço

    if(linha[conta_espacos]<=57 && linha[conta_espacos]>=48){
    //Se o primeiro carater diferente de espaço do array for um algarismo
      if(i==0){
        sscanf(linha, "%f %f", &configs[L_JANELA], &configs[H_JANELA]);
        //Retira do array "linha" os dois primeiros valores na forma float
        i=i+2;
      }
      else{
        sscanf(linha, "%f", &configs[i]);
        //Retira do array "linha" um dos seguintes valores na forma float
        i++;
      }
    }
  }

  fclose(pFILE_r_config);

  ver_leitura(i, configs);
}
void abre_janela(float configs[]){
  if(SDL_Init(SDL_INIT_EVERYTHING) >= 0){     //Se a inicialização for bem sucedida
    g_pWindow = SDL_CreateWindow("!!!Bolhas!!!",                              //Cria a janela com o cabeçalho "!!!Bolhas!!!"
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, configs[L_JANELA]*2*configs[R_BOLHA],
                configs[H_JANELA]*2*configs[R_BOLHA], SDL_WINDOW_SHOWN);

    if(g_pWindow != 0)     //Se a criação da janela for bem sucedida
      g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);     //Cria o renderer
  }
  else
    exit(8);
}
void novo_jogo(int matriz[][NUM_MAX_COL], int inteiros[][3], float configs[]){
  int linha;
  int coluna;
  time_t t;


  srand((unsigned) time(&t));
  //Cria a semente de aleatoriedade em função do tempo de calendário

  for(linha=0; linha<H_MAX-3; linha++)
    for(coluna=0; coluna<NUM_MAX_COL; coluna++)
      matriz[linha][coluna]=0;
  //Preenche com 0's toda a matriz

  for(linha=0; linha<configs[NL_INICIAL]; linha++)
    for(coluna=0; coluna<configs[L_JANELA]; coluna++)
      matriz[linha][coluna]=rand()%9+1;
  //Preenche com números inteiros de 1 a 9 e aleatoriamente a matriz das bolhas

  inteiros[ESPECIFICAS][ATUAL]=rand()%9+1;
  //Atribui um valor inteiro aleatório de 1 a 9 à bolha atual
  inteiros[ESPECIFICAS][SEGUINTE]=rand()%9+1;
  //Atribui um valor inteiro aleatório de 1 a 9 à bolha seguinte
}
void inicializacao(float configs[], int matriz[][NUM_MAX_COL],
                   int inteiros[][3]){
  ler_config(configs);

  abre_janela(configs);

  novo_jogo(matriz, inteiros, configs);
}

void captura_p_rato(int inteiros[][3]){
  if(!inteiros[V_LOGICOS][EXISTE_B_MOVER])
    SDL_GetMouseState(&inteiros[P_RATO][0], &inteiros[P_RATO][1]);
}

void atualiza_cores_bolhas(int inteiros[][3]){
  inteiros[ESPECIFICAS][A_MOVER]=inteiros[ESPECIFICAS][ATUAL];
  //Define a cor da bolha a mover como a da bolha atual

  inteiros[ESPECIFICAS][ATUAL]=inteiros[ESPECIFICAS][SEGUINTE];
  //Passa o valor numérico da cor da bolha seguinte para a bolha atual

  time_t t;
  srand((unsigned) time(&t));
  //Cria uma seed baseado no tempo de calendário
  inteiros[ESPECIFICAS][SEGUINTE]=rand()%9+1;
  //Cria um valor aleatório entre 1 e 9 para a cor da bolha seguinte
}
void gestao_eventos(int inteiros[][3], SDL_Event event, float configs[],
                    float c_bolha_mover[], int matriz[][NUM_MAX_COL]){
  switch (event.type){
    case SDL_QUIT:
      inteiros[V_LOGICOS][FIM]=1;
      break;
    case SDL_MOUSEBUTTONDOWN:
      if((inteiros[P_RATO][0]>=configs[R_BOLHA]/2 &&
          inteiros[P_RATO][0]<=4.5*configs[R_BOLHA]) &&
         (inteiros[P_RATO][1]>=configs[R_BOLHA]/2 &&
          inteiros[P_RATO][1]<=3.5*configs[R_BOLHA])){
      //Se a ponta do rato estiver dentro da caixa "Fim"
        inteiros[V_LOGICOS][FIM]=1;
        break;
      }
      else if((inteiros[P_RATO][0]>=5*configs[R_BOLHA] &&
               inteiros[P_RATO][0]<=13*configs[R_BOLHA]) &&
              (inteiros[P_RATO][1]>=configs[R_BOLHA]/2 &&
               inteiros[P_RATO][1]<=3.5*configs[R_BOLHA]) &&
               !inteiros[V_LOGICOS][EXISTE_B_MOVER]){
      //Se a ponta do rato estiver dentro da caixa "Novo jogo" e não existir
      //nenhuma bola a mover-se
        novo_jogo(matriz, inteiros, configs);
        inteiros[VARS][JOGADAS]=0;
        inteiros[VARS][PONTUACAO]=0;
        break;
      }
      else{
        if(inteiros[V_LOGICOS][EXISTE_B_MOVER]==0){
      //Se o rato estiver em qualquer outra posição e não existir nenhuma bolha
      //a mover-se
          c_bolha_mover[0]=configs[L_JANELA]*configs[R_BOLHA];
          c_bolha_mover[1]=configs[R_BOLHA]*(configs[H_JANELA]*2-1);

          inteiros[V_LOGICOS][EXISTE_B_MOVER]=1;
          //Inicia o movimento da bolha
          inteiros[VARS][JOGADAS]=inteiros[VARS][JOGADAS]+1;
          //Conta como uma jogada

          atualiza_cores_bolhas(inteiros);
          //Reorganiza as cores das bolhas
        }
        break;
      }
  }
}

void calcula_a(int inteiros[][3], float c_bolha_mover[], float configs[]){
  //O ângulo obtido com esta função é o ângulo formado pela vertical ao centro
  //da bolha atual e a reta que passa pela posição do rato e o centro da bolha
  //atual, sendo positivo para posições do rato à direita do centro e negativo
  //para posições à esquerda do centro

  if(inteiros[P_RATO][0]>configs[R_BOLHA]*configs[L_JANELA]){
  //Se o rato se encontrar à direita do centro da bolha atual
    if(inteiros[P_RATO][1]<=configs[R_BOLHA]*(configs[H_JANELA]*2-3))
    //E se o rato não estiver abaixo da altura de interseção durante o disparo
    //com a caixa da bolha seguinte
      c_bolha_mover[A_RATO]=
      atan((inteiros[P_RATO][0]-configs[R_BOLHA]*configs[L_JANELA])/
           (configs[R_BOLHA]*(configs[H_JANELA]*2-1)-inteiros[P_RATO][1]));
    else
      c_bolha_mover[A_RATO]=atan((1.0*configs[L_JANELA]/2-1));
    //Calcular o ângulo limite para não tocar na caixa da bolha seguinte
  }
  else{
    if(inteiros[P_RATO][1]<=configs[R_BOLHA]*(configs[H_JANELA]*2-3))
      c_bolha_mover[A_RATO]=
      -1*atan((configs[R_BOLHA]*configs[L_JANELA]-inteiros[P_RATO][0])/
              (configs[R_BOLHA]*(configs[H_JANELA]*2-1)-inteiros[P_RATO][1]));
    else
      c_bolha_mover[A_RATO]=-1*atan((1.0*configs[L_JANELA]/2-1));
    //Para ficar simétrico
  }
}
void calcula_c_bolha_mover(float c_bolha_mover[], float configs[],
                           int inteiros[][3]){
  float desloc=configs[R_BOLHA]/4;

  if(inteiros[V_LOGICOS][EXISTE_B_MOVER]){
  //Caso exista uma bolha a mover-se
    if(c_bolha_mover[A_RATO]>0){
      c_bolha_mover[0]=c_bolha_mover[0]+desloc*sin(c_bolha_mover[A_RATO]);
      c_bolha_mover[1]=c_bolha_mover[1]-desloc*cos(c_bolha_mover[A_RATO]);
    }
    else if(c_bolha_mover[A_RATO]<=0){
      c_bolha_mover[0]=c_bolha_mover[0]-desloc*sin((-1.0)*c_bolha_mover[A_RATO]);
      c_bolha_mover[1]=c_bolha_mover[1]-desloc*cos((-1.0)*c_bolha_mover[A_RATO]);
    }
    if(c_bolha_mover[0]>configs[R_BOLHA]*(configs[L_JANELA]*2+2) ||
       c_bolha_mover[0]<-2.0*configs[R_BOLHA])
       //Se a bolha sair completamente dos limites laterais
        inteiros[V_LOGICOS][EXISTE_B_MOVER]=0;
    //Apaga-se a bolha
  }
}
void calcula_pmat_mover(float c_bolha_mover[], float configs[],
                         int inteiros[][3], int matriz[][NUM_MAX_COL]){
  if(inteiros[V_LOGICOS][EXISTE_B_MOVER]){
    if((c_bolha_mover[0]-configs[R_BOLHA])/(configs[R_BOLHA]*2)-(int)(c_bolha_mover[0]/(configs[R_BOLHA]*2))<0.5)     //Verifica se a bolha a mover-se está mais próxima da posição à esquerda
      inteiros[P_MAT_MOVER][1]=(int)(c_bolha_mover[0]/(configs[R_BOLHA]*2));
    else
      inteiros[P_MAT_MOVER][1]=(int)(c_bolha_mover[0]/(configs[R_BOLHA]*2)+1);

    if((c_bolha_mover[1]-configs[R_BOLHA])/(configs[R_BOLHA]*2)-(int)(c_bolha_mover[1]/(configs[R_BOLHA]*2))<0.5)     //Verifica se a bolha a mover-se está mais próxima da posição à esquerda
      inteiros[P_MAT_MOVER][0]=(int)(c_bolha_mover[1]/(configs[R_BOLHA]*2)-2);
    else
      inteiros[P_MAT_MOVER][0]=(int)(c_bolha_mover[1]/(configs[R_BOLHA]*2)-1);
  }
}
void calculos(int inteiros[][3], float c_bolha_mover[], float configs[],
              int matriz[][NUM_MAX_COL]){
  calcula_a(inteiros, c_bolha_mover, configs);     //Descobre o ângulo formado entre a reta vertical que passa pelo centro da bolha atual e a reta que passa pela posição do rato e o centro da bolha atual

  calcula_c_bolha_mover(c_bolha_mover, configs, inteiros);     //Calcula o centro da bolha a mover-se

  calcula_pmat_mover(c_bolha_mover, configs, inteiros, matriz);
}

void processa_vizinhos(int linha, int coluna, int cor,
                       int matriz[][NUM_MAX_COL], float configs[]){
  for(int vizinhanca_l=linha-1; vizinhanca_l<=linha+1; vizinhanca_l++){
    for(int vizinhanca_c=coluna-1; vizinhanca_c<=coluna+1; vizinhanca_c++){
      if(matriz[vizinhanca_l][vizinhanca_c]==-1*cor){
        matriz[vizinhanca_l][vizinhanca_c]=cor;
      }
    }
  }
}
void processa_caminhos(int* encontrou, int* conta, int cor, float configs[],
                       int matriz[][NUM_MAX_COL]){
  while(*encontrou){
    *encontrou=0;

    for(int linha=0; linha<configs[H_JANELA]-4; linha++){
      for(int coluna=0; coluna<configs[L_JANELA]; coluna++){
        if(matriz[linha][coluna]==cor){
          *conta=*conta+1;
          processa_vizinhos(linha, coluna, cor, matriz, configs);
          matriz[linha][coluna]=cor-100;
          *encontrou=1;
        }
      }
    }
  }
}
void rebenta(int conta, int matriz[][NUM_MAX_COL], int cor, float configs[],
             int inteiros[][3]){
  if(conta>=3){
    for(int linha=0; linha<configs[H_JANELA]-4; linha++){
      for(int coluna=0; coluna<configs[L_JANELA]; coluna++){
        if(matriz[linha][coluna]<0)
          matriz[linha][coluna]=0;
      }
    }
    inteiros[VARS][PONTUACAO]=inteiros[VARS][PONTUACAO]+conta*10;
  }
  else{
    for(int linha=0; linha<configs[H_JANELA]; linha++){
      for(int coluna=0; coluna<configs[L_JANELA]; coluna++){
        if(matriz[linha][coluna]<0)
          matriz[linha][coluna]=-1*cor;
      }
    }
  }
}
void desaparecer(int matriz[][NUM_MAX_COL], int inteiros[][3], int cor_bolha,
                 float configs[]){
  int encontrou=1;
  int cor=-1*cor_bolha;
  int conta=0;

  processa_caminhos(&encontrou, &conta, cor, configs, matriz);
  rebenta(conta, matriz, cor, configs, inteiros);
}
int ver_colisao(int inteiros[][3], int matriz[][NUM_MAX_COL],
                float c_bolha_mover[], float configs[]){
  float d_centros;
  float d_colisao;

  d_colisao=2*configs[R_BOLHA]+configs[R_BOLHA]*2*configs[DL_COLISAO]/100;

  for(int linha=inteiros[P_MAT_MOVER][0]-1;
      linha<=inteiros[P_MAT_MOVER][0]+1;
      linha++){
    for(int coluna=inteiros[P_MAT_MOVER][1]-1;
        coluna<=inteiros[P_MAT_MOVER][1]+1; coluna++){
      d_centros=sqrt(pow(c_bolha_mover[0]-configs[R_BOLHA]*(coluna*2+1), 2)+
                     pow(c_bolha_mover[1]-configs[R_BOLHA]*(linha*2+5), 2));

      if(matriz[linha][coluna]!=0 && linha<configs[H_JANELA]-3 && coluna>=0 &&d_centros<=d_colisao){
        return 1;
      }
    }
  }
  return 0;
}
void posiciona_bolha(float c_bolha_mover[], int matriz[][NUM_MAX_COL],
                     int inteiros[][3], float configs[]){
  float d_posicoes=99999, aux;
  int p_colocada[2];
  int colocacao=0;

  for(int linha=inteiros[P_MAT_MOVER][0]-1; linha<=inteiros[P_MAT_MOVER][0]+1; linha++){
    for(int coluna=inteiros[P_MAT_MOVER][1]-1; coluna<=inteiros[P_MAT_MOVER][1]+1; coluna++){
      if(matriz[linha][coluna]==0){
        aux=sqrt(pow(c_bolha_mover[0]-coluna*configs[R_BOLHA]*2-configs[R_BOLHA], 2)+
                 pow(c_bolha_mover[1]-linha*configs[R_BOLHA]*2-5*configs[R_BOLHA], 2));
        if(aux<d_posicoes && aux<=2*configs[R_BOLHA]*(1+configs[DL_COLISAO]/100)){
          p_colocada[0]=linha;
          p_colocada[1]=coluna;
          d_posicoes=aux;
          colocacao=1;
        }
      }
    }
  }

  if(colocacao)
    matriz[p_colocada[0]][p_colocada[1]]=-1*inteiros[ESPECIFICAS][A_MOVER];
}
void colisao_bolha(int inteiros[][3], float c_bolha_mover[], float configs[],
                   int matriz[][NUM_MAX_COL]){
  if(ver_colisao(inteiros, matriz, c_bolha_mover, configs)){
    inteiros[V_LOGICOS][EXISTE_B_MOVER]=0;
    posiciona_bolha(c_bolha_mover, matriz, inteiros, configs);
    desaparecer(matriz, inteiros, inteiros[ESPECIFICAS][A_MOVER], configs);
  }
}
void colisao_parede(int inteiros[][3], float c_bolha_mover[], float configs[],
                    int matriz[][NUM_MAX_COL]){
  int aux0=inteiros[2][0], aux1=inteiros[2][1];

  inteiros[V_LOGICOS][EXISTE_B_MOVER]=0;
  matriz[aux0][aux1]=inteiros[ESPECIFICAS][A_MOVER];
  desaparecer(matriz, inteiros, inteiros[ESPECIFICAS][A_MOVER], configs);
}
void colisoes(int inteiros[][3], float c_bolha_mover[], float configs[],
                   int matriz[][NUM_MAX_COL]){
  if(inteiros[V_LOGICOS][EXISTE_B_MOVER]){
  //Caso exista movimento
    if(c_bolha_mover[1]<=configs[R_BOLHA]*(5+2*(configs[DL_COLISAO]/100)))
    //Caso colida com a parede superior
      colisao_parede(inteiros, c_bolha_mover, configs, matriz);
    else{
      colisao_bolha(inteiros, c_bolha_mover, configs, matriz);
    }
  }
}

void gera_novas(float configs[], int matriz[][NUM_MAX_COL], int inteiros[][3]){
  time_t t;
  int matriz_c_bolha_mat[H_MAX-3][NUM_MAX_COL];

  if(inteiros[VARS][JOGADAS]==configs[NJ_GERACAO]){
  //Se o número de jogadas for igual ao número de jogadas para geração de uma
  //nova linha

    for(int conta_linha=0; conta_linha<configs[H_JANELA]-3; conta_linha++){
      for (int conta_coluna=0; conta_coluna<configs[L_JANELA]; conta_coluna++){
        matriz_c_bolha_mat[conta_linha][conta_coluna]=
        matriz[conta_linha][conta_coluna];
      }
    }
    //Copia a matriz para outra matriz auxiliar

    for(int conta_linha=0; conta_linha<=H_MAX-4; conta_linha++){
    //Copia os elementos da matriz auxiliar para a matriz original uma linha
    //abaixo do que anteriormente
      for(int conta_coluna=0; conta_coluna<=NUM_MAX_COL; conta_coluna++){
        matriz[conta_linha+1][conta_coluna]=
        matriz_c_bolha_mat[conta_linha][conta_coluna];
      }
    }

    srand((unsigned) time(&t));
    //Cria uma seed para geração de valores aleatórios a partir do tempo
    //de calendário

    for(int conta_coluna=0; conta_coluna<configs[L_JANELA]; conta_coluna++)
      matriz[0][conta_coluna]=rand()%9+1;
    //Atribui valores aleatórios aos elementos da primeira linha da matriz
    //original

    inteiros[VARS][JOGADAS]=0;
    //Reinicia o valor da variável inteiros[VARS][JOGADAS]
  }
}
void ver_acumulacao(float configs[], int inteiros[][3],
                    int matriz[][NUM_MAX_COL]){
  int aux=configs[H_JANELA];

  if(!inteiros[V_LOGICOS][EXISTE_B_MOVER]){
    for(int i=0; i<configs[L_JANELA]; i++){
      if(matriz[aux-4][i]!=0){
        inteiros[V_LOGICOS][FIM]=1;
        printf("\nBolhas a maisss!\n");
      }
    }
  }

  //Verifica, caso não hajam bolhas a mover-se,  para todas as posições da
  //última linha se lá existe uma bolha
}
void ver_esvaziamento(int matriz[][NUM_MAX_COL], int inteiros[][3]){
  int j=0;

  //Conta o número de bolhas existentes
  for(int linha=0; linha<H_MAX-3; linha++)
    for(int coluna=0; coluna<NUM_MAX_COL; coluna++)
      if(matriz[linha][coluna]!=0)
        j++;

  //Termina o jogo caso não existam bolhas
  if(j==0){
    inteiros[V_LOGICOS][FIM]=1;
    inteiros[V_LOGICOS][GANHOU]=1;
  }
}
void verificacoes(float configs[], int inteiros[][3],
                  int matriz[][NUM_MAX_COL]){
  if(inteiros[V_LOGICOS][EXISTE_B_MOVER]==0){
    gera_novas(configs, matriz, inteiros);
    //Gera uma nova linha
    ver_acumulacao(configs, inteiros, matriz);
    //Verifica se, pelo menos, uma das bolhas atingiu o limite e termina o jogo
    //caso isso aconteça
    ver_esvaziamento(matriz, inteiros);
    //Verifica se não existem bolhas e caso não existam termina o jogo
  }
}

void pinta_branco(int inteiros[][3]){
  SDL_SetRenderDrawColor(g_pRenderer, inteiros[AP_BRANCO][0],
                         inteiros[AP_BRANCO][1], inteiros[AP_BRANCO][2], 255);
  SDL_RenderClear(g_pRenderer);
}
void desenha_bolhas(float configs[], int matriz[][NUM_MAX_COL],
                    int inteiros[][3]){
  for(int conta_linha=0; conta_linha<configs[H_JANELA]-3; conta_linha++){
    for (int conta_coluna = 0; conta_coluna<configs[L_JANELA]; conta_coluna++){
      int val_mat=matriz[conta_linha][conta_coluna];
      //Vai buscar o valor da matriz na posição em que se encontra o
      //ciclo

      if(val_mat!=0){
      //Se esse valor for diferente de 0
      filledCircleRGBA(g_pRenderer, configs[R_BOLHA]*(conta_coluna*2+1),
                       configs[R_BOLHA]*(conta_linha*2+5), configs[R_BOLHA],
                       inteiros[val_mat+3][0], inteiros[val_mat+3][1],
                       inteiros[val_mat+3][2], 255);
      //Desenha o preenchimento da bolha com a cor da matriz
      circleRGBA(g_pRenderer,
                 configs[R_BOLHA]*(conta_coluna*2+1),
                 configs[R_BOLHA]*(conta_linha*2+5),
                 configs[R_BOLHA],
                 0, 0, 0, 255);
      //Desenha o perímetro da bolha a preto
      }
    }
  }
  //Indo a cada posição da matriz, cria o desenho de cada bolha que ela
  //representa
}
void desenha_bolha(int inteiros[][3], float c_bolha_mover[],
                   float configs[]){
  if(inteiros[V_LOGICOS][EXISTE_B_MOVER]){
    filledCircleRGBA(g_pRenderer, c_bolha_mover[0], c_bolha_mover[1],
                     configs[R_BOLHA], inteiros[inteiros[ESPECIFICAS][A_MOVER]+3][0],
                     inteiros[inteiros[ESPECIFICAS][A_MOVER]+3][1],
                     inteiros[inteiros[ESPECIFICAS][A_MOVER]+3][2], 255);

    circleRGBA(g_pRenderer, c_bolha_mover[0], c_bolha_mover[1],
               configs[R_BOLHA], 0, 0, 0, 255);
  }
}
void desenha_bolhas_estaticas(float configs[], int inteiros[][3]){
  int aux=inteiros[ESPECIFICAS][ATUAL];
  int aux2=inteiros[ESPECIFICAS][SEGUINTE];

  filledCircleRGBA(g_pRenderer, configs[L_JANELA]*configs[R_BOLHA],
                   configs[R_BOLHA]*(configs[H_JANELA]*2-1), configs[R_BOLHA],
                   inteiros[aux+3][0], inteiros[aux+3][1], inteiros[aux+3][2], 255);

  circleRGBA(g_pRenderer, configs[L_JANELA]*configs[R_BOLHA],
             configs[R_BOLHA]*(configs[H_JANELA]*2-1), configs[R_BOLHA],
             0, 0, 0, 255);

  filledCircleRGBA(g_pRenderer, configs[R_BOLHA],
                   configs[R_BOLHA]*(configs[H_JANELA]*2-1), configs[R_BOLHA],
                   inteiros[aux2+3][0], inteiros[aux2+3][1], inteiros[aux2+3][2], 255);

  circleRGBA(g_pRenderer, configs[R_BOLHA],
             configs[R_BOLHA]*(configs[H_JANELA]*2-1),
             configs[R_BOLHA], 0, 0, 0, 255);
}
void desenha_caixas(float configs[]){
  SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);                              //Seleciona a cor a que se pintarão as caixas

  SDL_RenderDrawLine(g_pRenderer, 0, 4*configs[R_BOLHA], configs[L_JANELA]*2*configs[R_BOLHA], 4*configs[R_BOLHA]);           //Cabeçalho

  SDL_Rect caixa_fim={configs[R_BOLHA]/2, configs[R_BOLHA]/2, 4*configs[R_BOLHA], 3*configs[R_BOLHA]};                //Caixa Fim
  SDL_RenderDrawRect(g_pRenderer, &caixa_fim);                                    //

  SDL_Rect caixa_novo_jogo={5*configs[R_BOLHA], configs[R_BOLHA]/2, 8*configs[R_BOLHA],                      //
                              3*configs[R_BOLHA]};                                         //Caixa Novo Jogo
  SDL_RenderDrawRect(g_pRenderer, &caixa_novo_jogo);                              //

  SDL_Rect caixa_pontos={configs[L_JANELA]*2*configs[R_BOLHA]-12.5*configs[R_BOLHA], configs[R_BOLHA]/2, 12*configs[R_BOLHA],       //
                           3*configs[R_BOLHA]};                                            //Caixa Pontos
  SDL_RenderDrawRect(g_pRenderer, &caixa_pontos);                                 //

  SDL_RenderDrawLine(g_pRenderer, 0, configs[H_JANELA]*2*configs[R_BOLHA]-configs[R_BOLHA]*2, configs[R_BOLHA]*2,             //
                                  configs[H_JANELA]*2*configs[R_BOLHA]-configs[R_BOLHA]*2);                          //Caixa da bolha seguinte
  SDL_RenderDrawLine(g_pRenderer, configs[R_BOLHA]*2, configs[H_JANELA]*2*configs[R_BOLHA]-configs[R_BOLHA]*2, configs[R_BOLHA]*2,     //
                                  configs[H_JANELA]*2*configs[R_BOLHA]);                                    //
}
void desenha_letras(float configs[], int inteiros[][3]){
  char fim_txt[]="FIM";
  char novo_jogo_txt[]="NOVO JOGO";
  char pontos_c_bolha_mat[7];
  //Tem dimensão 7, pois aquilo que armazenará será positivo e terá sete ou
  //menos carateres
  char pontos_txt[16]="Pontos:";
  //Tem dimensão 16, pois armazenará, para além da mensagem (8 carateres),
  //também o valor da pontuação (7 carateres) e um indicador de término da
  //carater (/0)

  sprintf(pontos_c_bolha_mat, "%d", inteiros[VARS][PONTUACAO]);
  //Transforma a pontuação numa string
  strcat(pontos_txt, pontos_c_bolha_mat);
  //Junta a string da pontuação e a que diz "Pontos: "
  pontos_txt[15]='\0';

  stringRGBA(g_pRenderer, configs[R_BOLHA], configs[R_BOLHA], fim_txt, 0, 0,
             0, 255);
  //Escreve "Fim"
  stringRGBA(g_pRenderer, 5.5*configs[R_BOLHA], configs[R_BOLHA], novo_jogo_txt,
             0, 0, 0, 255);
  //Escreve dentro da respetiva caixa "Novo Jogo"
  stringRGBA(g_pRenderer, configs[R_BOLHA]*(configs[L_JANELA]*2-12),
             configs[R_BOLHA], pontos_txt, 0, 0, 0, 255);
  //Escreve dentro da respetiva caixa "Pomtos: (PONTUAÇÂO)"
}
void desenha_seta(float configs[], float c_bolha_mover[]){
  int p_inicio[2], p_ponta[2];
  int c_seta=2*configs[R_BOLHA];

  //Descoberta das coordenadas das extermidades da seta
  if(c_bolha_mover[A_RATO]>0){
      p_inicio[0]=configs[R_BOLHA]*
                 (configs[L_JANELA]+2*sin(c_bolha_mover[A_RATO]));
      p_inicio[1]=configs[R_BOLHA]*
                 (configs[H_JANELA]*2-1-2*cos(c_bolha_mover[A_RATO]));

      p_ponta[0]=p_inicio[0]+sin(c_bolha_mover[A_RATO])*c_seta;
      p_ponta[1]=p_inicio[1]-cos(c_bolha_mover[A_RATO])*c_seta;
  }
  else{
    if(c_bolha_mover[A_RATO]<=0){
      p_inicio[0]=configs[R_BOLHA]*
                  (configs[L_JANELA]-2*sin((-1.0)*c_bolha_mover[A_RATO]));
      p_inicio[1]=configs[R_BOLHA]*
                 (configs[H_JANELA]*2-1-2*cos((-1.0)*c_bolha_mover[A_RATO]));

      p_ponta[0]=p_inicio[0]-sin((-1.0)*c_bolha_mover[A_RATO])*c_seta;
      p_ponta[1]=p_inicio[1]-cos((-1.0)*c_bolha_mover[A_RATO])*c_seta;
    }
  }

  //Desenho do círculo terminal e da linha
  filledCircleRGBA(g_pRenderer, p_ponta[0], p_ponta[1], configs[R_BOLHA]/4, 0, 0, 0, 255);         //Desenha o circulo com centro na extremidade mais afastada da bolha atual
  SDL_RenderDrawLine (g_pRenderer, p_inicio[0], p_inicio[1],                                       //Desenha a linha
                                   p_ponta[0], p_ponta[1]);
}
void rendering(float configs[], int matriz[][NUM_MAX_COL],
               int inteiros[][3], float c_bolha_mover[]){
    pinta_branco(inteiros);
    desenha_bolhas(configs, matriz, inteiros);
    desenha_bolha(inteiros, c_bolha_mover, configs);
    desenha_bolhas_estaticas(configs, inteiros);
    desenha_caixas(configs);
    desenha_letras(configs, inteiros);
    desenha_seta(configs, c_bolha_mover);

    SDL_RenderPresent(g_pRenderer);
}

void regista_resultado(int inteiros[][3]){
  FILE* pFILE_r_resul;
  char nome_jogador[DIM_MAX_NOME+2];

  if(inteiros[VARS][PONTUACAO]==PONTUACAO_MAX || inteiros[V_LOGICOS][GANHOU]==1)
    printf("\nParabéns!\nGanhou o jogo!\n");

  printf("\nFim do jogo!\n");
  printf("Insira o seu nome sem espaços (máximo de 10 carateres):");

  fgets(nome_jogador, DIM_MAX_NOME+2, stdin);

  for(int i=0; i<DIM_MAX_NOME+2; i++)
    if(nome_jogador[i]=='\n')
      nome_jogador[DIM_MAX_NOME]='\0';

  pFILE_r_resul=fopen ("resultados.txt", "a");

  if(pFILE_r_resul==NULL)
    printf("Impossível abrir o ficheiro \n");
  else{
    fprintf(pFILE_r_resul, "%s %10s","Nome do jogador:", nome_jogador);
    fprintf(pFILE_r_resul, " Pontuacao: %d %s",inteiros[VARS][PONTUACAO],"\n");
  }

  fclose(pFILE_r_resul);

  //Saudação ao jogador
  printf("Registo realizado.\n\nFoi um prazer jogar consigo!\n\n");
}

int main (void){
  //--------- Zona de declaração de variáveis ---------
  float configs[6];
  //Configurações do jogo
  float c_bolha_mover[3]={0,0,0};
  //Posição do centro da bolha a mover-se e ângulo do rato
  int inteiros[14][3]={{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, VERDE, BRANCO,
                       VERMELHO, ROXO, AMARELO, CASTANHO, CYAN, AZUL, PRETO,
                       {0,0,0}};
  //Variáveis de: valores lógicos, dados do jogo, posição da bolha a mover-se na
  //matriz, posição do rato, 9 valores rgb de cores e as cores da bolha atual,
  //da bolha seguinte e da bolha a mover-se
  int matriz[H_MAX-3][NUM_MAX_COL];
  //Existência das bolhas e a sua cor
  SDL_Event event;

  //--------- Zona de Instruções ---------
  inicializacao(configs, matriz, inteiros);

  while(!inteiros[V_LOGICOS][FIM] && inteiros[VARS][PONTUACAO]<PONTUACAO_MAX){
  //Enquanto fim for falso e a pontuacao for menor que a máxima
    SDL_PollEvent(&event);

    captura_p_rato(inteiros);
    gestao_eventos(inteiros, event, configs, c_bolha_mover, matriz);
    calculos(inteiros, c_bolha_mover, configs, matriz);
    colisoes(inteiros, c_bolha_mover, configs, matriz);
    verificacoes(configs, inteiros, matriz);
    rendering(configs, matriz, inteiros, c_bolha_mover);
  }
  SDL_Quit();

  regista_resultado(inteiros);

  return 0;
}
