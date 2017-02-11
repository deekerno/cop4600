#include <stdio.h>
#include <string.h>

#define MAXPROCS 50

typedef struct
{
    char name[20];
    int num;
    int arrival;
    int burst;
    int finish;
    int remain;
} process;

process turnTime[MAXPROCS];
process proc[MAXPROCS];
process queue[MAXPROCS];
int front = -1;
int rear = -1;

void roundRobin(int procCount, int runFor, int quantum);
void fcfs(int procCount, int runFor);
void sjf(int procCount, int runFor);
void push(process x);
void pop();

FILE *ifp, *ofp;

int main()
{
    int procCount;
    int runFor = 0;
    char use[5];
    int quantum;
    char ignore[1000];

    //For the words "processcount", "runfor", "use", "quantum" in input file
    char str1[12], str2[6], str3[3], str4[7];

    ifp = fopen("processes.in", "r");
    ofp = fopen("processes.out", "w");

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
        fprintf(ofp, "%s %d %d \n", proc[i].name, proc[i].arrival, proc[i].burst);
    fprintf(ofp, "\n"); */

    //top of the output file
    fprintf(ofp, "%d processes\n", procCount);

    if(strcmp(use, "rr") == 0)
    {
        fprintf(ofp, "Using Round-Robin \n");
        fprintf(ofp, "Quantum %d\n", quantum);
    }
    else if(strcmp(use, "fcfs") == 0)
    {
        fprintf(ofp, "Using First-Come First Served \n");
    }
    else if(strcmp(use, "sjf") == 0)
    {
        fprintf(ofp, "Using Shortest Job First \n");
    }
    fprintf(ofp, "\n");


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
        fprintf(ofp, "%s %d %d \n", queue[i].name, queue[i].arrival, queue[i].burst);
    fprintf(ofp, "\n");

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
                fprintf(ofp, "Time %d: Idle \n", curTime);
                curTime++;
            }
        }

        //process has arrived and queue not empty
        if(curTime >=queue[front].arrival && front!=-1)
        {
            if(curTime == queue[front].arrival)
                fprintf(ofp, "Time %d: %s arrived \n", curTime, queue[front].name);

            //a full quantum can run
            if(queue[front].burst >= quantum)
            {
                fprintf(ofp, "Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

                for(i=0; i<quantum; i++)  //leave alone
                {
                    curTime++;
                    queue[front].burst -= 1;

                    if(curTime == queue[front+1].arrival)
                        fprintf(ofp, "Time %d: %s arrived \n", curTime, queue[front+1].name);
                }

                //time is up
                if(curTime > runFor)
                    break;

                if(queue[front].burst == 0)
                {
                    fprintf(ofp, "Time %d: %s finished \n", curTime, queue[front].name);
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
                    fprintf(ofp, "Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);

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
                    fprintf(ofp, "Time %d: %s finished \n", curTime, queue[front].name);
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
            fprintf(ofp, "Time %d: Idle \n", curTime);
            curTime++;
        }

        //queue is empty and all processes are done
        if((front==-1 && runFor-curTime==0))
        {
            fprintf(ofp, "Finished at time %d \n", curTime);
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
        fprintf(ofp, "\n");

        for(i=0; i<numElements; i++)
        {
            index = (front + i) % MAXPROCS;

            if(queue[index].burst >= 0)  //used to be !=
                fprintf(ofp, "Process %s did not finish in time. \n", queue[index].name);
        }
    }

    fprintf(ofp, "\n");
    for(i=0; i<procCount; i++)
    {
        if((turnTime[i].finish)-(turnTime[i].arrival) > 0)
            fprintf(ofp, "%s wait %d turnaround %d \n", turnTime[i].name,
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
    fprintf(ofp, "Time %d: %s arrived\n", time, queue[front].name, queue[front].burst);
    fprintf(ofp, "Time %d: %s selected (burst %d)\n", time, queue[front].name, queue[front].burst);
    pop(proc[index1]);
    index1 = index1 + 1;

    while (time < runFor)
    {
        // Do the following:
        while (proc[index1].arrival < burstCurProc && index1 < procCount)
        {
            push(proc[index1]);
            time = time + queue[index1 - 1].arrival;
            fprintf(ofp, "Time %d: %s arrived\n", time, queue[index1 - 1].name, queue[index1 - 1].burst);
            index1 = index1 + 1;
        }
        time = timePrevProcEnded + burstCurProc;
        timePrevProcEnded = time;
        fprintf(ofp, "Time %d: %s finished\n", time, nameCurProc);
        procToServe = procToServe - 1;
        if (procToServe == 0)
            break;
        burstCurProc = queue[front].burst;
        strcpy(nameCurProc, queue[front].name);
        fprintf(ofp, "Time %d: %s selected (burst %d)\n", time, queue[front].name, queue[front].burst);
        pop(proc[0]);
    }
    fprintf(ofp, "Finished at time %d\n", runFor);
}

void sjf(int procCount, int runFor)
{
    int wait, turnaround;
    int arrival[procCount+1];
    int arrived[procCount+1];
    int burst[procCount+1];
    int selected[procCount+1];
    
    int i = 0;
    int small2;
    int curTime = 0;
    int did_some = 0;
    int small = procCount;
    int finishedProcs = 0;

    burst[small] = 1000;

    for(i=0; i<procCount; i++)
        turnTime[i] = proc[i];

    for (int i = 0; i < procCount; i++)
    {
        arrival[i] = proc[i].arrival;
        burst[i] = proc[i].burst;
    }

    while (curTime < runFor)
    {
        if (did_some == 0)
            fprintf(ofp, "Time %d: IDLE\n", curTime+1);

        small = procCount;
        for (i = 0; i < procCount; i++)
        {
            //if (i == 0 && arrival[i] == curTime)
            //    fprintf(ofp, "Time %d: %s arrived\n", curTime, proc[i+1].name);

            if (arrival[i] == curTime)
            {
                fprintf(ofp, "Time %d: %s arrived\n", curTime, proc[i].name);
                did_some = 1;
            }


            if (arrival[i] <= curTime && burst[i] < burst[small] && burst[i] > 0)
            {
                small = i;
                did_some = 1;
                //fprintf(ofp, "Time %d: %s selected (burst %d)\n", curTime, proc[i].name, burst[i]);
            }
        }
        
        //if (small != small2)
        //{
        //    fprintf(ofp, "Time %d: %s selected (burst %d)\n", curTime, proc[small].name, burst[i]);
        //    small2 = small;
        //}

        burst[small]--;

        if (burst[small] == 0)
        {
            fprintf(ofp, "Time %d: %s finished\n", curTime + 1, proc[small].name);
            burst[small] = 1000;
            finishedProcs++;
            turnTime[small].finish = curTime;
        }


        if (finishedProcs == procCount && curTime+1 < runFor)
            fprintf(ofp, "Time %d: IDLE\n", curTime+1);


        curTime++;
    }

    fprintf(ofp, "Finished at time %d\n\n", runFor);

    for (i = 0; i < procCount; i++)
    {
        wait = turnTime[i].finish - turnTime[i].arrival - turnTime[i].burst + 1;
        turnaround = turnTime[i].finish - turnTime[i].arrival + 1;
        fprintf(ofp, "%s wait %d turnaround %d\n", proc[i].name, wait, turnaround);
    }

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



