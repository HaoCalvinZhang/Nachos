// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
int count = 0;

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;
	
	
	case SC_SysFork:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

 {
        //SpaceId id= SysFork();
        
        std::cout<<"Fork ..... Started. in exception.cc!\n";
 	Thread *th = new Thread ("ForkSysCall");
    	th->space = new AddrSpace(th->space);
    	//th->space = new AddrSpace(th->space);
    	//AddrSpace *space = new AddrSpace(th->space);
    	//th->space->AddrSpace(th->space->getPageTable());
    	std::cout<<"Space. in exception.cc: "<<th->space<<"\n";
    	std::cout<<"current space. in exception.cc: "<<th->getName()<<"\n";
    	std::cout<<"Get num page. in exception.cc: "<<th->space->getNumPage()<<"\n";
    	std::cout<<"Get num page. in exception.cc: "<<th->space->getPageTable()<<"\n";
    	SpaceId id = SpaceId(th);
        kernel->machine->WriteRegister(2, int(id));
        th->Fork((VoidFunctionPtr) threadFunction, (void *) 1);
        int newPID = kernel->ThreadId;
    	std::cout<<"Current PID is: "<<newPID<<"\n";
        std::cout<<"Fork .....Finished. in exception.cc !\n";
                /* Modify return point */
        {
            /* set previous programm counter (debugging only)*/
            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

            /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

            /* set next programm counter for brach execution */
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
        }
        return ;
        ASSERTNOTREACHED();
    }
	break;
	
	
	case SC_Exit:
		{   
            std::cout<<"Exit. in exception.cc\n";
            std::cout<<"register 4: "<<(int)kernel->machine->ReadRegister(4)<<"\n";
	    kernel->currentThread->Finish();
	   {
            /* set previous programm counter (debugging only)*/
            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

            /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

            /* set next programm counter for brach execution */
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
        }
    
        return;
        ASSERTNOTREACHED();
		}
		break;
	
	
	

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}


void threadFunction(Thread* currentThread)
{
        std::cout<<"ThreadFunction. in exception.cc\n";
        DEBUG(dbgSys, "Thread forked ");
        currentThread->RestoreUserState();
        currentThread->SaveUserState();
        kernel->machine->Run();
        
}

