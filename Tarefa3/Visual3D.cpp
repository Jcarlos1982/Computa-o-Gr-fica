#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <sstream>
#include <vector>
#include <math.h>
#include<time.h>//necessário p/ função time()


#include <fstream> //para leitura de arquivos texto
using std::string;


using namespace std;



// Variáveis
int qtVertices=0, qtFaces=0; //usadas para criação dos vetores para guardar vertices e faces
int qtPropriedades =0;

char **fileList; //Variáveis para arquivos
char texto[100];

string nome, res, param;
char nomeArquivo[100];
char resultado[100];



GLfloat win, r, g, b, angRot=0, angulo=0, fAspect,  escala=1, posX=0, posY=0, nx, ny, nz;
GLint view_w, view_h, obj=0, eixo=2;

//luzes da cena - inicial
GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0}; 
GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   // "cor" 
GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho" 
GLfloat posicaoLuz[4]={500.0, 500.0, -500.0, 1.0};


//posições da cena
// POSICAO INICIAL - gluLookAt(0,80,200, 0,0,0, 0,1,0);
GLdouble camX=0, camY=80, camZ=200, alvoX=0, alvoY=0, alvoZ=0, cimaX=0, cimaY=1, cimaZ=0, zNear=0.4;
GLint cena=0, zFar=500;
GLdouble corpadrao[3];
double maiorR=0;
int rodadas=0, iteracoes=10000, tpMedia=0;

bool solido = false, solido1 = false, transl=false, rot=false, luz0=true, luz1=true, orig = true , malha1 = false, malha2 = false, temp=true, temp2=true, colorido=false;


//parametros da divisão da tela
int larg = 1366, alt = 768;

//histograma
// Vetores para guardar a quantidade de triangulos com razão de aspecto de 0.0 a 1.0, variando em 0.1 a cada posição
long long vetO[9]; //Malha original
long long vetN[9]; //Malha nova | Esse vetor vai mudar se o modo de triangulação for modificado e um histograma for requisitado para ser mostrado na tela

bool flagH = false; //Mostra ou não mostra histograma

//Estruturas para representar os dados dos arquivos ply

struct vertice{
    double x, y, z, nx, ny, nz ; //Temos objetos com mais propriedades porém trabalharemos apenas com estas
};

struct face{ //Triangulo

    double p1, p2, p3; //posições dos vértices no vetor de vértives
    double l1, l2, l3; //lados do triangulo
    double Area; //área do triângulo (Módulo do Determinante dividido por 2)
    double rAspecto; //razão de aspecto do trinnângulo (entre 0 e 1, sendo que mais próximo de 1 é melhor)
    bool RV = false; //Razao válida (  0.7 < RV < 1 )
    vertice D[3]; //vértices do vetor diretor de cada reta

     //Função para calcular os lados do triangulo (raiz quadrada da soma dos quadrados da diferença)
     void calculaLados(vector<vertice> v ){
          l1 = sqrt( pow(v[p2].x - v[p1].x, 2 ) + pow( v[p2].y - v[p1].y, 2 ) + pow( v[p2].z - v[p1].z, 2 ) ) ;
          l2 = sqrt( pow(v[p3].x - v[p2].x, 2 ) + pow( v[p3].y - v[p2].y, 2 ) + pow( v[p3].z - v[p2].z, 2 ) ) ;
          l3 = sqrt( pow(v[p3].x - v[p1].x, 2 ) + pow( v[p3].y - v[p1].y, 2 ) + pow( v[p3].z - v[p1].z, 2 ) ) ;
          
     }

     //calcula pontos do vetor diretor 
     void calculaD(vector<vertice> v ){
          //face 1 (p1->p2)
          D[0].x = (v[p2].x - v[p1].x);    D[0].y = (v[p2].y - v[p1].y);   D[0].z = (v[p2].z - v[p1].z);
          //face 2 (p1->p3;)
          D[1].x = (v[p3].x - v[p1].x);    D[1].y = (v[p3].y - v[p1].y);   D[1].z = (v[p3].z - v[p1].z);
          //face 3 (p2->p3)
          D[2].x = (v[p3].x - v[p2].x);    D[2].y = (v[p3].y - v[p2].y);   D[2].z = (v[p3].z - v[p2].z);

          //cout<<"m1 "<<D[0].x<<" "<<D[0].y<<" "<<D[0].z<<endl;
          //cout<<"m2 "<<D[1].x<<" "<<D[1].y<<" "<<D[1].z<<endl;
          //cout<<"m3 "<<D[2].x<<" "<<D[2].y<<" "<<D[2].z<<endl<<endl;
     }


     //Função para calcular a área do triangulo (face)
     void calculaArea(){ //area = raiz ( p*(p-a)*(p-b)*(p-c) ), onde p=(a+b+c)/2, a, b e c são os lados
          
          double p = (l1+l2+l3)/2;
          Area = abs(sqrt(p*(p-l1)*(p-l2)*(p-l3)));    
      
     }

   


     //Função para calcular a razão de aspecto do triangulo (face)
     void calculaAspecto(){ //Prof. Leandro (4 x raiz de 3 x área / pela soma dos quadrados dos lados)

          rAspecto = (4 * sqrt(3) * Area ) / (l1*l1 + l2*l2 + l3*l3);

          if( rAspecto >= 0.7 && rAspecto <= 1){ //valida razão de aspecto
               RV = true;
          }
    
     }

};

//Cria os vetores com os tamanhos dos dados de vertices e faces lidas
 vector<vertice> v ; // vértices da triangulacao
 vector<face> f, f2 ; //f2 será usado para receber as faces da nova triangulacao


//Função para criar vértice a partir do vetor diretor da reta e o ponto inicial
vertice criaVertice(face FC, int reta){

     vertice vt; 

     vertice A, B, C; //vértices do triangulo original
     A = v[FC.p1]; B = v[FC.p2]; C=v[FC.p3]; 

     //K - parâmetro utilizado para deslocar os pontos na reta
     double k = (rand()/(RAND_MAX+0.0)); 
   
     
     //cria o vértice
     if(reta == 1){ //A -> B
          vt.x = A.x + FC.D[0].x * k ;
          vt.y = A.y + FC.D[0].y * k ;
          vt.z = A.z + FC.D[0].z * k ;

          

     }else if(reta == 2){ //A -> C
          vt.x = A.x + FC.D[1].x * k ;
          vt.y = A.y + FC.D[1].y * k ;
          vt.z = A.z + FC.D[1].z * k ;

        

     }else{ //B -> C
          vt.x = B.x + FC.D[2].x * k ;
          vt.y = B.y + FC.D[2].y * k ;
          vt.z = B.z + FC.D[2].z * k ;

        

     }
     
     //cout<<"VT Gerado "<<vt.x<<"  |  "<<vt.y<<"  |  "<<vt.z<<endl;
     
     return vt; //retorna o vértice criado

}

//Calcula o vetor normal de uma face - ESTOU USANDO ESSA FUNÇÃO APENAS PARA AS NOVAS FACES CRIADAS
void calculaNormal( face FC )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = v[FC.p1].x - v[FC.p2].x;
   va[1] = v[FC.p1].y - v[FC.p2].y;
   va[2] = v[FC.p1].z - v[FC.p2].z;
 
   vb[0] = v[FC.p1].x - v[FC.p3].x;
   vb[1] = v[FC.p1].y - v[FC.p3].y;
   vb[2] = v[FC.p1].z - v[FC.p3].z;
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	
	nx = vr[0]/val;
	ny = vr[1]/val;
	nz = vr[2]/val;
 
}


