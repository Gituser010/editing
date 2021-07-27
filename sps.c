#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

typedef struct 
{
    unsigned length;
    char *obsah;    
} Bunka;

typedef struct
{
    unsigned bunky;
    Bunka *data;
} Riadok;

typedef struct 
{
    unsigned riadky;
    Riadok *data;
} Tabulka;

typedef struct
{
    unsigned size;
    char **operace;
    char **parametry;
}Prikaz;

typedef struct 
{
    unsigned r1;
    unsigned r2;
    unsigned c1;
    unsigned c2;
}Vyber;

int Pow(int cislo,int mocnitel)
{
  int navratova_hodnota=cislo;
  if (mocnitel==0)
    return 1;
  for(int i=0;i<=mocnitel;i++)
  {
    if(i>1)
    {
      navratova_hodnota=navratova_hodnota*cislo;
    }
  }
  return navratova_hodnota;
}

int convert_to_int(char *cislo)
{
  int cislo_int[15];
  int vysledne_cislo=0;
  int i=0;

  //zmení pole znakov na pole intov
  while(cislo[i]!='\0')
  {
    cislo_int[i]=(int)cislo[i]-'0';
    if(i>16)
    {
      break;
    }
    if(cislo_int[i]>9)
    {
      return -1;
    }
      i++;
    }
  if (i>15)
  {
    return -1;
  }
  i--; // cislo[i] by nebolo cislo ale \0 alebo iny znak  
  int zaciatok =i;
  int a = 10;
  //uloží jednotlivé inty na správnu pozíciu v čísle;
  while(i>=0)
  {
    cislo_int[zaciatok-i]=cislo_int[zaciatok-i]* Pow(a,i);
    vysledne_cislo=vysledne_cislo+cislo_int[zaciatok-i];
    i--;
  }  
  return vysledne_cislo;
}

int convert_to_int_limited(char cislo[],int velkost_pola) 
{
  int pole_cisla[15];
  int vysledne_cislo=0;
  int i;
  //zmení pole znakov na pole intov
  for(i=0;i<=velkost_pola;i++)
  {
    pole_cisla[i]=(int)cislo[i]-'0';
  }
  
  //uloží jednotlivé inty na správnu pozíciu v čísle;
  for(int j=0;j<=velkost_pola;j++)
  {
    pole_cisla[j]=pole_cisla[j]* Pow(10,velkost_pola-j);
    vysledne_cislo=vysledne_cislo+pole_cisla[j];  
  }  
  return vysledne_cislo;
}

void vytvor_tab(Tabulka *t)
{
    t->riadky=0;
    t->data=NULL;
}
//funkcia zistuje ci sa znaka nenachaza v poli 
bool not_in_array(char *array,char znak)
{
    for(unsigned i=0;i<strlen(array);i++)
    {
      if(znak==array[i] || znak=='"' || znak==92) 
        {
        return false;
        }
    }  
    return true;
}

char nahraj_bunku(char *delim,Bunka *b,FILE *fp)
{
    char znak;
    b->length =0;
    b->obsah = calloc(1,sizeof(char));
    while((znak=fgetc(fp))!='\n' && znak!=EOF && not_in_array(delim,znak))
    {
        b->obsah[b->length]=znak;
        b->length++;
        b->obsah=realloc(b->obsah,strlen(b->obsah)+1);   
        b->obsah[b->length]='\0';
    }
    b->obsah[b->length]=znak;
    return znak;
}

char nahraj_riadok(char *delim,Riadok *r,FILE *fp)
{
    char znak;
    znak=' ';
    r->bunky=0;
    r->data=malloc(sizeof(Bunka));
    while(znak!='\n' && znak!=EOF)
    {
        znak=nahraj_bunku(delim,&r->data[r->bunky],fp);
        r->bunky++;
        r->data=realloc(r->data,sizeof(Bunka)+sizeof(Bunka)*r->bunky);        
    }
    return znak;    
    
}

bool nahraj_tabulku(char *delim,Tabulka *t,char *str)
{
    char znak;
    t->data=malloc(sizeof(Riadok));
    FILE *fp;
    fp=fopen(str,"r");
    if(fp==NULL)
    {
        fclose(fp);
        return false;
    }
    while(znak!=EOF)
    {     
        znak=nahraj_riadok(delim,&t->data[t->riadky],fp);
        t->riadky++;
        t->data=realloc(t->data,t->riadky*(sizeof(Riadok))+sizeof(Riadok));
    }
    return true;
}


