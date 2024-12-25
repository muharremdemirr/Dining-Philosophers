/*
* 
* 200101010 - Muharrem DEMÝR
* 210101157 - Berkay DURSUN
* 200101007 - Murat EKER
*  
* Legend kýsmýnda;
* T --> Think
* E --> Eat
* H --> Hungry
* D --> Dead
* olarak gösterilmiþtir. "HUNGRY" ve "DEAD" durumlarý gözlemi kolaylaþtýrmak adýna eklenmiþtir.
* 
* Proje grup üyelerinin ortak katýlýmý ile yapýlmýþtýr.
* 
*/


#include "icb_gui.h"
#include <windows.h>
#include <stdbool.h>

#define PHILOSOPHER_COUNT 5
#define MAX_RETRIES 5 // 5 tur yemek yiyemeyen ölü kabul edilir

HANDLE tableSemaphore;


enum PhilosopherState { THINKING, HUNGRY, EATING, DEAD };
PhilosopherState states[PHILOSOPHER_COUNT] = { THINKING, THINKING, THINKING, THINKING, THINKING };

HANDLE chopsticks[PHILOSOPHER_COUNT]; // Çubuklar
bool thread_continue = false;
int FRM1;
ICBYTES panel;

int positionsX[PHILOSOPHER_COUNT] = { 400, 650, 550, 250, 150 };
int positionsY[PHILOSOPHER_COUNT] = { 150, 300, 600, 600, 300 };

// Filozof renkleri
int philosopherColors[PHILOSOPHER_COUNT] = {
    0xFFA500, // P1 - Turuncu
    0x800080, // P2 - Mor
    0x808000, // P3 - Zeytin Yeþili
    0x800000, // P4 - Bordo
    0xcd6090, // P5 - Pembe
};

// Çubuklarýn renklerini tutacak array
int chopstickColors[PHILOSOPHER_COUNT] = { 0x000000, 0x000000, 0x000000, 0x000000, 0x000000 }; // Baþlangýçta siyah

// Filozofun yemek yiyememe sayacý
int philosopherRetries[PHILOSOPHER_COUNT];

// Durum renkleri
int getStateColor(PhilosopherState state) {
    switch (state) {
    case THINKING:
        return 0x008000; // Yeþil (Düþünen)
    case HUNGRY:
        return 0xFF0000;  // Kýrmýzý (Açlýk)
    case EATING:
        return 0x0000FF;  // Mavi (Yemek yiyor)
    case DEAD:
        return 0x000000; // Siyah (Ölü)
    default:
        return 0xFF0000; // Kýrmýzý (Varsayýlan)
    }
}

// Sað üst köþede durumlarý göster
void drawLegend() {
    // "T" harfini çiz
    FillRect(panel, 900, 50, 50, 10, 0x008000); // Yeþil (T - Thinking)
    FillRect(panel, 920, 60, 10, 40, 0x008000); // Ortadaki dikey çubuk

    // "H" harfini çiz
    FillRect(panel, 900, 120, 10, 50, 0xFF0000); // Kýrmýzý (H - Hungry)
    FillRect(panel, 940, 120, 10, 50, 0xFF0000); // Sað dikey çubuk
    FillRect(panel, 910, 140, 30, 10, 0xFF0000); // Ortadaki yatay çubuk

    // "E" harfini çiz
    FillRect(panel, 900, 200, 10, 50, 0x0000FF); // Mavi (E - Eating)
    FillRect(panel, 910, 200, 30, 10, 0x0000FF); // Üst yatay çubuk
    FillRect(panel, 910, 225, 20, 10, 0x0000FF); // Orta yatay çubuk
    FillRect(panel, 900, 250, 40, 10, 0x0000FF); // Alt yatay çubuk

    // "D" harfini çiz
    FillRect(panel, 900, 300, 10, 50, 0x000000); // Siyah (D - Dead)
    FillRect(panel, 910, 300, 30, 10, 0x000000); // Üst Çubuk
    FillRect(panel, 910, 340, 30, 10, 0x000000); // Alt Çubuk
    FillRect(panel, 940, 310, 10, 30, 0x000000); // Sað Yay
}

