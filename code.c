#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PATIENTS 100
#define MAX_NAME_LENGTH 50
#define MAX_SYMPTOM_LENGTH 100

// Enum untuk tingkat prioritas
typedef enum {
    EMERGENCY = 1,      // Gawat Darurat - Prioritas tertinggi
    URGENT = 2,         // Mendesak 
    LESS_URGENT = 3,    // Kurang Mendesak
    NON_URGENT = 4      // Tidak Mendesak - Prioritas terendah
} Priority;

// Struktur data pasien
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    char symptoms[MAX_SYMPTOM_LENGTH];
    Priority priority;
    char arrivalTime[20];
    int waitingTime; // dalam menit
} Patient;

// Struktur Priority Queue menggunakan Min Heap
typedef struct {
    Patient patients[MAX_PATIENTS];
    int size;
} PriorityQueue;

// Global variables
PriorityQueue hospitalQueue;
int nextPatientId = 1;
Patient treatedPatients[MAX_PATIENTS];
int treatedCount = 0;

// Fungsi untuk mendapatkan waktu saat ini
void getCurrentTime(char *timeStr) {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    sprintf(timeStr, "%02d:%02d:%02d", 
            timeinfo->tm_hour, 
            timeinfo->tm_min, 
            timeinfo->tm_sec);
}

// Fungsi untuk mendapatkan string prioritas
char* getPriorityString(Priority p) {
    switch(p) {
        case EMERGENCY: return "GAWAT DARURAT";
        case URGENT: return "MENDESAK";
        case LESS_URGENT: return "KURANG MENDESAK";
        case NON_URGENT: return "TIDAK MENDESAK";
        default: return "UNKNOWN";
    }
}

// Inisialisasi priority queue
void initQueue(PriorityQueue *pq) {
    pq->size = 0;
}

// Fungsi untuk swap dua pasien
void swap(Patient *a, Patient *b) {
    Patient temp = *a;
    *a = *b;
    *b = temp;
}

// Fungsi untuk mendapatkan parent index
int parent(int i) {
    return (i - 1) / 2;
}

// Fungsi untuk mendapatkan left child index
int leftChild(int i) {
    return 2 * i + 1;
}

// Fungsi untuk mendapatkan right child index
int rightChild(int i) {
    return 2 * i + 2;
}

// Fungsi untuk heapify up (setelah insert)
void heapifyUp(PriorityQueue *pq, int index) {
    while (index > 0 && pq->patients[parent(index)].priority > pq->patients[index].priority) {
        swap(&pq->patients[parent(index)], &pq->patients[index]);
        index = parent(index);
    }
}

// Fungsi untuk heapify down (setelah extract)
void heapifyDown(PriorityQueue *pq, int index) {
    int minIndex = index;
    int left = leftChild(index);
    int right = rightChild(index);
    
    if (left < pq->size && pq->patients[left].priority < pq->patients[minIndex].priority) {
        minIndex = left;
    }
    
    if (right < pq->size && pq->patients[right].priority < pq->patients[minIndex].priority) {
        minIndex = right;
    }
    
    if (index != minIndex) {
        swap(&pq->patients[index], &pq->patients[minIndex]);
        heapifyDown(pq, minIndex);
    }
}