//Função para criar o novo vértice (ponto)
bool criaFaces(face FC, vertice v1, vertice v2, vertice v3 ){
    
     //insere no vetor
     v.push_back(v1); //(v.size() - 3) - Pos 1
     v.push_back(v2); //(v.size() - 2) - Pos 2
     v.push_back(v3); //(v.size() - 1) - Pos 3
     
     face F[4];

     //Face 1
     F[0].p1 = FC.p1;
     F[0].p2 = v.size() - 3; 
     F[0].p3 = v.size() - 2; 

     
     //guarda normais do original
     v[F[0].p2].nx = v[FC.p2].nx; v[F[0].p2].ny = v[FC.p2].ny; v[F[0].p2].nz = v[FC.p2].nz;
     v[F[0].p3].nx = v[FC.p3].nx; v[F[0].p3].ny = v[FC.p3].ny; v[F[0].p3].nz = v[FC.p3].nz;
       
     
       
     //Face 2
     F[1].p1 = FC.p2;
     F[1].p2 = v.size() - 3; 
     F[1].p3 = v.size() - 1; 

     //guarda normais do original
     v[F[1].p2].nx = v[FC.p2].nx; v[F[1].p2].ny = v[FC.p2].ny; v[F[1].p2].nz = v[FC.p2].nz;
     v[F[1].p3].nx = v[FC.p3].nx; v[F[1].p3].ny = v[FC.p3].ny; v[F[1].p3].nz = v[FC.p3].nz;

     
     //Face 3
     F[2].p1 = FC.p3;
     F[2].p2 = v.size() - 2; 
     F[2].p3 = v.size() - 1; 

     //guarda normais do original
     v[F[2].p2].nx = v[FC.p2].nx; v[F[2].p2].ny = v[FC.p2].ny; v[F[2].p2].nz = v[FC.p2].nz;
     v[F[2].p3].nx = v[FC.p3].nx; v[F[2].p3].ny = v[FC.p3].ny; v[F[2].p3].nz = v[FC.p3].nz;

     //Face 4
     F[3].p1 = v.size() - 3; 
     F[3].p2 = v.size() - 2; 
     F[3].p3 = v.size() - 1; 

     //guarda normais do original
     v[F[3].p1].nx = v[FC.p1].nx; v[F[3].p1].ny = v[FC.p1].ny; v[F[3].p1].nz = v[FC.p1].nz;
     v[F[3].p2].nx = v[FC.p2].nx; v[F[3].p2].ny = v[FC.p2].ny; v[F[3].p2].nz = v[FC.p2].nz;
     v[F[3].p3].nx = v[FC.p3].nx; v[F[3].p3].ny = v[FC.p3].ny; v[F[3].p3].nz = v[FC.p3].nz;
    
    /* EXCLUIR DEPOIS
     for(int i=0; i < vtemp.size(); i++){
          cout<<"VTemp Gerado "<<vtemp[i].x<<"  |  "<<vtemp[i].y<<"  |  "<<vtemp[i].z<<endl;
          
     }
     exit(EXIT_FAILURE);
     */
     //Calcula os lados, a área e a razão de aspecto das faces criadas
     //double somaASP=0;
     for(int i=0; i< 4; i++){
          F[i].calculaLados(v);
          F[i].calculaArea();
          F[i].calculaAspecto(); 
          
          //cout<<"Face "<<i<<" X "<<v[F[i].p1].x<<" Y "<<v[F[i].p1].y<<" Z "<<v[F[i].p1].z<<endl;
          //cout<<"       X "<<v[F[i].p2].x<<" Y "<<v[F[i].p2].y<<" Z "<<v[F[i].p2].z<<endl;
          //cout<<"       X "<<v[F[i].p3].x<<" Y "<<v[F[i].p3].y<<" Z "<<v[F[i].p3].z<<endl<<endl;
          //cout<<"lados "<<F[i].l1<<"  "<<F[i].l2<<"  "<<F[i].l3<<"    Area    "<<F[i].Area<<" aspcc  "<<F[i].rAspecto<<"   ASP  ORIG "<<FC.rAspecto<<endl;

          //cout<<"ASP "<<F[i].rAspecto<<" RASP "<<FC.rAspecto<<endl;   
         
     }
     
     

     //Calcula a média aritmética das razões de aspecto das novas faces
     if(tpMedia == 0){
          double mediaAspecto = ( F[0].rAspecto + F[1].rAspecto + F[2].rAspecto + F[3].rAspecto)/4; 
                   
     }
     //média geométrica das razões de aspecto das novas faces
     if(tpMedia == 1){
          double mediaAspecto = pow( (F[0].rAspecto * F[1].rAspecto * F[2].rAspecto * F[3].rAspecto ), 1.0/4.0); 
          
     }
     //maior valor gerado será a média se esta for maior que o original 
     double mediaAspecto = F[0].rAspecto;
     if(tpMedia == 2){
          if(mediaAspecto  > F[1].rAspecto) mediaAspecto = F[1].rAspecto;
          if(mediaAspecto  > F[2].rAspecto) mediaAspecto = F[2].rAspecto;
          if(mediaAspecto  > F[3].rAspecto) mediaAspecto = F[3].rAspecto;
     }
     
  

     if(maiorR < mediaAspecto) {
          maiorR = mediaAspecto; //guarda o melhor até agora
     
          rodadas = 0;
          
     }
     rodadas++;
    
          //cout<<"medias 1 " <<F[0].rAspecto<<"  2   "<<F[1].rAspecto<<" 3  "<<F[2].rAspecto<<"  4  "<<F[3].rAspecto<<"   ORIG   "<<FC.rAspecto<<" GERADA  "<<mediaAspecto<<endl;
     

     int media, aspecto;
     media = mediaAspecto *1000;
     aspecto = FC.rAspecto*1000;

     //cout<<"Tam f  "<<f.size()<<"    Tam f2 "<<f2.size()<<"    tam v  "<<v.size()<<endl;

     if(media <= aspecto){ //Se a média não superar a razão original mantém o triangulo original
          //cout<<"   Rodadas "<<rodadas; 
         
          if(rodadas > iteracoes){
                                    
                    f2.push_back(FC); //guarda a face
                    v.pop_back(); //exclui os vértices criados
                    v.pop_back();
                    v.pop_back();
                    return true;
                
          }
          v.pop_back();
          v.pop_back();
          v.pop_back();
          
          return false;
    
         
     }else{ //Se a média gerada for melhor devemos inserir as novas faces na trianguulação final
          //cout<<"medias " <<F[0].rAspecto<<"     "<<F[1].rAspecto<<"   "<<F[2].rAspecto<<"  "<<F[3].rAspecto<<"   ORIG   "<<FC.rAspecto<<"  MENOR GERADA  "<<mediaAspecto<<endl;

          for(int i=0; i< 4; i++){
        
               f2.push_back(F[i]); //guarda a face
                    
          } 
          
     }
     return true;
     
}