void nahraj_delim(char *argument,char *delim)
{
    unsigned j=1;

    for(unsigned i=0;i<strlen(argument);i++)
    {
        if(not_in_array(delim,argument[i]))
        {            
            delim=realloc(delim,j*sizeof(char));
            delim[j-1]=argument[i];
            j++;

        }        
    }
    delim[j-1]='\0';    
}
//program ziti ci je delim zadany ak ano --> nahraj_delim(x,y);
bool ziskaj_delim(int argc,char **argument,char *delim)
{
    if( argc>2)
    {    
        if(strcmp(argument[1],"-d")==0)
        {
            nahraj_delim(argument[2],delim);
            return true;
        }
        else
            return false;
    }
    else
        return false;  
}
//test ci bol zadany dostatocny pocet argumentov
bool arg_test(int argc,bool delim_is_set,int *delim)
{
    if(delim_is_set)
    {
        if(argc>3)
        {
            *delim=2;
            return true; 
        }
        else 
            return false;
    }
    else
    {
        if(argc>1)     
            return true;       
        else       
            return false;
    }        
}

void vypis_tabulku(Tabulka *t,char *str)
{
    FILE *fptr;

   // use appropriate location if you are using MacOS or Linux
   fptr = fopen(str,"w");

   if(fptr == NULL)
   {
      printf("Error!");   
      return;             
   } 
    for(unsigned i=0;i<t->riadky;i++)   
        for(unsigned j=0;j<t->data[i].bunky;j++) 
            for(unsigned k=0;k<=t->data[i].data[j].length;k++)
             {
                if(i!=(t->riadky-1))
                    fprintf(fptr,"%c",t->data[i].data[j].obsah[k]);    
             }
}

void vycisti_tabulku(Tabulka *t)
{
       for(unsigned i=0;i<t->riadky;i++)   
       {
        for(unsigned j=0;j<t->data[i].bunky;j++) 
        {
                free(t->data[i].data[j].obsah);   
        }
        free(t->data[i].data);
       }
       free(t->data);    
}

void nacitaj_cmd(char *prikazy,Riadok *cmd)
{
   int j=0;
    
   cmd->bunky=0;    
   cmd->data=malloc(sizeof(Bunka));
   while(prikazy[j]!='\0')
   {
        cmd->data=realloc(cmd->data,sizeof(Bunka)*cmd->bunky+sizeof(Bunka));
        cmd->data[cmd->bunky].length=0;
        cmd->data[cmd->bunky].obsah=malloc(sizeof(char));
        while (prikazy[j]!=';' && prikazy[j]!='\0')
        {
            cmd->data[cmd->bunky].obsah[cmd->data[cmd->bunky].length]=prikazy[j];
            cmd->data[cmd->bunky].length++;
            cmd->data[cmd->bunky].obsah=realloc(cmd->data[cmd->bunky].obsah,strlen(cmd->data[cmd->bunky].obsah)+1);
            j++;
        }
        cmd->bunky++;
        if(prikazy[j]=='\0')
        {
            break;
        }
        j++;       
   }
}

void cmd_struct(Riadok *cmd,Prikaz *prikaz)
{
    prikaz->operace=malloc(sizeof(char *));
    prikaz->parametry=malloc(sizeof(char *));
    prikaz->parametry=NULL;
    for(unsigned i=0;i<cmd->bunky;i++)
    {
        int k=0;
        prikaz->operace=realloc(prikaz->operace,sizeof(char *)*(i+1));
        prikaz->parametry=realloc(prikaz->parametry,sizeof(char *)*(i+1));
        prikaz->operace[i]=malloc(sizeof(char *)*(i+1));
        prikaz->parametry[i]=malloc(sizeof(char *)*(i+1));
        prikaz->parametry[i]=NULL;

        for(unsigned j=0;j<cmd->data[i].length;j++)
        {               
            if(k==0 && cmd->data[i].obsah[j]!=' ')
            {
                prikaz->operace[i]=realloc(prikaz->operace[i],sizeof(char)*(j+1)+sizeof(char));
                prikaz->operace[i][j]=cmd->data[i].obsah[j]; 
                prikaz->operace[i][j+1]='\0';
                
            }
            else
            {
                if(cmd->data[i].obsah[j]!='[' || cmd->data[i].obsah[j]!=']')
                {
                    prikaz->parametry[i]=realloc(prikaz->parametry[i],sizeof(char)*(k+1)+sizeof(char)+1);
                    prikaz->parametry[i][k]=cmd->data[i].obsah[j];
                    k++;
                    prikaz->parametry[i][k]='\0';

                }
            }
        }
        prikaz->size=i+1;
    }
}

