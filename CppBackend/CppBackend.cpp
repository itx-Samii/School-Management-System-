#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

struct Student {
    int id;
    string name;
    string className;
    string password;   // NEW
    Student* next;
};

Student* head = NULL;

struct Attendance {
    int studentID;
    string date;
    string status;
    Attendance* next;
};
Attendance* top = NULL;

class StudentRecord {
public:
    StudentRecord() {
        loadStudentsFromFile();
        loadAttendanceFromFile();
    }

    ~StudentRecord() {
        saveStudentsToFile();
        saveAttendanceToFile();
    }

    void addStudent() {
    Student* newStudent = new Student;

    cout << endl << "Enter Student ID: ";
    cin >> newStudent->id;
    cin.ignore();

    cout << "Enter Student Name: ";
    getline(cin, newStudent->name);

    cout << "Enter Class Name: ";
    getline(cin, newStudent->className);

    cout << "Enter Password (leave blank to auto-generate): ";
    string pass;
    getline(cin, pass);
    if (pass.empty()) {
        pass = to_string(newStudent->id) + "123";
    }
    newStudent->password = pass; // make sure Student struct has password member

    newStudent->next = NULL;
    // ... (rest unchanged)
    saveStudentsToFile();
    cout << endl << "Student added successfully! Password: " << pass << endl;
}

    void viewStudents() {
        if (head == NULL) {
            cout << endl << "No student records found." << endl;
            return;
        }

        Student* temp = head;
        cout << endl << "Student List:" << endl;
        while (temp != NULL) {
            cout << "ID: " << temp->id
                 << " | Name: " << temp->name
                 << " | Class: " << temp->className << endl;
            temp = temp->next;
        }
    }

    void deleteStudent() {
        if (head == NULL) {
            cout << endl << "No records to delete." << endl;
            return;
        }

        int id;
        cout << endl << "Enter Student ID to delete: ";
        cin >> id;

        Student* temp = head;
        Student* prev = NULL;

        while (temp != NULL && temp->id != id) {
            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL) {
            cout << endl << "Student not found." << endl;
            return;
        }

        if (prev == NULL) head = temp->next;
        else prev->next = temp->next;

        delete temp;
        saveStudentsToFile();
        cout << endl << "Student deleted successfully." << endl;
    }

    void updateStudent() {
        if (head == NULL) {
            cout << endl << "No records to update." << endl;
            return;
        }

        int id;
        cout << endl << "Enter Student ID to update: ";
        cin >> id;
        cin.ignore();

        Student* temp = head;
        while (temp != NULL && temp->id != id) temp = temp->next;

        if (temp == NULL) {
            cout << endl << "Student not found." << endl;
            return;
        }

        cout << endl << "Enter new name (leave blank to keep current): ";
        string newName;
        getline(cin, newName);

        cout << "Enter new class name (leave blank to keep current): ";
        string newClass;
        getline(cin, newClass);

        if (!newName.empty()) temp->name = newName;
        if (!newClass.empty()) temp->className = newClass;

        saveStudentsToFile();
        cout << endl << "Student record updated successfully!" << endl;
    }

    void searchStudent() {
        if (head == NULL) {
            cout << endl << "No records to search." << endl;
            return;
        }

        int id;
        cout << endl << "Enter Student ID to search: ";
        cin >> id;

        Student* temp = head;
        while (temp != NULL) {
            if (temp->id == id) {
                cout << endl << "Student Found:" << endl;
                cout << "ID: " << temp->id << endl;
                cout << "Name: " << temp->name << endl;
                cout << "Class: " << temp->className << endl;
                return;
            }
            temp = temp->next;
        }

        cout << endl << "Student not found." << endl;
    }

    void markAttendance() {
        int studentID;
        string date, status;

        cout << endl << "Enter Student ID: ";
        cin >> studentID;
        cin.ignore();

        cout << "Enter Date (DD-MM-YYYY): ";
        getline(cin, date);

        cout << "Enter Status (Present/Absent): ";
        getline(cin, status);

        Attendance* newRecord = new Attendance;
        newRecord->studentID = studentID;
        newRecord->date = date;
        newRecord->status = status;
        newRecord->next = top;
        top = newRecord;

        saveAttendanceToFile();
        cout << endl << "Attendance recorded successfully!" << endl;
    }

    void viewRecentAttendance() {
        if (top == NULL) {
            cout << endl << "No attendance records found." << endl;
            return;
        }

        cout << endl << "Recent Attendance Records:" << endl;
        Attendance* temp = top;
        while (temp != NULL) {
            cout << "Student ID: " << temp->studentID
                 << " | Date: " << temp->date
                 << " | Status: " << temp->status << endl;
            temp = temp->next;
        }
    }

    void undoLastAttendance() {
        if (top == NULL) {
            cout << endl << "No attendance records to undo." << endl;
            return;
        }

        Attendance* temp = top;
        top = top->next;
        delete temp;

        saveAttendanceToFile();
        cout << endl << "Last attendance record removed." << endl;
    }

    void saveStudentsToFile() {
    ofstream file("students.txt");
    Student* temp = head;
    while (temp != NULL) {
        file << temp->id << "," << temp->name << "," << temp->className << "," << temp->password << endl;
        temp = temp->next;
    }
    file.close();
}


    void loadStudentsFromFile() {
    ifstream file("students.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, className, password;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, className, ',');
        getline(ss, password, ',');

        Student* newStudent = new Student;
        newStudent->id = stoi(idStr);
        newStudent->name = name;
        newStudent->className = className;
        newStudent->password = password;
        newStudent->next = NULL;

        if (head == NULL) head = newStudent;
        else {
            Student* temp = head;
            while (temp->next != NULL)
                temp = temp->next;
            temp->next = newStudent;
        }
    }
    file.close();
}


    void saveAttendanceToFile() {
        ofstream file("attendance.txt");
        Attendance* temp = top;
        while (temp != NULL) {
            file << temp->studentID << "," << temp->date << "," << temp->status << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadAttendanceFromFile() {
        ifstream file("attendance.txt");
        if (!file) return;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string idStr, date, status;
            getline(ss, idStr, ',');
            getline(ss, date, ',');
            getline(ss, status, ',');

            Attendance* newRecord = new Attendance;
            newRecord->studentID = stoi(idStr);
            newRecord->date = date;
            newRecord->status = status;
            newRecord->next = top;
            top = newRecord;
        }
        file.close();
    }
};

int main() {
    StudentRecord record;
    int choice;
    do {
        cout << endl << "SCHOOL MANAGEMENT MENU" << endl;
        cout << "1. Add Student" << endl;
        cout << "2. View Students" << endl;
        cout << "3. Delete Student" << endl;
        cout << "4. Update Student" << endl;
        cout << "5. Search Student" << endl;
        cout << "6. Mark Attendance" << endl;
        cout << "7. View Attendance" << endl;
        cout << "8. Undo Last Attendance" << endl;
        cout << "9. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: record.addStudent(); break;
            case 2: record.viewStudents(); break;
            case 3: record.deleteStudent(); break;
            case 4: record.updateStudent(); break;
            case 5: record.searchStudent(); break;
            case 6: record.markAttendance(); break;
            case 7: record.viewRecentAttendance(); break;
            case 8: record.undoLastAttendance(); break;
            case 9: cout << endl << "Exiting program..." << endl; break;
            default: cout << endl << "Invalid choice! Try again." << endl;
        }
    } while (choice != 9);
    return 0;
}