// 1. Menambah data (Menambah pasien ke antrian)
void addPatient() {
    if (hospitalQueue.size >= MAX_PATIENTS) {
        printf("Antrian penuh! Tidak dapat menambah pasien baru.\n");
        return;
    }
    
    Patient newPatient;
    int priorityChoice;
    
    newPatient.id = nextPatientId++;
    
    printf("\n=== PENDAFTARAN PASIEN BARU ===\n");
    printf("ID Pasien: %d\n", newPatient.id);
    
    printf("Nama Pasien: ");
    getchar(); // consume newline
    fgets(newPatient.name, sizeof(newPatient.name), stdin);
    newPatient.name[strcspn(newPatient.name, "\n")] = 0; // remove newline
    
    printf("Umur: ");
    scanf("%d", &newPatient.age);
    
    printf("Gejala/Keluhan: ");
    getchar(); // consume newline
    fgets(newPatient.symptoms, sizeof(newPatient.symptoms), stdin);
    newPatient.symptoms[strcspn(newPatient.symptoms, "\n")] = 0;
    
    printf("\nTingkat Prioritas:\n");
    printf("1. GAWAT DARURAT (Kondisi mengancam nyawa)\n");
    printf("2. MENDESAK (Perlu penanganan segera)\n");
    printf("3. KURANG MENDESAK (Dapat menunggu sebentar)\n");
    printf("4. TIDAK MENDESAK (Kondisi stabil)\n");
    printf("Pilih prioritas (1-4): ");
    scanf("%d", &priorityChoice);
    
    if (priorityChoice < 1 || priorityChoice > 4) {
        printf("Prioritas tidak valid! Menggunakan prioritas TIDAK MENDESAK.\n");
        newPatient.priority = NON_URGENT;
    } else {
        newPatient.priority = (Priority)priorityChoice;
    }
    
    getCurrentTime(newPatient.arrivalTime);
    newPatient.waitingTime = 0;
    
    // Insert ke priority queue
    hospitalQueue.patients[hospitalQueue.size] = newPatient;
    heapifyUp(&hospitalQueue, hospitalQueue.size);
    hospitalQueue.size++;
    
    printf("\nPasien berhasil didaftarkan!\n");
    printf("Nama: %s\n", newPatient.name);
    printf("Prioritas: %s\n", getPriorityString(newPatient.priority));
    printf("Waktu Kedatangan: %s\n", newPatient.arrivalTime);
}

// 2. Menghapus data (Memanggil pasien berikutnya)
void callNextPatient() {
    if (hospitalQueue.size == 0) {
        printf("Tidak ada pasien dalam antrian!\n");
        return;
    }
    
    // Extract pasien dengan prioritas tertinggi (nilai priority terkecil)
    Patient nextPatient = hospitalQueue.patients[0];
    
    // Pindahkan pasien terakhir ke root dan heapify down
    hospitalQueue.patients[0] = hospitalQueue.patients[hospitalQueue.size - 1];
    hospitalQueue.size--;
    
    if (hospitalQueue.size > 0) {
        heapifyDown(&hospitalQueue, 0);
    }
    
    // Simpan ke daftar pasien yang sudah ditangani
    treatedPatients[treatedCount] = nextPatient;
    treatedCount++;
    
    printf("\n=== PASIEN DIPANGGIL ===\n");
    printf("ID: %d\n", nextPatient.id);
    printf("Nama: %s\n", nextPatient.name);
    printf("Umur: %d tahun\n", nextPatient.age);
    printf("Gejala: %s\n", nextPatient.symptoms);
    printf("Prioritas: %s\n", getPriorityString(nextPatient.priority));
    printf("Waktu Kedatangan: %s\n", nextPatient.arrivalTime);
    printf("\nSilakan menuju ruang periksa!\n");
}

