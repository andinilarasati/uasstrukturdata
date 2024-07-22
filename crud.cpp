#include <iostream>
#include <mysql/mysql.h>
#include <sstream>

using namespace std;

const char* hostname = "127.0.0.1";
const char* user = "root";
const char* pass = "123";
const char* dbname = "toko";
unsigned int port = 31235;
const char* unixsocket = NULL;
unsigned long clientflag = 0;

MYSQL* connect_db() {
    MYSQL* conn = mysql_init(0);
    if (conn) {
        conn = mysql_real_connect(conn, hostname, user, pass, dbname, port, unixsocket, clientflag);
        if (conn) {
            cout << "Berhasil terhubung ke database." << endl;
        } else {
            cerr << "Koneksi gagal: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "mysql_init gagal" << endl;
    }
    return conn;
}

void buat_tabel() {
    MYSQL* conn = connect_db();
    if (conn) {
        const char* adminTable = "CREATE TABLE IF NOT EXISTS Admins ("
                                 "ID INT AUTO_INCREMENT PRIMARY KEY, "
                                 "Username VARCHAR(255) NOT NULL UNIQUE, "
                                 "Password VARCHAR(255) NOT NULL);";
        const char* userTable = "CREATE TABLE IF NOT EXISTS Users ("
                                "ID INT AUTO_INCREMENT PRIMARY KEY, "
                                "Username VARCHAR(255) NOT NULL UNIQUE, "
                                "Password VARCHAR(255) NOT NULL);";
        const char* productTable = "CREATE TABLE IF NOT EXISTS Products ("
                                   "ID INT AUTO_INCREMENT PRIMARY KEY, "
                                   "Name VARCHAR(255) NOT NULL, "
                                   "Price DOUBLE NOT NULL, "
                                   "Stock INT NOT NULL);";
        if (mysql_query(conn, adminTable) || mysql_query(conn, userTable) || mysql_query(conn, productTable)) {
            cerr << "Pembuatan tabel gagal: " << mysql_error(conn) << endl;
        } else {
            cout << "Tabel berhasil dibuat." << endl;
        }
        mysql_close(conn);
    }
}

bool autentikasi(const string& table, const string& username, const string& password) {
    MYSQL* conn = connect_db();
    if (conn) {
        string query = "SELECT * FROM " + table + " WHERE Username = '" + username + "' AND Password = '" + password + "';";
        if (mysql_query(conn, query.c_str())) {
            cerr << "SELECT gagal: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return false;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result gagal: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return false;
        }

        bool authenticated = mysql_num_rows(res) > 0;
        mysql_free_result(res);
        mysql_close(conn);
        return authenticated;
    }
    return false;
}

void buat_produk() {
    string nama;
    double harga;
    int stok;

    cout << "Masukkan nama produk: ";
    cin.ignore();
    getline(cin, nama);
    cout << "Masukkan harga produk: ";
    cin >> harga;
    cout << "Masukkan stok produk: ";
    cin >> stok;

    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "INSERT INTO Products (Name, Price, Stock) VALUES ('" << nama << "', " << harga << ", " << stok << ")";
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "INSERT gagal: " << mysql_error(conn) << endl;
        } else {
            cout << "Produk berhasil ditambahkan." << endl;
        }
        mysql_close(conn);
    }
}

void tampilkan_produk() {
    MYSQL* conn = connect_db();
    if (conn) {
        if (mysql_query(conn, "SELECT * FROM Products")) {
            cerr << "SELECT gagal: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result gagal: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_ROW row;
        cout << "Daftar Produk:" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "ID: " << row[0] << ", Nama: " << row[1] << ", Harga: " << row[2] << ", Stok: " << row[3] << endl;
        }

        mysql_free_result(res);
        mysql_close(conn);
    }
}

void perbarui_produk() {
    int id;
    string nama;
    double harga;
    int stok;

    cout << "Masukkan ID produk yang akan diperbarui: ";
    cin >> id;
    cin.ignore();
    cout << "Masukkan nama produk baru: ";
    getline(cin, nama);
    cout << "Masukkan harga produk baru: ";
    cin >> harga;
    cout << "Masukkan stok produk baru: ";
    cin >> stok;

    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "UPDATE Products SET Name = '" << nama << "', Price = " << harga << ", Stock = " << stok << " WHERE ID = " << id;
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "UPDATE gagal: " << mysql_error(conn) << endl;
        } else {
            cout << "Produk berhasil diperbarui." << endl;
        }
        mysql_close(conn);
    }
}

void hapus_produk() {
    int id;

    cout << "Masukkan ID produk yang akan dihapus: ";
    cin >> id;

    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "DELETE FROM Products WHERE ID = " << id;
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "DELETE gagal: " << mysql_error(conn) << endl;
        } else {
            cout << "Produk berhasil dihapus." << endl;
        }
        mysql_close(conn);
    }
}

int main() {
    buat_tabel();

    string admin_username, admin_password;
    cout << "Login Admin\n";
    cout << "Username: ";
    cin >> admin_username;
    cout << "Password: ";
    cin >> admin_password;

    if (!autentikasi("Admins", admin_username, admin_password)) {
        cerr << "Autentikasi gagal!" << endl;
        return 1;
    }

    int pilihan;
    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Tambah Produk\n";
        cout << "2. Tampilkan Semua Produk\n";
        cout << "3. Perbarui Produk\n";
        cout << "4. Hapus Produk\n";
        cout << "5. Keluar\n";
        cout << "Masukkan pilihan: ";
        cin >> pilihan;

        if (pilihan == 1) {
            buat_produk();
        } else if (pilihan == 2) {
            tampilkan_produk();
        } else if (pilihan == 3) {
            perbarui_produk();
        } else if (pilihan == 4) {
            hapus_produk();
        } else if (pilihan == 5) {
            break;
        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
        }
    }
    return 0;
}