void vloz_riadok(char delim,Riadok *r,Tabulka tab)
{
    unsigned bunky=tab.data[0].bunky;
    r->data=NULL;
    r->data=malloc(sizeof(Bunka));
    r->bunky=bunky;

    for(int j=0;j<(int)bunky;j++)
    {
        r->data=realloc(r->data,sizeof(Bunka)*j+sizeof(Bunka));
        r->data[j].obsah=malloc(sizeof(char));
        r->data[j].obsah[0]=delim;
        r->data[j].length=0;
    }
    r->data[bunky-1].length=0;
    r->data[bunky-1].obsah[0]='\n';

}

void vymaz_riadok(Riadok *r)
{
   for(unsigned i=0;i<r->bunky;i++)
    {
        free(r->data[i].obsah);
        r->data[i].length=0;
    }
    free(r->data);
    r->bunky=0;
}

void vymaz_bunku(Bunka *b)
{
    
    free(b->obsah);
    b->length=0;
}

void vloz_bunku(Bunka *b,char delim)
{
    b->obsah=NULL;    
    b->obsah=malloc(sizeof(char));
    b->obsah[0]=delim;
    b->length=0;
}

void posun_riadky(unsigned index,unsigned posun,Tabulka *tab)
{
    for(unsigned i=index;i<tab->riadky;i++)
    {
        tab->data[i-posun]=tab->data[i];
    }
    tab->riadky=tab->riadky-posun;
    tab->data=realloc(tab->data,tab->riadky*(sizeof(Riadok))+sizeof(Riadok));
}

void irow(Tabulka *tab,unsigned r,char *delim)
{
    unsigned tab_size;
    tab_size=tab->riadky;
    tab->riadky++;
    tab->data=realloc(tab->data,tab->riadky*(sizeof(Riadok))+sizeof(Riadok));
    
    for(int i=tab_size-1;i>=(int)(r-1);i--)
    {
        tab->data[i+1]=tab->data[i];
    }
    
    vloz_riadok(delim[0],&tab->data[r-1],*tab);
  
}

void drow(Tabulka *tab,Vyber vyb)
{

    unsigned posun=(vyb.r2-vyb.r1+1);
    for(unsigned i=vyb.r1-1;i<vyb.r2;i++)
    {
        vymaz_riadok(&tab->data[i]);
        if((posun+i)<tab->riadky)
        {
            tab->data[i]=tab->data[i+posun];
        }
        
    }
    posun_riadky(vyb.r2,posun,tab);
}

void icol(Tabulka *tab,unsigned c,char delim)
{
    for(unsigned i=0;i<tab->riadky-1;i++)
    {
        tab->data[i].bunky++;
        tab->data[i].data=realloc(tab->data[i].data,tab->data[i].bunky*sizeof(Bunka)+sizeof(Bunka));
       
        
       
            for(int j=(int)tab->data[i].bunky;j>=(int)c;j--)
            {
                tab->data[i].data[j]=tab->data[i].data[j-1];
            }
            if((c-1)>=(tab->data[i].bunky-1))
            {
                Bunka *bunka=&tab->data[i].data[c-2];
                bunka->obsah[bunka->length]=delim;
                bunka=&tab->data[i].data[c-1];
                vloz_bunku(bunka,'\n');                    
            }
            else
            {
                Bunka *bunka=&tab->data[i].data[c-1];
                vloz_bunku(bunka,delim);                    
            }
    }
}

