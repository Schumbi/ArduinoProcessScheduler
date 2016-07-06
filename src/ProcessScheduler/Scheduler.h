#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Includes.h"

#ifndef SCHEDULER_JOB_QUEUE_SIZE
    #define SCHEDULER_JOB_QUEUE_SIZE 20
#endif

#define ALL_PRIORITY_LEVELS -1

typedef struct RingBuf RingBuf;

class Process;



class Scheduler
{

public:
    Scheduler();
    ~Scheduler();
    bool add(Process &process, bool enableIfNot = false);
    bool disable(Process &process);
    bool enable(Process &process);
    bool destroy(Process &process);
    bool halt();

    uint8_t getID(Process &process);
    bool isRunningProcess(Process &process);
    bool isNotDestroyed(Process &process);
    bool isEnabled(Process &process);

    static Process *getActive();
    Process *findProcById(uint8_t id);
    uint8_t countProcesses(int priority = ALL_PRIORITY_LEVELS, bool enabledOnly = true);
    static uint32_t getCurrTS();

    int run();
protected:

    // Inner queue object class
    class QueableOperation
    {
    public:
        enum OperationType
        {
            NONE = 0,
            ADD_SERVICE,
            DESTROY_SERVICE,
            DISABLE_SERVICE,
            ENABLE_SERVICE,
            HALT,
#ifdef _PROCESS_STATISTICS
            UPDATE_STATS,
#endif
        };

        QueableOperation();
        QueableOperation(OperationType op);
        QueableOperation(Process *serv, OperationType op);

        Process *getProcess();
        OperationType getOperation();
        bool queue(RingBuf *queue);

    private:
        Process *_process;
        const uint8_t _operation;
    };

    // Methods that process queued operations
    void procDisable(Process &process);
    void procEnable(Process &process);
    void procDestroy(Process &process);
    void procAdd(Process &process);
    void procHalt();

    void processQueue();
    // Linked list methods
    bool appendNode(Process &node); // true on success
    bool removeNode(Process &node); // true on success
    bool findNode(Process &node); // True if node exists in list
    Process *findPrevNode(Process &node);


    static Process *_active;
    uint8_t _lastID;
    RingBuf *_queue;

    struct SchedulerPriorityLevel
    {
        Process *head;
        Process *next;
    };
    struct SchedulerPriorityLevel _pLevels[NUM_PRIORITY_LEVELS];


/* CUSTOM COMPILE OPTIONS*/
/*
    bool swapNode(Process &n1, Process &n2);
    void reOrderProcs(ProcPriority level);
*/

#ifdef _PROCESS_STATISTICS
public:
    bool updateStats();
private:
    void procUpdateStats();
    void handleHistOverFlow(uint8_t div);

#endif


#ifdef _PROCESS_EXCEPTION_HANDLING
public:
    void raiseException(int e);
    virtual void handleException(Process &process, int e) { };
    static jmp_buf _env; // have to do this to access it from ISR
protected:
    bool jmpHandler(int e);
#endif

};

#endif
