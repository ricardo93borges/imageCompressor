#include <math.h>
#include <string.h>		// para usar strings
#include <limits.h>
#include <float.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Variáveis globais a serem utilizadas:

// Dimensões da imagem de entrada
int sizeX, sizeY;

// Imagem de entrada
Image24 image;

// Imagem de 8 bits
Image8 image8;

// Imagem RLE
ImageRLE imageRLE;

// Modo de exibição atual
int modo;

// Função principal de processamento: ela deve chamar outras funções
// quando for necessário
void process()
{
    //
    // EXEMPLO: preenche a imagem com pixels de cores diversas
    //
    //
    // SUBSTITUA este código pelos algoritmos a serem implementados
    //
    int pos = 0;
    unsigned char r, g, b;
    r = 255; g = 0; b = 0;
    /*for(int y=0; y<sizeY; y++) {
        if (y>sizeY/4)
            g = 127;
        if (y>sizeY/2)
            b = 127;
        if (y>sizeY-sizeY/4)
        {
            r = 0;
        }
        for(int x=0; x<sizeX; x++) {
            image.pixels[pos].r = r;
            image.pixels[pos].g = g;
            image.pixels[pos].b = b;
            pos++;
        }
    }*/

    buildImage8();

    pos = 0;
    r = 0;
    /*for(int y=0; y<sizeY; y++) {
        if (y>sizeY/4)
            r = 1;
        if (y>sizeY/2)
            r = 2;
        if (y>sizeY-sizeY/4)
        {
            r = 3;
        }
        for(int x=0; x<sizeX; x++) {
            image8.pixels[pos] = r;
            pos++;
        }
    }
    // Exemplo: imagem de 8 bits (com outras cores, para testar)
    image8.pal[0].r = 80;
    image8.pal[0].g = 0;
    image8.pal[0].b = 0;
    image8.pal[1].r = 127;
    image8.pal[1].g = 0;
    image8.pal[1].b = 0;
    image8.pal[2].r = 192;
    image8.pal[2].g = 0;
    image8.pal[2].b = 0;
    image8.pal[3].r = 255;
    image8.pal[3].g = 0;
    image8.pal[3].b = 0;*/

    // Exemplo: imagem RLE

    // Copia a palheta da imagem de 8 bits
    memcpy(imageRLE.pal, image8.pal, 256 * 3);

    // Aloca mem. para a imagem RLE (e' muito mais do que precisa, mas assim simplifica)
    imageRLE.data = (unsigned char*) malloc(sizeX * sizeY);

    // Gera 600 linhas na imagem RLE (a cada 50, troca a cor)
    // Cada linha cheia ocupa 8 bytes + 2 no final para marcar o fim da imagem
    unsigned char auxdata[4802];
    pos = 0;
    int y = 0;
    int index = 0;
    while(y < 600) {
        // Faz uma linha na imagem (255+255+255+35 => 800)
        auxdata[pos]   = index;
        auxdata[pos+1] = 255;
        auxdata[pos+2] = index;
        auxdata[pos+3] = 255;
        auxdata[pos+4] = index;
        auxdata[pos+5] = 255;
        auxdata[pos+6] = index;
        auxdata[pos+7] = 35;
        pos+=8;
        y++;
        if(y%50 == 0) {
           // Aumenta o índice da cor, volta para 0 quando passar de 3
           index = (index +1) % 4;
        }
    }
    // Marca o fim com dois zeros
    auxdata[pos] = 0;
    auxdata[pos+1] = 0;

    // Visualizando os primeiros 128 bytes...
    printf("Pos: %d\n", pos);
    for(pos=0; pos<128; pos++) {
        if(pos%16==0)
            printf("\n%04x: ",pos);
        printf("%02X ", auxdata[pos]);
    }
    printf("\n");

    // Copia para o buffer da imagem
    memcpy(imageRLE.data, auxdata, sizeof(auxdata));

    //
    // NÃO ALTERAR A PARTIR DAQUI!!!!
    //
    buildTex();
}