void dcol(Tabulka *tab,Vyber vyb)
{
    unsigned posun=vyb.c2-vyb.c1+1;
    for(unsigned i=0;i<tab->riadky-1;i++)
    {
        for(unsigned j=vyb.c1-1;j<=vyb.c2-1;j++)
        {
            vymaz_bunku(&tab->data[i].data[j]);
        }
        for(unsigned j=vyb.c2;j<=tab->data[i].bunky;j++)
        {
            tab->data[i].data[j-posun]=tab->data[i].data[j];
        }
        tab->data[i].bunky--;
        if(vyb.c2>=tab->data[i].bunky)
        {
            Bunka *bunka=&tab->data[i].data[tab->data[i].bunky-1];
            bunka->obsah[bunka->length]='\n';
        }
    }  
}

void clear(Tabulka *tab,Vyber vyb)
{
    for(unsigned i=vyb.r1-1;i<vyb.r2;i++)
    {
        for(unsigned j=vyb.c1-1;j<vyb.c2;j++)
        {
            char znak = tab->data[i].data[j].obsah[tab->data[i].data[j].length];
            tab->data[i].data[j].obsah=realloc(tab->data[i].data[j].obsah,sizeof(char));
            tab->data[i].data[j].obsah[0]=znak;
            tab->data[i].data[j].length=0;
        }
    }
}

void uprava_struktury__clr(Vyber vyb,Tabulka *tab,char *cmd,char *delim)
{
    if(strcmp("irow",cmd)==0)
        irow(tab,vyb.r1,delim);
    else if(strcmp("arow",cmd)==0)
        irow(tab,vyb.r2+1,delim);
    else if(strcmp("drow",cmd)==0)
        drow(tab,vyb);
    else if(strcmp("icol",cmd)==0)
        icol(tab,vyb.c1,delim[0]);
    else if(strcmp("acol",cmd)==0)
        icol(tab,vyb.c2+1,delim[0]);
    else if(strcmp("dcol",cmd)==0)
        dcol(tab,vyb);
    else if(strcmp("clear",cmd)==0)
        clear(tab,vyb);
    else
    {
        fprintf(stderr,"nespravny prikaz");
    }         
}

void vyber_2_parametre(Vyber *vyb,char **retazec,Tabulka *tab)
{
    int cislo;

    if(strcmp("_",retazec[0])==0 || strcmp(retazec[0],"-")==0)
    {
        vyb->r1=1;
        vyb->r2=tab->riadky-1;
        if(strcmp("_",retazec[1])==0 || strcmp(retazec[1],"-")==0)
        {
            vyb->c1=1;
            vyb->c2=tab->data[0].bunky;
        }
        else if((cislo=convert_to_int(retazec[1]))!=-1)
        {
            vyb->c1=(unsigned)cislo;
            vyb->c2=(unsigned)cislo;
        }
        else
        {
            fprintf(stderr,"neplatny vyber");
        }
    }

    else if((cislo=convert_to_int(retazec[0]))!=-1)
    {
        vyb->r1=cislo;
        vyb->r2=cislo;
        if(strcmp("_",retazec[1])==0 || strcmp("-",retazec[1])==0)
        {
            vyb->c1=1;
            vyb->c2=tab->data[0].bunky;
        }
        else if((cislo=convert_to_int(retazec[1]))!=-1)
        {
            vyb->c1=(unsigned)cislo;
            vyb->c2=(unsigned)cislo;
        }
    }
    
}

int ziskaj_max(Riadok r,Vyber *vyb,int max)
{
    int n_max=-2147483648;
    int cislo;
    int stlpec =0;
    unsigned i;
    char help,help1;

    for(i=vyb->c1-1;i<vyb->c2;i++)
    {
        if(r.data[i].obsah[0]=='-')
        {
            help=r.data[i].obsah[0];
            r.data[i].obsah[0]='0';
            help1=r.data[i].obsah[r.data[i].length];
            r.data[i].obsah[r.data[i].length]='\0';
            if(convert_to_int(r.data[i].obsah)!=-1)
            {

                if(((-1)*(cislo=convert_to_int_limited(r.data[i].obsah,r.data[i].length-1)))>n_max)
                {
                    n_max=cislo*(-1);
                    stlpec=i;
                }
            }
            r.data[i].obsah[r.data[i].length]=help1;
            r.data[i].obsah[0]=help;          
        }
        else
        {
            help1=r.data[i].obsah[r.data[i].length];
            r.data[i].obsah[r.data[i].length]='\0';
            if(convert_to_int(r.data[i].obsah)!=-1 && r.data[i].length>0)
            {
                if((cislo=convert_to_int_limited(r.data[i].obsah,r.data[i].length-1))>n_max)
                {
                    n_max=cislo;
                    stlpec=i;
                }
            }
            r.data[i].obsah[r.data[i].length]=help1;
        }        
    }
    if(n_max>max)
    {
        vyb->c1=stlpec+1;
        vyb->c2=stlpec+1;
        return n_max;
    }
    else
    {
        return max;
    }
}

