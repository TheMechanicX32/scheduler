/**********************************************************************/
/*                                                                    */
/* Program Name: scheduler - Simulates a scheduler algorithm          */
/* Author:       Ethan Nicholas Huckabee                              */
/* Installation: Pensacola Chrsitian College, Pensacola, Florida      */
/* Course:       CS326, Operating Systems                             */
/* Date Written: March 25, 2022                                       */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* I pledge this assignment is my own first time work.                */
/* I pledge I did not copy or try to copy work from the Internet.     */
/* I pledge I did not copy or try to copy work from any student.      */
/* I pledge I did not copy or try to copy work from any where else.   */
/* I pledge the only person I asked for help from was my teacher.     */
/* I pledge I did not attempt to help any student on this assignment. */
/* I understand if I violate this pledge I will receive a 0 grade.    */
/*                                                                    */
/*                                                                    */
/*                      Signed: _____________________________________ */
/*                                           (signature)              */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* This program simulates the scheduling of processes. A process is   */
/* executed on the CPU until either its max time is reached, or its   */
/* block time is reached. A process gets blocked when it reaches its  */
/* block time, and a new process is scheduled on the CPU. Processes   */
/* that are done executing are removed from the process table. New    */
/* processes are added to the process table, until the predefined     */
/* number of processes have been executed.                            */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>  /* malloc(), free(), exit(), rand()              */

/**********************************************************************/
/*                         Symbolic Constants                         */
/**********************************************************************/
#define MINIMUM_PID       1             /* Min possible process ID    */
#define MAXIMUM_PID       100           /* Max possible process ID    */
#define QUANTUM           6             /* Process quantum in ticks   */
#define TABLE_HEADER      MINIMUM_PID-1 /* Lowest PID minus one       */
#define TABLE_TRAILER     MAXIMUM_PID+1 /* Highest PID plus one       */
#define HEADER_ALLOC_ERR  1             /* Header  memory alloc error */
#define TRAILER_ALLOC_ERR 2             /* Trailer memory alloc error */
#define PROCESS_ALLOC_ERR 3             /* Process memory alloc error */
#define PROCESS_STATE_ERR 4             /* Impossible state error     */
#define TRUE              1             /* True constant value        */
#define FALSE             0             /* False constant value       */

/**********************************************************************/
/*                         Program Structure                          */
/**********************************************************************/
// A table of processes
struct process
{
   int process_id,  /* Process ID, assigned 1 to 99                  */
      cpu_used,     /* Clock ticks used by the process so far        */
      max_time,     /* Maximum needed clock ticks                    */
      priority,     /* Process priority                              */
      quantum_used, /* Quantum, in ticks, used by the process        */
      block_time,   /* Clock ticks left before the process blocks    */
      wait_ticks;   /* Amount of ticks waited in the ready state     */
   char state;      /* Process state (ruNning, Blocked, Ready)       */
   struct process *p_next_process;
                    /* Points to the next process                    */
};
typedef struct process PROCESS;

/**********************************************************************/
/*                        Function Prototypes                         */
/**********************************************************************/
PROCESS *create_empty_table();
   /* Creates an empty process table with a valid header and trailer  */
void sort_process(PROCESS *p_process_table, PROCESS *p_process);
   /* Sorts each process into its respective priority queue           */
PROCESS *create_process(PROCESS *p_process_table, int *process_id);
   /* Creates a new process                                           */
int is_executing(PROCESS *p_process_table);
   /* Checks to see if there is a process in execution on the CPU     */
int is_blocked(PROCESS *p_process_table);
   /* Checks to see if all processes are blocked                      */
void print_process_table(PROCESS *p_process_table, int process_count,
   int next_pid, char *before_or_after);
   /* Prints the processes and their data from the process table      */
int count_processes(PROCESS *p_process_table);
   /* Counts the number of processes in the process table             */
void schedule_new_process(PROCESS *p_process_table);
   /* Schedules a new process onto the CPU                            */
void terminate_process(PROCESS *p_process, int process_id);
   /* Removes a process from the process table and frees its memory   */
