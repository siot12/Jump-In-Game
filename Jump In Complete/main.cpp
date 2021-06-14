#include <iostream>
#include <graphics.h>
#include <stdio.h>
#include <string.h>

#define width 962 //un multiplu al lui 37
#define height 780 //un multiplu al lui 30
//iar pentru un joc cat mai optimizat catul impartirilor trebuie sa fie egal
//exemple de rezolutii: 962x780  925x750  888x720   851x690 etc.
using namespace std;

int Lx,Ly,latura,lungimetabla,sus,stanga; //aceste variabile le folosim pentru pozitionarea tablei de joc
bool ok;
int limba=1;
//setam limba romana ca predefinit
//1= romana   2= engleza   3= franceza
int pagina=1,nivel;
int tablajoc[6][6];
int contorstele[6];//utilizat pentru a afisa scorul de stele la fiecare pagina in parte
int stele[61];  //folosim stele pt a afisa stelele obtinute pt fiecare nivel -1=nu s-a rezolvat nivelul niciodata, 0=0 stele,1=1 stea, 2=2 stele, 3=3 stele
int pozitii[4]; //pozitii folosim pentru a stii peste cate patratele sare un iepure sau merge o vulpe
int mutari[61],nrmutari;
//mutari retine mutarile minime necesare pt fiecare nivel
//nrmutari reprezinta mutarile facute pana la castig, aceasta creste la fiecare mutare a unui iepure sau a unei vulpi
bool castigat;
int nriepuri; //e folositor pentru a determina daca am castigat, se contorizeaza in functia pune_piese
char scor[5];



struct undo{
    int tip,l1,c1,l2,c2,m;  // tip = 1=iepure, 2=vulpe orizontala, 3=vulpe verticala
};

struct nod{
    undo info;
    nod* succ;
};

nod* S;
undo inapoi;

void push(nod* &S,undo e)
{
	nod* q=new nod;
	q->info=e;
	q->succ=S;
	S=q;
}

void pop(nod* &S)
{
	if(S!=NULL)
    {
        nod* q;
        q=S;
        S=S->succ;
        delete q;
    }
}



void initializare_valori()
{
    Lx=width/37;
    Ly=height/30;
    lungimetabla=15*Ly;
    latura=lungimetabla/5;
    stanga=(getmaxx()-lungimetabla)/2;
    sus=(getmaxy()-lungimetabla)/2;
    setbkcolor(BROWN);
    //citim mutarile minime pentru fiecare nivel si initializam nivelele la -1(adica nu au fost rezolvate inca)
    FILE* f=fopen("joc\\mutari.txt","r");
    FILE* g=fopen("stele\\s.txt","r");
    for(int i=1; i<=60; i++)
        fscanf(f,"%d",&mutari[i]),fscanf(g,"%d",&stele[i]);
    fclose(f);
    fclose(g);
}

void initializare_matrice()
{
    char s[4],n[]="nivele\\";
    sprintf(s,"%d", nivel);
    strcat(s,".txt");
    strcat(n,s);
    FILE* f=fopen(n,"r");
    int i,j;
    for(i=1; i<=5; i++)
        for(j=1; j<=5; j++)
            fscanf(f,"%d",&tablajoc[i][j]);
    fclose(f);
}