void find_max(Tabulka tab,Vyber *vyb)
{
    int max=-2147483648;
    int cislo;
    for(unsigned i=vyb->r1-1;i<vyb->r2-1;i++)
    {
        cislo=ziskaj_max(tab.data[i],vyb,max);   
        if(cislo>max)
        {
            max=cislo;
            vyb->r1=i+1;
            vyb->r2=i+1;
        }
    }
}

int ziskaj_min(Riadok r,Vyber *vyb,int min)
{
    int n_min=2147483647;
    int cislo;
    int stlpec =0;
    unsigned i;
    char help,help1;

    for(i=vyb->c1-1;i<vyb->c2;i++)
    {
        if(r.data[i].obsah[0]=='-')
        {
            help=r.data[i].obsah[0];
            r.data[i].obsah[0]='0';
            help1=r.data[i].obsah[r.data[i].length];
            r.data[i].obsah[r.data[i].length]='\0';
            if(convert_to_int(r.data[i].obsah)!=-1)
            {
                if(((-1)*(cislo=convert_to_int_limited(r.data[i].obsah,r.data[i].length-1)))<n_min)
                {
                    n_min=cislo*(-1);
                    stlpec=i;
                }
            }
            r.data[i].obsah[r.data[i].length]=help1;
            r.data[i].obsah[0]=help;          
        }
        else
        {
            help1=r.data[i].obsah[r.data[i].length];
            r.data[i].obsah[r.data[i].length]='\0';
            if(convert_to_int(r.data[i].obsah)!=-1 && r.data[i].length>0)
            {
                if((cislo=convert_to_int_limited(r.data[i].obsah,r.data[i].length-1))<n_min)
                {
                    n_min=cislo;
                    stlpec=i;
                }
            }
            r.data[i].obsah[r.data[i].length]=help1;
        }        
    }
    if(n_min<min)
    {
        vyb->c1=stlpec+1;
        vyb->c2=stlpec+1;
        return n_min;
    }
    else
    {
        return min;
    }
}

void find_min(Tabulka tab,Vyber *vyb)
{
    int min=214748364;
    int cislo;
    for(unsigned i=vyb->r1-1;i<vyb->r2-1;i++)
    {
        cislo=ziskaj_min(tab.data[i],vyb,min);   
        if(cislo<min)
        {
            min=cislo;
            vyb->r1=i+1;
            vyb->r2=i+1;
        }
    }
    

}

bool contains(char *podretazec,char *retazec)
{
    
    bool value=false;
    for(unsigned i=0; i<strlen(retazec);i++)
    {
        if(retazec[i]==podretazec[1])
        {
            for(unsigned j=1;j<strlen(podretazec)-2;j++)
            {
                if(retazec[i+j]!=podretazec[1+j])
                {
                    value=false;
                    break;
                }
                else
                    value=true;
            }
            if(value==true)
            {
                break;
            }
        }
    }
    return value;  
}

void find_str(char *retazec,Tabulka tab,Vyber *vyb)
{   bool contain=false;
    unsigned k;
     for(unsigned i=vyb->r1-1;i<vyb->r2-1;i++)
     {        
        for(k=vyb->c1-1;k<vyb->c2;k++)
        {
            if(contains(retazec,tab.data[i].data[k].obsah))
            {
                vyb->r1=i+1;
                vyb->r2=i+1;
                vyb->c1=k+1;
                vyb->c2=k+1;
                contain=true;
                break;
            }
        }
        if(contain)
            break;
     }
}