void drawPhilosophers() {
    // Filozoflarý çiz
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        FillCircle(panel, positionsX[i], positionsY[i], 60, getStateColor(states[i])); // Düþünen, açlýk vs.
        FillCircle(panel, positionsX[i], positionsY[i], 20, philosopherColors[i]); // Filozofun rengi
    }

    // Çubuklarý çiz (Her bir filozof arasýnda bir çubuk olacak)
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        int x1 = positionsX[i] + positionsX[(i + 1) % PHILOSOPHER_COUNT]; // Filozofun saðýndaki çubuk
        int y1 = positionsY[i] + positionsY[(i + 1) % PHILOSOPHER_COUNT];


        // Çubuðun rengini belirle
        int chopstickColor = chopstickColors[i]; // Varsayýlan çubuk rengi (siyah)

        // Eðer bir filozof yemek yiyorsa, o çubuðun rengini deðiþtireceðiz
        if (states[i] == EATING) {
            chopstickColor = philosopherColors[i]; // Çubuðun rengi, yemek yiyen filozofun rengiyle ayný olacak
        }

        // Çubuðu çiz
        FillRect(panel, x1 / 2, y1 / 2, 20, 100, chopstickColor); // Çubuðun rengi
    }

    FillCircle(panel, 400, 400, 120, 0x8B4513); // Kahverengi masa
    FillCircle(panel, 400, 400, 60, 0x070F0F);//Pilav
    FillCircle(panel, 400, 400, 50, 0xFFFFFF);//Pilav



    // Masanýn üzerine yemekler (Küçük yuvarlaklar)
    FillCircle(panel, 340, 340, 15, 0xFF6347); // Yemek 1 (Kýrmýzý - Domates)
    FillCircle(panel, 470, 340, 15, 0xFFFF00); // Yemek 2 (Sarý - Limon)
    FillCircle(panel, 340, 470, 15, 0x90EE90); // Yemek 3 (Yeþil - Marul)
    FillCircle(panel, 470, 470, 15, 0xFF4500); // Yemek 4 (Turuncu - Havuç)

    drawLegend(); // Sað üst köþeye legend çiz
    DisplayImage(FRM1, panel);
}

void PhilosopherSimulation(void* param) {
    int id = (int)param;

    Sleep(id * 100); // Baþlangýç sýralamasý
    while (thread_continue) {
        states[id] = HUNGRY;
        drawPhilosophers();
        Sleep(1000);

        // Semaphore'dan izin al
        WaitForSingleObject(tableSemaphore, INFINITE);

        // Çubuklarý al
        WaitForSingleObject(chopsticks[id], INFINITE); // Sol çubuk
        WaitForSingleObject(chopsticks[(id + 1) % PHILOSOPHER_COUNT], INFINITE); // Sað çubuk

        // Yemek yeme durumu
        states[id] = EATING;
        chopstickColors[id] = philosopherColors[id]; // Sol çubuðun rengini filozofun rengiyle deðiþtir
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = philosopherColors[id]; // Sað çubuðun rengini filozofun rengiyle deðiþtir
        drawPhilosophers();
        Sleep(2000); // Yemek yeme süresi

        // Çubuklarý serbest býrak
        ReleaseMutex(chopsticks[id]);
        ReleaseMutex(chopsticks[(id + 1) % PHILOSOPHER_COUNT]);

        // Semaphore iznini býrak
        ReleaseSemaphore(tableSemaphore, 1, NULL);

        // Düþünme durumu
        states[id] = THINKING;
        chopstickColors[id] = 0x000000; // Çubuðu siyah yap
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = 0x000000; // Çubuðu siyah yap
        drawPhilosophers();
        Sleep(1000);
    }
}

void PhilosopherSimulationDeadlock(void* param) {
    int id = (int)param;

    Sleep(id * 100); // Baþlangýç sýralamasý
    while (thread_continue) {
        if (states[id] == DEAD) break;

        states[id] = HUNGRY;
        drawPhilosophers();
        Sleep(1000);

        // Çubuklarý al
        if (WaitForSingleObject(chopsticks[id], 1000) == WAIT_TIMEOUT) {
            philosopherRetries[id]++;
            if (philosopherRetries[id] >= MAX_RETRIES) {
                states[id] = DEAD;
                drawPhilosophers();
                Sleep(1000);
                break;
            }
            continue; // Çubuðu alamazsa döngünün baþýna dön
        }

        if (WaitForSingleObject(chopsticks[(id + 1) % PHILOSOPHER_COUNT], 1000) == WAIT_TIMEOUT) {
            // Sað çubuðu alamazsa sol çubuðu serbest býrak ve tekrar dene
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
        philosopherRetries[id] = 0; // Yemek yiyince açlýk seviyesi sýfýrlanýr
        chopstickColors[id] = philosopherColors[id]; // Sol çubuðun rengini yemek yiyen filozofun rengiyle deðiþtir
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = philosopherColors[id]; // Sað çubuðun rengini yemek yiyen filozofun rengiyle deðiþtir
        drawPhilosophers();
        Sleep(2000); // Yemek yeme süresi

        // Çubuklarý serbest býrak
        ReleaseMutex(chopsticks[id]);
        ReleaseMutex(chopsticks[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT]);

        states[id] = THINKING;
        chopstickColors[id] = 0x000000; // Çubuðu siyah yap
        chopstickColors[(id + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT] = 0x000000; // Çubuðu siyah yap
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
        chopsticks[i] = CreateMutex(NULL, FALSE, NULL); // Çubuklarý paylaþmak için mutexler
    }

    CreateImage(panel, 1200, 800, ICB_UINT);
    FRM1 = ICG_FrameMedium(0, 50, 1200, 800);
    FillRect(panel, 0, 0, 1200, 800, 0xD3D3D3); // Açýk gri
    ICG_Button(50, 20, 200, 30, "Deadlock", buttonDeadlock);
    ICG_Button(270, 20, 200, 30, "Safe", buttonSafe);

    drawPhilosophers();
}