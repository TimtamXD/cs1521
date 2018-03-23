// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#include <stdlib.h>
#include <stdio.h>
#include "Memory.h"
#include "Stats.h"
#include "PageTable.h"

// Symbolic constants

#define NOT_USED 0
#define IN_MEMORY 1
#define ON_DISK 2

typedef struct PTE {
   char status;                         // NOT_USED, IN_MEMORY, ON_DISK
   char modified;                       // boolean: changed since loaded
   int  frame;                          // memory frame holding this page
   int  accessTime;                     // clock tick for last access
   int  loadTime;                       // clock tick for last time loaded
   int  nPeeks;                         // total number times this page read
   int  nPokes;                         // total number times this page modified
   // TODO:                             // add more fields here, if needed ...
   int  Page;                           // the page entry
   struct PTE *next;                    // pointer to next node
   struct PTE *prev;                    // pointer to previous node
} PTE;

static PTE *PageTable;                  // array of page table entries
static int  nPages;                     // # entries in page table
static int  replacePolicy;              // how to do page replacement
static int  fifoList;                   // index of first PTE in FIFO list
static int  fifoLast;                   // index of last PTE in FIFO list
static PTE *head;                       // head pointer
static PTE *tail;                       // tail pointer
static PTE *temp;                       // temporary pointer

// Forward refs for private functions

static int findVictim(int);

// initPageTable: create/initialise Page Table data structures

void initPageTable(int policy, int np)
{
   PageTable = malloc(np * sizeof(PTE));
   if (PageTable == NULL) {
      fprintf(stderr, "Can't initialise Memory\n");
      exit(EXIT_FAILURE);
   }
   replacePolicy = policy;
   nPages = np;
   fifoList = 0;
   fifoLast = nPages-1;
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      p->status = NOT_USED;
      p->modified = 0;
      p->frame = NONE;
      p->accessTime = NONE;
      p->loadTime = NONE;
      p->nPeeks = p->nPokes = 0;
   }
}

// requestPage: request access to page pno in mode
// returns memory frame holding this page
// page may have to be loaded
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

int requestPage(int pno, char mode, int time)
{
   if (pno < 0 || pno >= nPages) {
      fprintf(stderr,"Invalid page reference\n");
      exit(EXIT_FAILURE);
   }
   PTE *p = &PageTable[pno];            // collect page table entry
   int temporary;                       // temporary int
   int fno;                             // frame number - this is the return value for function
   switch (p->status) {                 // check if status is NOT_USED,IN_MEMORY or ON_DISK
   case NOT_USED:                       // nothing happens, we just use the free frame
   case ON_DISK:                        // if memory is full of pages, we need to kick one out
    switch (replacePolicy) {
      case REPL_LRU:                    // make list based on LRU order
        // I'VE GOT NO CLUE AS TO HOW TO ORDER IT WITHOUT USING O(n)
        break;
      case REPL_FIFO:                   // make list based on FIFO order
        if (head == NULL && tail == NULL) {
          head = p;                     // set head pointer to p
          tail = p;                     // set tail pointer to p
          p->next = NULL;               // next is NULL
          p->prev = NULL;               // prev is NULL
        } else if (nPages > 1) {        // if nPages > 1
          temp = tail;                  // set temp to be last known node
          tail->next = p;               // set link to next node
          tail = p;                     // move tail so it's the last node
          tail->prev = temp;            // point to last known node
          temp = temp->next;            // move temp so it's same as tail
          tail->next = NULL;            // end
        }
        break;
    }
      // TODO:                          // add stats collection
      countPageFault();                 // load into frame if on disk
      fno = findFreeFrame();
      if (fno == NONE) {
         int vno = findVictim(time);    // find Victim
#ifdef DBUG
         printf("Evict page %d\n",vno); // victim number
#endif
         // TODO:                       // reset victim page to indicate it's no longer in memory, not modified etc
         p->Page = pno;                 // page number loaded
         p->status = IN_MEMORY;         // page is now in memory
         p->modified = 1;               // page has been touched
         p->accessTime = time;          // load accessTime
         p->loadTime = time;            // load loadTime
         PTE *p = &PageTable[vno];      // pointer to victim
         if (p->modified != 0) {        // if victim page modified, save its frame
           saveFrame(fno);              // collect frame# (fno) for victim page
         }
         p->status = ON_DISK;           // now the pageframe is on disk - new status
         p->modified = 0;               // set modification to 0 - no longer modified
         temporary = p->frame;          // int temporary = memory frame
         p->frame = NONE;               // no frame
         p->loadTime = NONE;            // not loaded
         p->accessTime = NONE;          // not accessed
      }
      if (fno != NONE) {                // if frame number does not equal NONE
        p->frame = fno;                 // set it's memory frame to fno
      } else {
        p->frame = temporary;           // else set frame equal to temporary
        fno = temporary;                // also set fno to temporary
      }
      printf("Page %d given frame %d\n",pno,fno);
      // TODO:
      p->Page = pno;
      loadFrame(fno,pno,time);          // load page pno into frame fno, update PTE for page, associated with frame fno
      p->status = IN_MEMORY;            // new status
      p->modified = 0;                  // not yet modified
      p->accessTime = time;             // just loaded
      p->loadTime = time;
      break;
   case IN_MEMORY:
      // TODO:                          // add stats collection
      countPageHit();
      break;
   default:
      fprintf(stderr,"Invalid page status\n");
      exit(EXIT_FAILURE);
   }
   if (mode == 'r')
      p->nPeeks++;
   else if (mode == 'w') {
      p->nPokes++;
      p->modified = 1;
   }
   p->accessTime = time;
   return p->frame;
}

// findVictim: find a page to be replaced

static int findVictim(int time)
{
   int victim = NONE;
   int i = 0;
   int oldest = time;
   switch (replacePolicy) {
   case REPL_LRU:                       // finds least recently used
      // TODO:                          // implement LRU strategy
      // NOT DONE IN O(n)
      for (i = 0; i < nPages; i++) {
        PTE *P = &PageTable[i];
        if (P->accessTime < oldest && P->accessTime >= 0) {
          oldest = P->accessTime;
          victim = P->Page;
        }
      }
      break;
   case REPL_FIFO:                      // finds first frame
      // TODO:                          // implement FIFO strategy
      if (nPages == 1) {                // if only one node
        victim = head->Page;
      } else {
        victim = head->Page;            // take first in
        head = head->next;              // delete first in
        head->prev = NULL;
      }
      break;
   case REPL_CLOCK:
      return 0;
   }
   return victim;
}

// showPageTableStatus: dump page table
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

void showPageTableStatus(void)
{
   char *s;
   printf("%4s %6s %4s %6s %7s %7s %7s %7s\n",
          "Page","Status","Mod?","Frame","Acc(t)","Load(t)","#Peeks","#Pokes");
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      printf("[%02d]", i);
      switch (p->status) {
      case NOT_USED:  s = "-"; break;
      case IN_MEMORY: s = "mem"; break;
      case ON_DISK:   s = "disk"; break;
      }
      printf(" %6s", s);
      printf(" %4s", p->modified ? "yes" : "no");
      if (p->frame == NONE)
         printf(" %6s", "-");
      else
         printf(" %6d", p->frame);
      if (p->accessTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->accessTime);
      if (p->loadTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->loadTime);
      printf(" %7d", p->nPeeks);
      printf(" %7d", p->nPokes);
      printf("\n");
   }
}

