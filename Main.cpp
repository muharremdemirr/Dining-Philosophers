/*
* 
* 200101010 - Muharrem DEM�R
* 210101157 - Berkay DURSUN
* 200101007 - Murat EKER
*  
* Legend k�sm�nda;
* T --> Think
* E --> Eat
* H --> Hungry
* D --> Dead
* olarak g�sterilmi�tir. "HUNGRY" ve "DEAD" durumlar� g�zlemi kolayla�t�rmak ad�na eklenmi�tir.
* 
* Proje grup �yelerinin ortak kat�l�m� ile yap�lm��t�r.
* 
*/


#include "icb_gui.h"
#include <windows.h>
#include <stdbool.h>

#define PHILOSOPHER_COUNT 5
#define MAX_RETRIES 5 // 5 tur yemek yiyemeyen �l� kabul edilir

HANDLE tableSemaphore;


enum PhilosopherState { THINKING, HUNGRY, EATING, DEAD };
PhilosopherState states[PHILOSOPHER_COUNT] = { THINKING, THINKING, THINKING, THINKING, THINKING };

HANDLE chopsticks[PHILOSOPHER_COUNT]; // �ubuklar
bool thread_continue = false;
int FRM1;
ICBYTES panel;

int positionsX[PHILOSOPHER_COUNT] = { 400, 650, 550, 250, 150 };
int positionsY[PHILOSOPHER_COUNT] = { 150, 300, 600, 600, 300 };

// Filozof renkleri
int philosopherColors[PHILOSOPHER_COUNT] = {
    0xFFA500, // P1 - Turuncu
    0x800080, // P2 - Mor
    0x808000, // P3 - Zeytin Ye�ili
    0x800000, // P4 - Bordo
    0xcd6090, // P5 - Pembe
};

// �ubuklar�n renklerini tutacak array
int chopstickColors[PHILOSOPHER_COUNT] = { 0x000000, 0x000000, 0x000000, 0x000000, 0x000000 }; // Ba�lang��ta siyah

// Filozofun yemek yiyememe sayac�
int philosopherRetries[PHILOSOPHER_COUNT];

// Durum renkleri
int getStateColor(PhilosopherState state) {
    switch (state) {
    case THINKING:
        return 0x008000; // Ye�il (D���nen)
    case HUNGRY:
        return 0xFF0000;  // K�rm�z� (A�l�k)
    case EATING:
        return 0x0000FF;  // Mavi (Yemek yiyor)
    case DEAD:
        return 0x000000; // Siyah (�l�)
    default:
        return 0xFF0000; // K�rm�z� (Varsay�lan)
    }
}

// Sa� �st k��ede durumlar� g�ster
void drawLegend() {
    // "T" harfini �iz
    FillRect(panel, 900, 50, 50, 10, 0x008000); // Ye�il (T - Thinking)
    FillRect(panel, 920, 60, 10, 40, 0x008000); // Ortadaki dikey �ubuk

    // "H" harfini �iz
    FillRect(panel, 900, 120, 10, 50, 0xFF0000); // K�rm�z� (H - Hungry)
    FillRect(panel, 940, 120, 10, 50, 0xFF0000); // Sa� dikey �ubuk
    FillRect(panel, 910, 140, 30, 10, 0xFF0000); // Ortadaki yatay �ubuk

    // "E" harfini �iz
    FillRect(panel, 900, 200, 10, 50, 0x0000FF); // Mavi (E - Eating)
    FillRect(panel, 910, 200, 30, 10, 0x0000FF); // �st yatay �ubuk
    FillRect(panel, 910, 225, 20, 10, 0x0000FF); // Orta yatay �ubuk
    FillRect(panel, 900, 250, 40, 10, 0x0000FF); // Alt yatay �ubuk

    // "D" harfini �iz
    FillRect(panel, 900, 300, 10, 50, 0x000000); // Siyah (D - Dead)
    FillRect(panel, 910, 300, 30, 10, 0x000000); // �st �ubuk
    FillRect(panel, 910, 340, 30, 10, 0x000000); // Alt �ubuk
    FillRect(panel, 940, 310, 10, 30, 0x000000); // Sa� Yay
}