// 3. Mencari data (Mencari pasien dalam antrian)
void searchPatient() {
    if (hospitalQueue.size == 0) {
        printf("Tidak ada pasien dalam antrian!\n");
        return;
    }
    
    int searchChoice;
    printf("\n=== PENCARIAN PASIEN ===\n");
    printf("1. Cari berdasarkan ID\n");
    printf("2. Cari berdasarkan Nama\n");
    printf("3. Cari berdasarkan Prioritas\n");
    printf("Pilih metode pencarian: ");
    scanf("%d", &searchChoice);
    
    int found = 0;
    
    switch (searchChoice) {
        case 1: {
            int searchId;
            printf("Masukkan ID Pasien: ");
            scanf("%d", &searchId);
            
            printf("\nHasil Pencarian:\n");
            for (int i = 0; i < hospitalQueue.size; i++) {
                if (hospitalQueue.patients[i].id == searchId) {
                    printf("DITEMUKAN!\n");
                    printf("ID: %d\n", hospitalQueue.patients[i].id);
                    printf("Nama: %s\n", hospitalQueue.patients[i].name);
                    printf("Umur: %d tahun\n", hospitalQueue.patients[i].age);
                    printf("Gejala: %s\n", hospitalQueue.patients[i].symptoms);
                    printf("Prioritas: %s\n", getPriorityString(hospitalQueue.patients[i].priority));
                    printf("Waktu Kedatangan: %s\n", hospitalQueue.patients[i].arrivalTime);
                    found = 1;
                    break;
                }
            }
            break;
        }
        case 2: {
            char searchName[MAX_NAME_LENGTH];
            printf("Masukkan Nama Pasien: ");
            getchar();
            fgets(searchName, sizeof(searchName), stdin);
            searchName[strcspn(searchName, "\n")] = 0;
            
            printf("\nHasil Pencarian:\n");
            for (int i = 0; i < hospitalQueue.size; i++) {
                if (strstr(hospitalQueue.patients[i].name, searchName) != NULL) {
                    printf("DITEMUKAN!\n");
                    printf("ID: %d\n", hospitalQueue.patients[i].id);
                    printf("Nama: %s\n", hospitalQueue.patients[i].name);
                    printf("Umur: %d tahun\n", hospitalQueue.patients[i].age);
                    printf("Gejala: %s\n", hospitalQueue.patients[i].symptoms);
                    printf("Prioritas: %s\n", getPriorityString(hospitalQueue.patients[i].priority));
                    printf("Waktu Kedatangan: %s\n", hospitalQueue.patients[i].arrivalTime);
                    found = 1;
                }
            }
            break;
        }
        case 3: {
            int searchPriority;
            printf("Pilih Prioritas:\n");
            printf("1. GAWAT DARURAT\n");
            printf("2. MENDESAK\n");
            printf("3. KURANG MENDESAK\n");
            printf("4. TIDAK MENDESAK\n");
            printf("Masukkan prioritas (1-4): ");
            scanf("%d", &searchPriority);
            
            if (searchPriority < 1 || searchPriority > 4) {
                printf("Prioritas tidak valid!\n");
                return;
            }
            
            printf("\nPasien dengan prioritas %s:\n", getPriorityString((Priority)searchPriority));
            for (int i = 0; i < hospitalQueue.size; i++) {
                if (hospitalQueue.patients[i].priority == (Priority)searchPriority) {
                    printf("- ID: %d, Nama: %s, Umur: %d\n", 
                           hospitalQueue.patients[i].id,
                           hospitalQueue.patients[i].name,
                           hospitalQueue.patients[i].age);
                    found = 1;
                }
            }
            break;
        }
        default:
            printf("Pilihan tidak valid!\n");
            return;
    }
    
    if (!found) {
        printf("Pasien tidak ditemukan!\n");
    }
}

// 4. Menampilkan data secara terstruktur
void displayQueue() {
    printf("\n=== STATUS ANTRIAN RUMAH SAKIT ===\n");
    printf("Total pasien dalam antrian: %d\n", hospitalQueue.size);
    printf("Total pasien yang sudah ditangani: %d\n", treatedCount);
    
    if (hospitalQueue.size == 0) {
        printf("\nAntrian kosong.\n");
    } else {
        printf("\n=== DAFTAR ANTRIAN (Berdasarkan Prioritas) ===\n");
        printf("%-4s %-20s %-5s %-20s %-15s %-10s\n", 
               "ID", "Nama", "Umur", "Prioritas", "Waktu Datang", "Gejala");
        printf("================================================================================\n");
        
        // Tampilkan antrian sesuai urutan prioritas
        // Buat salinan temporary untuk sorting tampilan
        Patient tempQueue[MAX_PATIENTS];
        for (int i = 0; i < hospitalQueue.size; i++) {
            tempQueue[i] = hospitalQueue.patients[i];
        }
        
        // Simple selection sort berdasarkan prioritas untuk tampilan
        for (int i = 0; i < hospitalQueue.size - 1; i++) {
            for (int j = i + 1; j < hospitalQueue.size; j++) {
                if (tempQueue[i].priority > tempQueue[j].priority) {
                    Patient temp = tempQueue[i];
                    tempQueue[i] = tempQueue[j];
                    tempQueue[j] = temp;
                }
            }
        }
        
        for (int i = 0; i < hospitalQueue.size; i++) {
            printf("%-4d %-20s %-5d %-20s %-15s %-10.10s\n",
                   tempQueue[i].id,
                   tempQueue[i].name,
                   tempQueue[i].age,
                   getPriorityString(tempQueue[i].priority),
                   tempQueue[i].arrivalTime,
                   tempQueue[i].symptoms);
        }
    }
    
    // Statistik berdasarkan prioritas
    printf("\n=== STATISTIK ANTRIAN ===\n");
    int priorityCount[5] = {0}; // index 1-4 untuk priority 1-4
    
    for (int i = 0; i < hospitalQueue.size; i++) {
        priorityCount[hospitalQueue.patients[i].priority]++;
    }
    
    printf("Gawat Darurat: %d pasien\n", priorityCount[EMERGENCY]);
    printf("Mendesak: %d pasien\n", priorityCount[URGENT]);
    printf("Kurang Mendesak: %d pasien\n", priorityCount[LESS_URGENT]);
    printf("Tidak Mendesak: %d pasien\n", priorityCount[NON_URGENT]);
    
    // Tampilkan pasien berikutnya yang akan dipanggil
    if (hospitalQueue.size > 0) {
        printf("\n=== PASIEN BERIKUTNYA ===\n");
        printf("ID: %d\n", hospitalQueue.patients[0].id);
        printf("Nama: %s\n", hospitalQueue.patients[0].name);
        printf("Prioritas: %s\n", getPriorityString(hospitalQueue.patients[0].priority));
    }
}