void buildImage8(){
    //Verifica a frequencia de uso de cada cor da imagem
    int totalPixels = sizeX * sizeY;//sizeof(RGB8) * sizeX * sizeY;
    /*int cores;
    cores = malloc ((sizeof(int) * totalPixels) * 4);
    cores[totalPixels][4];*/

    typedef struct {
        int r,g,b;
        int frequencia;
    } CorFreq;

    CorFreq *cores;
    cores = malloc (sizeof(CorFreq) * totalPixels);

    int exists;
    int coresPos = 0;
    for(int i=0; i < totalPixels; i++){
        exists = 0;
        //int coresSize = sizeof(cores)/sizeof(CorFreq);
        for(int j=0; j <= coresPos; j++){
            if(cores[j].r == image.pixels[i].r && cores[j].g == image.pixels[i].g && cores[j].b == image.pixels[i].b){
                cores[j].frequencia++;
                exists = 1;
                break;
            }
        }

        if(exists == 0){
            cores[coresPos].r = image.pixels[i].r;
            cores[coresPos].g = image.pixels[i].g;
            cores[coresPos].b = image.pixels[i].b;
            cores[coresPos].frequencia = 1;
            coresPos++;
        }
    }


    //ordena as cores por frequencia
    //int n = sizeof(cores) / sizeof(cores[0]);
    int k = coresPos - 1;
    for(int i = 0; i < coresPos; i++){
        for(int j = 0; j < k; j++){
            if(cores[j].frequencia < cores[j+1].frequencia){
                CorFreq aux = cores[j];
                cores[j] = cores[j+1];
                cores[j+1] = aux;
            }
         }
         k--;
    }

    //Insere as 16 cores mais usadas na paleta da imagem8
    for(int i=0; i<16; i++){
        image8.pal[i].r = cores[i].r;
        image8.pal[i].g = cores[i].g;
        image8.pal[i].b = cores[i].b;
        //printf("\n pal[%d] : %d %d %d %d", i, cores[i].r, cores[i].g, cores[i].b, cores[i].frequencia);
        //printf("\n");
    }

    free(cores);

    /*for(int i=0; i<=16; i++){
        printf("\n %d %d %d", image8.pal[i].r, image8.pal[i].g, image8.pal[i].b);
    }*/

    //Preenche a image8 com as cores mais proximas da image original
    double distancia = 15284715;
    int palPos = 0;
    double ed = 0;
    for(int i=0; i < totalPixels; i++){
        for(int j=0; j < 16; j++){
            ed = (double)sqrt(
                         pow(image.pixels[i].r-image8.pal[j].r,2)+
                         pow(image.pixels[i].g-image8.pal[j].g,2)+
                         pow(image.pixels[i].b-image8.pal[j].b,2));

            if(ed > distancia){
                distancia = ed;
                palPos = j;
                printf("\n pp: %d -- %d", palPos, ed);
            }
        }

        image8.pixels[i] = distancia;
        image.pixels[i].r = image8.pal[palPos].r;
        image.pixels[i].g = image8.pal[palPos].g;
        image.pixels[i].b = image8.pal[palPos].b;
    }

}

int main(int argc, char** argv)
{
    if(argc==1) {
        printf("rleconv [image file.ppm]\n");
        exit(1);
    }

    // Inicialização da janela gráfica
    init(argc,argv);

    //
    // INCLUA aqui o código para LER a imagem de entrada
    //
    // Siga as orientações no enunciado para:
    //
    // 1. Descobrir o tamanho da imagem (ler header)
    // 2. Ler os pixels
    //

    char fileFormat[2];
    FILE *fp;
    char buff[16];
    int valorMaxCor;
    fp = fopen("facin.ppm","r");

    //linha 1 cores
    fscanf(fp, "%s", fileFormat);
    //linha 2 width height
    fscanf(fp, "%d %d", &sizeX, &sizeY);
    //linha 3 valor maximo de cada cor
    fscanf(fp, "%d\n", &valorMaxCor);

    //Atribui valores a image24 e aloca memoria para armazenar os pixels
    image.width  = sizeX;
    image.height = sizeY;
    image.pixels = (RGB8*) malloc(sizeof(RGB8) * sizeX * sizeY);
    //printf("%d x %d - %d", image.width,image.height,sizeof(image.pixels));

    //Le imagem para a memoria
    int totalPixels = sizeof(RGB8) * sizeX * sizeY;
    fread(image.pixels, totalPixels, 1, fp);

    /*for(int i=0; i < totalPixels; i++){
        printf("\n%d %d %d", image.pixels[i].r, image.pixels[i].g, image.pixels[i].b);
    }*/

    fclose(fp);

    modo = IMG24;

    //medianCut();

    // Aplica processamento inicial
   // process();

    //return 0;
    /**********************************************/

    // TESTE: cria uma imagem de 800x600
    //sizeX = 800;
    //sizeY = 600;

    //printf("%d x %d\n", sizeX, sizeY);

    // Aloca memória para imagem de 24 bits
    /*image.width  = sizeX;
    image.height = sizeY;
    image.pixels = (RGB8*) malloc(sizeof(RGB8) * sizeX * sizeY);
    */
    // Aloca memória para imagem de 8 bits
    image8.width  = sizeX;
    image8.height = sizeY;
    image8.pixels = (unsigned char*) malloc(sizeX * sizeY);

    modo = IMG24;

    // Aplica processamento inicial
    process();

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}
