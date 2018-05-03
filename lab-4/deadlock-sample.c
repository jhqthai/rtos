////************************************************////
//-----------This is the program code of L5--------////
//-----------It uses to detect deadlock process----////
////***********************************************////
 
#include<stdio.h>
int Request[5][3]; //the process request
int Allocation[5][3];// already allocate resources to process
int Available[3];//available instances for each resource
int p,r;//the no. of process and resource

//input example
void input_example()
{
    int i,j;
    p=5;//Process No.
    r=3;//Resource no.
    //Enter the Request Matrix
    Request[0][0]=0;Request[0][1]=0;Request[0][2]=0;
    Request[1][0]=2;Request[1][1]=0;Request[1][2]=2;
    Request[2][0]=0;Request[2][1]=0;Request[2][2]=0;
    Request[3][0]=1;Request[3][1]=0;Request[3][2]=0;
    Request[4][0]=0;Request[4][1]=0;Request[4][2]=2;

    //Enter the Allocation Matrix
    Allocation[0][0]=0;Allocation[0][1]=1;Allocation[0][2]=0;
    Allocation[1][0]=2;Allocation[1][1]=0;Allocation[1][2]=0;
    Allocation[2][0]=3;Allocation[2][1]=0;Allocation[2][2]=3;
    Allocation[1][0]=2;Allocation[1][1]=1;Allocation[1][2]=1;
    Allocation[2][0]=0;Allocation[2][1]=0;Allocation[2][2]=2;

    //Enter the available Resources
    Available[0]=0;Available[1]=0;Available[2]=0;
}

//calculate dead-lock
void cal()
{
    int finish[5],dead[5];//store whether the process finish and dead process id.
    int k,i,j,deadlockNum,flag;//k,i,j is index variable. flag use for whether there is deadlock

    for(i=0;i<p;i++)
    {
         finish[i]=0;
    }
    ////---------------------------detect deadlock process----------------------------////
    flag=0;//whether there is a deadlock: 0 is no deadlock, 1 is deadlock
    i=0;
    while(!flag && i<p)
    {
       int c=0;
       for(j=0;j<r;j++)//for every instance
       { 
	  //need less than available source of instance j
          if(Request[i][j]<=Available[j])
          {
             c++;
             if(c==r)//if all resources satisfy the request of process i. It is not deadlock.
             {
				flag=0;//not deadlock
		        //if it can be done, the allocated resources can be released after finish
                for(k=0;k<r;k++)
                {
                   //release the allocation resource of process i
				   // add your program
				   
                   //if it is done, set finish to 1
				   //add your program 
                }			 
              }
           }
	   else// if there is one resource not satisfy the request, it is deadlock
	   {
	      
		  //add your program here.
		  
	      break;
	   }
        }//for j end
	if(flag)break;//if there is deadlock
	i++;
     }//while end
     

     ////----------------------------display deadlock process--------------------------/////
     if(flag==1)
     {
         printf("\nSystem is in Deadlock\n\n");
     }
     else
     {
         printf("\nNo Deadlock Occur\n\n");
     }  
}

int main()
{
    printf("\n********** Deadlock Detection Algo ************\n");
    input_example();
    cal();//detect deadlock
    return 0;
}