int calculate_priority(PROCESS *p_process);
   /* Calculates and rounds the priority of a process                 */

/**********************************************************************/
/*                          Main Function                             */
/**********************************************************************/
int main()
{
   int     next_pid = MINIMUM_PID; /* Tracks the next PID to be used  */
   PROCESS *p_process_table,       /* Points to the process table     */
           *p_process;             /* Points to the next process      */

   /* Create a new process table                                      */
   p_process_table = create_empty_table();

   /* Insert the five initial processes into the table                */
   sort_process(p_process_table,
                create_process(p_process_table, &next_pid));
   sort_process(p_process_table,
                create_process(p_process_table, &next_pid));
   sort_process(p_process_table,
                create_process(p_process_table, &next_pid));
   sort_process(p_process_table,
                create_process(p_process_table, &next_pid));
   sort_process(p_process_table,
                create_process(p_process_table, &next_pid));

   /* Loops scheduling processes until 100 processes have executed    */
   while (next_pid <= MAXIMUM_PID)
   {
      p_process = p_process_table;

      /* Loops processing each process in the process table           */
      while (p_process = p_process->p_next_process,
             p_process->process_id != TABLE_TRAILER)
      {
         /* Schedule a new process if there is none executing, and    */
         /* and there is at least one unblocked process               */
         if (is_executing(p_process_table) == FALSE &&
             is_blocked  (p_process_table) == FALSE)
         {
            print_process_table (p_process_table,
                count_processes (p_process_table), next_pid, "BEFORE");

            schedule_new_process(p_process_table);

            print_process_table (p_process_table,
                 count_processes(p_process_table), next_pid, "AFTER");
         }

         /* Appropriately update each process in the process table    */
         switch (p_process->state)
         {
            case 'N':
               p_process->cpu_used     += 1;
               p_process->quantum_used += 1;

               /* Terminate the process if it reaches its max time or */
               /* preempt the CPU if the process uses its quantum     */
               if (p_process->cpu_used == p_process->max_time)
               {
                  print_process_table (p_process_table,
                                       count_processes (p_process_table),
                                       next_pid, "BEFORE");
                  terminate_process   (p_process_table,
                                       p_process->process_id);
                  schedule_new_process(p_process_table);
                  print_process_table (p_process_table,
                                       count_processes (p_process_table),
                                       next_pid, "AFTER");
               }
               else if (p_process->quantum_used == p_process->block_time)
               {
                  print_process_table (p_process_table,
                                       count_processes (p_process_table),
                                       next_pid, "BEFORE");

                  p_process->priority     = calculate_priority(p_process);
                  p_process->quantum_used = 0;
                  if (p_process->block_time == QUANTUM)
                     p_process->state     = 'R';
                  else
                  {
                     p_process->state     = 'B';
                     p_process->priority  = -(p_process->priority);
                  }

                  sort_process        (p_process_table, p_process);
                  schedule_new_process(p_process_table);
                  print_process_table (p_process_table,
                                       count_processes (p_process_table),
                                       next_pid, "AFTER");
               }
               break;
            case 'R':
               /* Process is waiting on the CPU, increment wait ticks */
               p_process->wait_ticks   += 1;
               break;
            case 'B':
               /* 5% chance each blocked process becomes unblocked    */
               if (rand() % 20 == FALSE)
                  p_process->state = 'R';
               break;
           default:
              printf("\nError #%d occured in main().", PROCESS_STATE_ERR);
              printf("\nImpossible process state occured.");
              printf("\nThe program is aborting.");
              exit(PROCESS_STATE_ERR);
              break;
         }
      }

      /* 20% chance a new process is created if 10 or fewer processes */
      if (rand() % 5 == FALSE && count_processes(p_process_table) < 10)
         sort_process(p_process_table,
                      create_process(p_process_table, &next_pid));
   }
   return 0;
}

