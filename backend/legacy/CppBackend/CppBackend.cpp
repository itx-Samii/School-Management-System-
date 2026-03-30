#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm> // For string comparison
#include <stdexcept> // For exception handling
#include <conio.h>   // For _getch() (Password masking) - Windows
#include <cstdlib>
#include <vector>

using namespace std;

// ==========================================
//             DATA STRUCTURES
// ==========================================

struct Student {
    int id;
    string name;
    string className;
    string password;
    Student* next;
};

struct Teacher {
    int id;
    string name;
    string password;
    string assignedClass; // Links teacher to a specific class
    Teacher* next;
};

struct Parent {
    int childID; // Acts as the username/login ID
    string password;
    Parent* next;
};

struct Attendance {
    int studentID;
    string date;
    string status;
    Attendance* next;
};

struct Fee {
    int studentID;
    string name;
    string className;
    float amount;
    string status;
    Fee* next;
};

struct Result {
    int studentID;
    string name;
    float obtained;
    float total;
    float percentage;
    char grade;
    Result* next;
};

struct Message {
    string sender;
    string content;
    string reply; // New field for Admin reply
    Message* next;
};

// ==========================================
//            HELPER FUNCTIONS
// ==========================================

string getHiddenPassword() {
    string password = "";
    char ch;
    while (true) {
        ch = _getch();
        if (ch == 13) { // Enter key
            cout << endl;
            break;
        } else if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password += ch;
            cout << "*";
        }
    }
    return password;
}

// Validate Date Format (DD-MM-YYYY)
bool isValidDate(string date) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(date[i])) return false;
    }
    // Basic logical checks
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    if (day < 1 || day > 31) return false;
    if (month < 1 || month > 12) return false;
    return true;
}

// Trim whitespace
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \n\r\t");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \n\r\t");
    return s.substr(start, end - start + 1);
}

// ==========================================
//             CORE MODULES
// ==========================================

// ---------------- STUDENT & ATTENDANCE RECORD ----------------

class StudentRecord {
private:
    Student* head;
    Attendance* top;

public:
    StudentRecord() {
        head = NULL;
        top = NULL;
        loadStudentsFromFile();
        loadAttendanceFromFile();
    }

    ~StudentRecord() {
        while (head != NULL) {
            Student* temp = head;
            head = head->next;
            delete temp;
        }
        while (top != NULL) {
            Attendance* temp = top;
            top = top->next;
            delete temp;
        }
    }

