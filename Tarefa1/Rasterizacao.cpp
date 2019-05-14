#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<cmath>

using namespace std;

int X, Y; //variáveis de apoio para os pontos


struct ponto{
    int x, y;
};

struct reta{
    ponto inicio, fim; //pontos incial e final
    double dX, dY; //deltas x e y
    double m; //coeficiente angular
    
    void calculaDeltas()
    {   
        
        dX = fim.x - inicio.x;
        dY = fim.y - inicio.y;  
       
    }
    void calculaM(){ //calcula o valor do coeficiente angular
        calculaDeltas();
        m = dY / dX;

    }
};


/*
    a) Rasterização de linhas dados os pontos inicial e final
    - Algoritmo Simples
    - Algoritmo Incremental
    - Algoritmo de Bresenham

*/
void pintaRetaSimples()
{
    
    //cria os pontos
    ponto p, p1;
    p.x=100;    p.y=100;
    p1.x=500;  p1.y=200;
    

    reta r;
    r.inicio = p; r.fim = p1;
        r.calculaM();
        // Define a cor corrente
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_POINTS);
            for (X = r.inicio.x; X <= r.fim.x; X++){        
                    Y = r.inicio.y + floor(r.m * (X - r.inicio.x) + 0.5);
                    glVertex2f(X, Y);        
                }     

        glEnd();
        glFlush();
  
}

void pintaRetaIncremental()
{
    
    //cria os pontos
    ponto p, p1;
    p.x=0;    p.y=0;
    p1.x=200;  p1.y=200;
    

    reta r;
    r.inicio = p; r.fim = p1;
    r.calculaM();
    
    // Define a cor corrente
    glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_POINTS);
        for (X = p.x; X <= p1.x; X++){
            glVertex2f(X, floor(Y + 0.5));       
            Y = Y + r.m; 
        }
        glEnd();
        glFlush();
        
}

void pintaRetaBresenham() 
{
    
    //cria os pontos (vértices)
    ponto p, p1;
    p.x=100;    p.y=100;
    p1.x=200;  p1.y=150;


    reta r;
    r.inicio = p; r.fim = p1;
    r.calculaM();

    cout<<"m é igual a "<<r.m<<endl;

    int Pk = 2*r.dY - r.dX; //Pk inicial

    // Define a cor corrente
    glColor3f(0.0f, 0.0f, 1.0f);
    
    //m entre 0 e 1
    if (r.m > 0 && r.m < 1){
    //pinta o primeiro ponto (0,0)
    glBegin(GL_POINTS);
        glVertex2f(r.inicio.x, r.inicio.y);          
    glEnd();
    glFlush();    

        glBegin(GL_POINTS);
        Y = r.inicio.y; 
            for (X = p.x; X <= p1.x; X++){
                if(Pk < 0){           
                    glVertex2f(X + 1, Y); 
                    Pk = Pk + 2*r.dY;
                }else{
                    glVertex2f(X + 1, Y+1);  
                    Pk = Pk + 2*r.dY - 2*r.dX;
                }
                    Y++;
            }
        glEnd();
        glFlush();
    } else if (r.m > 1){ //m > 1
    //pinta o primeiro ponto (0,0)
    glBegin(GL_POINTS);
        glVertex2f(r.inicio.y, r.inicio.x);          
    glEnd();
    glFlush();    

        glBegin(GL_POINTS);
        Y = r.inicio.x; 
            for (X = p.y; X <= p1.y; X++){
                if(Pk < 0){           
                    glVertex2f(Y + 1, X); 
                    Pk = Pk + 2*r.dY;
                }else{
                    glVertex2f(Y + 1, X+1);  
                    Pk = Pk + 2*r.dY - 2*r.dX;
                }
                    Y++;
            }
        glEnd();
        glFlush();
        }else if (r.m < 0) { //m < 1
    //pinta o primeiro ponto (x,y)
    glBegin(GL_POINTS);
        glVertex2f(r.inicio.x, r.inicio.y);          
    glEnd();
    glFlush();    

        glBegin(GL_POINTS);
        Y = r.inicio.y; 
            for (X = p.x; X <= p1.x; X++){
                if(Pk < 0){           
                    glVertex2f(X + 1, Y); 
                    Pk = Pk + 2*r.dY;
                }else{
                    glVertex2f(X + 1, Y - 1);  
                    Pk = Pk + 2*r.dY - 2*r.dX;
                }
                    //if (Y > 0)
                         Y--;
                   
            }
        glEnd();
        glFlush();
    }        
}

/*
        b) Rasterização de cículos, dado o raio de entrada.
    - Utilizando equação do círculo.
    - Utilizando coordenadas polares
    - Utilizando o algoritmo de Bresenham
*/
void pintaCirculoEQ(){
    int r = 400; //raio
    int xc = 500; int yc = 500; //centro (x,y)

    // Define a cor corrente
    glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_POINTS);
        for (X = xc - r; X <= xc + r; X++){
            Y = yc + sqrt( pow(r,2) - pow(xc - X, 2) );
            glVertex2f(X , Y); 
            Y = yc - sqrt( pow(r,2) - pow(xc - X, 2) );
            glVertex2f(X , Y); 
        }

    glEnd();
    glFlush();



}

void pintaCirculoCP(){
    int r = 400; //raio
    int xc = 500; int yc = 500; //centro (x,y)

    // Define a cor corrente
    glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_POINTS);
        for (int i = 0; i <= 360; i++){
            X = xc + r * cos(i);
            Y = yc + r * sin(i);
            glVertex2f(X , Y); 
            
        }

    glEnd();
    glFlush();

}

void pintaCirculoBresenham(){ //DÚVIDAS AQUI
    int r = 400; //raio
    int xc = 300; int yc = 300; //centro (x,y)
   

    // Define a cor corrente
    glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_POINTS);
         glVertex2f(X , Y);
    glEnd();
    glFlush();
   

}

/*
        c) Implemente a rasterização de uma parábola da forma y = a*x*x + b*x + c, em que a, b e c são
    parâmetros reais (dados um x inicial e um x final).
    - Utilizando a equação da parábola
    - Utilizando o algoritmo incremental criado em sala
*/
void pintaParabola(){

    double a, b, c;
    a=1; b=-11; c=30; 

    // Define a cor corrente
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
        for (double X = -300; X <= 300; X+=0.01){        
                Y = a * pow(X, 2) + b * X + c;

                glVertex2f(X, Y);        
            }     

    glEnd();
    glFlush();



}

void pintaParabolaInncremental(){
    //NÃO LEMBRO DO ALGORITMO, ESQUECI DE ANOTAR
}


void desenha(){

   //pintaRetaSimples();
    //pintaRetaIncremental();
    //pintaRetaBresenham();
    //pintaCirculoEQ();
    //pintaCirculoCP();
    //pintaCirculoBresenham(); //TIVE DÚVIDAS E NÃO TERMINEI
    pintaParabola();
    //pintaParabolaInncremental //TIVE DÚVIDAS E NÃO TERMINEI
}

void init (void)
{
    glClearColor (.0, 0.0, 0.0, 0.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0);

    //Desenha o plano cartesiano
    // Define a cor corrente
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);
         glVertex2f(0.0f, 1000.0f);
         glVertex2f(0.0f, -1000.0f);
    glEnd();

    glBegin(GL_LINES);
         glVertex2f(-1000.0f, 0.0f);
         glVertex2f(1000.0f,0.0f);
    glEnd();

    glFlush();  

    
}

int main(int argc, char** argv)
{
    

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Rasterizacao");
    init ();
    glutDisplayFunc(desenha);

    glutMainLoop();

    return 0;
}