// Fungsi untuk menampilkan riwayat pasien yang sudah ditangani
void displayTreatedPatients() {
    printf("\n=== RIWAYAT PASIEN YANG SUDAH DITANGANI ===\n");
    
    if (treatedCount == 0) {
        printf("Belum ada pasien yang ditangani.\n");
        return;
    }
    
    printf("Total: %d pasien\n\n", treatedCount);
    printf("%-4s %-20s %-5s %-20s %-15s\n", 
           "ID", "Nama", "Umur", "Prioritas", "Waktu Datang");
    printf("==================================================================\n");
    
    for (int i = 0; i < treatedCount; i++) {
        printf("%-4d %-20s %-5d %-20s %-15s\n",
               treatedPatients[i].id,
               treatedPatients[i].name,
               treatedPatients[i].age,
               getPriorityString(treatedPatients[i].priority),
               treatedPatients[i].arrivalTime);
    }
}

// Fungsi untuk menghapus pasien dari antrian (batalkan antrian)
void removePatient() {
    if (hospitalQueue.size == 0) {
        printf("Tidak ada pasien dalam antrian!\n");
        return;
    }
    
    int removeId;
    printf("Masukkan ID pasien yang akan dibatalkan: ");
    scanf("%d", &removeId);
    
    int found = -1;
    for (int i = 0; i < hospitalQueue.size; i++) {
        if (hospitalQueue.patients[i].id == removeId) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        printf("Pasien dengan ID %d tidak ditemukan!\n", removeId);
        return;
    }
    
    printf("Pasien yang akan dihapus:\n");
    printf("ID: %d\n", hospitalQueue.patients[found].id);
    printf("Nama: %s\n", hospitalQueue.patients[found].name);
    printf("Prioritas: %s\n", getPriorityString(hospitalQueue.patients[found].priority));
    
    // Pindahkan elemen terakhir ke posisi yang dihapus
    hospitalQueue.patients[found] = hospitalQueue.patients[hospitalQueue.size - 1];
    hospitalQueue.size--;
    
    // Heapify untuk menjaga property heap
    if (hospitalQueue.size > 0) {
        // Cek apakah perlu heapify up atau down
        if (found > 0 && hospitalQueue.patients[found].priority < hospitalQueue.patients[parent(found)].priority) {
            heapifyUp(&hospitalQueue, found);
        } else {
            heapifyDown(&hospitalQueue, found);
        }
    }
    
    printf("Pasien berhasil dihapus dari antrian!\n");
}

// Fungsi untuk mengupdate prioritas pasien
void updatePriority() {
    if (hospitalQueue.size == 0) {
        printf("Tidak ada pasien dalam antrian!\n");
        return;
    }
    
    int updateId, newPriority;
    printf("Masukkan ID pasien yang akan diupdate prioritasnya: ");
    scanf("%d", &updateId);
    
    int found = -1;
    for (int i = 0; i < hospitalQueue.size; i++) {
        if (hospitalQueue.patients[i].id == updateId) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        printf("Pasien dengan ID %d tidak ditemukan!\n", updateId);
        return;
    }
    
    printf("Pasien ditemukan:\n");
    printf("Nama: %s\n", hospitalQueue.patients[found].name);
    printf("Prioritas saat ini: %s\n", getPriorityString(hospitalQueue.patients[found].priority));
    
    printf("\nPrioritas baru:\n");
    printf("1. GAWAT DARURAT\n");
    printf("2. MENDESAK\n");
    printf("3. KURANG MENDESAK\n");
    printf("4. TIDAK MENDESAK\n");
    printf("Pilih prioritas baru (1-4): ");
    scanf("%d", &newPriority);
    
    if (newPriority < 1 || newPriority > 4) {
        printf("Prioritas tidak valid!\n");
        return;
    }
    
    Priority oldPriority = hospitalQueue.patients[found].priority;
    hospitalQueue.patients[found].priority = (Priority)newPriority;
    
    // Heapify untuk menjaga property heap
    if (newPriority < oldPriority) {
        // Prioritas meningkat, heapify up
        heapifyUp(&hospitalQueue, found);
    } else if (newPriority > oldPriority) {
        // Prioritas menurun, heapify down
        heapifyDown(&hospitalQueue, found);
    }
    
    printf("Prioritas berhasil diupdate!\n");
    printf("Prioritas baru: %s\n", getPriorityString((Priority)newPriority));
}

