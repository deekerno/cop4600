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

process turnTime[MAXPROCS];
process proc[MAXPROCS];
process queue[MAXPROCS];
int front = -1;
int rear = -1;

void roundRobin(int procCount, int runFor, int quantum);
void fcfs(int procCount, int runFor);
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
    ifp = fopen("processes3.in", "r");

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

    /*//check struct
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", proc[i].name, proc[i].arrival, proc[i].burst);
    printf("\n"); */

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
        fcfs(procCount, runFor);
    }
    else if(strcmp(use, "sjf") == 0)
    {
        sjf(procCount, runFor);
    }

    return 0;
}

void roundRobin(int procCount, int runFor, int quantum)
{
    int i,j,k;
    int curTime = 0;
    process _switch;
    process temp;

    //sort processes by arrival time via bubble sort
    for(i=0; i<procCount-1; i++)
    {
        for(j=0; j<procCount-i-1; j++)
        {
            if(proc[j].arrival > proc[j+1].arrival)
            {
                temp = proc[j];
                proc[j] = proc[j+1];
                proc[j+1] = temp;
            }
        }
    }

    //add processes to a queue
    for(i=0; i<procCount; i++)
        push(proc[i]);

    //check queue
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", queue[i].name, queue[i].arrival, queue[i].burst);
    printf("\n");

    //make a copy
    for(i=0; i<procCount; i++)
        turnTime[i] = proc[i];


    //primary loop
    while(curTime != runFor)
    {
        //waiting on process and queue not empty
        if(curTime <queue[front].arrival && front!=-1)
        {
            while(curTime < queue[front].arrival) {
                printf("Time %d: Idle \n", curTime);
                curTime++;
            }
        }

        //process has arrived and queue not empty
        if(curTime >=queue[front].arrival && front!=-1)
        {
            if(curTime == queue[front].arrival)
                printf("Time %d: %s arrived \n", curTime, queue[front].name);

            //a full quantum can run
            if(queue[front].burst >= quantum)
            {
                printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

                for(i=0; i<quantum; i++)  //leave alone
                {
                    curTime++;
                    queue[front].burst -= 1;

                    if(curTime == queue[front+1].arrival)
                        printf("Time %d: %s arrived \n", curTime, queue[front+1].name);
                }

                //time is up
                if(curTime > runFor)
                    break;

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

                //switch when...  //&& queue[front].burst <=queue[front+1].burst
                else if(curTime >=queue[front+1].arrival)
                {
                    _switch = queue[front];
                    pop();
                    push(_switch);
                }
            }

            //not enough for a full quantum. a process is guaranteed to finish if it enters this
            else
            {
                int num = queue[front].burst;

                if(queue[front].burst == 0)
                    ;
                else
                    printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

                for(j=0; j<num; j++)
                {
                    curTime++;
                    queue[front].burst -= 1;
                }

                //time is up
                if(curTime > runFor)
                    break;

                //if(flag == 0)
                //    ;
                //else
                    printf("Time %d: %s finished \n", curTime, queue[front].name);
                for(k=0; k<MAXPROCS; k++)
                {
                    if(strcmp(queue[front].name, turnTime[k].name) == 0)
                        turnTime[k].finish = curTime;
                }
                pop();
            }
        }

        //queue is empty and there's still time left to run
        if(front==-1 && runFor-curTime!=0)
        {
            printf("Time %d: Idle \n", curTime);
            curTime++;
        }

        //queue is empty and all processes are done
        if((front==-1 && runFor-curTime==0))
        {
            printf("Finished at time %d \n", curTime);
        }

        //when the processes don't finish
        if(curTime > runFor)
            break;
    }

    //print if any processes did not finish in time.
    //time is up and the queue still not empty
    if(front != -1)
    {
        int numElements;
        int index;

        numElements = (rear + MAXPROCS - front) % MAXPROCS + 1;
        printf("\n");

        for(i=0; i<numElements; i++)
        {
            index = (front + i) % MAXPROCS;

            if(queue[index].burst >= 0)  //used to be !=
                printf("Process %s did not finish in time. \n", queue[index].name);
        }
    }

    printf("\n");
    for(i=0; i<procCount; i++)
    {
        if((turnTime[i].finish)-(turnTime[i].arrival) > 0)
            printf("%s wait %d turnaround %d \n", turnTime[i].name,
                   (turnTime[i].finish)-(turnTime[i].arrival)-(turnTime[i].burst),
                   (turnTime[i].finish)-(turnTime[i].arrival));
    }
}

// Implements First Come First Serve Scheduling Algorithm.
void fcfs(int procCount, int runFor)
{
    int time = 0, timePrevProcEnded = 0, index1 = 0, index2 = 0;
    int burstCurProc;
    int procToServe = procCount;
    char nameCurProc[20];

    push(proc[index1]);
    burstCurProc = queue[front].burst;
    strcpy(nameCurProc, proc[index1].name);
    printf("Time %d: %s arrived\n", time, queue[front].name, queue[front].burst);
    printf("Time %d: %s selected (burst %d)\n", time, queue[front].name, queue[front].burst);
    pop(proc[index1]);
    index1 = index1 + 1;

    while (time < runFor)
    {
        // Do the following:
        while (proc[index1].arrival < burstCurProc && index1 < procCount)
        {
            push(proc[index1]);
            time = time + queue[index1 - 1].arrival;
            printf("Time %d: %s arrived\n", time, queue[index1 - 1].name, queue[index1 - 1].burst);
            index1 = index1 + 1;
        }
        time = timePrevProcEnded + burstCurProc;
        timePrevProcEnded = time;
        printf("Time %d: %s finished\n", time, nameCurProc);
        procToServe = procToServe - 1;
        if (procToServe == 0)
            break;
        burstCurProc = queue[front].burst;
        strcpy(nameCurProc, queue[front].name);
        printf("Time %d: %s selected (burst %d)\n", time, queue[front].name, queue[front].burst);
        pop(proc[0]);
    }
    printf("Finished at time %d\n", runFor);
}

void sjf(int procCount, int runFor)
{
    int index;
    int curTime = 0;    // time counter
    int swapped = 0;    // easy swap tell
    process temp;       // temp process for sorting
    process other;      // temp process for context swtiching

    // Bubble sort by arrival.
    for (int i = 0; i < procCount; i++)
    {
        swapped = 0;
        for (int j = 0; j < procCount; j++)
        {
            if (proc[j].arrival > proc[j+1].arrival)
            {
                temp = proc[j];
                proc[j] = proc[j+1];
                proc[j+1] = temp;
                swapped = 1;
            }
        }

        if (swapped == 0)
            break;
    }

    while (curTime < runFor)
    {

    }

    printf("Finished at time %d\n", runFor);
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