void vyber_4_parametre(Vyber *vyb,char **retazec,Tabulka *tab)
{
    for(int i=0;i<4;i++)
    {
        if(strcmp(retazec[i],"_")==0 || strcmp(retazec[i],"-")==0)
        {
            if(i==0)
                vyb->r1=1;
            if(i==1)
                vyb->c1=1;
            if(i==2)
                vyb->r2=tab->riadky-1;
            if(i==3)
                vyb->c2=tab->data[0].bunky;
        }
        else
        {
            if(i==0)
                vyb->r1=(unsigned)convert_to_int(retazec[i]);
            if(i==1)
                vyb->c1=(unsigned)convert_to_int(retazec[i]);
            if(i==2)
                vyb->r2=(unsigned)convert_to_int(retazec[i]);
            if(i==3)
                vyb->c2=(unsigned)convert_to_int(retazec[i]);
        }
    }
}

void zmena_vyberu(Vyber *vyb,char operacia[],Tabulka *tab)
{
  int dlzka =strlen(operacia); 
  int j=0;
  char **retazec;
  retazec = malloc(4*sizeof(char*));
  int k=0;
  if(operacia[dlzka-1]==']')
  {
      retazec[0]=malloc(sizeof(char));
      retazec[1]=malloc(sizeof(char));
      retazec[2]=malloc(sizeof(char));
      retazec[3]=malloc(sizeof(char));
      for(int i=1;i<dlzka-1;i++)
      {
        if(operacia[i]==',')
        {
            retazec[j][k]='\0';
            j++;
            k=0;
            continue;      
        }
        retazec[j]=realloc(retazec[j],sizeof(char)*k+sizeof(char));
        retazec[j][k]=operacia[i];
        k++;
      }
      retazec[j][k]='\0';
     if(j==0)
      {
        if(strcmp("max",retazec[0])==0)
            find_max(*tab,vyb);
        else if(strcmp("min",retazec[0])==0)
            find_min(*tab,vyb);
   
        else
        {
          fprintf(stderr,"---neplatny vyber");
        } 
      }
      
      if(j==1)
      {
          vyber_2_parametre(vyb,retazec,tab);
      }
      if(j==3)
      {
          vyber_4_parametre(vyb,retazec,tab);
      }
      for(int i=0;i<4;i++)
        free(retazec[i]);
      free(retazec);
  }
  else
  {
      fprintf(stderr,"neplatny vyber");
  }
}

void set_str(char *str,Tabulka *tab,Vyber *vyb)
{
   //printf("%d %d ",vyb->r1,vyb->r2);
    for(int i=(int)vyb->r1-1;i<(int)vyb->r2;i++)
    {
        //printf("in");
        for(int j=(int)vyb->c1-1;j<(int)vyb->c2;j++)
        {
            if(strlen(str)==0)
            {
                free(tab->data[i].data[j].obsah);
                break;
            }       
            Bunka *bunka=&tab->data[i].data[j];
            char znak = bunka->obsah[bunka->length];
            free(tab->data[i].data[j].obsah);
            bunka->obsah=malloc(sizeof(char)*strlen(str));
            for(int i=0;i<(int)strlen(str)-1;i++)
            {
                if(str[i+1]=='\n' || str[i+1]==':')
                    break;
                bunka->obsah[i]=str[i+1];
            }
            bunka->length=strlen(str)-1;
            bunka->obsah[bunka->length]=znak;
        }
    }
}

void swap(Vyber *vyb2,Vyber *vyb1,Tabulka *tab)
{
    Bunka *bunka1 =&tab->data[vyb1->r1-1].data[vyb1->c1-1];
    Bunka *bunka2 =&tab->data[vyb2->r1-1].data[vyb2->c1-1];

    char obsah1[bunka1->length];
    obsah1[0]=' ';
    
    for(int i=1 ;i<=(int)bunka1->length;i++)
    {
        obsah1[i]=bunka1->obsah[i-1];
        obsah1[i+1]='\0';
    }

    char obsah2[bunka2->length];
    obsah2[0]=' ';
    for(int j=1 ;j<=(int)bunka2->length;j++)
    {
        obsah2[j]=bunka2->obsah[j-1];
        obsah2[j+1]='\0';
    }
        
       if(bunka2->length==0)
       {
            set_str(obsah1,tab,vyb2);
            set_str(" ",tab,vyb1);   
       }
       else
       {
              set_str(obsah1,tab,vyb2);
              set_str(obsah2,tab,vyb1); 
       }  
}