void pune_piese()
{
    nriepuri=0;
    int i,j;
    for(i=1; i<=5; i++)
        for(j=1; j<=5; j++)
            switch(tablajoc[i][j])
            {
            case 0:
                readimagefile("imagini_piese\\liber.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i);
                break;
            case 1:
                readimagefile("imagini_piese\\gaura.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i);
                break;
            case 2:
                readimagefile("imagini_piese\\iepurealb.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i),nriepuri++;
                break;
            case 3:
                readimagefile("imagini_piese\\gauraalb.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i),nriepuri++;
                break;
            case 4:
                readimagefile("imagini_piese\\iepuremaro.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i),nriepuri++;
                break;
            case 5:
                readimagefile("imagini_piese\\gauramaro.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i),nriepuri++;
                break;
            case 6:
                readimagefile("imagini_piese\\iepuregri.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i),nriepuri++;
                break;
            case 7:
                readimagefile("imagini_piese\\gauragri.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i),nriepuri++;
                break;
            case 8:
                readimagefile("imagini_piese\\ciuperca.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i);
                break;
            case 9:
                if(tablajoc[i][j+1]==10)
                    readimagefile("imagini_piese\\vulpeorizontala.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j+latura,sus+latura*i);
                else if(tablajoc[i+1][j]==10)
                    readimagefile("imagini_piese\\vulpeverticala.jpg",stanga+latura*(j-1),sus+latura*(i-1),stanga+latura*j,sus+latura*i+latura);
                break;
            }

    int xc,yc;
    yc=Ly/2+5;
    xc=Lx*6-Lx/2;
    settextstyle(10,0,4);
    sprintf(scor,"%d",mutari[nivel]);
    outtextxy(xc,Ly*11-yc,scor); //afisam mutarile necesare pentru 3 stele

    sprintf(scor,"%d",mutari[nivel]+pagina);
    outtextxy(xc,Ly*14-yc,scor); //afisam mutarile necesare pentru 2 stele

    sprintf(scor,"%d",mutari[nivel]+2*pagina);
    outtextxy(xc,Ly*17-yc,scor); //afisam mutarile necesare pentru 1 stea
    char s[3];
    strcpy(s,scor);
    strcpy(scor,">");
    strcat(scor,s);
    outtextxy(xc,Ly*20-yc,scor); //afisam mutarile necesare pentru 0 stele
    settextstyle(10,0,6);
    outtextxy(Lx*3,Ly*7,"00");
}

void memorare_nivele()
{
    FILE* f=fopen("stele\\s.txt","w");
    for(int i=1; i<=60; i++)
        fprintf(f,"%d",stele[i]),fprintf(f," ");
    fclose(f);
}



void titluNIVEL()
{
    settextstyle(10,0,6);
    //settextjustify(1,1);
    char s[10],nr[4];
    sprintf(nr,"%d",nivel);
    if(limba==1)
    {
        strcpy(s,"NIVEL ");
        strcat(s,nr);
    }
    else if(limba==2)
    {
        strcpy(s,"LEVEL ");
        strcat(s,nr);
    }
    else
    {
        strcpy(s,"NIVEAU ");
        strcat(s,nr);
    }
    outtextxy(Lx*15-Lx/2,Ly*2-Ly/2,s);
}

void interfata_meniu()
{
    switch(limba)
    {
        case 1: readimagefile("ro\\meniu.jpg",0,0,width,height); break;
        case 2: readimagefile("en\\meniu.jpg",0,0,width,height); break;
        case 3: readimagefile("fr\\meniu.jpg",0,0,width,height); break;
    }
}

void interfata_setari()
{
  switch(limba)
  {
      case 1: readimagefile("ro\\SetariRo.jpg",0,0,width,height);break;
      case 2: readimagefile("en\\SetariEng.jpg",0,0,width,height);break;
      case 3: readimagefile("fr\\SetariFr.jpg",0,0,width,height);break;
  }
}

void interfata_instructiuni()
{
    switch(limba)
    {
      case 1: readimagefile("ro\\InstructiuniROF.jpg",0,0,width,height);break;
      case 2: readimagefile("en\\InstructiuniENF.jpg",0,0,width,height);break;
      case 3: readimagefile("fr\\InstructiuniFRF.jpg",0,0,width,height);break;
    }

}

void interfata_start()
{
    //afisam imaginile cu nivele in functie de pagina si limba
    switch(pagina)
    {
        case 1: switch(limba){
                        case 1: readimagefile("ro\\start1.jpg",0,0,width,height); break;
                        case 2: readimagefile("en\\start1.jpg",0,0,width,height); break;
                        case 3: readimagefile("fr\\start1.jpg",0,0,width,height); break;
                    }break;
        case 2: switch(limba){
                        case 1: readimagefile("ro\\start2.jpg",0,0,width,height); break;
                        case 2: readimagefile("en\\start2.jpg",0,0,width,height); break;
                        case 3: readimagefile("fr\\start2.jpg",0,0,width,height); break;
                    }break;
        case 3: switch(limba){
                        case 1: readimagefile("ro\\start3.jpg",0,0,width,height); break;
                        case 2: readimagefile("en\\start3.jpg",0,0,width,height); break;
                        case 3: readimagefile("fr\\start3.jpg",0,0,width,height); break;
                    }break;
        case 4: switch(limba){
                        case 1: readimagefile("ro\\start4.jpg",0,0,width,height); break;
                        case 2: readimagefile("en\\start4.jpg",0,0,width,height); break;
                        case 3: readimagefile("fr\\start4.jpg",0,0,width,height); break;
                    }break;
        case 5: switch(limba){
                        case 1: readimagefile("ro\\start5.jpg",0,0,width,height); break;
                        case 2: readimagefile("en\\start5.jpg",0,0,width,height); break;
                        case 3: readimagefile("fr\\start5.jpg",0,0,width,height); break;
                    }break;
    }
    //afisam stelele
    int i,j,n;
    n=1+12*(pagina-1);
    contorstele[pagina]=0;
    for(i=4; i<=18; i=i+7)
        for(j=3; j<=30; j=j+9)
        {
            switch(stele[n])
            {
                case 0:readimagefile("stele\\0s.jpg",Lx*j,Ly*i,Lx*(j+5),Ly*(i+1)); break;
                case 1:readimagefile("stele\\1s.jpg",Lx*j,Ly*i,Lx*(j+5),Ly*(i+1)); break;
                case 2:readimagefile("stele\\2s.jpg",Lx*j,Ly*i,Lx*(j+5),Ly*(i+1)); break;
                case 3:readimagefile("stele\\3s.jpg",Lx*j,Ly*i,Lx*(j+5),Ly*(i+1)); break;
            }
            if(stele[n]>0)
                contorstele[pagina]+=stele[n]; //se contorizeaza stelele totale pe pagina respectiva
            n++;
        }
    readimagefile("stele\\star.jpg",Lx*6,Ly,Lx*12,Ly*3);
    char s[10];
    sprintf(s,"%d",contorstele[pagina]);
    strcat(s,"/36");
    settextstyle(10,0,4);
    outtextxy(Lx*6.1,Ly*1.5,s);
    /*
    Lx*3, Ly*4, Lx*8, Ly*5
    Lx*12,Ly*4,Lx*17, Ly*5
    Lx*21,Ly*4,Lx*26, Ly*5
    Lx*30,Ly*4,Lx*35, Ly*5

    Lx*3, Ly*11,Lx*8, Ly*12
    Lx*12,Ly*11,Lx*17,Ly*12
    Lx*21,Ly*11,Lx*26,Ly*12
    Lx*30,Ly*11,Lx*35,Ly*12

    Lx*3, Ly*18,Lx*8, Ly*19
    Lx*12,Ly*18,Lx*17,Ly*19
    Lx*21,Ly*18,Lx*26,Ly*19
    Lx*30,Ly*18,Lx*35,Ly*19
    */
    switch(limba) //butonul de restart joc
    {
        case 1: readimagefile("ro\\rsro.jpg",Lx*30,Ly,Lx*36,Ly*3); break;
        case 2: readimagefile("en\\rsen.jpg",Lx*30,Ly,Lx*36,Ly*3); break;
        case 3: readimagefile("fr\\rsfr.jpg",Lx*30,Ly,Lx*36,Ly*3); break;
    }

}

void interfata_joc()
{
    readimagefile("joc\\joc.jpg",0,0,width,height);
    titluNIVEL();
    initializare_matrice(); //cream tabla de joc in memorie
    pune_piese(); //cream tabla de joc grafic si contorizam nriepuri
    castigat=false;
    nrmutari=0;
    S=NULL; //initializam stiva la nul
}

void interfata_insigne()
{
    switch(limba)
    {
        case 1: readimagefile("ro\\b.jpg",0,0,width,height); break;
        case 2: readimagefile("en\\b.jpg",0,0,width,height); break;
        case 3: readimagefile("fr\\b.jpg",0,0,width,height); break;
    }

    //contorstele[1]=36;
    //contorstele[2]=36;
    //contorstele[3]=36;
    //contorstele[4]=36;
    //contorstele[5]=36;

    if(contorstele[1]==36)
        readimagefile("joc\\1.jpg",Lx*2,Ly*6,Lx*12,Ly*16);
    if(contorstele[2]==36)
        readimagefile("joc\\2.jpg",Lx*14,Ly*6,Lx*24,Ly*16);
    if(contorstele[3]==36)
        readimagefile("joc\\3.jpg",Lx*26,Ly*6,Lx*36,Ly*16);
    if(contorstele[4]==36)
        readimagefile("joc\\4.jpg",Lx*9,Ly*18,Lx*19,Ly*28);
    if(contorstele[5]==36)
        readimagefile("joc\\5.jpg",Lx*21,Ly*18,Lx*31,Ly*28);
}



void inserare_eliminare_imagine_vulpe_orizontala(int linia,int coloana,int l,int c)
{
    char s[3];
    sprintf(scor,"%d",nrmutari);
    if(nrmutari<10)
        strcpy(s,scor),strcpy(scor,"0"),strcat(scor,s);
    outtextxy(Lx*3,Ly*7,scor);

    readimagefile("imagini_piese\\liber.jpg",stanga+latura*(coloana-1),sus+latura*(linia-1),stanga+latura*(coloana+1),sus+latura*linia);//afisam liber alungit
    tablajoc[linia][coloana]=tablajoc[linia][coloana+1]=0;
    if(c>coloana) //inseamna ca s-a ales sa se mute in est
    {
        readimagefile("imagini_piese\\vulpeorizontala.jpg",stanga+latura*(c-2),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
        tablajoc[l][c-1]=9;
        tablajoc[l][c]=10;
    }
    else
    {
        readimagefile("imagini_piese\\vulpeorizontala.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*(c+1),sus+latura*l);
        tablajoc[l][c]=9;
        tablajoc[l][c+1]=10;
    }
}

void inserare_eliminare_imagine_vulpe_verticala(int linia,int coloana,int l,int c)
{
    char s[3];
    sprintf(scor,"%d",nrmutari);
    if(nrmutari<10)
        strcpy(s,scor),strcpy(scor,"0"),strcat(scor,s);
    outtextxy(Lx*3,Ly*7,scor);

    readimagefile("imagini_piese\\liber.jpg",stanga+latura*(coloana-1),sus+latura*(linia-1),stanga+latura*coloana,sus+latura*(linia+1));//imagine liber alungita
    tablajoc[linia][coloana]=tablajoc[linia+1][coloana]=0;
    if(l>linia) //s a ales sa se mute in sud
    {
        readimagefile("imagini_piese\\vulpeverticala.jpg",stanga+latura*(c-1),sus+latura*(l-2),stanga+latura*c,sus+latura*l);
        tablajoc[l-1][c]=9;
        tablajoc[l][c]=10;
    }
    else
    {
        readimagefile("imagini_piese\\vulpeverticala.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*(l+1));
        tablajoc[l][c]=9;
        tablajoc[l+1][c]=10;
    }
}

void evidentiere_vulpe_verticala(int l,int c,bool ok)
{
    char s[40];
    if(ok)
        strcpy(s,"imagini_piese\\vulpeverticalaev.jpg");
    else
        strcpy(s,"imagini_piese\\vulpeverticala.jpg");

    if(tablajoc[l][c]==9)
        readimagefile(s,stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l+latura);
    else
        readimagefile(s,stanga+latura*(c-1),sus+latura*(l-2),stanga+latura*c,sus+latura*(l-1)+latura);
}

void evidentiere_vulpe_orizontala(int l,int c,bool ok)
{
    char s[40];
    if(ok)
        strcpy(s,"imagini_piese\\vulpeorizontalaev.jpg");
    else
        strcpy(s,"imagini_piese\\vulpeorizontala.jpg");

    if(tablajoc[l][c]==9)
        readimagefile(s,stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c+latura,sus+latura*l);
    else
        readimagefile(s,stanga+latura*(c-2),sus+latura*(l-1),stanga+latura*(c-1)+latura,sus+latura*l);
}

void afisare_mutari_posibile_vulpe_orizontala(int l,int c,char liber[40])
{
    int copie;// facem o copie pentru a nu strica vectorul de pozitii , daca nu facem copie se vor afisa in continuare sagetile unde se misca vulpea si dupa ce am deselectat-o
    if(tablajoc[l][c]==9)
        for(int i=0; i<4; i++)
        {
            copie=pozitii[i];
            if(copie)
            switch(i)
            {
                case 1:
                {
                    if(tablajoc[l][c+pozitii[i]+1]==0)
                        while(copie)
                            readimagefile(liber,stanga+latura*(c+copie),sus+latura*(l-1),stanga+latura*(c+copie+1),sus+latura*l),copie--;
                            // in liber se va incarca ori imaginea cu cercul unde se poate muta vulpea, ori imaginea libera
                }break;
            case 3:
            {
                if(tablajoc[l][c-pozitii[i]]==0)
                {
                    while(copie)
                    {
                        readimagefile(liber,stanga+latura*(c-copie-1),sus+latura*(l-1),stanga+latura*(c-copie),sus+latura*l);
                        copie--;
                    }
                }
            }
            break;
            }
        }
    else
        for(int i=0; i<4; i++)
        {
            copie=pozitii[i];
            if(copie)
            switch(i)
            {

            case 1:
            {
                if(tablajoc[l][c+pozitii[i]]==0)
                {
                    while(copie)
                    {
                        readimagefile(liber,stanga+latura*(c+copie-1),sus+latura*(l-1),stanga+latura*(c+copie),sus+latura*l);
                        copie--;

                    }
                }
            }break;
            case 3:
            {
                if(tablajoc[l][c-pozitii[i]-1]==0)
                {
                    while(copie)
                    {
                        readimagefile(liber,stanga+latura*(c-copie-2),sus+latura*(l-1),stanga+latura*(c-copie-1),sus+latura*l);
                        copie--;
                    }
                }

            }break;
            }
        }
}

void afisare_mutari_posibile_vulpe_verticala(int l,int c,char liber[40])
{
    int copie;
    if(tablajoc[l][c]==9)
    {
        for(int i=0;i<4;i++)
          {
              copie=pozitii[i];
                if(copie)
                switch(i)
                {
        case 0:
            {
              if(tablajoc[l-pozitii[i]][c]==0)
              {
                  while( copie)
                  {
                      readimagefile(liber,stanga+latura*(c-1),sus+latura*(l-copie-1),stanga+latura*c,sus+latura*(l-copie-1)+latura);
                       copie--;
                  }
              }
            }
            break;
        case 2:
            {
                if(tablajoc[l+pozitii[i]+1][c]==0)
                {
                    while( copie)
                    {
                        readimagefile(liber,stanga+latura*(c-1),sus+latura*(l+copie),stanga+latura*c,sus+latura*(l+copie)+latura);
                         copie--;
                    }
                }
                }
            break;
                }
          }
    }
    else
    {
        for(int i=0;i<4;i++)
        {
            copie=pozitii[i];
            if(copie)
              switch(i)
        {
        case 0:
            {
                if(tablajoc[l-pozitii[i]-1][c]==0)
                {
                    while(copie)
                    {
                        readimagefile(liber,stanga+latura*(c-1),sus+latura*(l-copie-2),stanga+latura*c,sus+latura*(l-copie-2)+latura);
                        copie--;

                    }
                }

            }
            break;
        case 2:
            {
                if(tablajoc[l+pozitii[i]][c]==0)
                {
                    while(copie)
                    {
                        readimagefile(liber,stanga+latura*(c-1),sus+latura*(l+copie-1),stanga+latura*c,sus+latura*(l+copie-1)+latura);
                        copie--;
                    }
                }
            }
            break;
        }
        }
    }

}

void mutare_vulpe_orizontala(int linia,int coloana)
{
    evidentiere_vulpe_orizontala(linia,coloana,true);
    int l,c,contor=0;
    if(tablajoc[linia][coloana]==9)
    {
        for(int i=0; i<4; i++)
            pozitii[i]=0;
        //est
        c=coloana;
        l=linia,contor=0;
        while(c<4 && tablajoc[l][c+2]==0)
            contor++,c++;
        if(coloana+contor>6)
        {
            contor=0;
        }
        pozitii[1]=contor;//pe a doua pozitie in vectorul pozitii am retinut cate pozitii merge vulpea in est(in dreapta)
        //vest
        l=linia,contor=0;
        c=coloana;
        while(c>1 && tablajoc[l][c-1]==0)
            contor++,c--;
        if(coloana-contor<-2)
            contor=0;
        pozitii[3]=contor;
    }
    else
    {
        for(int i=0; i<4; i++)
            pozitii[i]=0;
        //est
        c=coloana;
        l=linia,contor=0;
        while(c<5 && tablajoc[l][c+1]==0)
            contor++,c++;
        if(coloana+contor>7)
        {
            contor=0;
        }
        pozitii[1]=contor;//pe a doua pozitie in vectorul pozitii cate pozitii merge vulpea in in est(in dreapta)
        //vest
        l=linia,contor=0;
        c=coloana;
        while(c>2 && tablajoc[l][c-2]==0)
            contor++,c--;
        if(coloana-contor<-1)
            contor=0;
        pozitii[3]=contor;
    }
    afisare_mutari_posibile_vulpe_orizontala(linia,coloana,"imagini_piese\\sageataliber.jpg");
    if(tablajoc[linia][coloana]==10)//daca coloana se afla pe coada, o mutam la cap ca sa ne fie mai usor
        coloana--;
    bool mutat=false;
    int x,y;
    while(!mutat)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            afisare_mutari_posibile_vulpe_orizontala(linia,coloana,"imagini_piese\\liber.jpg");
            evidentiere_vulpe_orizontala(linia,coloana,false);
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=stanga && x<=stanga+lungimetabla && y>=sus && y<=sus+lungimetabla)
            {
                l=(y-sus)/latura+1;
                c=(x-stanga)/latura+1;
                if(l==linia)//daca se face click pe linia cu vulpea
                {
                    //verificam daca in est se pot efectua mutari
                    if(c-coloana-1>0 && c-coloana-1<=pozitii[1]) //scadem un 1 deoarece scadem patratica cu coada
                    {
                        nrmutari=nrmutari+(c-coloana-1),inserare_eliminare_imagine_vulpe_orizontala(linia,coloana,l,c);//incarcam noua vulpe pe tabla de joc
                        //punem valorile in stiva
                        inapoi.tip=2,inapoi.m=c-coloana-1,inapoi.l1=linia,inapoi.c1=coloana,inapoi.l2=l,inapoi.c2=c-1;
                        push(S,inapoi);
                    }
                    //altfel verificam in vest
                    else if(coloana-c>0 && coloana-c<=pozitii[3])
                    {
                        nrmutari=nrmutari+(coloana-c),inserare_eliminare_imagine_vulpe_orizontala(linia,coloana,l,c);
                        //punem valorile in stiva
                        inapoi.tip=2,inapoi.m=coloana-c,inapoi.l1=linia,inapoi.c1=coloana+1,inapoi.l2=l,inapoi.c2=c;
                        push(S,inapoi);
                    }
                }
                mutat=true;
            }
            else if(x>=15 && x<=125 && y>=10 && y<=50)
            {
                interfata_start();
                ok=false;
                mutat=true;
            }
            else
                mutat=true;
        }
    }
}

void mutare_vulpe_verticala(int linia,int coloana)
{
    evidentiere_vulpe_verticala(linia,coloana,true);
    int l,c,contor=0;
    if(tablajoc[linia][coloana]==9)
    {
        for(int i=0; i<4; i++)
            pozitii[i]=0;
           //nord
        l=linia,c=coloana;
        while(l>1 && tablajoc[l-1][c]==0)
            contor++,l--;
        if(linia-contor<-2)
            contor=0;
        pozitii[0]=contor;
        c=coloana,contor=0;
        l=linia;
        while(l<4 && tablajoc[l+2][c]==0)
            contor++,l++;
        if(linia+contor>6)
        contor=0;
        pozitii[2]=contor;
    }
    else
    {
        for(int i=0; i<4; i++)
            pozitii[i]=0;
           //nord
        l=linia,c=coloana;
        while(l>2 && tablajoc[l-2][c]==0)
            contor++,l--;
        if(linia-contor<-1)
            contor=0;
        pozitii[0]=contor;
           //sud
        c=coloana,contor=0;
        l=linia;
        while(l<5 && tablajoc[l+1][c]==0)
            contor++,l++;
        if(linia+contor>7)
            contor=0;
        pozitii[2]=contor;
    }
    afisare_mutari_posibile_vulpe_verticala(linia,coloana,"imagini_piese\\sageataliber.jpg");
    if(tablajoc[linia][coloana]==10)
        linia--;
    bool mutat=false;
    int x,y;
    while(!mutat)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            afisare_mutari_posibile_vulpe_verticala(linia,coloana,"imagini_piese\\liber.jpg");
            evidentiere_vulpe_verticala(linia,coloana,false);
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=stanga && x<=stanga+lungimetabla && y>=sus && y<=sus+lungimetabla)
            {
                l=(y-sus)/latura+1;
                c=(x-stanga)/latura+1;
                if(c==coloana)//daca se face click pe coloana cu vulpea
                {
                    //verificam daca in sud se pot efectua mutari
                    if(l-linia-1>0 && l-linia-1<=pozitii[2]) //scadem un 1 deoarece scadem patratica cu coada
                    {
                        nrmutari=nrmutari+(l-linia-1),inserare_eliminare_imagine_vulpe_verticala(linia,coloana,l,c);
                        //punem valorile in stiva
                        inapoi.tip=3,inapoi.m=l-linia-1,inapoi.l1=linia,inapoi.c1=coloana,inapoi.l2=l-1,inapoi.c2=c;
                        push(S,inapoi);
                    }
                    //altfel verificam in nord
                    else if(linia-l>0 && linia-l<=pozitii[0])
                    {
                        nrmutari=nrmutari+(linia-l),inserare_eliminare_imagine_vulpe_verticala(linia,coloana,l,c);
                         //punem valorile in stiva
                        inapoi.tip=3,inapoi.m=linia-l,inapoi.l1=linia+1,inapoi.c1=coloana,inapoi.l2=l,inapoi.c2=c;
                        push(S,inapoi);
                    }
                }
                mutat=true;
            }
            else if(x>=15 && x<=125 && y>=10 && y<=50)// butonul de inapoi neactualizat -> actualizrea x>=Lx && x<=Lx*4 && y>=Ly && y<=Ly*3 cu noul sistem de coordonate
            {
                interfata_start();
                ok=false;
                mutat=true;
            }
            else
                mutat=true;
        }
    }
}



bool verificare_castig()
{
    //pentru a castiga trebuie sa contorizam numarul total de iepuri in gauri, adica codurile 3, 5 si 7
    //daca acest contor este egal cu nr total de iepuri, atunci se castiga
    //nrtotal de iepuri l-am calculat in functia pune_piese
    int contor=0;
    if(tablajoc[1][1]>1 && tablajoc[1][1]<8)
        contor++;
    if(tablajoc[1][5]>1 && tablajoc[1][5]<8)
        contor++;
    if(tablajoc[3][3]>1 && tablajoc[3][3]<8)
        contor++;
    if(tablajoc[5][1]>1 && tablajoc[5][1]<8)
        contor++;
    if(tablajoc[5][5]>1 && tablajoc[5][5]<8)
        contor++;
    if(contor==nriepuri)
        return true;
    else
        return false;
}

void afisare_stele()
{
    //daca nr de mutari efectuate este egal cu numarul minim pt acel nivel atunci nivelul e efectuat perfect, cu 3 stele
    if(nrmutari==mutari[nivel])
    {
        readimagefile("joc\\3stele.jpg",stanga+latura,sus+latura,stanga+latura*4,sus+latura*4);
        if(stele[nivel]<3)
            stele[nivel]=3;
    }

    else if(nrmutari<=mutari[nivel]+pagina)
    {
        readimagefile("joc\\2stele.jpg",stanga+latura,sus+latura,stanga+latura*4,sus+latura*4);
        if(stele[nivel]<2)
            stele[nivel]=2;
    }

    else if(nrmutari<=mutari[nivel]+2*pagina)
    {
        readimagefile("joc\\1stea.jpg",stanga+latura,sus+latura,stanga+latura*4,sus+latura*4);
        if(stele[nivel]<1)
            stele[nivel]=1;
    }

    else
    {
        readimagefile("joc\\0stele.jpg",stanga+latura,sus+latura,stanga+latura*4,sus+latura*4);
        if(stele[nivel]<0)
        stele[nivel]=0;
    }
}

void nivel_terminat()
{
    afisare_stele(); // afisam pe ecran cate stele a obtinut jucatorul in functie de numarul de mutari pe care le a facut
    bool gata=true;
    int x,y,l,c; // l,c-> linia si coloana curente
    while(gata)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(y>=Ly*16 && y<=Ly*18)
            {
                if(x>=Lx*15 && x<=Lx*17) //buton nivele
                {
                    interfata_start();
                    ok=false,gata=false;
                }
                else if(x>=Lx*17+Lx/2 && x<=Lx*19+Lx/2) //resetare nivel
                {
                    interfata_joc();
                    gata=false;
                }
                else if(x>=Lx*20 && x<=Lx*22) //urmatorul nivel
                {
                    if(nivel<60)
                    {
                        nivel++;
                        interfata_joc();
                        gata=false;
                    }
                    else
                    {
                        interfata_start();
                        ok=false,gata=false;
                    }
                }
            }
            else if(x>=Lx && x<=Lx*4 && y>=Ly && y<=Ly*3)
            {
                interfata_start();
                ok=false,gata=false;
            }
        }
    }
    if(contorstele[pagina]==36)
    {

    }
}

void afisare_mutari_posibile(int l,int c,char liber[40],char gaura[40])
{
    for(int i=0; i<4; i++)
        if(pozitii[i])
            switch(i)
            {
            case 0:
            {
                if(tablajoc[l-pozitii[i]-1][c]==0)
                    readimagefile(liber,stanga+latura*(c-1),sus+latura*(l-pozitii[i]-2),stanga+latura*c,sus+latura*(l-pozitii[i]-1));
                else
                    readimagefile(gaura,stanga+latura*(c-1),sus+latura*(l-pozitii[i]-2),stanga+latura*c,sus+latura*(l-pozitii[i]-1));
                break;
            }
            case 1:
            {
                if(tablajoc[l][c+pozitii[i]+1]==0)
                    readimagefile(liber,stanga+latura*(c+pozitii[i]),sus+latura*(l-1),stanga+latura*(c+pozitii[i]+1),sus+latura*l);
                else
                    readimagefile(gaura,stanga+latura*(c+pozitii[i]),sus+latura*(l-1),stanga+latura*(c+pozitii[i]+1),sus+latura*l);
                break;
            }
            case 2:
            {
                if(tablajoc[l+pozitii[i]+1][c]==0)
                    readimagefile(liber,stanga+latura*(c-1),sus+latura*(l+pozitii[i]),stanga+latura*c,sus+latura*(l+pozitii[i]+1));
                else
                    readimagefile(gaura,stanga+latura*(c-1),sus+latura*(l+pozitii[i]),stanga+latura*c,sus+latura*(l+pozitii[i]+1));
                break;
            }
            case 3:
            {
                if(tablajoc[l][c-pozitii[i]-1]==0)
                    readimagefile(liber,stanga+latura*(c-pozitii[i]-2),sus+latura*(l-1),stanga+latura*(c-pozitii[i]-1),sus+latura*l);
                else
                    readimagefile(gaura,stanga+latura*(c-pozitii[i]-2),sus+latura*(l-1),stanga+latura*(c-pozitii[i]-1),sus+latura*l);
                break;
            }
            }
}

void evidentiere_iepure(int l,int c,bool da)
{
    char s[40];
    if(da)
        switch(tablajoc[l][c])
        {
        case 2:
            strcpy(s,"imagini_piese\\iepurealbev.jpg");
            break;
        case 4:
            strcpy(s,"imagini_piese\\iepuremaroev.jpg");
            break;
        case 6:
            strcpy(s,"imagini_piese\\iepuregriev.jpg");
            break;
        case 3:
            strcpy(s,"imagini_piese\\gauraalbev.jpg");
            break;
        case 5:
            strcpy(s,"imagini_piese\\gauramaroev.jpg");
            break;
        case 7:
            strcpy(s,"imagini_piese\\gauragriev.jpg");
            break;
        }
    else
        switch(tablajoc[l][c])
        {
        case 2:
            strcpy(s,"imagini_piese\\iepurealb.jpg");
            break;
        case 4:
            strcpy(s,"imagini_piese\\iepuremaro.jpg");
            break;
        case 6:
            strcpy(s,"imagini_piese\\iepuregri.jpg");
            break;
        case 3:
            strcpy(s,"imagini_piese\\gauraalb.jpg");
            break;
        case 5:
            strcpy(s,"imagini_piese\\gauramaro.jpg");
            break;
        case 7:
            strcpy(s,"imagini_piese\\gauragri.jpg");
            break;
        }
    readimagefile(s,stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
}

void inserare_eliminare_imagine_iepure(int linia,int coloana,int l,int c)
{
    char s[3];
    sprintf(scor,"%d",nrmutari);
    if(nrmutari<10)
        strcpy(s,scor),strcpy(scor,"0"),strcat(scor,s);
    outtextxy(Lx*3,Ly*7,scor);

    if(tablajoc[l][c]==0)  //verificam daca trebuie sa adaugam imagine cu un iepure liber sau un iepure in gaura
    {
        if(tablajoc[linia][coloana]%2==0)  //aici verificam daca aducem iepurele de pe un spatiu liber sau dintr-o gaura
            tablajoc[l][c]=tablajoc[linia][coloana];//desenam iepure normal
        else
            tablajoc[l][c]=tablajoc[linia][coloana]-1;// desenam iepure in gaura
        switch(tablajoc[l][c])
        {
        case 2:
            readimagefile("imagini_piese\\iepurealb.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
            break;
        case 4:
            readimagefile("imagini_piese\\iepuremaro.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
            break;
        case 6:
            readimagefile("imagini_piese\\iepuregri.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
            break;
        }
    }
    else  //iepure in gaura
    {
        if(tablajoc[linia][coloana]%2==1)  //aici verificam daca aducem iepurele de pe un spatiu liber sau dintr-o gaura
            tablajoc[l][c]=tablajoc[linia][coloana];
        else
            tablajoc[l][c]=tablajoc[linia][coloana]+1;
        switch(tablajoc[l][c])
        {
        case 3:
            readimagefile("imagini_piese\\gauraalb.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
            break;
        case 5:
            readimagefile("imagini_piese\\gauramaro.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
            break;
        case 7:
            readimagefile("imagini_piese\\gauragri.jpg",stanga+latura*(c-1),sus+latura*(l-1),stanga+latura*c,sus+latura*l);
            break;
        }
    }
    if(tablajoc[linia][coloana]%2==0)   //verificam daca in urma mutarii lasam spatiu liber sau gaura
    {
        tablajoc[linia][coloana]=0;
        readimagefile("imagini_piese\\liber.jpg",stanga+latura*(coloana-1),sus+latura*(linia-1),stanga+latura*coloana,sus+latura*linia);
    }
    else
    {
        tablajoc[linia][coloana]=1;
        readimagefile("imagini_piese\\gaura.jpg",stanga+latura*(coloana-1),sus+latura*(linia-1),stanga+latura*coloana,sus+latura*linia);
    }
}

bool mutare(int linia,int coloana,int l,int c)
{
    if(pozitii[0]!=0 && l==linia-pozitii[0]-1 && c==coloana)//verificam daca se doreste mutarea in partea de nord a iepurelui
        return true;
    else if(pozitii[1]!=0 && l==linia && c==coloana+pozitii[1]+1)  //in est
        return true;
    else if(pozitii[2]!=0 && l==linia+pozitii[2]+1 && c==coloana)  //in sud
        return true;
    else if(pozitii[3]!=0 && l==linia && c==coloana-pozitii[3]-1)  //in vest
        return true;
    return false;
}

void mutare_iepure(int linia,int coloana)
{
    evidentiere_iepure(linia,coloana,true);//evidentiem iepurele daca am dat click pe el
    int l,c,contor=0;
    for(int i=0; i<4; i++)
        pozitii[i]=0;
    //nord
    l=linia,c=coloana;
    while(l>1 && tablajoc[l-1][c]>1)//contorul creste cat timp nu am ajuns pe prima linie si pozitia respectiva este este diferita de liber(0) sau gaura(1)
        contor++,l--;
    if(linia-contor==1)//aici verificam ca iepurele sa nu sara in "afara tablei",trateaza cazul in care pe coloana sunt numai obstacole
        contor=0;
    pozitii[0]=contor;//pe prima pozitie in vectorul pozitii am retinut peste cate obstacole poate sarii iepurele in nord(in sus)
    //est
    l=linia,contor=0;
    while(c<5 && tablajoc[l][c+1]>1)
        contor++,c++;
    if(coloana+contor==5)
        contor=0;
    pozitii[1]=contor;//pe a doua pozitie in vectorul pozitii am retinut peste cate obstacole poate sarii iepurele in est(in dreapta)
    //sud
    c=coloana,contor=0;
    while(l<5 && tablajoc[l+1][c]>1)
        contor++,l++;
    if(linia+contor==5)
        contor=0;
    pozitii[2]=contor;
    //vest
    l=linia,contor=0;
    while(c>1 && tablajoc[l][c-1]>1)
        contor++,c--;
    if(coloana-contor==1)
        contor=0;
    pozitii[3]=contor;

    afisare_mutari_posibile(linia,coloana,"imagini_piese\\sageataliber.jpg","imagini_piese\\sageatagaura.jpg");//afisam sagetile

    bool mutat=false;
    int x,y;
    while(!mutat)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            //stergem sagetile si evidentierea iepurelui
            afisare_mutari_posibile(linia,coloana,"imagini_piese\\liber.jpg","imagini_piese\\gaura.jpg");
            evidentiere_iepure(linia,coloana,false);
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=stanga && x<=stanga+lungimetabla && y>=sus && y<=sus+lungimetabla)//daca s a facut click pe tabla
            {
                l=(y-sus)/latura+1;  //se calculeaza noua linie
                c=(x-stanga)/latura+1;  //si noua coloana
                if(mutare(linia,coloana,l,c))
                {
                    nrmutari++,inserare_eliminare_imagine_iepure(linia,coloana,l,c); // in aceasta functie se schimba matricea atat in memorie cat si grafic
                    //punem valorile in stiva
                    inapoi.tip=1,inapoi.m=1,inapoi.l1=linia,inapoi.c1=coloana,inapoi.l2=l,inapoi.c2=c;// se pune pe stiva
                    push(S,inapoi);
                }
                mutat=true;
            }
            else if(x>=Lx && x<=Lx*4 && y>=Ly && y<=Ly*3)// butonul de inapoi iesim in alegerea nivelului
            {
                interfata_start();
                ok=false,mutat=true;
            }
            else
                mutat=true;
        }
    }
    if(nrmutari>=mutari[nivel]) //verificam daca am efectuat un numar de mutari mai mare sau egal cu nr minim de mutari pt nivelul respectiv
    {
        castigat=verificare_castig();
        if(castigat)
            nivel_terminat();
    }
}



void joc()
{
    interfata_joc(); // se incarca interfata
    ok=true;
    int x,y,linia,coloana; //coordonate mouse
    while(ok)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=stanga && x<=stanga+lungimetabla && y>=sus && y<=sus+lungimetabla) //click pe tabla
            {
                linia=(y-sus)/latura+1; // se calculeaza linia si coloana unde am apasat pentru a identifica in memorie pe ce am apasat
                coloana=(x-stanga)/latura+1;
                if(tablajoc[linia][coloana]>=2 && tablajoc[linia][coloana]<=7)//verificam daca e iepure(2-iepure alb, 4-iepure marom, 6-iepure gri)
                    mutare_iepure(linia,coloana);//se muta iepurele, dar se verifica si daca s-a terminat nivelul
                else if((tablajoc[linia][coloana]==9 && tablajoc[linia][coloana+1]==10) || (tablajoc[linia][coloana]==10 && tablajoc[linia][coloana-1]==9))
                    mutare_vulpe_orizontala(linia,coloana);//verificam daca am dat click pe capul sau coada vulpii,in ambele cazuri putem sa o mutam
                else if((tablajoc[linia][coloana]==9 && tablajoc[linia+1][coloana]==10) || (tablajoc[linia][coloana]==10 && tablajoc[linia-1][coloana]==9))
                    mutare_vulpe_verticala(linia,coloana);//verificam daca am dat click pe capul sau coada vulpii,in ambele cazuri putem sa o mutam
            }
            else if(x>=Lx*30 && x<=Lx*36 && y>=Ly*7 && y<=Ly*9) //butonul restart
                interfata_joc();

            else if(x>=Lx*30 && x<=Lx*36 && y>=Ly*11 && y<=Ly*13) //butonul undo
            {
                if(S!=NULL)
                {
                    inapoi=S->info;
                    pop(S);
                    nrmutari=nrmutari-inapoi.m;
                    switch(inapoi.tip)
                    {
                        case 1: inserare_eliminare_imagine_iepure(inapoi.l2,inapoi.c2,inapoi.l1,inapoi.c1); break;
                        case 2: inserare_eliminare_imagine_vulpe_orizontala(inapoi.l2,inapoi.c2,inapoi.l1,inapoi.c1); break;
                        case 3: inserare_eliminare_imagine_vulpe_verticala(inapoi.l2,inapoi.c2,inapoi.l1,inapoi.c1); break;
                    }
                }
            }
            else if(x>=Lx && x<=Lx*4 && y>=Ly && y<=Ly*3) //inapoi la start
                interfata_start(),ok=false;
        }
    }
}

void start()
{
    interfata_start(); // incarcam interfata
    int x,y; // coordonate mouse
    ok=false;
    while(!ok)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=Lx && x<=Lx*4 && y>=Ly && y<=Ly*3) //inapoi la meniu
                interfata_meniu(),ok=true;
            else if(x>=Lx*30 && x<=Lx*36 && y>=Ly && y<=Ly*3) //se reseteaza nivelele
            {
                for(int i=1;i<=60;i++)
                    stele[i]=-1;
                interfata_start();
            }
            else if(x>=Lx*2 && y>=Ly*27 && y<=Ly*29) //daca am facut click pe linia cu butoanele de comutare a paginilor
            {
                x=(x-Lx*2)/Lx;  //aducem la o forma mai simpla, acum x poate lua valori intre 0 si 34
                pagina=x/7;  //determinam pagina (valorile de la 0 la 6, pagina 1, de la 7 la 13, pagina 2 etc.)
                if(x>=pagina*7 && x<=pagina*7+4)//schimbam pagina doar daca x este o voalare din primele 5
                    pagina++,interfata_start();
            }
            else if(x>=Lx*3)
            {
                x=(x-Lx*3)/Lx;  //x acum poate lua valori de la 0 la 33
                nivel=x/9;   //valorile de la 0 la 8 sunt primul nivel de pe linie, de la 9 la 17,al doilea samd
                if(x>=nivel*9 && x<=nivel*9+4) //intram in joc doar daca x este o valoare din primele 5
                {
                    if(y>=Ly*5 && y<=Ly*10)  //daca am facut click pe prima linie,cea cu primele 4 nivele din pagina
                        nivel=nivel+1+12*(pagina-1),joc();

                    else if(y>=Ly*12 && y<=Ly*17)  //daca am facut click pe a doua linie,cu urmatoarele 4 nivele din pagina
                        nivel=nivel+5+12*(pagina-1),joc();

                    else if(y>=Ly*19 && y<=Ly*24)//daca am facut click pe ultima linie, cu ultimele 4 nivele din pagina
                        nivel=nivel+9+12*(pagina-1),joc();
                }
            }
        }
    }
}

void setari()
{
    interfata_setari(); // se incarca interfata
    int x,y; //coordonate mouse
    ok=false;
    while(!ok)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=Lx && x<=Lx*5 && y>=Ly && y<=Ly*3) //inapoi la meniu
                interfata_meniu(),ok=true;
            else if(x>=Lx*12 && x<=Lx*25 && y>=Ly*14 && y<=Ly*16)//daca am dat click pe  butonul de romana
                limba=1,interfata_setari(); // se schimba limba in romana
            else if(x>=Lx*12 && x<=Lx*25 && y>=Ly*20 && y<=Ly*22)//butonul de engleza
                limba=2,interfata_setari();//...
            else if(x>=Lx*12 && x<=Lx*25 && y>=Ly*26 && y<=Ly*28)//butonul de franceza
                limba=3,interfata_setari();//...
        }
    }
}

