#include <stdio.h>

#define MAXSIZE 10

//Prototypes
void Push(int x);
void Pop();
void Print();

//global variables
int A[MAXSIZE];
int front = -1;
int rear = -1;

int main()
{
    int selection;
    int num;

    printf("1 - Insert number in queue \n");
    printf("2 - Remove number in queue \n");
    printf("3 - Quit \n");
    printf("Enter selection: ");
    scanf("%d", &selection);

    while(selection != 3)
    {
        if(selection == 1)
        {
            printf("Enter number: ");
            scanf("%d", &num);
            Push(num);
            Print();
        }

        else if(selection == 2)
        {
            Pop();
            Print();
        }

        else
        {
            return;
        }

        printf("\n");
        printf("1 - Insert number in queue \n");
        printf("2 - Remove number in queue \n");
        printf("3 - Quit \n");
        printf("Enter selection: ");
        scanf("%d", &selection);
    }

    return 0;
}


void Push(int x)
{
    //check if queue is full
    if((rear+1) % MAXSIZE == front)
    {
        printf("Error: Queue is full \n");
        return;
    }

    //check if queue is empty
    if(front == -1 && rear == -1)
    {
        front = 0;
        rear = 0;
    }

    //normal case
    else
    {
        rear = (rear+1) % MAXSIZE;
    }

    A[rear] = x; //could place this in both the if/else
}

void Pop()
{
    //if queue is empty
    if(front == -1 && rear == -1)
    {
        printf("Error: Queue is empty. \n");
    }

    //when there is 1 element left in queue.
    else if(front == rear)
    {
        //mark the queue as empty now
        front = -1;
        rear = -1;
    }

    //normal
    else
    {
        front = (front+1) % MAXSIZE;
    }
}

void Print()
{
    int count;
    int index;

    /*
    //find number of elements in queue
    count = (rear+MAXSIZE-front) % MAXSIZE + 1;

    printf("Queue is: ");

    int i;
    for(i = 0; i < count; i++)
    {
        index = (front + i) % MAXSIZE;
        printf("%d ", A[index]);
    }
    printf("\n");
    */

    if(front == -1 && rear == -1)
    {
        count = (rear+MAXSIZE-front) % MAXSIZE;

        printf("Queue is: ");

        int i;
        for(i = 0; i < count; i++)
        {
            index = (front + i) % MAXSIZE;
            printf("%d ", A[index]);
        }
        printf("\n");
    }

    else
    {
        count = (rear+MAXSIZE-front) % MAXSIZE + 1;

        printf("Queue is: ");

        int i;
        for(i = 0; i < count; i++)
        {
            index = (front + i) % MAXSIZE;
            printf("%d ", A[index]);
        }
        printf("\n");
    }

    return;
}
