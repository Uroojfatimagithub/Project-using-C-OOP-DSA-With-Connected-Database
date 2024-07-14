#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <sqlite3.h>

using namespace std;

class node {
public:
    int id, date;
    string name, roomtype;
    node* next;
    node() : next(NULL) {}
};

class Hotel {
private:
    sqlite3* db;
    char* zErrMsg;
    int rc;
    const char* sql;

public:
    node* head;
    Hotel() : db(NULL), zErrMsg(NULL), head(NULL) {
        rc = sqlite3_open("hotel.db", &db);
        if (rc) {
            cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
            exit(0);
        } else {
            cout << "Opened database successfully\n";
        }
        sql = "CREATE TABLE IF NOT EXISTS ROOMS("
              "ID INT PRIMARY KEY NOT NULL,"
              "NAME TEXT NOT NULL,"
              "DATE INT NOT NULL,"
              "ROOMTYPE TEXT NOT NULL);";
        rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        } else {
            cout << "Table created successfully\n";
        }

        // Initialize linked list from database
        populateLinkedListFromDB();
    }

    ~Hotel() {
        sqlite3_close(db);
    }

    void insert();
    void menu();
    void update();
    void search();
    void Delete();
    void show();
    void searchByName();
    void searchByDate();
    void measureTime(void (Hotel::*func)(), const string& operationName);
    void populateLinkedListFromDB();
};

void Hotel::measureTime(void (Hotel::*func)(), const string& operationName) {
    clock_t start, end;
    double elapsed;
    start = clock();
    (this->*func)();
    end = clock();
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout << "\n" << operationName << " took " << elapsed << " seconds\n";
}

void Hotel::populateLinkedListFromDB() {
    const char* select_query = "SELECT ID, NAME, DATE, ROOMTYPE FROM ROOMS;";
    sqlite3_stmt* stmt;
    
    rc = sqlite3_prepare_v2(db, select_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        int date = sqlite3_column_int(stmt, 2);
        const unsigned char* roomtype = sqlite3_column_text(stmt, 3);

        node* temp = new node;
        temp->id = id;
        temp->name = reinterpret_cast<const char*>(name);
        temp->date = date;
        temp->roomtype = reinterpret_cast<const char*>(roomtype);

        temp->next = head;
        head = temp;
    }

    sqlite3_finalize(stmt);
}

void Hotel::insert() {
    cout << "\n\t...............Hotel Management System................";
    node* temp = new node;
    cout << "\nEnter Room ID :" << endl;
    cin >> temp->id;
    cout << "Enter Customer name :" << endl;
    cin.ignore();
    getline(cin, temp->name);
    cout << "Enter Allocated Date :" << endl;
    cin >> temp->date;
    cout << "Enter Room Type(single/double/twin) :" << endl;
    cin.ignore();
    getline(cin, temp->roomtype);

    temp->next = NULL;

    if (head == NULL) {
        head = temp;
    } else {
        node* ptr = head;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = temp;
    }

    stringstream ss;
    ss << "INSERT INTO ROOMS (ID, NAME, DATE, ROOMTYPE) VALUES (" <<
          temp->id << ", '" << temp->name << "', " << temp->date << ", '" << temp->roomtype << "');";
    string insert_sql = ss.str();
    rc = sqlite3_exec(db, insert_sql.c_str(), 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "\n\n\t\tNew Room Inserted";
    }
}

void Hotel::search() {
    cout << "\n\t...............Hotel Management System................";
    int t_id;
    if (head == NULL) {
        cout << "\n\nLinked list is Empty";
    } else {
        cout << "\n\nRoom ID";
        cin >> t_id;
        node* ptr = head;
        while (ptr != NULL) {
            if (t_id == ptr->id) {
                cout << "\n\nRoom ID :" << ptr->id;
                cout << "\n\nCustomer Name :" << ptr->name;
                cout << "\n\nRoom Allocated Date :" << ptr->date;
                cout << "\n\nRoom Type :" << ptr->roomtype;
            }
            ptr = ptr->next;
        }
    }
}

void Hotel::searchByName() {
    cout << "\n\t...............Hotel Management System................";
    string t_name;
    if (head == NULL) {
        cout << "\n\nLinked list is Empty";
    } else {
        cout << "\n\nEnter Customer Name to Search: ";
        cin.ignore();
        getline(cin, t_name);
        node* ptr = head;
        bool found = false;
        while (ptr != NULL) {
            if (t_name == ptr->name) {
                cout << "\n\nRoom ID: " << ptr->id;
                cout << "\n\nCustomer Name: " << ptr->name;
                cout << "\n\nRoom Allocated Date: " << ptr->date;
                cout << "\n\nRoom Type: " << ptr->roomtype;
                found = true;
            }
            ptr = ptr->next;
        }
        if (!found) {
            cout << "\n\nRoom with Customer Name '" << t_name << "' not found.";
        }
    }
}

void Hotel::searchByDate() {
    cout << "\n\t...............Hotel Management System................";
    int t_date;
    if (head == NULL) {
        cout << "\n\nLinked list is Empty";
    } else {
        cout << "\n\nEnter Allocated Date to Search: ";
        cin >> t_date;
        node* ptr = head;
        bool found = false;
        while (ptr != NULL) {
            if (t_date == ptr->date) {
                cout << "\n\nRoom ID: " << ptr->id;
                cout << "\n\nCustomer Name: " << ptr->name;
                cout << "\n\nRoom Allocated Date: " << ptr->date;
                cout << "\n\nRoom Type: " << ptr->roomtype;
                found = true;
            }
            ptr = ptr->next;
        }
        if (!found) {
            cout << "\n\nRoom with Allocated Date '" << t_date << "' not found.";
        }
    }
}