//Função para criar a melhor triangulação
void triangula(){
          
          for (int i=0; i< f.size(); i++){ 

               if(f[i].RV){ //se a razão de aspecto for válida
                 
                    f2.push_back(f[i]); //guarda a face    
                              
                
               }else{ //se a razão não for válida
                    bool ok = false;
                    while (ok == false){ //Tenta encontrar um razão melhor até o máximo de rodadas 
                        
                        ok = criaFaces(f[i], criaVertice(f[i], 1), criaVertice(f[i], 2), criaVertice(f[i], 3));
                        
                    }
                    
               }              
               
          }
          /*
          for(int i=0; i<f2.size(); i++){ 
               f2[i].calculaArea();
               f2[i].calculaAspecto(); 
          }
          */
}

//Função para escrever arquivo da imagem com a nova triangulação
void criaArquivo(){
     ofstream f_out; //nomeArquivo.c_str(), ios::binary 
     
     nomeArquivo[0] = 'T'; nomeArquivo[1] = 'N'; nomeArquivo[2] = '_'; //TN_(Triangulação nova))
     for(int i=0; i<nome.size(); i++){
          nomeArquivo[i+3] = nome[i];
     }

    f_out.open(nomeArquivo); //Cria arquivo com novo nome

    if(! f_out.good())
        cout<<"falha ao criar arquivo de Triagulacao"<<endl;
    else cout << nomeArquivo<<" criado! (Nova Triangulação)"<<endl;

    //Escrever arquivo ply
         /* Cabeçalho
               ply
               format ascii 1.0
               element vertex 6906
               property float x
               property float y
               property float z
               property float nx
               property float ny
               property float nz
               element face 13594
               property list uchar uint vertex_indices
               end_header

         */
     
          f_out<<"ply"<<endl;                         
          f_out<<"format ascii 1.0"<<endl;               
          f_out<<"element vertex "<<v.size()<<endl;              
          f_out<<"property float x "<<endl;               
          f_out<<"property float y"<<endl;
          f_out<<"property float z"<<endl;
          f_out<<"property float nx"<<endl;
          f_out<<"property float ny"<<endl;
          f_out<<"property float nz"<<endl;
          f_out<<"element face "<<f2.size()<<endl;
          f_out<<"property list uchar uint vertex_indices"<<endl;
          f_out<<"end_header"<<endl;
          
          //vértices
          for (int i=0; i< v.size(); i++){
               f_out<<v[i].x<<" "<<v[i].y<<" "<<v[i].z<<" "<<v[i].nx<<" "<<v[i].ny<<" "<<v[i].nz<<endl;
          }
          
               //faces
          for (int i=0; i< f2.size(); i++){
               f_out<<"3 "<<f2[i].p1<<" "<<f2[i].p2<<" "<<f2[i].p3<<endl;
          }
         

    f_out.close();

}



//Função para escrever arquivo com os resultados obtidos
void criaArquivoResultado(){
    ofstream f_out;

    nomeArquivo[0] = 'T'; 
    nomeArquivo[1] = 'N'; 
    nomeArquivo[2] = '_'; 
    nomeArquivo[3] = 'R';
    nomeArquivo[4] = 'e'; 
    nomeArquivo[5] = 's'; 
    nomeArquivo[6] = '_'; //TN_Res_(Resultados obitidos da Triangulação nova))
     for(int i=0; i<nome.size(); i++){
          nomeArquivo[i+7] = nome[i];
     }

    f_out.open(nomeArquivo);

    if(! f_out.good())
        cout<<"falha ao criar arquivo de Resultado"<<endl;
    else cout << nomeArquivo<<" criado! (Resultados)"<<endl;

     double VO, VN, FO, FN, OV, OI, NV, NI; //Valores de triangulação - Origem e Novo
          
          int val = 0; int inval =0;
          for (int i=0; i< qtFaces; i++){

               if(f[i].RV){
                    val++;
               }else{
                     inval++;
               }
                    
          }
         //Dados da imagem original
         VO = qtVertices; FO = qtFaces; OV = val; OI = inval;

         val = 0; inval = 0;
         for (int i=0; i< f2.size(); i++){
               f2[i].calculaLados(v);
               f2[i].calculaArea();
               f2[i].calculaAspecto();
               if(f2[i].RV){
                    val++;
               }else{
                     inval++;
               }
                    
          }


          //Dados da imagem nova
         VN = v.size(); FN = f2.size(); NV = val; NI = inval;
          
          f_out<<"Trabalho Final - Melhorar triangulação de imagem.ply"<<endl;                         
          f_out<<"Computação Gráfica"<<endl;               
          f_out<<"Prof. Leandro Souza "<<endl;              
          f_out<<"Alunos: Andre , Igo Joctan e José Carlos "<<endl<<endl<<endl;               


          f_out<<"Imagem Original"<<endl<<endl;
          f_out<<"Quantidade de vértices: "<<VO<<endl;
          f_out<<"Quantidade de Faces: "<<FO<<endl;
          f_out<<"Triangulos válidos (razão de aspecto entre 0.7 e 1.0): "<<OV<<endl;
          f_out<<"Triangulos inválidos (razão de aspecto menor que 0.7): "<<OI<<endl;
          
          double pvO = OV / FO * 100; //percentual válido do original
          pvO = round(pvO * 100)/100;  
          double piO = OI / FO * 100; //percentual inválido do original
          piO = round(piO * 100)/100;
          f_out<<"Percentual de triangulos válidos: "<<pvO<<"%"<<endl;
          f_out<<"Percentual de triangulos inválidos: "<<piO<<"%"<<endl;


          f_out<<endl<<endl<<"Imagem Gerada"<<endl<<endl;
          f_out<<"Quantidade de vértices: "<<VN<<endl;
          f_out<<"Quantidade de Faces: "<<FN<<endl;
          f_out<<"Triangulos válidos (razão de aspecto entre 0.7 e 1.0): "<<NV<<endl;
          f_out<<"Triangulos inválidos (razão de aspecto menor que 0.7): "<<NI<<endl;
          
          double pvN = NV / FN * 100; //percentual válido do original
          pvN = round(pvN * 100)/100;
          double piN = NI / FN * 100; //percentual inválido do original
          piN = round(piN * 100)/100;
          f_out<<"Percentual de triangulos válidos: "<<pvN<<"%"<<endl;
          f_out<<"Percentual de triangulos inválidos: "<<piN<<"%"<<endl;

          f_out<<endl<<endl<<"Conclusões"<<endl;

          f_out<<endl<<"Quantidade de novos triângulos: "<<FN - FO<<" ";
          f_out<<"(substituição de "<<round(FN - FO) / 3 <<" dos "<< OI<<" triângulos inválidos)"<<endl;

          double pA = (FN - FO) / FO * 100; //percentual de aumento de faces
          pA = round(pA * 100)/100;

          f_out<<"Percentual de aumento de faces: "<<pA<<"%"<<endl;

          double R = pvN - pvO;

          

          if(R < 0){
               f_out<<"Perda de "<<R*(-1)<<"% na nova triangulação. "<<endl;
               std::ostringstream buffer;
               buffer  <<" (-) "<<R*(-1)<< "%.";
               res =  buffer.str();
          }else{
               f_out<<"Ganho de "<<R<<"% na nova triangulacao.  "<<endl<<endl;
               std::ostringstream buffer;
               buffer  <<" (+) "<<R<<"%.";
               res = buffer.str();
          }

          f_out<<"                             Fim                             ";



         

    f_out.close();

}