// Menu utama
void showMenu() {
    printf("\n=== SISTEM ANTRIAN PASIEN RUMAH SAKIT ===\n");
    printf("1. Daftar Pasien Baru (Menambah data)\n");
    printf("2. Panggil Pasien Berikutnya (Menghapus data)\n");
    printf("3. Cari Pasien (Mencari data)\n");
    printf("4. Tampilkan Antrian (Menampilkan data)\n");
    printf("5. Riwayat Pasien Ditangani\n");
    printf("6. Batalkan Antrian Pasien\n");
    printf("7. Update Prioritas Pasien\n");
    printf("8. Statistik Harian\n");
    printf("0. Keluar\n");
    printf("Pilih menu: ");
}

// Fungsi untuk menampilkan statistik harian
void displayStatistics() {
    printf("\n=== STATISTIK HARIAN ===\n");
    printf("Total pasien terdaftar hari ini: %d\n", nextPatientId - 1);
    printf("Pasien dalam antrian: %d\n", hospitalQueue.size);
    printf("Pasien sudah ditangani: %d\n", treatedCount);
    
    if (nextPatientId > 1) {
        float efficiency = ((float)treatedCount / (nextPatientId - 1)) * 100;
        printf("Efisiensi pelayanan: %.1f%%\n", efficiency);
    }
    
    // Statistik berdasarkan prioritas untuk semua pasien yang pernah terdaftar
    int totalPriorityCount[5] = {0};
    
    // Hitung dari antrian saat ini
    for (int i = 0; i < hospitalQueue.size; i++) {
        totalPriorityCount[hospitalQueue.patients[i].priority]++;
    }
    
    // Hitung dari pasien yang sudah ditangani
    for (int i = 0; i < treatedCount; i++) {
        totalPriorityCount[treatedPatients[i].priority]++;
    }
    
    printf("\nDistribusi Prioritas:\n");
    printf("- Gawat Darurat: %d pasien\n", totalPriorityCount[EMERGENCY]);
    printf("- Mendesak: %d pasien\n", totalPriorityCount[URGENT]);
    printf("- Kurang Mendesak: %d pasien\n", totalPriorityCount[LESS_URGENT]);
    printf("- Tidak Mendesak: %d pasien\n", totalPriorityCount[NON_URGENT]);
}

int main() {
    int choice;
    
    // Inisialisasi priority queue
    initQueue(&hospitalQueue);
    
    printf("=== SELAMAT DATANG DI SISTEM ANTRIAN RUMAH SAKIT ===\n");
    printf("Sistem ini menggunakan Priority Queue untuk mengelola antrian pasien\n");
    printf("berdasarkan tingkat keparahan kondisi medis.\n");
    
    do {
        showMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addPatient();
                break;
            case 2:
                callNextPatient();
                break;
            case 3:
                searchPatient();
                break;
            case 4:
                displayQueue();
                break;
            case 5:
                displayTreatedPatients();
                break;
            case 6:
                removePatient();
                break;
            case 7:
                updatePriority();
                break;
            case 8:
                displayStatistics();
                break;
            case 0:
                printf("Terima kasih telah menggunakan Sistem Antrian Rumah Sakit!\n");
                printf("Semoga pelayanan kesehatan berjalan dengan baik.\n");
                break;
            default:
                printf("Pilihan tidak valid!\n");
        }
    } while (choice != 0);
    
    return 0;
}