void Hotel::update() {
    cout << "\n\t...............Hotel Management System................";
    int t_id;
    if (head == NULL) {
        cout << "\n\nLinked list is Empty";
    } else {
        cout << "\n\nRoom ID to Update";
        cin >> t_id;
        node* ptr = head;
        while (ptr != NULL) {
            if (t_id == ptr->id) {
                cout << "\n\nRoom ID :" << ptr->id;
                cout << "\n\nCustomer Name :" << ptr->name;
                cout << "\n\nRoom Allocated Date :" << ptr->date;
                cout << "\n\nRoom Type :" << ptr->roomtype;

                cout << "\n\nEnter New Room ID :";
                cin >> ptr->id;
                cout << "Enter New Customer name :" << endl;
                cin.ignore();
                getline(cin, ptr->name);
                cout << "Enter New Allocated Date :" << endl;
                cin >> ptr->date;
                cout << "Enter New Room Type(single/double/twin) :" << endl;
                cin.ignore();
                getline(cin, ptr->roomtype);

                stringstream ss;
                ss << "UPDATE ROOMS SET ID = " << ptr->id << ", NAME = '" << ptr->name <<
                      "', DATE = " << ptr->date << ", ROOMTYPE = '" << ptr->roomtype << "' WHERE ID = " << t_id << ";";
                string update_sql = ss.str();
                rc = sqlite3_exec(db, update_sql.c_str(), 0, 0, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL error: " << zErrMsg << endl;
                    sqlite3_free(zErrMsg);
                } else {
                    cout << "\n\n\t\tRoom Updated";
                }

                break;
            }
            ptr = ptr->next;
        }
    }
}

void Hotel::Delete() {
    cout << "\n\t...............Hotel Management System................";
    int t_id;
    if (head == NULL) {
        cout << "\n\nLinked list is Empty";
    } else {
        cout << "\n\nRoom ID to Delete";
        cin >> t_id;
        node* temp = head;
        if (temp->id == t_id) {
            head = temp->next;
            delete temp;
            cout << "\n\n\t\tRoom Deleted";
        } else {
            node* ptr = NULL;
            while (temp != NULL && temp->id != t_id) {
                ptr = temp;
                temp = temp->next;
            }
            if (temp == NULL) {
                cout << "\n\nRoom ID not found.";
            } else {
                ptr->next = temp->next;
                delete temp;
                cout << "\n\n\t\tRoom Deleted";
            }
        }

        stringstream ss;
        ss << "DELETE FROM ROOMS WHERE ID = " << t_id << ";";
        string delete_sql = ss.str();
        rc = sqlite3_exec(db, delete_sql.c_str(), 0, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
    }
}

void Hotel::show() {
    cout << "\n\t...............Hotel Management System................";
    node* ptr = head;
    if (ptr == NULL) {
        cout << "\n\n\t\tNo Records Found\n";
    } else {
        while (ptr != NULL) {
            cout << "\n\nRoom ID: " << ptr->id;
            cout << "\n\nCustomer Name: " << ptr->name;
            cout << "\n\nRoom Allocated Date: " << ptr->date;
            cout << "\n\nRoom Type: " << ptr->roomtype;
            cout << "\n";
            ptr = ptr->next;
        }
    }
}

void Hotel::menu() {
    int choice;
    do {
        cout << "\n\tWelcome To Hotel Management System Application\n" << endl;
        cout << "\n\t...............Hotel Management System................";
        cout << "\n\nS.No          Functions              Descriptions" << endl;
        cout << "\n1\tAllocate Room\t\t\tInsert New Room";
        cout << "\n2\tSearch Room\t\t\tSearch Room with Room ID";
        cout << "\n3\tUpdate Room\t\t\tUpdate Room Record";
        cout << "\n4\tDelete Room\t\t\tDelete Room with Room ID";
        cout << "\n5\tShow Room Records\t\tShow Room Records that(we added)";
        cout << "\n6\tSearch Room\t\t\tSearch Room with Customer Name";
        cout << "\n7\tSearch Room\t\t\tSearch Room with Date";
        cout << "\n8\tExit" << endl;

        cout << "Enter your choice" << endl;
        cin >> choice;

        switch (choice) {
            case 1:
                measureTime(&Hotel::insert, "Insert");
                break;
            case 2:
                measureTime(&Hotel::search, "Search");
                break;
            case 3:
                measureTime(&Hotel::update, "Update");
                break;
            case 4:
                measureTime(&Hotel::Delete, "Delete");
                break;
            case 5:
                measureTime(&Hotel::show, "Show");
                break;
            case 6:
                measureTime(&Hotel::searchByName, "Search by Name");
                break;
            case 7:
                measureTime(&Hotel::searchByDate, "Search by Date");
                break;
            case 8:
                exit(0);
            default:
                cout << "Invalid";
        }

    } while (choice != 8);
}

int main() {
    Hotel h;
    h.menu();
    system("Pause");
    return 0;
}