void drawPhilosophers() {
    // Filozoflar� �iz
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        FillCircle(panel, positionsX[i], positionsY[i], 60, getStateColor(states[i])); // D���nen, a�l�k vs.
        FillCircle(panel, positionsX[i], positionsY[i], 20, philosopherColors[i]); // Filozofun rengi
    }

    // �ubuklar� �iz (Her bir filozof aras�nda bir �ubuk olacak)
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        int x1 = positionsX[i] + positionsX[(i + 1) % PHILOSOPHER_COUNT]; // Filozofun sa��ndaki �ubuk
        int y1 = positionsY[i] + positionsY[(i + 1) % PHILOSOPHER_COUNT];


        // �ubu�un rengini belirle
        int chopstickColor = chopstickColors[i]; // Varsay�lan �ubuk rengi (siyah)

        // E�er bir filozof yemek yiyorsa, o �ubu�un rengini de�i�tirece�iz
        if (states[i] == EATING) {
            chopstickColor = philosopherColors[i]; // �ubu�un rengi, yemek yiyen filozofun rengiyle ayn� olacak
        }

        // �ubu�u �iz
        FillRect(panel, x1 / 2, y1 / 2, 20, 100, chopstickColor); // �ubu�un rengi
    }

    FillCircle(panel, 400, 400, 120, 0x8B4513); // Kahverengi masa
    FillCircle(panel, 400, 400, 60, 0x070F0F);//Pilav
    FillCircle(panel, 400, 400, 50, 0xFFFFFF);//Pilav



    // Masan�n �zerine yemekler (K���k yuvarlaklar)
    FillCircle(panel, 340, 340, 15, 0xFF6347); // Yemek 1 (K�rm�z� - Domates)
    FillCircle(panel, 470, 340, 15, 0xFFFF00); // Yemek 2 (Sar� - Limon)
    FillCircle(panel, 340, 470, 15, 0x90EE90); // Yemek 3 (Ye�il - Marul)
    FillCircle(panel, 470, 470, 15, 0xFF4500); // Yemek 4 (Turuncu - Havu�)

    drawLegend(); // Sa� �st k��eye legend �iz
    DisplayImage(FRM1, panel);
}

void PhilosopherSimulation(void* param) {
    int id = (int)param;

    Sleep(id * 100); // Ba�lang�� s�ralamas�
    while (thread_continue) {
        states[id] = HUNGRY;
        drawPhilosophers();
        Sleep(1000);

        // Semaphore'dan izin al
        WaitForSingleObject(tableSemaphore, INFINITE);

        // �ubuklar� al
        WaitForSingleObject(chopsticks[id], INFINITE); // Sol �ubuk
        WaitForSingleObject(chopsticks[(id + 1) % PHILOSOPHER_COUNT], INFINITE); // Sa� �ubuk

        // Yemek yeme durumu
        states[id] = EATING;
        chopstickColors[id] = philosopherColors[id]; // Sol �ubu�un rengini filozofun rengiyle de�i�tir
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = philosopherColors[id]; // Sa� �ubu�un rengini filozofun rengiyle de�i�tir
        drawPhilosophers();
        Sleep(2000); // Yemek yeme s�resi

        // �ubuklar� serbest b�rak
        ReleaseMutex(chopsticks[id]);
        ReleaseMutex(chopsticks[(id + 1) % PHILOSOPHER_COUNT]);

        // Semaphore iznini b�rak
        ReleaseSemaphore(tableSemaphore, 1, NULL);

        // D���nme durumu
        states[id] = THINKING;
        chopstickColors[id] = 0x000000; // �ubu�u siyah yap
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = 0x000000; // �ubu�u siyah yap
        drawPhilosophers();
        Sleep(1000);
    }
}

