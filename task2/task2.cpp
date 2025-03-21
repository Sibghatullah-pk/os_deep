/*Shared memory ek aisa memory area hai jo multiple processes ke beech share
 hota hai. Matlab, ek process ismein data likh sakti hai, aur dusri process 
usay padh sakti hai.
shmget() ek function hai jo shared memory segment banata hai ya existing
 shared memory ko access karta hai.
Return Value:
Success pe ek shared memory ID (shmid) return karta hai.
Failure pe -1 return karta hai.

*/
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
using namespace std;


struct SharedMemory {
    char message[256];
};

int main() {
 
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (shmid == -1) {
        cerr << "Shared memory creation failed!\n";
        return 1;
    }

    SharedMemory* shared_mem = (SharedMemory*) shmat(shmid, nullptr, 0);
    if (shared_mem == (SharedMemory*) -1) {
        cerr << "Shared memory attachment failed!\n";
        return 1;
    }

    strcpy(shared_mem->message, "");

    pid_t pid = fork();
    if (pid == -1) {
        cerr << "Fork failed!\n";
        return 1;
    }

    if (pid == 0) {
      
        cout << "Client 2: Waiting for message from Client 1...\n";
        while (strlen(shared_mem->message) == 0) {
         
        }
        cout << "Client 2 received: " << shared_mem->message << endl;

        strcpy(shared_mem->message, "Hello from Client 2");
        cout << "Client 2 sent: " << shared_mem->message << endl;

        shmdt(shared_mem);
        exit(0);
    } else {
       
        strcpy(shared_mem->message, "Hello from Client 1");
        cout << "Client 1 sent: " << shared_mem->message << endl;


        while (strcmp(shared_mem->message, "Hello from Client 1") == 0) {
        }
        cout << "Client 1 received: " << shared_mem->message << endl;


        wait(nullptr);

        shmdt(shared_mem);
        shmctl(shmid, IPC_RMID, nullptr);
    }

    cout << "Chat ended.\n";
    return 0;

/*
Synchronization Issues with Shared Memory:

1. **Race Condition**:
   - Problem: Parent aur child dono shared memory ko access kar rahe hain. Agar parent message likh raha hai aur child usay padh raha hai, to child ko partial message mil sakta hai (e.g., "Hel" instead of "Hello").
   - Impact: Data galat ho sakta hai.
   - Solution: Semaphores use karo—parent jab likh raha ho, child wait kare.

2. **Busy Waiting**:
   - Problem: Is program mein parent aur child loop mein wait kar rahe hain (`while` loops). Yeh CPU waste karta hai.
   - Impact: Program inefficient hai.
   - Solution: Semaphores ya flags use karo taake busy waiting na karna pade.

How to Resolve:
- **Semaphores**: `semget()` se semaphore banayein aur `semop()` se lock/unlock karein.
- **Flags**: Shared memory mein ek flag add karein jo bataye ke message ready hai ya nahi.
- Yeh program mein implement nahi kiya gaya, jaise requirement mein kaha gaya.
*/
}
