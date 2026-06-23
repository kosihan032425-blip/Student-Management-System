#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
//  Data Model
// ─────────────────────────────────────────────
struct Student {
    int    id;
    string name;
    int    age;
    string course;
    float  gpa;
};

// ─────────────────────────────────────────────
//  Constants
// ─────────────────────────────────────────────
const string FILE_NAME = "students.dat";

// ─────────────────────────────────────────────
//  Terminal Helpers
// ─────────────────────────────────────────────
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printBorder(char c = '=', int width = 60) {
    cout << "  " << string(width, c) << "\n";
}

void printTitle(const string& title) {
    printBorder();
    int pad = (56 - (int)title.size()) / 2;
    cout << "  |" << string(pad, ' ') << title
         << string(56 - pad - (int)title.size(), ' ') << "|\n";
    printBorder();
}

void printRow(const Student& s) {
    cout << "  | "
         << left  << setw(6)  << s.id
         << setw(22) << s.name
         << setw(5)  << s.age
         << setw(14) << s.course
         << right << setw(5) << fixed << setprecision(2) << s.gpa
         << "  |\n";
}

void printTableHeader() {
    printBorder('-');
    cout << "  | "
         << left << setw(6)  << "ID"
         << setw(22) << "Name"
         << setw(5)  << "Age"
         << setw(14) << "Course"
         << right << setw(5) << "GPA"
         << "  |\n";
    printBorder('-');
}

// ─────────────────────────────────────────────
//  Input Helpers
// ─────────────────────────────────────────────
int getInt(const string& prompt) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v) { cin.ignore(); return v; }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Try again.\n";
    }
}

float getFloat(const string& prompt) {
    float v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= 0.0f && v <= 4.0f) { cin.ignore(); return v; }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] GPA must be between 0.0 and 4.0. Try again.\n";
    }
}

