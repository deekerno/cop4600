#include <stdio.h>
#include <string.h>

#define MAXPROCS 20

typedef struct
{
    char name[20];
    int arrival;
    int burst;
} process;

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
        fcfs(procCount, runFor);
    }
    else if(strcmp(use, "sjf") == 0)
    {
        sjf();
    }
}

void roundRobin(int procCount, int runFor, int quantum)
{
    int i;
    int curTime = 0;
    int max;

    //sort processes by arrival time
    max = proc[0].arrival;
    process temp;
    for(i=1; i<procCount; i++)
    {
        //swap when necessary
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

    //printf("front: %d\n", front);
    //printf("rear: %d\n", rear);

    /* //check queue
    printf("the queue: \n");
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", queue[i].name, queue[i].arrival, queue[i].burst);
    printf("\n"); */

    /* //pop check
    pop();
    printf("front: %d\n", front);
    printf("rear: %d\n", rear);
    for(i=0; i<procCount; i++)
        printf("%s %d %d \n", queue[i].name, queue[i].arrival, queue[i].burst); */


    //Note: changed all index to front
    int count = 0;
    int index = 0;
    process temp2;
    while(curTime != runFor)
    {
        if(curTime == queue[front].arrival)
        {
            printf("Time %d: %s arrived \n", curTime, queue[front].name);

            count++;
            if(count == 2)
                break;
        }

        //when next process hasnt arrived yet
        while(curTime < queue[front+1].arrival)
        {
            printf("Time %d: %s selected (burst %d)\n", curTime, queue[front].name, queue[front].burst);
            curTime += quantum;
            queue[front].burst -= quantum;

            if(curTime > queue[front+1].arrival)
            {
                curTime = queue[front+1].arrival;
                //printf("curTime: %d \n", curTime);

                temp2 = queue[front];
                pop();
                push(temp2);
            }
        }

        //both are here now


    }
}

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


