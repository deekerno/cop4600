#include <stdio.h>
#include <string.h>

#define MAXPROCS 50

typedef struct
{
    char name[20];
    int arrival;
    int burst;
    int finish;
} process;

process turnTime[MAXPROCS];  //assume arrival=arr and burst=fin
process proc[MAXPROCS];
process queue[MAXPROCS];
int front = -1;
int rear = -1;

void roundRobin(int procCount, int runFor, int quantum);
void fcfs();
void sjf();
void push(process x);
void pop();

int main()
{
    int procCount;
    int runFor = 0;
    char use[5];
    int quantum;
    char ignore[1000];

    //For the words "processcount", "runfor", "use", "quantum" in input file
    char str1[12], str2[6], str3[3], str4[7];

    FILE *ifp;
    ifp = fopen("processes.in", "r");

    fscanf(ifp, "%s %d", str1, &procCount);
    while(fgetc(ifp) != '\n')  //ignore rest of the line
        continue;

    fscanf(ifp, "%s %d", str2, &runFor);
    while(fgetc(ifp) != '\n')
        continue;

    fscanf(ifp, "%s %s", str3, use);
    while(fgetc(ifp) != '\n')
        continue;

    if(strcmp(use, "rr") == 0)
    {
        fscanf(ifp, "%s %d", str4, &quantum);
        while(fgetc(ifp) != '\n')
            continue;
    }
    else if(strcmp(use, "fcfs") == 0 || strcmp(use, "sjf") == 0)
    {
        //skip the entire next line
        while(fgetc(ifp) != '\n')
            continue;
    }

    //store info for each process
    int i;
    for(i=0; i<procCount; i++)
    {
        fscanf(ifp, "%s %s %s %s %d %s %d", ignore, ignore, proc[i].name, ignore,
               &proc[i].arrival, ignore, &proc[i].burst);
    }

    //check struct
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", proc[i].name, proc[i].arrival, proc[i].burst);
    printf("\n");

    //top of the output file
    printf("%d processes\n", procCount);

    if(strcmp(use, "rr") == 0)
    {
        printf("Using Round-Robin \n");
        printf("Quantum %d\n", quantum);
    }
    else if(strcmp(use, "fcfs") == 0)
    {
        printf("Using First-Come First Served \n");
    }
    else if(strcmp(use, "sjf") == 0)
    {
        printf("Using Shortest Job First \n");
    }
    printf("\n");


    //function calls
    if(strcmp(use, "rr") == 0)
    {
        roundRobin(procCount, runFor, quantum);
    }
    else if(strcmp(use, "fcfs") == 0)
    {
        fcfs();
    }
    else if(strcmp(use, "sjf") == 0)
    {
        sjf();
    }
}

