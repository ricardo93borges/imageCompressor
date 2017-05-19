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

    buildImage8();//CONSTROI IMAGEM 8BITS

    pos = 0;
    r = 0;
    // Copia a palheta da imagem de 8 bits
    memcpy(imageRLE.pal, image8.pal, 256 * 3);

    // Aloca mem. para a imagem RLE (e' muito mais do que precisa, mas assim simplifica)
    imageRLE.data = (unsigned char*) malloc(sizeX * sizeY);

    /*IMAGE RLE*/
    int image8PixelsSize = sizeX*sizeY;
    unsigned char auxdata[image8PixelsSize];

    int c; // QUANTIDADE DE CORES QUE JÁ PASSARAM
    int k = 0; // SOMA DAS POSIÇÕES QUE JÁ PASSARAM
    int i = 0;
    pos = 0;
    int j = 0;
    while(i < image8PixelsSize){
        c = 0;
        auxdata[pos] = image8.pixels[i];
        auxdata[pos+1] = 0;
        while(auxdata[pos] == image8.pixels[i]){
            c++;
            i+=2;
        }
        k+=c;
        auxdata[pos+1] = c;
        pos += 2;
        i = k;
    }

    // Copia para o buffer da imagem
    memcpy(imageRLE.data, auxdata, sizeof(auxdata));

    imageRLE.data[pos] = 0;
    imageRLE.data[pos+1] = 0;
    //
    // NÃO ALTERAR A PARTIR DAQUI!!!!
    //
    buildTex();
}
/*
    Constroi image8

    -Escolhe as cores mais frequentes
    -Aplica a formula distancia euclidiana
*/
void buildImage8(){
    //Verifica a frequencia de uso de cada cor da imagem
    int totalPixels = sizeX * sizeY;//sizeof(RGB8) * sizeX * sizeY;

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
    for(int i=0; i<15; i++){
        image8.pal[i].r = cores[i].r;
        image8.pal[i].g = cores[i].g;
        image8.pal[i].b = cores[i].b;
    }

    //alocar memória
    free(cores);

    //Preenche a image8 com as cores mais proximas da image original
    double distancia = 99999999;
    int palPos = 0;
    double ed = 0;
    //euclides esta aqui
    for(int i=0; i < totalPixels; i++){
        distancia=999999;
        for(int j=0; j < 15; j++){
            ed = (double)sqrt(
                         pow(image.pixels[i].r-image8.pal[j].r,2)+
                         pow(image.pixels[i].g-image8.pal[j].g,2)+
                         pow(image.pixels[i].b-image8.pal[j].b,2));

            if(ed < distancia){
                distancia = ed;
                palPos = j;
            }
        }
        image8.pixels[i] = palPos;
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

    //Le imagem para a memoria
    int totalPixels = sizeof(RGB8) * sizeX * sizeY;
    fread(image.pixels, totalPixels, 1, fp);

    fclose(fp);

    modo = IMG24;

    // Aloca memória para imagem de 8 bits
    image8.width  = sizeX;
    image8.height = sizeY;
    image8.pixels = (unsigned char*) malloc(sizeX * sizeY);
    // Aplica processamento inicial
    process();
    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}