//Aplica escala
void aplicaEscala(GLdouble escala){
     glScalef(escala, escala, escala);
}

//aplica rotação
void aplicaRotacao(GLdouble angRot, GLint eixo){
          

          switch (eixo){
          case 0: 
                    glRotated(angRot, 1.0, 0.0, 0.0);                    
               break;

          case 1:
                   glRotated(angRot, 0.0, 1.0, 0.0);
              break;


          case 2:
                    glRotated(angRot, 0.0, 0.0, 1.0);
               break;
     }

}



// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
     
	// Especifica sistema de coordenadas de projeção
         
	glMatrixMode(GL_PROJECTION);
     glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0); 
	glLoadIdentity();
     
     

     // Especifica a projeção perspectiva
     gluPerspective(angulo,fAspect,zNear,zFar);

          // Especifica posição do observador e do alvo
     gluLookAt(camX,camY,camZ, alvoX, alvoY,alvoZ, cimaX,cimaY,cimaZ);
   
     aplicaEscala(escala);

     if(transl){ //se a translação estiver habilitada
         glTranslatef(posX, posY, 0.0); 
        
     }

     if (rot){ //se a rotação estiver habilitada
          aplicaRotacao(angRot, eixo);
     }

  
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();   
     	
}

// Desenha um texto na janela GLUT - passa texto e posição
void DesenhaTexto(char *string, double x, double y) 
{  
  	glPushMatrix();
       
        // Posição no universo onde o texto será colocado   
             
        glRasterPos2f(x, y); 
        // Exibe caracter a caracter
        while(*string)
        
             glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++); 
	glPopMatrix();
}

// Função callback chamada para fazer o desenho
void Desenha()
{   
    glClear(GL_COLOR_BUFFER_BIT);

    if(flagH)
    {
             glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
     gluOrtho2D(0,1000,0,1000);


        glColor3f(0.0, 0.0, 0.0);
        
        DesenhaTexto("0.1", -1500, -100);
        DesenhaTexto("0.2", -1350, -100);
        DesenhaTexto("0.3", -1200, -100);
        DesenhaTexto("0.4", -1050, -100);
        DesenhaTexto("0.5", -900, -100);
        DesenhaTexto("0.6", -750, -100);
        DesenhaTexto("0.7", -600, -100);
        DesenhaTexto("0.8", -450, -100);
        DesenhaTexto("0.9", -300, -100);
        DesenhaTexto("1.0", -150, -100);

        DesenhaTexto("0.1", 0, -100);
        DesenhaTexto("0.2", 150, -100);
        DesenhaTexto("0.3", 300, -100);
        DesenhaTexto("0.4", 450, -100);
        DesenhaTexto("0.5", 600, -100);
        DesenhaTexto("0.6", 750, -100);
        DesenhaTexto("0.7", 900, -100);
        DesenhaTexto("0.8", 1050, -100);
        DesenhaTexto("0.9", 1200, -100);
        DesenhaTexto("1.0", 1350, -100);

        
        int largura = 100;
        int base = -1500;
        int dist = 0;

        glBegin(GL_LINES);
            glVertex2f(base, 0);
            glVertex2f(base, 1000);

            glVertex2f(base,0);
            glVertex2f(base+1450,0);
        glEnd();

        DesenhaTexto("Malha Original", base, 1300);

        for(int i = 0; i<10; ++i, dist += 150)
        {
            glColor3f(rand()/(RAND_MAX +1.0), rand()/(RAND_MAX +1.0), rand()/(RAND_MAX +1.0));
            glRectf(base+dist,0,base+dist+largura,vetO[i]/5);

            char * x;
            

            

            vetO[i] = 0; //Aproveitando o loop de exibição para limpar o vetor, para que ele não tenha lixo para a próxima rodada
        }

        base = 0;
        dist = 0;

        glBegin(GL_LINES);
            glVertex2f(base, 0);
            glVertex2f(base, 1000);

            glVertex2f(base,0);
            glVertex2f(base+1450,0);
        glEnd();

        DesenhaTexto("Malha Modificada", base, 1300);


        for(int i = 0; i<10; ++i, dist += 150)
        {
            glColor3f(rand()/(RAND_MAX +1.0), rand()/(RAND_MAX +1.0), rand()/(RAND_MAX +1.0));
           
            glRectf(base+dist,0,base+dist+largura,vetN[i]/5);
            vetN[i] = 0; //Aproveitando o loop de exibição para limpar o vetor, para que ele não tenha lixo para a próxima rodada
        }
        
     
          
        
    
        flagH = false;
        
        /*
        cemitério de código maldito
        glMatrixMode(GL_VIEWPORT);
        glViewport(0,0,larg/2+larg/4,alt);
        glScissor(0,0,larg/2+larg/4,alt);
        glEnable(GL_SCISSOR_TEST);

        //glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_VIEWPORT);
        glViewport(larg/2+larg/4,0,larg/4,alt);

        glScissor(larg/2+larg/4,0,larg/4,alt);
        glEnable(GL_SCISSOR_TEST);
        */
    }
    else
    {
        EspecificaParametrosVisualizacao();

        

     glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
     

     glLineWidth(1.0); //largura da linha
          if(orig){
               
               for (int i=0; i< qtFaces; i++){
                    /*
                    calculaNormal(f[i]); //Calcula os vetores normais de cada face
                    //ESSA FUNÇÃO NÃO ESTÁ SENDO USADA PORQUÊ ESTOU ADICIONANDO OS VETORES NORMAIS NO BLENDER
                    
                    //guarda os resultados dos normais
                    f[i].p1.nx = nx; f[i].p1.ny = ny; f[i].p1.nz = nz; 
                    f[i].p2.nx = nx; f[i].p2.ny = ny; f[i].p2.nz = nz; 
                    f[i].p3.nx = nx; f[i].p3.ny = ny; f[i].p3.nz = nz; 
                    */
                    
                    glColor3f(r, g, b); //cor da primeira malha
                    
                    if (solido)
                         glBegin(GL_POLYGON);	//sólido
                    else
                         glBegin(GL_LINE_LOOP);		//malha

                              if(colorido){
                                   if(f[i].rAspecto >= 0.0 && f[i].rAspecto < 0.7 ) {
                                        r=1; g=0;b=0;
                                   }else if(f[i].rAspecto >= 1.25 && f[i].rAspecto < 1.5 ) {
                                        r=0; g=1;b=0;
                                   }else if(f[i].rAspecto >= 1.5 && f[i].rAspecto < 1.7 ) {
                                        r=1; g=1;b=0;
                                   }
                                   else if(f[i].rAspecto >= 0.7 && f[i].rAspecto <= 1.0 ){ //de 0,75 até 1,0
                                        r=0; g=0;b=1;
                                   }
                              }else{//cor padrão
                                   r = corpadrao[0]; //Cor padrão do objeto
                                   g = corpadrao[1];
                                   b = corpadrao[2];
                              }
                              
                              glNormal3f(v[f[i].p1].nx, v[f[i].p1].ny, v[f[i].p1].nz);
                              glVertex3f(v[f[i].p1].x, v[f[i].p1].y, v[f[i].p1].z);

                              glNormal3f(v[f[i].p2].nx, v[f[i].p2].ny, v[f[i].p2].nz);
                              glVertex3f(v[f[i].p2].x, v[f[i].p2].y, v[f[i].p2].z);
                              
                              glNormal3f(v[f[i].p3].nx, v[f[i].p3].ny, v[f[i].p3].nz);
                              glVertex3f(v[f[i].p3].x, v[f[i].p3].y, v[f[i].p3].z);  


                    
                    glEnd();  
                      
                         
               }   
          }

          if (malha1){ //malha 1 de outra cor 
          glLineWidth(1.5); //largura da linha
          
               for (int i=0; i< qtFaces; i++){                   

                    glColor3f(0.0, 0.0, 0.0);
                    
                    glBegin(GL_LINE_LOOP);		//malha 1
                    
                         float ft = 0.01; //Fator de aumento 
                         
                         glNormal3f(v[f[i].p1].nx, v[f[i].p1].ny, v[f[i].p1].nz); 
                         glVertex3f(v[f[i].p1].x + ft * v[f[i].p1].nx  , v[f[i].p1].y + ft * v[f[i].p1].ny , v[f[i].p1].z + ft * v[f[i].p1].nz );
                         
                         glNormal3f(v[f[i].p2].nx , v[f[i].p2].ny , v[f[i].p2].nz);  
                         glVertex3f(v[f[i].p2].x + ft * v[f[i].p2].nx , v[f[i].p2].y + ft * v[f[i].p2].ny, v[f[i].p2].z + ft * v[f[i].p2].nz);
                                                            
                         glNormal3f(v[f[i].p3].nx , v[f[i].p3].ny , v[f[i].p3].nz ) ;
                         glVertex3f(v[f[i].p3].x + ft * v[f[i].p3].nx , v[f[i].p3].y + ft * v[f[i].p3].ny , v[f[i].p3].z + ft * v[f[i].p3].nz );        

                    glEnd();

               }
          }

          if (malha2){ //malha 2 de outra cor - OBS. ESSA MALHA SERÁ O RESULTADO MELHORADO
          glLineWidth(1.5); //largura da linha
          //if(v.size() == 0) //Triangula apenas uma vez
              // triangula(); 
               
               for (int i=0; i< f2.size(); i++){     
                    // f2[i].calculaLados(v);  
                    // f2[i].calculaArea();
                    // f2[i].calculaAspecto();             
                             
                    glColor3f(r, g, b); //cor da primeira malha
                    //glColor3f(1, 1, 0); //cor da primeira malha

                    
                    if (solido1){
                         
                         glBegin(GL_POLYGON);	//sólido
                    }
                    else
                         glBegin(GL_LINE_LOOP);		//malha2
                         
                          if(colorido){    
                              if(f2[i].rAspecto >= 0.0 && f2[i].rAspecto < 0.7 ) {
                                        r=1; g=0;b=0;
                                   }else if(f2[i].rAspecto >= 1.25 && f2[i].rAspecto < 1.5 ) {
                                        r=0; g=1;b=0;
                                   }else if(f2[i].rAspecto >= 1.5 && f2[i].rAspecto < 1.7 ) {
                                        r=1; g=1;b=0;
                                   }
                                   else if(f2[i].rAspecto >= 0.7 && f2[i].rAspecto <= 1.0 ){ //de 0,75 até 1,0
                                        r=0; g=0;b=1;
                                   }
                          }else{//cor padrão
                                   r = 0.01; //Cor padrão do objeto
                                   g = 1.0;
                                   b = 1.0;
                          }

                         
                         float ft = 0.01; //Fator de aumento 
                                                                                          
                         glNormal3f(v[f2[i].p1].nx, v[f2[i].p1].ny, v[f2[i].p1].nz); 
                         glVertex3f(v[f2[i].p1].x + ft * v[f2[i].p1].nx  , v[f2[i].p1].y + ft * v[f2[i].p1].ny , v[f2[i].p1].z + ft * v[f2[i].p1].nz );
                         
                         
                         glNormal3f(v[f2[i].p2].nx , v[f2[i].p2].ny , v[f2[i].p2].nz);  
                         glVertex3f(v[f2[i].p2].x + ft * v[f2[i].p2].nx , v[f2[i].p2].y + ft * v[f2[i].p2].ny, v[f2[i].p2].z + ft * v[f2[i].p2].nz);
                                                            
                                                          
                         glNormal3f(v[f2[i].p3].nx , v[f2[i].p3].ny , v[f2[i].p3].nz ) ;
                         glVertex3f(v[f2[i].p3].x + ft * v[f2[i].p3].nx , v[f2[i].p3].y + ft * v[f2[i].p3].ny , v[f2[i].p3].z + ft * v[f2[i].p3].nz );        

                    glEnd();
                    
               }
          
     }
     glColor3f(0.0f,0.0f, 0.0f);
     DesenhaTexto(resultado, 2.6,-2.5);
    // DesenhaTexto(resultado, 0.5,-1.9);
    // Executa os comandos OpenGL
     

    }
    

     

   

     glutSwapBuffers();
      
}



// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{ 
     
    // Para previnir uma divisão por zero
	if ( h == 0 ) h = 1;

	// Especifica o tamanho da viewport
	glViewport(0, 0, w, h);
 
	// Calcula a correção de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
        
    }


// Função callback chamada para gerenciar eventos de teclado
void GerenciaTeclado(unsigned char key, int x, int y)
{
     
     if (angRot < 0)
          angRot -=5;
     else
          angRot+=5;
          
    switch (key) {
            
           case 'A':
           case 'a':// Escala +
                    escala *=2; 
                   
                     break;
          case 'B':
          case 'b': //Solido malha2 ou não
               {
                    solido1 = !solido1;

                    break;
               }   
          case 'C':
          case 'c': //Colorir ou não
               {
                    colorido = !colorido;

                    break;
               }            
                          
          case 'D':
          case 'd': //Escala -
                    escala /=2; 
             
                    break;   
                     
          case 'I':
          case 'i':// inverte o sentido da rotação
                     angRot = -angRot;
                     
                     break;
          case 'M':
          case 'm': //Malha 2 ou não
               {
                    malha2 = !malha2;

                    break;
               }  
          case 'N':
          case 'n': //Malha 1 ou não
               {
                    malha1 = !malha1;

                    break;
               }                

          case 'O':
          case 'o':// Mostra original ou não
                     orig = !orig;
                     
                     break;                        
          
          case 'S':
          case 's': //Sólido ou não
               {
                    solido = !solido;

                    break;
               }    

         
          case 'T':
          case 't': //Triangula e cria arquivos com os resultados
               {
                    f2.clear();  temp2=true;
                    triangula();
                    malha2 = true;

                    if(temp){
                         temp = false;
                         criaArquivo();
                         
                    }
                    if(temp2){
                         temp2 = false;
                         criaArquivoResultado();
                    }

                    if(tpMedia==0){
                         std::ostringstream buffer;
                         buffer  <<"[A]"<<iteracoes<<":";
                         param = buffer.str();
                    }
                    if(tpMedia==1){
                         std::ostringstream buffer;
                         buffer  <<"[G]"<<iteracoes<<":";
                         param = buffer.str();
                    }
                    if(tpMedia==2){
                         std::ostringstream buffer;
                         buffer  <<"[MR]"<<iteracoes<<":";
                         param = buffer.str();
                    }

                    //Resultado que será escrito na tela
                    res = param + res; 
                    for(int i=0; i<res.size(); i++){
                         resultado[i] = res[i];
                              
                    }

                    break;
               } 
          
          case 'h':
          case 'H': //Calcula e mostra histograma
            flagH = true;

            for(auto const& triangulo:f)
            {
                if(triangulo.rAspecto > 0.0 && triangulo.rAspecto <= 0.1)
                {
                    vetO[0]++;
                }
                else if(triangulo.rAspecto > 0.1 && triangulo.rAspecto <= 0.2)
                {
                    vetO[1]++;
                }
                else if(triangulo.rAspecto > 0.2 && triangulo.rAspecto <= 0.3)
                {
                    vetO[2]++;
                }
                else if(triangulo.rAspecto > 0.3 && triangulo.rAspecto <= 0.4)
                {
                    vetO[3]++;
                }
                else if(triangulo.rAspecto > 0.4 && triangulo.rAspecto <= 0.5)
                {
                    vetO[4]++;
                }
                else if(triangulo.rAspecto > 0.5 && triangulo.rAspecto <= 0.6)
                {
                    vetO[5]++;
                }
                else if(triangulo.rAspecto > 0.6 && triangulo.rAspecto <= 0.7)
                {
                    vetO[6]++;
                }
                else if(triangulo.rAspecto > 0.7 && triangulo.rAspecto <= 0.8)
                {
                    vetO[7]++;
                }
                else if(triangulo.rAspecto > 0.8 && triangulo.rAspecto <= 0.9)
                {
                    vetO[8]++;
                }
                else if(triangulo.rAspecto > 0.9 && triangulo.rAspecto <= 1.0)
                {
                    vetO[9]++;
                }
            } 

            //se bugar, adicionar flag
            for(auto const& triangulo:f2) //Malha nova triangulada, talvez bug se a malha nova não tiver sido produzida
            {
                if(triangulo.rAspecto > 0.0 && triangulo.rAspecto <= 0.1)
                {
                    vetN[0]++;
                }
                else if(triangulo.rAspecto > 0.1 && triangulo.rAspecto <= 0.2)
                {
                    vetN[1]++;
                }
                else if(triangulo.rAspecto > 0.2 && triangulo.rAspecto <= 0.3)
                {
                    vetN[2]++;
                }
                else if(triangulo.rAspecto > 0.3 && triangulo.rAspecto <= 0.4)
                {
                    vetN[3]++;
                }
                else if(triangulo.rAspecto > 0.4 && triangulo.rAspecto <= 0.5)
                {
                    vetN[4]++;
                }
                else if(triangulo.rAspecto > 0.5 && triangulo.rAspecto <= 0.6)
                {
                    vetN[5]++;
                }
                else if(triangulo.rAspecto > 0.6 && triangulo.rAspecto <= 0.7)
                {
                    vetN[6]++;
                }
                else if(triangulo.rAspecto > 0.7 && triangulo.rAspecto <= 0.8)
                {
                    vetN[7]++;
                }
                else if(triangulo.rAspecto > 0.8 && triangulo.rAspecto <= 0.9)
                {
                    vetN[8]++;
                }
                else if(triangulo.rAspecto > 0.9 && triangulo.rAspecto <= 1.0)
                {
                    vetN[9]++;
                }
                

            }  

            /* for(int i = 0; i<10;++i)
            {
                cout << "vetO[" << i << "] = " << vetO[i] << '\n';
            }

            for(int i = 0; i<10;++i)
            {
                cout << "vetN[" << i << "] = " << vetN[i] << '\n';
            }   */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       

            break;
          
          case 'X': 
          case 'x':// muda para o eixo x
                    eixo = 0; 
                    break;
          case 'Y':
          case 'y':// muda para o eixo y
                    eixo = 1;
                    break;
          case 'Z':
          case 'z':// muda para o eixo z
                    eixo = 2;
                    break;     

          //Translação
          case '4':{ //Esquerda
               if (transl){
                    posX--;                  
               }  
               break;   
          }     
          case '6':{ //Direita
               if (transl){
                    posX++;
               }   
               break;  
          }   
          case '2':{ //Para baixo
               if (transl){
                    posY--;
               }   
               break;  
          }   
          case '8':{ //Para cima
               if (transl){
                    posY++;
               }     
               break;
          }  
          //diagonais 
          case '1':{ //Esquerda-baixo
               if (transl){
                    posX--; posY--;
               }  
               break;   
          }   
          case '7':{ //Esquerda-cima
               if (transl){
                    posX--; posY++;
               }   
               break;  
          }   
          case '3':{ //Direta-baixo
               if (transl){
                    posX++; posY--;
               }   
               break;  
          }   
          case '9':{ //Direita-cima
               if (transl){
                    posX++; posY++;
               }   
               break;  
          }       

                          
                                   
    }

    
    //EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}


// Função callback chamada para gerenciar eventos do teclado   
// para teclas especiais, tais como F1, PgDn e Home
void TeclasEspeciais(int key, int x, int y)  
{
    if(key == GLUT_KEY_UP) { //seta pra cima - aumenta Y
          switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camY+=5;
                    break;
               case 1: alvoY+=5;
                    break;
               case 2: cimaX = 0; 
                    cimaY=1;
                    cimaZ = 0;                      
                    break;
          }
                                  
     }
       
    if(key == GLUT_KEY_DOWN) { //seta pra baixo - diminui Y
          switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camY-=5;
                    break;
               case 1: alvoY-=5;
                    break;
               case 2: cimaX = 0; 
                    cimaY=1;
                    cimaZ = 0;                      
                    break;
          }
         

    }

      if(key == GLUT_KEY_LEFT) { //seta pra esquerda - diminui X
           switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camX-=5;
                    break;
               case 1: alvoX-=5;
                    break;
               case 2: cimaX = 1; 
                       cimaY=0;
                       cimaZ = 0;                      
                    break;
          }

    }

     if(key == GLUT_KEY_RIGHT) { //seta pra direita - aumenta X
         switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camX+=5;
                    break;
               case 1: alvoX+=5;
                    break;
               case 2: cimaX = 1; 
                       cimaY=0;
                       cimaZ = 0;                      
                    break;
          }
          

    }

    if(key == GLUT_KEY_PAGE_UP) { //page UP - aumenta Z
         switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camZ+=5;
                    break;
               case 1: alvoZ+=5;
                    break;
               case 2: cimaX = 0; 
                       cimaY=0;
                       cimaZ = 1;                      
                    break;
          }

    }

    if(key == GLUT_KEY_PAGE_DOWN) { //Page Down - diminui Z
         switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camZ-=5;
                    break;
               case 1: alvoZ-=5;
                    break;
               case 2: cimaX = 0; 
                       cimaY=0;
                       cimaZ = 1;                      
                    break;
          } 

    }

    if(key == GLUT_KEY_HOME) { //Home - Muda para Observador
         cena=0;

    }

    if(key == GLUT_KEY_END) { //End - Muda para o alvo
         cena=1; 

    }

   
     if(key == GLUT_KEY_F1) { //Habilita rotação
          
         rot = !rot;   

    }


     if(key == GLUT_KEY_F4) { //Habilita translação
          transl = !transl; 

    }


     if(key == GLUT_KEY_F10) { //Orientação em X
         cimaX=1; cimaY=0; cimaZ=0;

    }
    if(key == GLUT_KEY_F11) { //Orientação em Y
         cimaX=0; cimaY=1; cimaZ=0;

    }
    if(key == GLUT_KEY_F12) { //Orientação em Z
         cimaX=0; cimaY=0; cimaZ=1;

    }

   

   

    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}
           