void roundRobin(int procCount, int runFor, int quantum)
{
    int i,j,k;
    int curTime = 0;
    int max;

    //sort processes by arrival time
    max = proc[0].arrival;
    process temp;
    for(i=1; i<procCount; i++)
    {
        //swap
        if(proc[i].arrival < max)
        {
            temp = proc[i-1];
            proc[i-1] = proc[i];
            proc[i] = temp;
        }
    }

    /* //check sort
    printf("the sort: \n");
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", proc[i].name, proc[i].arrival, proc[i].burst);
    printf("\n"); */

    //add processes to a queue
    for(i=0; i<procCount; i++)
    {
        push(proc[i]);
    }

    //make a copy
    for(i=0; i<procCount; i++)
    {
        turnTime[i] = proc[i];
    }

    /* //check queue
    printf("the queue: \n");
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", queue[i].name, queue[i].arrival, queue[i].burst);
    printf("\n"); */


    int count = 0;
    int zeroBurst = 0;
    int tStatus = 0;  //timeStatus
    char status[15][10];
    process temp2;

    while(curTime != runFor)
    {
        if(curTime == queue[front].arrival)
        {
            printf("Time %d: %s arrived \n", curTime, queue[front].name);
            for(j=0; j<MAXPROCS; j++)
            {
                if(strcmp(queue[front].name, turnTime[j].name) == 0)
                    turnTime[j].arrival = curTime;
            }
        }

        //keep running process if others have not arrived
        if(curTime < queue[front+1].arrival)
        {
            printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

            //run it quantum times
            for(i=0; i<quantum; i++)
            {
                //array storage
                curTime++;

                if(curTime == queue[front+1].arrival)
                {
                    printf("Time %d: %s arrived \n", curTime, queue[front+1].name);
                    for(j=0; j<MAXPROCS; j++)
                    {
                        if(strcmp(queue[front+1].name, turnTime[j].name) == 0)
                            turnTime[j].arrival = curTime;
                    }
                }
                queue[front].burst -= 1;
            }

            if(curTime > queue[front+1].arrival)
            {
                temp2 = queue[front];
                pop();
                push(temp2);
            }
        }

        if(curTime >= queue[front+1].arrival && front!=-1)
        {
            //done with process in front for now
            //temp2 = queue[front];
			//pop();
			//push(temp2);
			//printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

            //a full quantum can run
            if(queue[front].burst >= quantum)
            {
                //temporary perhaps
                if(curTime == queue[front+1].arrival)
                {
                    printf("Time %d: %s arrived \n", curTime, queue[front+1].name);
                    for(j=0; j<MAXPROCS; j++)
                    {
                        if(strcmp(queue[front+1].name, turnTime[j].name) == 0)
                            turnTime[j].arrival = curTime;
                    }
                }

                printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

                for(i=0; i<quantum; i++)  //leave alone
                {
                    curTime++;
                    queue[front].burst -= 1;
                }

                if(queue[front].burst == 0)
                {
                    printf("Time %d: %s finished \n", curTime, queue[front].name);
                    for(k=0; k<MAXPROCS; k++)
                    {
                        if(strcmp(queue[front].name, turnTime[k].name) == 0)
                            turnTime[k].finish = curTime;
                    }
                    pop();
                }
                else
                {
                    temp2 = queue[front];
                    pop();
                    push(temp2);
                }
            }

            //not enough for a full quantum. a process is guaranteed to finish if it gets in here
            else
            {
                //if(queue[front].burst == 0)
                //    ;
                //else
                    printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

                for(j=0; j<queue[front].burst+1; j++)
                {
                    curTime++;
                    queue[front].burst -= 1;
                }

                printf("Time %d: %s finished \n", curTime, queue[front].name);
                for(k=0; k<MAXPROCS; k++)
                {
                    if(strcmp(queue[front].name, turnTime[k].name) == 0)
                        turnTime[k].finish = curTime;
                }
                pop();
            }
        }

        //queue is empty
        if(front == -1)
        {
            printf("Time %d: Idle \n", curTime);
            curTime++;
        }

        if(front==-1 && runFor-curTime==0)
        {
            printf("Finished at time %d \n", curTime);
        }

        //when the processes don't finish
        if(curTime > runFor)
        {
            break;
        }

    } //end of while


    //print if any processes didn't finish in time.
    //queue isn't empty, runFor is up, and processes still have burst
    //used to be if(front==-1 && runFor-curTime==0)
    if(front != -1)
    {
        int numElements;
        int index;

        numElements = (rear + MAXPROCS - front) % MAXPROCS + 1;
        printf("\n");

        for(i=0; i<numElements; i++)
        {
            index = (front + i) % MAXPROCS;

            if(queue[index].burst != 0)
                printf("Process %s did not finish in time. \n", queue[index].name);
        }
        printf("\n");;

        //printf("\n");
        //printf("Processes did not finish in time. \n");
    }

    /*printf("turn: \n");
    for(i=0; i<10; i++)
        printf("%s %d %d \n", turnTime[i].name, turnTime[i].arrival, turnTime[i].finish);
    printf("\n");
    */

    printf("\n");
    for(i=0; i<procCount; i++)
    {
        printf("%s wait  turnaround %d \n", turnTime[i].name, (turnTime[i].finish)-(turnTime[i].arrival));
    }

}

void fcfs()
{

}

void sjf()
{

}

void push(process x)
{
    //check if queue is empty
    if(front == -1 && rear == -1)
    {
        front = 0;
        rear = 0;
    }

    //normal case
    else
        rear = (rear+1) % MAXPROCS;

    queue[rear] = x;
}

void pop()
{
    //when there is 1 element left in queue.
    if(front == rear)
    {
        //mark the queue as empty now
        front = -1;
        rear = -1;
    }

    //normal
    else
        front = (front+1) % MAXPROCS;
}