string getString(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// ─────────────────────────────────────────────
//  File I/O
// ─────────────────────────────────────────────
vector<Student> loadAll() {
    vector<Student> list;
    ifstream fin(FILE_NAME, ios::binary);
    if (!fin) return list;
    Student s;
    size_t len;

    while (fin.read(reinterpret_cast<char*>(&s.id),  sizeof(s.id)) &&
           fin.read(reinterpret_cast<char*>(&s.age), sizeof(s.age)) &&
           fin.read(reinterpret_cast<char*>(&s.gpa), sizeof(s.gpa))) {

        // name
        fin.read(reinterpret_cast<char*>(&len), sizeof(len));
        s.name.resize(len);
        fin.read(&s.name[0], len);

        // course
        fin.read(reinterpret_cast<char*>(&len), sizeof(len));
        s.course.resize(len);
        fin.read(&s.course[0], len);

        list.push_back(s);
    }
    return list;
}

void saveAll(const vector<Student>& list) {
    ofstream fout(FILE_NAME, ios::binary | ios::trunc);
    for (const auto& s : list) {
        fout.write(reinterpret_cast<const char*>(&s.id),  sizeof(s.id));
        fout.write(reinterpret_cast<const char*>(&s.age), sizeof(s.age));
        fout.write(reinterpret_cast<const char*>(&s.gpa), sizeof(s.gpa));

        size_t len = s.name.size();
        fout.write(reinterpret_cast<const char*>(&len), sizeof(len));
        fout.write(s.name.c_str(), len);

        len = s.course.size();
        fout.write(reinterpret_cast<const char*>(&len), sizeof(len));
        fout.write(s.course.c_str(), len);
    }
}

// Auto-generate next unique ID
int nextID(const vector<Student>& list) {
    int maxID = 0;
    for (const auto& s : list) maxID = max(maxID, s.id);
    return maxID + 1;
}

// ─────────────────────────────────────────────
//  Operations
// ─────────────────────────────────────────────
void addStudent() {
    clearScreen();
    printTitle("  ADD NEW STUDENT  ");
    cout << "\n";

    vector<Student> list = loadAll();
    Student s;
    s.id     = nextID(list);
    s.name   = getString("  Name   : ");
    s.age    = getInt   ("  Age    : ");
    s.course = getString("  Course : ");
    s.gpa    = getFloat ("  GPA    : ");

    list.push_back(s);
    saveAll(list);

    cout << "\n  [✓] Student added successfully! (ID: " << s.id << ")\n";
    pauseScreen();
}

void displayAll() {
    clearScreen();
    printTitle("  ALL STUDENT RECORDS  ");

    vector<Student> list = loadAll();
    if (list.empty()) {
        cout << "\n  No records found.\n";
    } else {
        printTableHeader();
        for (const auto& s : list) printRow(s);
        printBorder('-');
        cout << "  Total records: " << list.size() << "\n";
    }
    pauseScreen();
}

void searchStudent() {
    clearScreen();
    printTitle("  SEARCH STUDENT  ");
    cout << "\n";
    int id = getInt("  Enter Student ID to search: ");

    vector<Student> list = loadAll();
    auto it = find_if(list.begin(), list.end(),
                      [id](const Student& s){ return s.id == id; });

    if (it == list.end()) {
        cout << "\n  [!] Student with ID " << id << " not found.\n";
    } else {
        cout << "\n";
        printTableHeader();
        printRow(*it);
        printBorder('-');
    }
    pauseScreen();
}

void updateStudent() {
    clearScreen();
    printTitle("  UPDATE STUDENT  ");
    cout << "\n";
    int id = getInt("  Enter Student ID to update: ");

    vector<Student> list = loadAll();
    auto it = find_if(list.begin(), list.end(),
                      [id](const Student& s){ return s.id == id; });

    if (it == list.end()) {
        cout << "\n  [!] Student with ID " << id << " not found.\n";
    } else {
        cout << "\n  Current record:\n";
        printTableHeader();
        printRow(*it);
        printBorder('-');
        cout << "\n  Enter new details (leave blank to keep current):\n\n";

        // Name
        cout << "  Name   [" << it->name << "]: ";
        string tmp; getline(cin, tmp);
        if (!tmp.empty()) it->name = tmp;

        // Age
        cout << "  Age    [" << it->age << "]: ";
        getline(cin, tmp);
        if (!tmp.empty()) it->age = stoi(tmp);

        // Course
        cout << "  Course [" << it->course << "]: ";
        getline(cin, tmp);
        if (!tmp.empty()) it->course = tmp;

        // GPA
        cout << "  GPA    [" << fixed << setprecision(2) << it->gpa << "]: ";
        getline(cin, tmp);
        if (!tmp.empty()) {
            float g = stof(tmp);
            it->gpa = (g >= 0.0f && g <= 4.0f) ? g : it->gpa;
        }

        saveAll(list);
        cout << "\n  [✓] Record updated successfully!\n";
    }
    pauseScreen();
}

void deleteStudent() {
    clearScreen();
    printTitle("  DELETE STUDENT  ");
    cout << "\n";
    int id = getInt("  Enter Student ID to delete: ");

    vector<Student> list = loadAll();
    auto it = find_if(list.begin(), list.end(),
                      [id](const Student& s){ return s.id == id; });

    if (it == list.end()) {
        cout << "\n  [!] Student with ID " << id << " not found.\n";
    } else {
        cout << "\n  Record to delete:\n";
        printTableHeader();
        printRow(*it);
        printBorder('-');
        cout << "\n  Confirm delete? (y/n): ";
        char c; cin >> c; cin.ignore();
        if (c == 'y' || c == 'Y') {
            list.erase(it);
            saveAll(list);
            cout << "\n  [✓] Record deleted successfully!\n";
        } else {
            cout << "\n  [i] Deletion cancelled.\n";
        }
    }
    pauseScreen();
}

void displaySummary() {
    clearScreen();
    printTitle("  STATISTICS SUMMARY  ");

    vector<Student> list = loadAll();
    if (list.empty()) {
        cout << "\n  No records to summarise.\n";
        pauseScreen(); return;
    }

    float total = 0, highest = list[0].gpa, lowest = list[0].gpa;
    for (const auto& s : list) {
        total   += s.gpa;
        highest  = max(highest, s.gpa);
        lowest   = min(lowest,  s.gpa);
    }

    cout << "\n"
         << "  Total Students : " << list.size()  << "\n"
         << "  Average GPA    : " << fixed << setprecision(2) << total / list.size() << "\n"
         << "  Highest GPA    : " << highest << "\n"
         << "  Lowest GPA     : " << lowest  << "\n";
    pauseScreen();
}

// ─────────────────────────────────────────────
//  Main Menu
// ─────────────────────────────────────────────
void showMenu() {
    clearScreen();
    printTitle("  STUDENT MANAGEMENT SYSTEM  ");
    cout << "\n"
         << "  [1]  Add Student\n"
         << "  [2]  Display All Students\n"
         << "  [3]  Search Student\n"
         << "  [4]  Update Student\n"
         << "  [5]  Delete Student\n"
         << "  [6]  Statistics Summary\n"
         << "  [0]  Exit\n\n";
    printBorder('-');
}

int main() {
    int choice;
    do {
        showMenu();
        choice = getInt("  Enter choice: ");
        switch (choice) {
            case 1: addStudent();     break;
            case 2: displayAll();     break;
            case 3: searchStudent();  break;
            case 4: updateStudent();  break;
            case 5: deleteStudent();  break;
            case 6: displaySummary(); break;
            case 0:
                clearScreen();
                printTitle("  GOODBYE!  ");
                cout << "\n  Data saved to: " << FILE_NAME << "\n\n";
                break;
            default:
                cout << "\n  [!] Invalid option. Try again.\n";
                pauseScreen();
        }
    } while (choice != 0);

    return 0;
}