void instructiuni()
{
    interfata_instructiuni();
    int x,y;
    ok=false;
    while(!ok)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=Lx && x<=Lx*5 && y>=Ly && y<=Ly*3) //inapoi la meniu
                interfata_meniu(),ok=true;
        }
    }
}

void insigne()
{
    interfata_insigne();
    int x,y;
    ok=false;
    while(!ok)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=Lx && x<=Lx*4 && y>=Ly && y<=Ly*3) //inapoi la meniu
                interfata_meniu(),ok=true;
        }
    }
}

void meniu()
{
    interfata_meniu(); //se incarca interfata cu limba pe care o alegem(default e pus pe romana)
    int x,y; // coordonatele unde dam click cu mouse-ul
    ok=true;
    while(ok)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,x,y);
            clearmouseclick(WM_LBUTTONDOWN);
            if(x>=Lx*13 && x<=Lx*21 && y>=Ly*9 && y<=Ly*12) // daca am dat click pe butonul de start
                start(); // intram in start
            else if(x>=Lx*13 && x<=Lx*21 && y>=Ly*13 && y<=Ly*16) // daca am dat click pe butonul de setari
                setari(); //intram in setari
            else if(x>=Lx*13 && x<=Lx*21 && y>=Ly*17 && y<=Ly*20) //daca am dat click pe butonul de instructiuni
                instructiuni();//intram in instructiuni
            else if(x>=Lx*13 && x<=Lx*21 && y>=Ly*21 && y<=Ly*24) //....
                insigne();
            else if(x>=Lx*13 && x<=Lx*25 && y>=Ly*21 && y<=Ly*28) // iesire din joc
                ok=false;
        }
    }
}

int main()
{
    initwindow(width,height);
    initializare_valori();
    meniu();
    memorare_nivele();
    return 0;
}