// Gerenciamento do menu com as opções de cores           
void MenuCorObjeto(int op)
{
   switch(op) {
            case 0:
                     corpadrao[0] = 1.0f;
                     corpadrao[1] = 0.0f;
                    corpadrao[2] = 0.0f;
                     break;
            case 1:
                     corpadrao[0] = 0.0f;
                     corpadrao[1] = 1.0f;
                     corpadrao[2] = 0.0f;
                     break;
            case 2:
                     corpadrao[0] = 0.0f;
                     corpadrao[1] = 0.0f;
                     corpadrao[2] = 1.0f;
                     break;
    }
    glutPostRedisplay();
}     

// Gerenciamento do menu com as opções de cor da iluminacção       
void MenuIluminacaoLuz(int op)
{
   switch(op) {
            case 0: { 
                      luz0 = !luz0;	   // habilita e desabilita luz 0         
                      break;
            }
            case 1: { 
                      luz1 = !luz1;	   // habilita e desabilita luz 1         
                      break;
            }
     }
     

              if (luz0) {   // Habilita a luz de número 0
                 glEnable(GL_LIGHT0);
               }else{
                   glDisable(GL_LIGHT0); 
               }

               if (luz1) {  // Habilita a luz de número 0
                 glEnable(GL_LIGHT1);
               }else{
                   glDisable(GL_LIGHT1); 
               }
                    
   
    
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}  