/**********************************************************************/
/*   Creates an empty process table with a valid header and trailer   */
/**********************************************************************/
PROCESS *create_empty_table()
{
   PROCESS *p_new_table; /* Points to the newly created process table */

   /* Get a new process and make it the table header                  */
   if ((p_new_table = (PROCESS *)malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError #%d in create_empty_table().", HEADER_ALLOC_ERR);
      printf("\nCannot allocate enough memory for the list header.");
      printf("\nThe program is aborting.");
      exit(HEADER_ALLOC_ERR);
   }
   p_new_table->process_id = TABLE_HEADER;

   /* Get a new process and attach to end of the table as the trailer */
   if ((p_new_table->p_next_process = (PROCESS *)malloc(sizeof(PROCESS)))
      == NULL)
   {
      printf("\nError #%d in create_empty_table().", TRAILER_ALLOC_ERR);
      printf("\nCannot allocate enough memory for the list trailer");
      printf("\nThe program is aborting.");
      exit(TRAILER_ALLOC_ERR);
   }
   p_new_table->p_next_process->process_id     = TABLE_TRAILER;
   p_new_table->p_next_process->p_next_process = NULL;

   /* Return the pointer to the newly created process table           */
   return p_new_table;
}


/**********************************************************************/
/*       Sorts each process into its respective priority queue        */
/**********************************************************************/
void sort_process(PROCESS *p_process_table, PROCESS *p_process)
{
   int     exists = FALSE; /* True if process already in the table    */
   PROCESS *p_previous,    /* Points to the previous process          */
           *p_current = p_process_table;
                           /* Points to the current process           */

   /* Traverse the table to determine if the process already exists   */
   while (p_current->process_id != p_process->process_id &&
          p_current->process_id != TABLE_TRAILER)
   {
      p_previous = p_current;
      p_current  = p_current->p_next_process;

      if (p_current->process_id == p_process->process_id)
         exists = TRUE;
   }

   /* If the process exists, unlink it from the process table         */
   if (exists == TRUE)
      p_previous->p_next_process = p_current->p_next_process;

   /* Find the priority queue insertion point for the process         */
   p_current = p_process_table;
   if (p_process->priority >= 0)
   {
      do
      {
         p_previous = p_current;
         p_current  = p_current->p_next_process;
      }
      while (p_current->priority   <= p_process->priority &&
             p_current->process_id != TABLE_TRAILER);
   }
   else if (p_process->priority < 0)
   {
      do
      {
         p_previous = p_current;
         p_current  = p_current->p_next_process;
      }
      while (p_current->priority   >= p_process->priority &&
             p_current->process_id != TABLE_TRAILER       &&
             p_current->priority   <  0);
   }

   /* Link the process into the end of its respective priority queue  */
   p_previous->p_next_process = p_process;
   p_process->p_next_process  = p_current;
   return;
}

/**********************************************************************/
/*                        Creates a new process                       */
/**********************************************************************/
PROCESS *create_process(PROCESS *p_process_table, int *process_id)
{
   PROCESS *p_new_process; /* Points to a new process                 */

   /* Create a new process                                            */
   if ((p_new_process = (PROCESS *)malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError #%d in create_process().", PROCESS_ALLOC_ERR);
      printf("\nCannot allocate enough memory for a process.");
      printf("\nThe program is aborting.");
      exit(PROCESS_ALLOC_ERR);
   }

   /* Populate the new process and increment the process ID by 1      */
   p_new_process->process_id    = *process_id;
   *process_id                 += 1;
   p_new_process->cpu_used      = 0;
   p_new_process->max_time      = (rand() % 18) + 1;
   p_new_process->state         = 'R';
   p_new_process->priority      = 0;
   p_new_process->quantum_used  = 0;
   if (rand() % 3)
      p_new_process->block_time = (rand() % 5) + 1;
   else
      p_new_process->block_time = 6;
   p_new_process->wait_ticks    = 0;

   return p_new_process;
}

/**********************************************************************/
/*    Checks to see if there is a process in execution on the CPU     */
/**********************************************************************/
int is_executing(PROCESS *p_process_table)
{
   int is_executing = FALSE; /* True for any running processes        */

   /* Traverse the process table, searching for any running processes */
   while (p_process_table = p_process_table->p_next_process,
          p_process_table->process_id != TABLE_TRAILER)
   {
      if (p_process_table->state == 'N')
      {
        is_executing = TRUE;
      }
   }

   return is_executing;
}

/**********************************************************************/
/*             Checks to see if all processes are blocked             */
/**********************************************************************/
int is_blocked(PROCESS *p_process_table)
{
   int is_blocked = TRUE; /* False for any unblocked processes        */

   /* Traverse the process table checking for any unblocked processes */
   while (p_process_table = p_process_table->p_next_process,
          p_process_table->process_id != TABLE_TRAILER)
   {
      if (p_process_table->state != 'B')
      {
        is_blocked = FALSE;
      }
   }
   return is_blocked;
}

/**********************************************************************/
/*     Prints the processes and their data from the process table     */
/**********************************************************************/
void print_process_table(PROCESS *p_process_table, int process_count,
                         int next_pid, char *before_or_after)
{
   printf("\n\n %s SCHEDULING CPU: ",      before_or_after);
   printf(    "Next PID  = %d,  ",         next_pid);
   printf(    "Number of processes  = %d", process_count);
   printf(  "\n  PID   CPU Used   MAX Time   STATE   PRI   QUANTUM USED");
   printf(    "  BLK TIME   WAIT TKS");
   while (p_process_table = p_process_table->p_next_process,
      p_process_table->process_id != TABLE_TRAILER)
   {
      printf("\n   %2d   ",   p_process_table->process_id);
      printf("     %2d   ",   p_process_table->cpu_used);
      printf("     %2d   ",   p_process_table->max_time);
      printf(  "    %c   ",   p_process_table->state);
      printf(  "   %2d   ",   p_process_table->priority);
      printf("     %2d     ", p_process_table->quantum_used);
      printf("     %2d     ", p_process_table->block_time);
      printf("     %3d",      p_process_table->wait_ticks);
   }
   return;
}

/**********************************************************************/
/*        Counts the number of processes in the process table         */
/**********************************************************************/
int count_processes(PROCESS *p_process_table)
{
   int total_processes = 0; /* Counts processes in the process table  */

   /* Traverse the process table counting every process               */
   while (p_process_table = p_process_table->p_next_process,
          p_process_table->process_id != TABLE_TRAILER)
      total_processes += 1;

   return total_processes;
}

/**********************************************************************/
/*                Schedules a new process onto the CPU                */
/**********************************************************************/
void schedule_new_process(PROCESS *p_process_table)
{
   int is_scheduled = FALSE; /* Determines whether process is running */

   /* Traverse the process table until first ready process is found   */
   while (p_process_table = p_process_table->p_next_process,
         p_process_table->process_id != TABLE_TRAILER && !is_scheduled)
   {
      if (p_process_table->state == 'R')
      {
         p_process_table->state = 'N';
         is_scheduled = TRUE;
      }
   }
   return;
}

/**********************************************************************/
/*   Removes a process from the process table and frees its memory    */
/**********************************************************************/
void terminate_process(PROCESS *p_process_table, int process_id)
{
   PROCESS *p_current  = p_process_table->p_next_process,
               /* Points to the current process                       */
           *p_previous = p_process_table;
               /* Points to the previous process                      */

   /* Traverse the table until the process to be terminated is found  */
   while (p_current->process_id != process_id &&
          p_current->process_id != TABLE_TRAILER)
   {
      p_previous = p_current;
      p_current  = p_current->p_next_process;
   }

   /* Unlink the process from the table and free its memory           */
   p_previous->p_next_process = p_current->p_next_process;
   free(p_current);
   return;
}

/**********************************************************************/
/*          Calculates and rounds the priority of a process           */
/**********************************************************************/
int calculate_priority(PROCESS *p_process)
{
   float priority = (abs(p_process->priority) +
                     p_process->quantum_used) * 0.5f;
                     /* Floating-point priority of a given process    */

   return priority - (int) priority >= 0.5f ?
                         (int) priority + 1 : (int) priority;
}