void nahraj_cislo(Vyber *bunka,Tabulka *tab,double value)
{
    char str[80];

    sprintf(str, "%g",value);
  

    for(int i=strlen(str)-1;i>=0;i--)
    {
         if(i==(int)strlen(str)-1)
        {
           str[i+2]='\0'; 
        }
        str[i+1]=str[i];
    
    }
    clear(tab,*bunka);
    set_str(str,tab,bunka);
}

void sum(Vyber *vyb,Vyber *bunka,Tabulka *tab)
{
    bool obsahuje=false;
    double value=0;
    for(int i=(int)vyb->r1-1;i<(int)vyb->r2;i++)
    {
        for(int j=(int)vyb->c1-1;j<(int)vyb->c2;j++)
        {
            if(strtod(tab->data[i].data[j].obsah,NULL)!=0)
              {
               obsahuje=true;
               value=value+strtod(tab->data[i].data[j].obsah, NULL);
              }
        }
    }
    if(obsahuje)
    {
        nahraj_cislo(bunka,tab,value);
    }
}

void avg(Vyber *vyb,Vyber *bunka,Tabulka *tab)
{
    double obsahuje=0;
    double value=0;
    for(int i=(int)vyb->r1-1;i<(int)vyb->r2;i++)
    {
        for(int j=(int)vyb->c1-1;j<(int)vyb->c2;j++)
        {
            if(strtod(tab->data[i].data[j].obsah,NULL)!=0)
              {
               obsahuje++;
               value=value+strtod(tab->data[i].data[j].obsah, NULL);
              }
        }
    }
    value=(value/obsahuje);
    if(obsahuje!=0)
    {
        nahraj_cislo(bunka,tab,value);
    }
}

void count(Vyber *vyb,Vyber *bunka,Tabulka *tab)
{
    double obsahuje=0;
    for(int i=(int)vyb->r1-1;i<(int)vyb->r2;i++)
    {
        for(int j=(int)vyb->c1-1;j<(int)vyb->c2;j++)
        {
            if(strtod(tab->data[i].data[j].obsah,NULL)!=0)
              {
               obsahuje++;
              }
        }
    }
    if(obsahuje!=0)
    {
        nahraj_cislo(bunka,tab,obsahuje);
    }
}

void len(Vyber *vyb,Vyber *bunka,Tabulka *tab)
{
    double dlzka;
    dlzka=tab->data[vyb->r1-1].data[vyb->c2-1].length;

    nahraj_cislo(bunka,tab,dlzka);
}

Vyber ziskaj_bunku(char *operace)
{
    Vyber vyb;
    char cislo1[12],cislo2[12];
    int j=-1;
    for(unsigned i=2;i<strlen(operace)-1;i++)
    {
        if(j>=0)
        {
            cislo2[j]=operace[i];
            j++;
            cislo2[j]='\0';
        }
        else if(operace[i]==',')
        {
            j++;
        }
        else
        {
            cislo1[i-2]=operace[i];    
            cislo1[i-1]='\0';
        }                
    }
    vyb.r1=(unsigned)convert_to_int(cislo1);
    vyb.c1=(unsigned)convert_to_int(cislo2);
    vyb.r2=(unsigned)convert_to_int(cislo1);
    vyb.c2=(unsigned)convert_to_int(cislo2);
    return vyb;
}

void uprava_obsahu(char *operace,char *parametre,Tabulka *tab,Vyber vyb)
{
    Vyber vyb2;
    vyb2=ziskaj_bunku(parametre);

    if(strcmp("swap",operace)==0)
        swap(&vyb,&vyb2,tab);
    else if(strcmp("sum",operace)==0)
        sum(&vyb,&vyb2,tab);
    else if(strcmp("avg",operace)==0)
        avg(&vyb,&vyb2,tab);
    else if(strcmp("count",operace)==0)
        count(&vyb,&vyb2,tab);
    else if(strcmp("len",operace)==0)
        len(&vyb,&vyb2,tab);
    else
    {
        fprintf(stderr,"cybne parametre");
    }
    

}

double ziskaj_x(char *paramtre)
{
    int a= strlen(paramtre);
    for (int i = 1; i < a; i++)
    {
        paramtre[i]=paramtre[i+1];
    }
    return strtod(paramtre,NULL);  
}
void def(Vyber vyb,int x,Tabulka *tab,Riadok *riadok)
{
    Bunka bunka=tab->data[vyb.r1-1].data[vyb.c1-1];
    riadok->data[x].obsah=malloc(sizeof(char)*bunka.length+sizeof(char)); 
    riadok->data[x].obsah= bunka.obsah;
    riadok->data[x].length=bunka.length;
}