void PhilosopherSimulationDeadlock(void* param) {
    int id = (int)param;

    Sleep(id * 100); // Ba�lang�� s�ralamas�
    while (thread_continue) {
        if (states[id] == DEAD) break;

        states[id] = HUNGRY;
        drawPhilosophers();
        Sleep(1000);

        // �ubuklar� al
        if (WaitForSingleObject(chopsticks[id], 1000) == WAIT_TIMEOUT) {
            philosopherRetries[id]++;
            if (philosopherRetries[id] >= MAX_RETRIES) {
                states[id] = DEAD;
                drawPhilosophers();
                Sleep(1000);
                break;
            }
            continue; // �ubu�u alamazsa d�ng�n�n ba��na d�n
        }

        if (WaitForSingleObject(chopsticks[(id + 1) % PHILOSOPHER_COUNT], 1000) == WAIT_TIMEOUT) {
            // Sa� �ubu�u alamazsa sol �ubu�u serbest b�rak ve tekrar dene
            ReleaseMutex(chopsticks[id]);
            philosopherRetries[id]++;
            if (philosopherRetries[id] >= MAX_RETRIES) {
                states[id] = DEAD;
                drawPhilosophers();
                Sleep(1000);
                break;
            }
            continue;
        }

        // Yemek yiyen filozofun rengini al
        states[id] = EATING;
        philosopherRetries[id] = 0; // Yemek yiyince a�l�k seviyesi s�f�rlan�r
        chopstickColors[id] = philosopherColors[id]; // Sol �ubu�un rengini yemek yiyen filozofun rengiyle de�i�tir
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = philosopherColors[id]; // Sa� �ubu�un rengini yemek yiyen filozofun rengiyle de�i�tir
        drawPhilosophers();
        Sleep(2000); // Yemek yeme s�resi

        // �ubuklar� serbest b�rak
        ReleaseMutex(chopsticks[id]);
        ReleaseMutex(chopsticks[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT]);

        states[id] = THINKING;
        chopstickColors[id] = 0x000000; // �ubu�u siyah yap
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = 0x000000; // �ubu�u siyah yap
        drawPhilosophers();
        Sleep(1000);
    }
}



void startSimulation() {
    thread_continue = true;
    static int philosopherIndices[PHILOSOPHER_COUNT] = { 0, 1, 2, 3, 4 };

    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PhilosopherSimulation, (void*)philosopherIndices[i], 0, NULL);
    }
}
//button deadlock
void philosopherDeadlock() {
    thread_continue = true;
    static int philosopherIndices[PHILOSOPHER_COUNT] = { 0, 1, 2, 3, 4 };

    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PhilosopherSimulationDeadlock, (void*)philosopherIndices[i], 0, NULL);
    }
}

void stopSimulation() {
    thread_continue = false;
}

void buttonSafe() {
    static bool isRunning = false;
    if (!isRunning) {
        for (int a = 0; a < PHILOSOPHER_COUNT; ++a) {
            philosopherRetries[a] = 0;
        }
        isRunning = true;
        startSimulation();
    }
    else {
        isRunning = false;
        stopSimulation();
    }
}

void buttonDeadlock() {
    static bool isRunning = false;
    
    
    if (!isRunning) {
        for (int a = 0; a < PHILOSOPHER_COUNT; ++a) {
            philosopherRetries[a] = 0;
        }
        isRunning = true;
        philosopherDeadlock();
    }
    else {
        isRunning = false;
        stopSimulation();
    }
}

void ICGUI_Create() {
    ICG_MWTitle("Dining Philosophers");
    ICG_MWSize(1200, 800);

    ICG_SetFont(30, 12, "");
}

void ICGUI_main() {
    tableSemaphore = CreateSemaphore(NULL, PHILOSOPHER_COUNT - 1, PHILOSOPHER_COUNT - 1, NULL);

    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        chopsticks[i] = CreateMutex(NULL, FALSE, NULL); // �ubuklar� payla�mak i�in mutexler
    }

    CreateImage(panel, 1200, 800, ICB_UINT);
    FRM1 = ICG_FrameMedium(0, 50, 1200, 800);
    FillRect(panel, 0, 0, 1200, 800, 0xD3D3D3); // A��k gri
    ICG_Button(50, 20, 200, 30, "Deadlock", buttonDeadlock);
    ICG_Button(270, 20, 200, 30, "Safe", buttonSafe);

    drawPhilosophers();
}