    // --- HELPER FUNCTIONS ---
    bool verifyStudentLogin(int id, string pass, string &outName) {
        Student* temp = head;
        while (temp != NULL) {
            if (temp->id == id && temp->password == pass) {
                outName = temp->name;
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    bool studentExists(int id, string &outName, string &outClass) {
        Student* temp = head;
        while (temp != NULL) {
            if (temp->id == id) {
                outName = temp->name;
                outClass = temp->className;
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    bool studentIDExists(int id) { // quick check
        Student* temp = head;
        while (temp != NULL) {
            if (temp->id == id) return true;
            temp = temp->next;
        }
        return false;
    }

    // --- ADMIN/TEACHER FUNCTIONS ---
    // Modified: Takes optional fixedClass. If set, forces that class (Teacher Mode).
    void addStudent(string fixedClass = "") {
        Student* newStudent = new Student;
        cout << endl << "Enter Student ID: ";
        while (!(cin >> newStudent->id)) {
            cout << "Invalid input! Enter a number: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        cin.ignore();

        string dummyName, dummyClass;
        if (studentExists(newStudent->id, dummyName, dummyClass)) {
            cout << endl << "Error: ID already exists!" << endl;
            delete newStudent;
            return;
        }

        cout << "Enter Student Name: "; getline(cin, newStudent->name);

        if (fixedClass.empty()) {
            cout << "Enter Class Name: "; getline(cin, newStudent->className);
        } else {
            newStudent->className = fixedClass;
            cout << "Assigning to Class: " << fixedClass << endl;
        }

        cout << "Enter Password (leave blank for auto-gen): ";
        string pass; getline(cin, pass);
        if (pass.empty()) pass = to_string(newStudent->id) + "123";
        newStudent->password = pass;

        newStudent->next = NULL;

        if (head == NULL) head = newStudent;
        else {
            Student* temp = head;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newStudent;
        }

        saveStudentsToFile();
        cout << endl << "Student added! Password: " << pass << endl;
    }

    // Modified: Takes optional filterClass. If set, only shows that class.
    void viewStudents(string filterClass = "") {
        if (head == NULL) {
            cout << endl << "No records." << endl;
            return;
        }
        Student* temp = head;

        cout << endl << "--- Student List";
        if (!filterClass.empty()) cout << " (Class: " << filterClass << ")";
        cout << " ---" << endl;

        cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(15) << "Class" << endl;
        cout << "--------------------------------------------------------" << endl;

        bool found = false;
        while (temp != NULL) {
            if (filterClass.empty() || temp->className == filterClass) {
                cout << left << setw(10) << temp->id << setw(25) << temp->name << setw(15) << temp->className << endl;
                found = true;
            }
            temp = temp->next;
        }
        if (!found) cout << "No students found for this class." << endl;
    }

    // Returns Deleted ID for cascade deletion
    int deleteStudent() {
        if (head == NULL) return -1;
        int id;
        cout << endl << "Enter ID to delete: ";
        cin >> id;

        Student* temp = head;
        Student* prev = NULL;
        while (temp != NULL && temp->id != id) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL) {
            cout << "Student not found." << endl;
            return -1;
        }
        if (prev == NULL) head = temp->next;
        else prev->next = temp->next;
        delete temp;

        // Cascade Delete Attendance for this ID
        Attendance* currAtt = top;
        Attendance* prevAtt = NULL;
        while(currAtt != NULL) {
            if(currAtt->studentID == id) {
                Attendance* toDelete = currAtt;
                if(prevAtt == NULL) {
                    top = currAtt->next;
                    currAtt = top;
                } else {
                    prevAtt->next = currAtt->next;
                    currAtt = prevAtt->next;
                }
                delete toDelete;
            } else {
                prevAtt = currAtt;
                currAtt = currAtt->next;
            }
        }

        saveStudentsToFile();
        saveAttendanceToFile();
        cout << "Student and Attendance records deleted." << endl;
        return id;
    }

    // markAttendance now optionally receives class filter -> teacher will pass their class
    void markAttendance(string filterClass = "") {
        // If filterClass provided, present a list of students in that class and prompt to pick ID
        vector<int> classStudentIDs;
        if (!filterClass.empty()) {
            Student* t = head;
            cout << endl << "--- Students in Class " << filterClass << " ---" << endl;
            cout << left << setw(10) << "ID" << setw(25) << "Name" << endl;
            cout << "-------------------------------------" << endl;
            while (t != NULL) {
                if (t->className == filterClass) {
                    cout << left << setw(10) << t->id << setw(25) << t->name << endl;
                    classStudentIDs.push_back(t->id);
                }
                t = t->next;
            }
            if (classStudentIDs.empty()) {
                cout << "No students found for class " << filterClass << endl;
                return;
            }
        }

        int id;
        cout << endl << "Enter Student ID: ";
        cin >> id;
        cin.ignore();

        string name, cls;
        if (!studentExists(id, name, cls)) {
            cout << "Student ID not found!" << endl;
            return;
        }
        if (!filterClass.empty() && cls != filterClass) {
            cout << "Error: You can only mark attendance for students in your assigned class (" << filterClass << ")." << endl;
            return;
        }

        Attendance* newRecord = new Attendance;
        newRecord->studentID = id;

        // Date Validation Loop (Manual date is available to both admin & teacher)
        while (true) {
            cout << "Enter Date (DD-MM-YYYY): ";
            getline(cin, newRecord->date);
            if (isValidDate(newRecord->date)) break;
            cout << "Invalid Date Format! Please use DD-MM-YYYY (e.g., 01-01-2024)." << endl;
        }

        // Status Validation Loop
        while (true) {
            cout << "Enter Status (P/A): ";
            getline(cin, newRecord->status);
            if (newRecord->status == "P" || newRecord->status == "A") break;
            cout << "Invalid Status! Please enter 'P' for Present or 'A' for Absent." << endl;
        }

        newRecord->next = top;
        top = newRecord;
        saveAttendanceToFile();
        cout << "Attendance Marked." << endl;
    }

    // View attendance for a particular student (unchanged) but more info
    void viewStudentAttendance(int myID) {
        Attendance* temp = top;
        bool found = false;
        cout << endl << "--- Attendance Record (ID: " << myID << ") ---" << endl;
        while(temp != NULL) {
            if(temp->studentID == myID) {
                cout << "Date: " << temp->date << " | Status: " << temp->status << endl;
                found = true;
            }
            temp = temp->next;
        }
        if(!found) cout << "No attendance records found." << endl;
    }

    // View attendance by class (Admin and Teacher filtered)
    void viewAttendanceByClass(string filterClass = "") {
        if (top == NULL) {
            cout << "No attendance records found." << endl;
            return;
        }
        cout << endl << "--- Attendance Records";
        if (!filterClass.empty()) cout << " (Class: " << filterClass << ")";
        cout << " ---" << endl;
        cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(15) << "Class" << setw(15) << "Date" << setw(8) << "Status" << endl;
        cout << "-------------------------------------------------------------------------------" << endl;

        // We'll traverse attendance and for each entry lookup student name/class
        bool any = false;
        Attendance* a = top;
        while (a != NULL) {
            string sname, sclass;
            if (studentExists(a->studentID, sname, sclass)) {
                if (filterClass.empty() || sclass == filterClass) {
                    cout << left << setw(10) << a->studentID << setw(25) << sname << setw(15) << sclass << setw(15) << a->date << setw(8) << a->status << endl;
                    any = true;
                }
            } else {
                // Student missing but still print ID
                if (filterClass.empty()) {
                    cout << left << setw(10) << a->studentID << setw(25) << "[Unknown]" << setw(15) << "[Unknown]" << setw(15) << a->date << setw(8) << a->status << endl;
                    any = true;
                }
            }
            a = a->next;
        }
        if (!any) cout << "No attendance records match this class." << endl;
    }

    // --- FILE I/O ---
    void saveStudentsToFile() {
        ofstream file("students.txt");
        Student* temp = head;
        while (temp != NULL) {
            file << temp->id << "|" << temp->name << "|" << temp->className << "|" << temp->password << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadStudentsFromFile() {
        ifstream file("students.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, name, className, password;
            getline(ss, idStr, '|'); getline(ss, name, '|'); getline(ss, className, '|'); getline(ss, password, '|');

            if (!idStr.empty()) {
                Student* newStudent = new Student;
                try { newStudent->id = stoi(idStr); }
                catch (...) { delete newStudent; continue; }
                newStudent->name = name;
                newStudent->className = className;
                newStudent->password = password;
                newStudent->next = NULL;

                if (head == NULL) head = newStudent;
                else {
                    Student* temp = head;
                    while (temp->next != NULL) temp = temp->next;
                    temp->next = newStudent;
                }
            }
        }
        file.close();
    }

    void saveAttendanceToFile() {
        ofstream file("attendance.txt");
        Attendance* temp = top;
        while (temp != NULL) {
            file << temp->studentID << "|" << temp->date << "|" << temp->status << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadAttendanceFromFile() {
        ifstream file("attendance.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, date, status;
            getline(ss, idStr, '|'); getline(ss, date, '|'); getline(ss, status, '|');

            if(!idStr.empty()){
                Attendance* newRecord = new Attendance;
                try { newRecord->studentID = stoi(idStr); }
                catch (...) { delete newRecord; continue; }
                newRecord->date = date;
                newRecord->status = status;
                newRecord->next = top;
                top = newRecord;
            }
        }
        file.close();
    }
};

// ---------------- TEACHER RECORD ----------------

class TeacherRecord {
private:
    Teacher* head;

public:
    TeacherRecord() {
        head = NULL;
        loadTeachersFromFile();
    }

    ~TeacherRecord() {
        while (head != NULL) {
            Teacher* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addTeacher() {
        Teacher* newTeacher = new Teacher;
        cout << endl << "Enter Teacher ID: ";
        while (!(cin >> newTeacher->id)) {
            cout << "Invalid input! Enter a number: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        cin.ignore();

        // Check for duplicates
        Teacher* temp = head;
        while(temp != NULL) {
            if(temp->id == newTeacher->id) {
                cout << "Error: Teacher ID already exists!" << endl;
                delete newTeacher;
                return;
            }
            temp = temp->next;
        }

        cout << "Enter Teacher Name: ";
        getline(cin, newTeacher->name);
        cout << "Enter Assigned Class (e.g. 10th): ";
        getline(cin, newTeacher->assignedClass); // Capture Class
        cout << "Enter Teacher Password: ";
        newTeacher->password = getHiddenPassword();

        newTeacher->next = head;
        head = newTeacher;

        saveTeachersToFile();
        cout << "Teacher Added Successfully! Assigned Class: " << newTeacher->assignedClass << endl;
    }

    bool loginTeacher(int id, string pass, string &outName, string &outClass) {
        Teacher* temp = head;
        while (temp != NULL) {
            if (temp->id == id && temp->password == pass) {
                outName = temp->name;
                outClass = temp->assignedClass; // Retrieve Class
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    void saveTeachersToFile() {
        ofstream file("teachers.txt");
        Teacher* temp = head;
        while(temp != NULL) {
            // Format: ID|Name|Password|Class
            file << temp->id << "|" << temp->name << "|" << temp->password << "|" << temp->assignedClass << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadTeachersFromFile() {
        ifstream file("teachers.txt");
        if (!file) return;
        string line;
        while(getline(file, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string idStr, name, pass, cls;
            getline(ss, idStr, '|');
            getline(ss, name, '|');
            getline(ss, pass, '|');
            getline(ss, cls, '|'); // Load Class

            if(!idStr.empty()) {
                Teacher* newTeacher = new Teacher;
                try { newTeacher->id = stoi(idStr); }
                catch (...) { delete newTeacher; continue; }
                newTeacher->name = name;
                newTeacher->password = pass;
                newTeacher->assignedClass = cls;
                newTeacher->next = head;
                head = newTeacher;
            }
        }
        file.close();
    }

    // View all teachers (Admin feature)
    void viewTeachers() {
        if (head == NULL) {
            cout << "No teachers registered." << endl;
            return;
        }
        Teacher* temp = head;
        cout << endl << "--- Teacher List ---" << endl;
        cout << left << setw(10) << "ID" << setw(30) << "Name" << setw(15) << "Assigned Class" << endl;
        cout << "--------------------------------------------------------" << endl;
        while (temp != NULL) {
            cout << left << setw(10) << temp->id << setw(30) << temp->name << setw(15) << temp->assignedClass << endl;
            temp = temp->next;
        }
    }
};

// ---------------- PARENT RECORD ----------------

class ParentRecord {
private:
    Parent* head;

public:
    ParentRecord() {
        head = NULL;
        loadParentsFromFile();
    }

    ~ParentRecord() {
        while (head != NULL) {
            Parent* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void registerParent(StudentRecord &studentHelper) {
        int childID;
        cout << endl << "Enter the Child's Student ID: ";
        cin >> childID;

        string name, cls;
        if (!studentHelper.studentExists(childID, name, cls)) {
            cout << "Error: Student ID " << childID << " does not exist." << endl;
            return;
        }

        Parent* check = head;
        while(check != NULL) {
            if(check->childID == childID) {
                cout << "Error: Parent account already exists for this Student ID." << endl;
                return;
            }
            check = check->next;
        }

        Parent* newParent = new Parent;
        newParent->childID = childID;
        cout << "Create Parent Password: ";
        newParent->password = getHiddenPassword();

        newParent->next = head;
        head = newParent;

        saveParentsToFile();
        cout << "Parent Account Created for Student: " << name << endl;
    }

    bool loginParent(int id, string pass) {
        Parent* temp = head;
        while (temp != NULL) {
            if (temp->childID == id && temp->password == pass) {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    void deleteParentByChildID(int id) {
        Parent* curr = head;
        Parent* prev = NULL;
        if (curr != NULL && curr->childID == id) {
            head = curr->next;
            delete curr;
            saveParentsToFile();
            return;
        }
        while (curr != NULL && curr->childID != id) {
            prev = curr;
            curr = curr->next;
        }
        if (curr == NULL) return;
        prev->next = curr->next;
        delete curr;
        saveParentsToFile();
    }

    void saveParentsToFile() {
        ofstream file("parents.txt");
        Parent* temp = head;
        while (temp != NULL) {
            file << temp->childID << "|" << temp->password << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadParentsFromFile() {
        ifstream file("parents.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, pass;
            getline(ss, idStr, '|'); getline(ss, pass, '|');

            if (!idStr.empty()) {
                Parent* newParent = new Parent;
                try { newParent->childID = stoi(idStr); }
                catch (...) { delete newParent; continue; }
                newParent->password = pass;
                newParent->next = head;
                head = newParent;
            }
        }
        file.close();
    }
};

// ==========================================
//            ACADEMIC & FINANCIAL MODULES
// ==========================================

// ---------------- FEE RECORD ----------------

class FeeRecord {
private:
    Fee* head;
public:
    FeeRecord() {
        head = NULL;
        loadFeesFromFile();
    }

    ~FeeRecord() {
        while(head != NULL) {
            Fee* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addFee(StudentRecord &studentHelper) {
        int id;
        cout << endl << "Enter Student ID: ";
        cin >> id;
        cin.ignore();
        string name, cls;
        if (!studentHelper.studentExists(id, name, cls)) {
            cout << "Student not found." << endl;
            return;
        }

        // Prevent duplicate fee entry for same student+class (optional)
        Fee* check = head;
        while (check != NULL) {
            if (check->studentID == id) {
                cout << "A fee record for this student already exists. Use Edit Fee Status to change status or delete existing record." << endl;
                return;
            }
            check = check->next;
        }

        Fee* newFee = new Fee;
        newFee->studentID = id;
        newFee->name = name;
        newFee->className = cls;
        cout << "Enter Amount: "; cin >> newFee->amount; cin.ignore();
        cout << "Enter Status (Paid/Unpaid): "; getline(cin, newFee->status);
        newFee->next = NULL;

        if (head == NULL) head = newFee;
        else {
            Fee* temp = head;
            while(temp->next != NULL) temp = temp->next;
            temp->next = newFee;
        }

        saveFeesToFile();
        cout << "Fee Added." << endl;
    }

    void deleteFeesByStudentID(int id) {
        Fee* curr = head;
        Fee* prev = NULL;
        bool deleted = false;

        while(curr != NULL) {
            if(curr->studentID == id) {
                Fee* toDelete = curr;
                if(prev == NULL) {
                    head = curr->next;
                    curr = head;
                } else {
                    prev->next = curr->next;
                    curr = prev->next;
                }
                delete toDelete;
                deleted = true;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
        if(deleted) saveFeesToFile();
    }

    void viewMyFees(int myID) {
        Fee* temp = head;
        bool found = false;
        cout << endl << "--- Fee Status ---" << endl;
        while (temp != NULL) {
            if (temp->studentID == myID) {
                cout << "Amount: $" << temp->amount << " | Status: " << temp->status << endl;
                found = true;
            }
            temp = temp->next;
        }
        if (!found) cout << "No fee records found." << endl;
    }

    // Admin: Edit (toggle) fee status
    void editFeeStatus() {
        if (head == NULL) {
            cout << "No fee records to edit." << endl;
            return;
        }
        int id;
        cout << "Enter Student ID to edit fee status: ";
        cin >> id;
        cin.ignore();
        Fee* temp = head;
        while (temp != NULL) {
            if (temp->studentID == id) {
                cout << "Current Status: " << temp->status << endl;
                cout << "Enter New Status (Paid/Unpaid): ";
                string s; getline(cin, s);
                s = trim(s);
                if (s != "Paid" && s != "Unpaid") {
                    cout << "Invalid status. Use exactly 'Paid' or 'Unpaid'." << endl;
                    return;
                }
                temp->status = s;
                saveFeesToFile();
                cout << "Status Updated." << endl;
                return;
            }
            temp = temp->next;
        }
        cout << "No fee record found for that Student ID." << endl;
    }

    // Admin: View fees by class or all
    void viewFeesByClass(string filterClass = "") {
        if (head == NULL) {
            cout << "No fee records." << endl;
            return;
        }
        cout << endl << "--- Fee Records";
        if (!filterClass.empty()) cout << " (Class: " << filterClass << ")";
        cout << " ---" << endl;
        cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(15) << "Class" << setw(10) << "Amount" << setw(10) << "Status" << endl;
        cout << "------------------------------------------------------------------" << endl;
        Fee* temp = head;
        bool any = false;
        while (temp != NULL) {
            if (filterClass.empty() || temp->className == filterClass) {
                cout << left << setw(10) << temp->studentID << setw(25) << temp->name << setw(15) << temp->className << setw(10) << temp->amount << setw(10) << temp->status << endl;
                any = true;
            }
            temp = temp->next;
        }
        if (!any) cout << "No fee records match this class." << endl;
    }

    void saveFeesToFile() {
        ofstream file("fees.txt");
        Fee* temp = head;
        while(temp != NULL) {
            file << temp->studentID << "|" << temp->name << "|" << temp->className << "|" << temp->amount << "|" << temp->status << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadFeesFromFile() {
        ifstream file("fees.txt");
        if (!file) return;
        string line;
        while(getline(file, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string idStr, name, cls, amtStr, status;
            getline(ss, idStr, '|'); getline(ss, name, '|'); getline(ss, cls, '|'); getline(ss, amtStr, '|'); getline(ss, status, '|');

            if(!idStr.empty()) {
                Fee* newFee = new Fee;
                try {
                    newFee->studentID = stoi(idStr);
                    newFee->amount = stof(amtStr);
                } catch(...) { delete newFee; continue; }
                newFee->name = name;
                newFee->className = cls;
                newFee->status = status;
                newFee->next = NULL;

                if(head == NULL) head = newFee;
                else {
                    Fee* temp = head;
                    while(temp->next != NULL) temp = temp->next;
                    temp->next = newFee;
                }
            }
        }
        file.close();
    }
};

// ---------------- RESULT RECORD ----------------

class ResultRecord {
private:
    Result* head;
public:
    ResultRecord() {
        head = NULL;
        loadResultsFromFile();
    }

    ~ResultRecord() {
        while(head != NULL) {
            Result* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Teachers can add results (Admin cannot; Admin will have search only)
    void addResult(StudentRecord &studentHelper) {
        int id;
        cout << endl << "Enter Student ID: ";
        cin >> id;
        cin.ignore();
        string name, cls;
        if (!studentHelper.studentExists(id, name, cls)) {
            cout << "Student not found." << endl;
            return;
        }

        // Prevent duplicate result entry for same student if desired:
        Result* check = head;
        while (check != NULL) {
            if (check->studentID == id) {
                cout << "A result for this student already exists. Delete or edit it instead." << endl;
                return;
            }
            check = check->next;
        }

        Result* newRes = new Result;
        newRes->studentID = id;
        newRes->name = name;
        cout << "Enter Total Marks: "; cin >> newRes->total;
        cout << "Enter Obtained Marks: "; cin >> newRes->obtained;
        cin.ignore();
        if (newRes->total <= 0) {
            cout << "Total must be positive." << endl;
            delete newRes;
            return;
        }
        newRes->percentage = (newRes->obtained / newRes->total) * 100.0f;
        newRes->grade = (newRes->percentage >= 80) ? 'A' : (newRes->percentage >= 50) ? 'B' : 'F';
        newRes->next = NULL;

        if (head == NULL) head = newRes;
        else {
            Result* temp = head;
            while(temp->next != NULL) temp = temp->next;
            temp->next = newRes;
        }

        saveResultsToFile();
        cout << "Result Added." << endl;
    }

    void deleteResultsByStudentID(int id) {
        Result* curr = head;
        Result* prev = NULL;
        bool deleted = false;
        while(curr != NULL) {
            if(curr->studentID == id) {
                Result* toDelete = curr;
                if(prev == NULL) {
                    head = curr->next;
                    curr = head;
                } else {
                    prev->next = curr->next;
                    curr = prev->next;
                }
                delete toDelete;
                deleted = true;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
        if(deleted) saveResultsToFile();
    }

    // Admin feature: Search result by Student ID (read-only)
    void searchResultByID() {
        if (head == NULL) {
            cout << "No results recorded yet." << endl;
            return;
        }
        int id;
        cout << "Enter Student ID to search result: ";
        cin >> id;
        cin.ignore();
        Result* temp = head;
        while (temp != NULL) {
            if (temp->studentID == id) {
                cout << endl << "--- Result for ID: " << id << " ---" << endl;
                cout << "Name: " << temp->name << endl;
                cout << "Score: " << temp->obtained << " / " << temp->total << endl;
                cout << "Percentage: " << temp->percentage << "%   Grade: " << temp->grade << endl;
                return;
            }
            temp = temp->next;
        }
        cout << "Result not found for this Student ID." << endl;
    }

    void viewMyResult(int myID) {
        Result* temp = head;
        bool found = false;
        cout << endl << "--- Academic Results ---" << endl;
        while (temp != NULL) {
            if (temp->studentID == myID) {
                cout << "Score: " << temp->obtained << "/" << temp->total << " (" << temp->percentage << "%) - Grade: " << temp->grade << endl;
                found = true;
            }
            temp = temp->next;
        }
        if (!found) cout << "Result not yet declared." << endl;
    }

    void saveResultsToFile() {
        ofstream file("results.txt");
        Result* temp = head;
        while(temp != NULL) {
            file << temp->studentID << "|" << temp->name << "|" << temp->obtained << "|" << temp->total << "|" << temp->percentage << "|" << temp->grade << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadResultsFromFile() {
        ifstream file("results.txt");
        if (!file) return;
        string line;
        while(getline(file, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string idStr, name, obtStr, totStr, perStr, gradeStr;
            getline(ss, idStr, '|'); getline(ss, name, '|'); getline(ss, obtStr, '|'); getline(ss, totStr, '|'); getline(ss, perStr, '|'); getline(ss, gradeStr, '|');

            if(!idStr.empty()) {
                Result* newRes = new Result;
                try {
                    newRes->studentID = stoi(idStr);
                    newRes->obtained = stof(obtStr);
                    newRes->total = stof(totStr);
                    newRes->percentage = stof(perStr);
                } catch(...) { delete newRes; continue; }
                newRes->name = name;
                newRes->grade = gradeStr.empty() ? 'F' : gradeStr[0];
                newRes->next = NULL;

                if(head == NULL) head = newRes;
                else {
                    Result* temp = head;
                    while(temp->next != NULL) temp = temp->next;
                    temp->next = newRes;
                }
            }
        }
        file.close();
    }
};

// ==========================================
//            COMMUNICATION MODULE
// ==========================================

// ---------------- MESSAGE RECORD ----------------

class MessageRecord {
private:
    Message* head;

    // Helper to rewrite file completely (used when replying)
    void saveAllMessages() {
        ofstream file("messages.txt"); // Truncate mode
        Message* temp = head;
        while(temp != NULL) {
            file << temp->sender << "|" << temp->content << "|" << temp->reply << endl;
            temp = temp->next;
        }
        file.close();
    }

public:
    MessageRecord() {
        head = NULL;
        loadMessagesFromFile();
    }

    ~MessageRecord() {
        while (head != NULL) {
            Message* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void sendMessage(string sender) {
        string content;
        cout << endl << "Enter your complaint/message: ";
        cin.ignore();
        getline(cin, content);

        Message* newTask = new Message;
        newTask->sender = sender;
        newTask->content = content;
        newTask->reply = "Pending"; // Default status
        newTask->next = head;
        head = newTask;

        // Append to file
        ofstream file("messages.txt", ios::app);
        file << sender << "|" << content << "|Pending" << endl;
        file.close();
        cout << "Message sent to Admin." << endl;
    }

    // Admin View: List all and option to reply
    void viewAllMessages(bool isAdmin) {
        if (head == NULL) {
            cout << "No messages found." << endl;
            return;
        }

        Message* temp = head;
        int count = 1;
        cout << endl << "--- Inbox (Complaints) ---" << endl;
        while (temp != NULL) {
            cout << "[" << count++ << "] From: " << temp->sender << endl;
            cout << "    Msg: " << temp->content << endl;
            cout << "    Reply: " << temp->reply << endl;
            cout << "------------------------" << endl;
            temp = temp->next;
        }

        if (isAdmin) {
            int choice;
            cout << "Enter Complaint Number to Reply (0 to exit): ";
            cin >> choice;
            if (choice > 0) {
                replyToMessage(choice);
            }
        }
    }

    void replyToMessage(int index) {
        Message* temp = head;
        int count = 1;
        while(temp != NULL && count < index) {
            temp = temp->next;
            count++;
        }

        if (temp != NULL) {
            cout << "Enter Reply for \"" << temp->sender << "\": ";
            cin.ignore();
            getline(cin, temp->reply);
            saveAllMessages(); // Rewrite file with new reply
            cout << "Reply sent successfully." << endl;
        } else {
            cout << "Invalid Complaint Number." << endl;
        }
    }

    // Parent View: Show only their messages
    void viewMyMessages(string senderSignature) {
        if (head == NULL) {
            cout << "No messages found." << endl;
            return;
        }

        Message* temp = head;
        bool found = false;
        cout << endl << "--- My Sent Complaints ---" << endl;
        while (temp != NULL) {
            if (temp->sender == senderSignature) {
                cout << "Msg: " << temp->content << endl;
                cout << "Reply: " << temp->reply << endl;
                cout << "------------------------" << endl;
                found = true;
            }
            temp = temp->next;
        }
        if (!found) cout << "You have not sent any complaints." << endl;
    }

    void loadMessagesFromFile() {
        ifstream file("messages.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string sender, content, reply;
            getline(ss, sender, '|');
            getline(ss, content, '|');
            getline(ss, reply, '|'); // Load reply

            if(!sender.empty()) {
                Message* newMessage = new Message;
                newMessage->sender = sender;
                newMessage->content = content;
                newMessage->reply = reply.empty() ? "Pending" : reply;
                newMessage->next = head;
                head = newMessage;
            }
        }
        file.close();
    }
};

// ==========================================
//            MENUS
// ==========================================

void adminMenu(StudentRecord &record, FeeRecord &fee, ResultRecord &result, ParentRecord &parentRec, MessageRecord &messageRec, TeacherRecord &teacherRec) {
    int choice;
    do {
        cout << endl << "===== ADMIN DASHBOARD =====" << endl;
        cout << "1. Add Teacher" << endl;
        cout << "2. Register New Parent" << endl;
        cout << "3. View All Students" << endl;
        cout << "4. View Student Attendance (by ID)" << endl;
        cout << "5. Add Fee Record" << endl;
        cout << "6. Search Result (View by Student ID)" << endl; // Admin no longer has Add Result
        cout << "7. View & Reply to Complaints" << endl;
        cout << "8. Edit Fee Status" << endl;
        cout << "9. View Attendance by Class (Search Bar)" << endl;
        cout << "10. View All Teachers" << endl;
        cout << "11. Logout" << endl;
        cout << "Enter Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: teacherRec.addTeacher(); break;
            case 2: parentRec.registerParent(record); break;
            case 3: record.viewStudents(); break; // Admin views ALL classes
            case 4: {
                int id;
                cout << "Enter Student ID: "; cin >> id;
                record.viewStudentAttendance(id);
                break;
            }
            case 5: fee.addFee(record); break;
            case 6: result.searchResultByID(); break;
            case 7: messageRec.viewAllMessages(true); break;
            case 8: fee.editFeeStatus(); break;
            case 9: {
                cin.ignore();
                cout << "Enter Class to filter by (e.g. 10th). Leave blank to view all: ";
                string cls; getline(cin, cls);
                cls = trim(cls);
                record.viewAttendanceByClass(cls);
                break;
            }
            case 10: teacherRec.viewTeachers(); break;
            case 11: cout << "Logging out..." << endl; break;
            default: cout << "Invalid Option." << endl;
        }
    } while (choice != 11);
}

void teacherMenu(string name, string assignedClass, StudentRecord &record, ResultRecord &result) {
    int choice;
    do {
        cout << endl << "===== TEACHER DASHBOARD (" << name << " | Class: " << assignedClass << ") =====" << endl;
        cout << "1. Add Student (to your class)" << endl;
        cout << "2. View All Students (Your Class)" << endl;
        cout << "3. Mark Attendance (Your Class only)" << endl;
        cout << "4. Add Result" << endl;
        cout << "5. View Attendance (Your Class filter)" << endl;
        cout << "6. Logout" << endl;
        cout << "Enter Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: record.addStudent(assignedClass); break; // Force add to THIS class
            case 2: record.viewStudents(assignedClass); break; // View ONLY this class
            case 3: record.markAttendance(assignedClass); break;
            case 4: result.addResult(record); break;
            case 5: record.viewAttendanceByClass(assignedClass); break;
            case 6: cout << "Logging out..." << endl; break;
            default: cout << "Invalid Option." << endl;
        }
    } while(choice != 6);
}

void studentMenu(int myID, string myName, StudentRecord &record, FeeRecord &fee, ResultRecord &result) {
    int choice;
    do {
        cout << endl << "===== WELCOME " << myName << " =====" << endl;
        cout << "1. View My Profile" << endl;
        cout << "2. View My Attendance" << endl;
        cout << "3. View My Result" << endl;
        cout << "4. View My Fees" << endl;
        cout << "5. Logout" << endl;
        cout << "Enter Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: {
                cout << endl << "--- My Profile ---" << endl;
                cout << "ID: " << myID << endl;
                cout << "Name: " << myName << endl;
                break;
            }
            case 2: record.viewStudentAttendance(myID); break;
            case 3: result.viewMyResult(myID); break;
            case 4: fee.viewMyFees(myID); break;
            case 5: cout << "Logging out..." << endl; break;
            default: cout << "Invalid Option." << endl;
        }
    } while (choice != 5);
}

void parentMenu(int childID, StudentRecord &record, FeeRecord &fee, ResultRecord &result, MessageRecord &messageRec) {
    int choice;
    string childName, childClass;
    record.studentExists(childID, childName, childClass);

    string senderSignature = "Parent of " + childName + " (ID: " + to_string(childID) + ")";

    do {
        cout << endl << "===== PARENT PORTAL (Child: " << childName << ") =====" << endl;
        cout << "1. View Child's Profile" << endl;
        cout << "2. View Child's Attendance" << endl;
        cout << "3. View Child's Result" << endl;
        cout << "4. View Child's Fee Status" << endl;
        cout << "5. Send Complaint" << endl;
        cout << "6. Inbox (View Replies)" << endl;
        cout << "7. Logout" << endl;
        cout << "Enter Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: {
                cout << endl << "--- Child's Profile ---" << endl;
                cout << "Name: " << childName << endl;
                cout << "Class: " << childClass << endl;
                cout << "ID: " << childID << endl;
                break;
            }
            case 2: record.viewStudentAttendance(childID); break;
            case 3: result.viewMyResult(childID); break;
            case 4: fee.viewMyFees(childID); break;
            case 5: messageRec.sendMessage(senderSignature); break;
            case 6: messageRec.viewMyMessages(senderSignature); break;
            case 7: cout << "Logging out..." << endl; break;
            default: cout << "Invalid Option." << endl;
        }
    } while (choice != 7);
}

// Provide a minimal viewMyProfile to show the student details
void showStudentProfileByID(StudentRecord &rec, int id) {
    // The StudentRecord contains a private list; we can add a public function but simpler to create a quick wrapper if needed.
    // For now, StudentRecord::viewMyProfile is defined; use that.
}

// ==========================================
//             MAIN
// ==========================================

int main() {
    StudentRecord record;
    FeeRecord fee;
    ResultRecord result;
    ParentRecord parentRec;
    MessageRecord messageRec;
    TeacherRecord teacherRec;

    int loginChoice;
    do {
        cout << endl << "###################################" << endl;
        cout << "    SCHOOL MANAGEMENT SYSTEM       " << endl;
        cout << "###################################" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. Student Login" << endl;
        cout << "3. Parent Login" << endl;
        cout << "4. Teacher Login" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter Choice: ";
        while (!(cin >> loginChoice)) {
            cout << "Enter a number: ";
            cin.clear();
            cin.ignore(100, '\n');
        }

        if (loginChoice == 1) {
            string user, pass;
            cout << "Enter Admin Username: "; cin >> user;
            cout << "Enter Admin Password: ";
            pass = getHiddenPassword();

            if (user == "admin" && pass == "admin123") {
                cout << endl << "Admin Login Successful!" << endl;
                adminMenu(record, fee, result, parentRec, messageRec, teacherRec);
            } else {
                cout << endl << "Invalid Credentials!" << endl;
                cout << "Press Enter..."; cin.ignore(); cin.get();
            }
        }
        else if (loginChoice == 2) {
            int id; string pass, name;
            cout << "Enter Student ID: "; cin >> id;
            cout << "Enter Password: ";
            pass = getHiddenPassword();

            if (record.verifyStudentLogin(id, pass, name)) {
                cout << endl << "Login Successful!" << endl;
                studentMenu(id, name, record, fee, result);
            } else {
                cout << endl << "Invalid ID or Password!" << endl;
                cout << "Press Enter..."; cin.ignore(); cin.get();
            }
        }
        else if (loginChoice == 3) {
            int id; string pass;
            cout << "Enter Student ID (Child's ID): "; cin >> id;
            cout << "Enter Parent Password: ";
            pass = getHiddenPassword();

            if (parentRec.loginParent(id, pass)) {
                cout << endl << "Parent Login Successful!" << endl;
                parentMenu(id, record, fee, result, messageRec);
            } else {
                cout << endl << "Invalid Parent Credentials!" << endl;
                cout << "Press Enter..."; cin.ignore(); cin.get();
            }
        }
        else if (loginChoice == 4) { // TEACHER LOGIN
            int id; string pass, name, assignedClass;
            cout << "Enter Teacher ID: "; cin >> id;
            cout << "Enter Password: ";
            pass = getHiddenPassword();

            if (teacherRec.loginTeacher(id, pass, name, assignedClass)) {
                cout << endl << "Teacher Login Successful!" << endl;
                // Pass the assigned class to the menu
                teacherMenu(name, assignedClass, record, result);
            } else {
                cout << endl << "Invalid Teacher Credentials!" << endl;
                cout << "Press Enter..."; cin.ignore(); cin.get();
            }
        }
        else {
            if (loginChoice != 5) {
                cout << endl << "Invalid Option! Please select 1-5." << endl;
                cout << "Press Enter..."; cin.ignore(); cin.get();
            }
        }

    } while (loginChoice != 5);

    cout << "Goodbye!" << endl;
    return 0;
}