void use(Vyber vyb,int x,Tabulka *tab,Riadok *riadok)
{
    char *str;
    str =malloc(sizeof(char)*tab->data[x].bunky+sizeof(char));

    str[0]=' ';
    for(int i=(int)(riadok->data[x].length-1);i>=0;i--)
    {
        str[i+1]=riadok->data[x].obsah[i];
    }
    str[(int)(riadok->data[x].length+1)]='\0';
    set_str(str,tab,&vyb);
    free(str);
}
void inc(Vyber vyb,int x,Tabulka *tab,Riadok *riadok)
{
   double i; 

   i=strtod(riadok->data[x].obsah,NULL);
   i++;
   nahraj_cislo(&vyb,tab,i);
}


void docasne_premenne(char *operace,char *parametre,Tabulka *tab,Vyber vyb,Riadok *riadok)
{
    double x=ziskaj_x(parametre);

    if(x<0||x>9)
        return;
    else
    {
        if(strcmp("def",operace)==0)
        {
            def(vyb,(int)x,tab,riadok);
        }
        else if(strcmp("use",operace)==0)
        {
            use(vyb,(int)x,tab,riadok);
        }
        else if(strcmp("inc",operace)==0)
        {
            inc(vyb,(int)x,tab,riadok);
        }
    }        
}

void exc_cmd(Tabulka *tab,Prikaz *cmd,char *delim)
{
    Vyber vyb;
    vyb.c1=1;
    vyb.r1=1;
    vyb.c2=1;
    vyb.r2=1;
    
    Riadok hodnota;
    hodnota.bunky=10;
    hodnota.data=malloc(sizeof(Bunka)*10);

    
    for(unsigned i=0;i<cmd->size;i++)
    {
        if(cmd->operace[i][0]=='[' && cmd->operace[i][strlen(cmd->operace[i])-1]==']')
            zmena_vyberu(&vyb,cmd->operace[i],tab);
        else if(strcmp("[find",cmd->operace[i])==0)
            find_str(cmd->parametry[i],*tab,&vyb);
        else if(cmd->parametry[i]==NULL)
            uprava_struktury__clr(vyb,tab,cmd->operace[i],delim);
        else if(strcmp("set",cmd->operace[i])==0)
            set_str(cmd->parametry[i],tab,&vyb);
        else if(cmd->parametry[i][1]=='[')
        {
            uprava_obsahu(cmd->operace[i],cmd->parametry[i],tab,vyb);
        }
        else if(cmd->parametry[i][1]=='_')
        {
            docasne_premenne(cmd->operace[i],cmd->parametry[i],tab,vyb,&hodnota);
        }
        
    }
    free(hodnota.data);
}

int main(int argc, char **argv)
{
 char *delim ;
 int *argc_delim; //premenna typu int ak je delim zadany argumenty zanim budu mat index +2 
 argc_delim= malloc(sizeof(int));
 delim = malloc(sizeof(char));
 *argc_delim=0;
 strcpy(delim, " ");

 bool delim_is_set= false;
 bool valid;

 delim_is_set = ziskaj_delim(argc,argv,delim);
 valid = arg_test(argc,delim_is_set,argc_delim);
 Tabulka tab;
 vytvor_tab(&tab);
 
 if(valid)
 {
    if(!nahraj_tabulku(delim,&tab,argv[2+*argc_delim]))  
    {
        fprintf(stderr,"error");
        return -1;
    }
    Riadok cmd;
    cmd.bunky=0;
    cmd.data=NULL;
    nacitaj_cmd(argv[1+*argc_delim],&cmd);
    Prikaz cmnd;
    cmd_struct(&cmd,&cmnd);  
    //vypis_tabulku(&tab);
    exc_cmd(&tab,&cmnd,delim);

    vypis_tabulku(&tab,argv[2+*argc_delim]);
 }

 else
 {
    fprintf(stderr,"malo parametrov");
 }
 vycisti_tabulku(&tab);
free(argc_delim);
free(delim); 
return 0;
}