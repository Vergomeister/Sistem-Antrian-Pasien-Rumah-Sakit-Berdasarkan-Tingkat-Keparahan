# Sistem Antrian Pasien Rumah Sakit (C Language - Priority Queue)

> Sistem antrian pasien rumah sakit berbasis C menggunakan priority queue (min-heap). Memproses pasien berdasarkan tingkat keparahan, bukan urutan datang. Fitur: tambah, panggil, cari, hapus, update prioritas, statistik, dan riwayat pasien. Antarmuka berbasis teks.

Program ini merupakan simulasi sistem antrian pasien rumah sakit yang dibangun menggunakan bahasa pemrograman **C**. Sistem ini dirancang untuk menangani pasien berdasarkan **tingkat keparahan kondisi** dengan menerapkan **struktur data Priority Queue** berbasis **Min-Heap**.

## 🏥 Fitur Utama

- 📌 **Pendaftaran Pasien Baru**
- 🔔 **Pemanggilan Pasien** berdasarkan prioritas tertinggi
- 🔍 **Pencarian Pasien**
- 📋 **Tampilan Antrian**
- 🔄 **Update Prioritas**
- ❌ **Pembatalan Antrian**
- 📈 **Statistik Harian & Riwayat**

## ⚙️ Prioritas Medis

1. Gawat Darurat
2. Mendesak
3. Kurang Mendesak
4. Tidak Mendesak

## 🛠️ Teknologi

- Bahasa: C
- Struktur Data: Min-Heap
- Platform: Console

## 📦 Menjalankan Program

```bash
gcc -o antrian_pasien code.c
./antrian_pasien
```