// Gerenciamento do menu com as opções de cor da iluminacção       
void MenuIluminacaoCor(int op)
{
   switch(op) {
            case 0: { //Branca
                      luzDifusa[0]=1.0;	 luzDifusa[1]=1.0; luzDifusa[2]=1.0; luzDifusa[3]=1.0;  //cor
                      luzEspecular[0]=1.0; luzEspecular[1]=1.0; luzEspecular[2]=1.0; luzEspecular[3]=1.0; // "brilho" 	           
                      break;
            }
            case 1: { //Amarela
                      luzDifusa[0]=1.0;	 luzDifusa[1]=1.0; luzDifusa[2]=0.0; luzDifusa[3]=1.0;  //cor
                      luzEspecular[0]=1.0; luzEspecular[1]=1.0; luzEspecular[2]=0.0; luzEspecular[3]=1.0; // "brilho" 	           
                      break;
            }
            case 2: { //Roxa
                      luzDifusa[0]=1.0;	 luzDifusa[1]=0.0; luzDifusa[2]=1.0; luzDifusa[3]=0.0;  //cor
                      luzEspecular[0]=1.0; luzEspecular[1]=0.0; luzEspecular[2]=1.0; luzEspecular[3]=0.0; // "brilho" 	                  
                      break;
            }

            
    }

    //muda somente na luz habilita
     if (luz0){
          // Define os parâmetros da luz de número 0
          glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
          glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
          glEnable(GL_LIGHT0);
         

     }
     if (luz1){
          // Define os parâmetros da luz de número 1
          glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa );
          glLightfv(GL_LIGHT1, GL_SPECULAR, luzEspecular );
          glEnable(GL_LIGHT1);

     }
    
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}   

// Gerenciamento do menu com as opções de cor da iluminacção       
void MenuIluminacaoPosicao(int op)
{
   switch(op) {
            case 0: {    
                      posicaoLuz[0]=0.0; posicaoLuz[1]=500.0; posicaoLuz[2]= 500.0; posicaoLuz[3]=1.0;         
                      break;
            }
            case 1: {    
                      posicaoLuz[0]=500.0; posicaoLuz[1]=500.0; posicaoLuz[2]=00.0; posicaoLuz[3]=1.0;          
                      break;
            }
            case 2: {    
                      posicaoLuz[0]=00.0; posicaoLuz[1]=-500.0; posicaoLuz[2]= 500.0; posicaoLuz[3]=1.0;         
                      break;
            }
            case 3: {    
                      posicaoLuz[0]=500.0; posicaoLuz[1]=-500.0; posicaoLuz[2]=00.0; posicaoLuz[3]=1.0;          
                      break;
            }
    }

     //muda somente na luz habilita
     if (luz0){
          // Define os parâmetros da luz de número 0
          glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
          glEnable(GL_LIGHT0);
         

     }
     if (luz1){
          // Define os parâmetros da luz de número 1
          glLightfv(GL_LIGHT1, GL_POSITION, posicaoLuz );
          glEnable(GL_LIGHT1);

     }
    
    
    
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
} 

// Gerenciamento das opções de cálculo da média das razões      
void MenuCalculoMedia(int op)
{
     switch(op) {
            case 0:
                    tpMedia = 0; //aritmetica
                     break;
            case 1:
                    tpMedia = 1; //geometrica
                     break;
            case 2:
                     tpMedia = 2; //menor razão gerada deve ser maior que a original
                     break;
    }
    glutPostRedisplay();
} 


// Gerenciamento das opções de cálculo da média das razões      
void MenuNumeroIteracoes(int op)
{
     switch(op) {
            case 0:
                    iteracoes = 10000; 
                     break;
            case 1:
                    iteracoes = 50000;
                     break;
            case 2:
                     iteracoes = 100000;
                     break;
            case 3:
                     iteracoes = 1000000;
                     break;          
    }
    glutPostRedisplay();
}  


        
// Gerenciamento do menu principal           
void MenuPrincipal(int op)
{
}
              
// Criacao do Menu
void CriaMenu() 
{
    int menu,submenu1,submenu2, submenu3, submenu4, submenu5, submenu6;

    submenu1 = glutCreateMenu(MenuCorObjeto); //Cor do objeto
    glutAddMenuEntry("Vermelho",0);
    glutAddMenuEntry("Verde",1);
    glutAddMenuEntry("Azul",2);

    submenu2 = glutCreateMenu(MenuIluminacaoLuz); //Habilita e desabilita luzes da cena
    glutAddMenuEntry("Liga/Desliga Luz 1",0);
    glutAddMenuEntry("Liga/Desliga Luz 2",1);
    

    submenu3 = glutCreateMenu(MenuIluminacaoCor); //Cor das luzes da cena
    glutAddMenuEntry("Branca",0);
    glutAddMenuEntry("Amarela",1);
    glutAddMenuEntry("Roxa",2);

    submenu4 = glutCreateMenu(MenuIluminacaoPosicao); //Posição das luzes da cena
    glutAddMenuEntry("Superior esquerda",0);
    glutAddMenuEntry("Superior direita",1);
    glutAddMenuEntry("Inferior Esquerda",2);
    glutAddMenuEntry("Inferior Direta",3);

    submenu5 = glutCreateMenu(MenuCalculoMedia); //Cálculo da média da Triangulação
    glutAddMenuEntry("Media Aritmetica (padrao)",0);
    glutAddMenuEntry("Media Geometrica",1);
    glutAddMenuEntry("Menor razao supera a razao original (melhores resultados)",2);
    

    submenu6 = glutCreateMenu(MenuNumeroIteracoes); //Innforma o número de iterações para escolha
    glutAddMenuEntry("10 mil (padrao)",0);
    glutAddMenuEntry("50 mil",1);
    glutAddMenuEntry("100 mil (ruim para arquivos grandes)",2);
    glutAddMenuEntry("1 milhao - (Menor Razao e arquivos pequenos)",3);



    menu = glutCreateMenu(MenuPrincipal);
    glutAddSubMenu("Cor do Objeto",submenu1);
    glutAddSubMenu("Luzes da Cena",submenu2);
    glutAddSubMenu("Cores das Luzes",submenu3);
    glutAddSubMenu("Posicao das Luzes",submenu4);
    glutAddSubMenu("Calculo da Media",submenu5);
    glutAddSubMenu("Numero de Iteracoes",submenu6);


   //criar menu para a iluminacao com opções para mudar a posição e a cor das luzes
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
           
// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{        
     if (state == GLUT_DOWN) 
          CriaMenu();

   
	EspecificaParametrosVisualizacao();
	glutPostRedisplay(); 

}



