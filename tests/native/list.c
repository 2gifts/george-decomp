#include <stdio.h>
#include <string.h>
#include "george/list.h"
const char D_00447040[]="null",D_00447058[]="header",D_004470B0[]="head",D_004470E0[]="tail",D_00447110[]="prevnull",D_00447140[]="broken",D_00447160[]="foreign";
static unsigned checks,failures;
#define CHECK(x) do{++checks;if(!(x)){++failures;printf("failure line%d\n",__LINE__);}}while(0)
static void order(GeorgeList *list,GeorgeListNode **expected,int count){
 GeorgeListNode *p=(GeorgeListNode*)list,*n=list->head;int i;
 CHECK(list->tail==0);CHECK(func_002ADB70(list)==0);
 for(i=0;i<count;i++){CHECK(n==expected[i]);CHECK(n->previous==p);CHECK(func_002ADC50(n)==(GeorgeListNode*)list);CHECK(func_002ADB48(p)==n);p=n;n=n->next;}
 CHECK(n==(GeorgeListNode*)&list->tail);CHECK(n->previous==p);CHECK(list->tail_previous==p);CHECK(func_002ADB48(p)==0);CHECK(func_002ADC50(n)==(GeorgeListNode*)list);
}
int main(void){
 GeorgeList a,b;GeorgeListNode nodes[10],*e[10],*n;int i,j,k,mode;
 memset(nodes,0,sizeof(nodes));func_002AD9A8(&a);order(&a,e,0);CHECK(func_002ADA00(&a)==0);
 func_002AD9C0(&a,&nodes[0]);func_002AD9E0(&a,&nodes[1]);func_002AD9C0(&a,&nodes[2]);e[0]=&nodes[1];e[1]=&nodes[0];e[2]=&nodes[2];order(&a,e,3);
 func_002ADB08(&nodes[2],&nodes[3]);func_002ADB28(&nodes[1],&nodes[4]);e[1]=&nodes[4];e[2]=&nodes[0];e[3]=&nodes[3];e[4]=&nodes[2];order(&a,e,5);
 func_002ADAE0(&nodes[0]);CHECK(nodes[0].next==0&&nodes[0].previous==0);e[2]=&nodes[3];e[3]=&nodes[2];order(&a,e,4);
 for(i=0;i<4;i++){n=func_002ADA00(&a);CHECK(n==e[i]);CHECK(n->next==0&&n->previous==0);}order(&a,e,0);
 for(mode=0;mode<2;mode++)for(i=0;i<4;i++)for(j=0;j<4;j++){
  func_002AD9A8(&a);func_002AD9A8(&b);memset(nodes,0,sizeof(nodes));
  for(k=0;k<i;k++)func_002AD9C0(&a,&nodes[k]);for(k=0;k<j;k++)func_002AD9C0(&b,&nodes[4+k]);
  if(mode==0){func_002ADA30(&a,&b);for(k=0;k<i;k++)e[k]=&nodes[k];for(k=0;k<j;k++)e[i+k]=&nodes[4+k];}
  else{func_002ADA88(&a,&b);for(k=0;k<j;k++)e[k]=&nodes[4+k];for(k=0;k<i;k++)e[j+k]=&nodes[k];}
  order(&a,e,i+j);order(&b,e,0);
 }
 CHECK(func_002ADB70(0)==D_00447040);func_002AD9A8(&a);a.head=0;CHECK(func_002ADB70(&a)==D_00447058);
 func_002AD9A8(&a);a.tail=&nodes[0];CHECK(func_002ADB70(&a)==D_00447058);func_002AD9A8(&a);a.tail_previous=0;CHECK(func_002ADB70(&a)==D_00447058);
 for(mode=0;mode<5;mode++){
  func_002AD9A8(&a);memset(nodes,0,sizeof(nodes));for(k=0;k<3;k++)func_002AD9C0(&a,&nodes[k]);
  if(mode==0){nodes[0].previous=&nodes[5];CHECK(func_002ADB70(&a)==D_004470B0);}
  if(mode==1){nodes[2].next=&nodes[5];CHECK(func_002ADB70(&a)==D_004470E0);}
  if(mode==2){nodes[1].previous=0;CHECK(func_002ADB70(&a)==D_00447110);}
  if(mode==3){nodes[1].previous=&nodes[5];CHECK(func_002ADB70(&a)==D_00447140);}
  if(mode==4){nodes[0].next=&nodes[5];CHECK(func_002ADB70(&a)==D_00447160);}
 }
 printf("list semantic checks: %u, failures: %u\n",checks,failures);return failures!=0;
}