// Inicializa parâmetros
void Inicializa (void)
{   
    // Define a cor de fundo da janela de visualização como preta
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //branco
     //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //preto

    // glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0);  

   // win=200.0f;
   
    corpadrao[0] = 0.0f; //Cor inicial do objeto
    corpadrao[1] = 0.0f;
    corpadrao[2] = 1.0f;
    
    angulo=5;
   
    
    
    //Configurações de ilmuninação   

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0}; 
	GLint especMaterial = 60;

     // Habilita o modelo de colorização de Gouraud
     glShadeModel(GL_SMOOTH);		

	// Define a refletância do material 
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
     
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

     // Ativa o uso da luz ambiente 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

     //muda a cor , o brilho e a posição para a luz 1
	luzDifusa[0]=0.5;	 luzDifusa[1]=0.5; luzDifusa[2]=0.5; luzDifusa[3]=1.0;  //cor
     luzEspecular[0]=1.0; luzEspecular[1]=1.0; luzEspecular[2]=1.0; luzEspecular[3]=1.0; // "brilho" 
     posicaoLuz[0]=-500.0; posicaoLuz[1]=-500.0; posicaoLuz[2]= 500.0; posicaoLuz[3]=1.0;

     // Define os parâmetros da luz de número 1
	glLightfv(GL_LIGHT1, GL_AMBIENT, luzAmbiente); 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT1, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT1, GL_POSITION, posicaoLuz );

     
     // Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);  
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);

     
     
	// Habilita o depth-buffering (Z-buffer)
	glEnable(GL_DEPTH_TEST);

     
}

//Faz a leitura das strings do arquivo
void lerDado(FILE *arq){
          //Lê um dado do arquivo e retorna em formato de string

          fscanf(arq, "%s",  texto);
}


// Programa Principal 
int main(int argc, char** argv)
{
     srand(time(NULL)); //Para uso de randômicos

     //Prepara a estrutura com os dados de interesse (vertices, faces, pontos ....)
     fileList = argv+1;  // salta nome do programa
   
		
		// Abrindo o arquivo
		
          FILE *arq;
          arq = fopen(fileList[0], "rb");
          if(arq == NULL)
               cout<<"Erro, nao foi possivel abrir o arquivo "<<fileList[0] <<endl;
          else{
               nome = fileList[0]; //pega o nome do arquivo
               
               //Faz a leitura das linhas do arquivo
               
               while (true){ 
                    lerDado(arq);
                    if (!strcmp( texto ,"property")){
                         qtPropriedades++;
                    }
                    if (!strcmp( texto ,"vertex")){
                        lerDado(arq); //pega a quantidade de vértices
                        qtVertices = atoi(texto);
                    }
                    if (!strcmp( texto ,"face") ) {
                        lerDado(arq); //pega a quanntidade de faces
                        qtFaces = atoi(texto);    
                    }
                    
                    if (!strcmp( texto ,"end_header")){ //pára quando encontrar end_header
                        break;
                    }     
  
               }
               qtPropriedades--; //ajusta a quantidade de propriedades
               

               cout<<"Propriedades lidas "<<qtPropriedades<<endl;
               cout<<"Vertices lidos: "<<qtVertices<<endl;
               cout<<"Faces lidas: "<<qtFaces<<endl;  
      

          //preenche o vetor de vertices
          for (int i=0; i< qtVertices; i++){
               vertice vt;
               
               if ( (qtPropriedades) == 3){
                    lerDado(arq); vt.x = atof(texto);
                    lerDado(arq); vt.y = atof(texto);
                    lerDado(arq); vt.z = atof(texto); 
               }

          

               
               // outras propriedades
               
               if ( (qtPropriedades) > 3){
                   switch (qtPropriedades){
                        case 4: {
                             lerDado(arq); vt.x = atof(texto); 
                             lerDado(arq); vt.y = atof(texto);
                             lerDado(arq); vt.z = atof(texto);
                             lerDado(arq); vt.nx = atof(texto); 
                             break;
                        }
                        case 5: {
                             lerDado(arq); vt.x = atof(texto); 
                             lerDado(arq); vt.y = atof(texto);
                             lerDado(arq); vt.z = atof(texto);
                             lerDado(arq);  vt.nx = atof(texto);
                             lerDado(arq); vt.ny = atof(texto);
                             break;
                        }
                        case 6: { //Se tiver 6 propriedades teremos os dados do vetor normal
                             lerDado(arq); vt.x = atof(texto); 
                             lerDado(arq); vt.y = atof(texto);
                             lerDado(arq); vt.z = atof(texto);
                             lerDado(arq); vt.nx = atof(texto);
                             lerDado(arq); vt.ny = atof(texto);
                             lerDado(arq); vt.nz = atof(texto);
                             break;
                        }
                   }

               }  

               v.push_back(vt);

          }

          //preenche o vetor de faces
          for (int i=0; i< qtFaces; i++){
               face fc;
               lerDado(arq); //quantidade de lados, não interessa pois sempre trataremos triângulos
               lerDado(arq);  fc.p1 = atoi(texto); //guarda os vértices
               lerDado(arq);  fc.p2 = atoi(texto); 
               lerDado(arq);  fc.p3 = atoi(texto); 
               f.push_back(fc);
               
          }

           //cálculos necessários para avaliar a triangulação
          int valido = 0; int invalido =0;
          for (int i=0; i< qtFaces; i++){
              
               f[i].calculaLados(v);
               f[i].calculaArea();
               f[i].calculaAspecto();
               f[i].calculaD(v); 

               if(f[i].RV){
                    valido++;
               }else{
                     invalido++;
               }
               
               
          }
          cout<<endl;		
	}

      
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);    
      
     glutInitWindowSize(1366,768); //Tela cheia

    // glutInitWindowPosition(10,10);
     glutCreateWindow("Leitor 3D.ply | F1 Rotacao X-Y-Z (I-inverter) | F4: (Translacao 0-9) | Escala: A-D| Setas: Cena | C - Colorido | T - Triangula e Cria arquivo | M-N-O: Exibe | S-B: solido");
     glutKeyboardFunc(GerenciaTeclado);
     glutReshapeFunc(AlteraTamanhoJanela);

     glutDisplayFunc(Desenha);
     glutMouseFunc(GerenciaMouse);    
     glutSpecialFunc(TeclasEspeciais); 

     Inicializa();
     glutMainLoop();
}
