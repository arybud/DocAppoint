#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <map>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

class Patient {
public:
    int id;
    string name;
    int age;
    string gender;
    string contact;
    string issue;

    Patient(int id, string name, int age, string gender, string contact, string issue)
        : id(id), name(name), age(age), gender(gender), contact(contact), issue(issue) {}

    string toString() const {
        return to_string(id) + "|" + name + "|" + to_string(age) + "|" + gender + "|" + contact + "|" + issue;
    }
};

class Doctor {
public:
    int id;
    string name;
    string specialization;
    string contact;
    map<string, vector<pair<string, string> > > schedule; 

    Doctor(int id, string name, string specialization, string contact)
        : id(id), name(name), specialization(specialization), contact(contact) {}

    void setSchedule(string day, vector<pair<string, string> > slots) {
        schedule[day] = slots;
    }

    vector<pair<string, string> > getSchedule(string day) {
        if (schedule.find(day) != schedule.end()) {
            return schedule[day];
        }
        return vector<pair<string, string> >();
    }

    string toString() const {
        ostringstream oss;
        oss << id << "|" << name << "|" << specialization << "|" << contact << "|";
        for (map<string, vector<pair<string, string> > >::const_iterator it = schedule.begin(); it != schedule.end(); ++it) {
            oss << it->first << ":";
            for (size_t i = 0; i < it->second.size(); ++i) {
                oss << it->second[i].first << "-" << it->second[i].second << ",";
            }
            oss << ";";
        }
        return oss.str();
    }
};

class Appointment {
public:
    int id;
    int patientId;
    int doctorId;
    string date;
    string time;

    Appointment(int id, int patientId, int doctorId, string date, string time)
        : id(id), patientId(patientId), doctorId(doctorId), date(date), time(time) {}

    string toString() const {
        return to_string(id) + "|" + to_string(patientId) + "|" + to_string(doctorId) + "|" + date + "|" + time;
    }
};

class FindPatientById {
    int id;
public:
    FindPatientById(int id) : id(id) {}
    bool operator()(const Patient& p) const {
        return p.id == id;
    }
};

class FindDoctorById {
    int id;
public:
    FindDoctorById(int id) : id(id) {}
    bool operator()(const Doctor& d) const {
        return d.id == id;
    }
};

class FindAppointmentById {
    int id;
public:
    FindAppointmentById(int id) : id(id) {}
    bool operator()(const Appointment& a) const {
        return a.id == id;
    }
};

class FindPatientByIdAndName {
    int id;
    string name;
public:
    FindPatientByIdAndName(int id, const string& name) : id(id), name(name) {}
    bool operator()(const Patient& p) const {
        return p.id == id && p.name == name;
    }
};

class AppointmentManager {
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Appointment> appointments;
    int nextPatientId;
    int nextDoctorId;
    int nextAppointmentId;

    string patientsFile;
    string doctorsFile;
    string appointmentsFile;

    void waitForUser() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Press Enter to continue...";
        cin.get();
    }

    bool validateContact(const string& contact) {
        return regex_match(contact, regex("\\d{10}"));
    }

    bool validateGender(const string& gender) {
        return gender == "male" || gender == "female" || gender == "other";
    }

    bool validateDate(const string& date) {
        return regex_match(date, regex("\\d{2}-\\d{2}-\\d{4}"));
    }

    bool validateFutureDate(const string& date) {
        struct tm tm = {};
        stringstream ss(date);
        ss >> get_time(&tm, "%d-%m-%Y");
        time_t appointmentTime = mktime(&tm);
        time_t currentTime = time(nullptr);

        // Allow booking for current date and future dates
        struct tm *currentTm = localtime(&currentTime);
        currentTm->tm_hour = 0;
        currentTm->tm_min = 0;
        currentTm->tm_sec = 0;
        currentTime = mktime(currentTm);

        return difftime(appointmentTime, currentTime) >= 0;
    }

    bool validateTime(const string& time) {
        return regex_match(time, regex("\\d{2}:\\d{2}"));
    }

    bool validateDay(const string& day) {
        static const string validDays[] = {
            "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
        };
        return find(begin(validDays), end(validDays), day) != end(validDays);
    }

    string getDayOfWeek(const string& date) {
        struct tm tm = {};
        stringstream ss(date);
        ss >> get_time(&tm, "%d-%m-%Y");
        mktime(&tm);
        const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        return days[tm.tm_wday];
    }

    bool isTimeWithinFiveMinutes(const string& time1, const string& time2) {
        int hours1, minutes1, hours2, minutes2;
        sscanf(time1.c_str(), "%d:%d", &hours1, &minutes1);
        sscanf(time2.c_str(), "%d:%d", &hours2, &minutes2);

        int totalMinutes1 = hours1 * 60 + minutes1;
        int totalMinutes2 = hours2 * 60 + minutes2;

        return abs(totalMinutes1 - totalMinutes2) <= 5;
    }

    bool isTimeWithinSlot(const string& time, const pair<string, string>& slot) {
        int slotStartHours, slotStartMinutes, slotEndHours, slotEndMinutes, timeHours, timeMinutes;
        sscanf(slot.first.c_str(), "%d:%d", &slotStartHours, &slotStartMinutes);
        sscanf(slot.second.c_str(), "%d:%d", &slotEndHours, &slotEndMinutes);
        sscanf(time.c_str(), "%d:%d", &timeHours, &timeMinutes);

        int slotStartTotalMinutes = slotStartHours * 60 + slotStartMinutes;
        int slotEndTotalMinutes = slotEndHours * 60 + slotEndMinutes;
        int timeTotalMinutes = timeHours * 60 + timeMinutes;

        return timeTotalMinutes >= slotStartTotalMinutes && timeTotalMinutes <= slotEndTotalMinutes;
    }

    bool validateAppointment(int doctorId, string date, string time, const vector<pair<string, string> >& slots) {
        bool withinSlot = false;

        for (size_t i = 0; i < slots.size(); ++i) {
            if (isTimeWithinSlot(time, slots[i])) {
                withinSlot = true;
                break;
            }
        }

        if (!withinSlot) {
            cout << "DEBUG: Time " << time << " is not within any of the available slots.\n";
            return false;
        }

        for (size_t i = 0; i < appointments.size(); ++i) {
            if (appointments[i].doctorId == doctorId && appointments[i].date == date) {
                if (isTimeWithinFiveMinutes(time, appointments[i].time)) {
                    cout << "Appointment already exists in the time range.\n";
                    return false;
                }
            }
        }

        return true;
    }

    Patient& getPatientById(int id) {
        return *find_if(patients.begin(), patients.end(), FindPatientById(id));
    }

    Doctor& getDoctorById(int id) {
        return *find_if(doctors.begin(), doctors.end(), FindDoctorById(id));
    }

    void savePatients() {
        ofstream file(patientsFile);
        for (size_t i = 0; i < patients.size(); ++i) {
            file << patients[i].toString() << endl;
        }
    }

    void loadPatients() {
        ifstream file(patientsFile);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, age, gender, contact, issue;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, age, '|');
            getline(ss, gender, '|');
            getline(ss, contact, '|');
            getline(ss, issue, '|');
            patients.push_back(Patient(stoi(id), name, stoi(age), gender, contact, issue));
            nextPatientId = max(nextPatientId, stoi(id) + 1);
        }
    }

    void saveDoctors() {
        ofstream file(doctorsFile);
        for (size_t i = 0; i < doctors.size(); ++i) {
            file << doctors[i].toString() << endl;
        }
    }

    void loadDoctors() {
        ifstream file(doctorsFile);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, specialization, contact, schedule;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, specialization, '|');
            getline(ss, contact, '|');
            getline(ss, schedule, '|');
            Doctor doctor(stoi(id), name, specialization, contact);
            stringstream scheduleStream(schedule);
            string day;
            while (getline(scheduleStream, day, ';')) {
                stringstream dayStream(day);
                string dayName, slots;
                getline(dayStream, dayName, ':');
                getline(dayStream, slots);
                vector<pair<string, string> > slotList;
                stringstream slotsStream(slots);
                string slot;
                while (getline(slotsStream, slot, ',')) {
                    stringstream slotStream(slot);
                    string start, end;
                    getline(slotStream, start, '-');
                    getline(slotStream, end, '-');
                    slotList.push_back(make_pair(start, end));
                }
                doctor.setSchedule(dayName, slotList);
            }
            doctors.push_back(doctor);
            nextDoctorId = max(nextDoctorId, stoi(id) + 1);
        }
    }

    void saveAppointments() {
        ofstream file(appointmentsFile);
        for (size_t i = 0; i < appointments.size(); ++i) {
            file << appointments[i].toString() << endl;
        }
    }

    void loadAppointments() {
        ifstream file(appointmentsFile);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, patientId, doctorId, date, time;
            getline(ss, id, '|');
            getline(ss, patientId, '|');
            getline(ss, doctorId, '|');
            getline(ss, date, '|');
            getline(ss, time, '|');
            appointments.push_back(Appointment(stoi(id), stoi(patientId), stoi(doctorId), date, time));
            nextAppointmentId = max(nextAppointmentId, stoi(id) + 1);
        }
    }

    void displayBookedAppointments(int doctorId, const string& date, const vector<pair<string, string> >& slots) {
        cout << "Existing appointments:\n";
        bool hasAppointments = false;
        for (size_t i = 0; i < appointments.size(); ++i) {
            if (appointments[i].doctorId == doctorId && appointments[i].date == date) {
                for (size_t j = 0; j < slots.size(); ++j) {
                    if (isTimeWithinSlot(appointments[i].time, slots[j])) {
                        cout << appointments[i].time << "\n";
                        hasAppointments = true;
                        break;
                    }
                }
            }
        }
        if (!hasAppointments) {
            cout << "No existing appointments in this slot.\n";
        }
    }

public:
    AppointmentManager() : nextPatientId(1), nextDoctorId(1), nextAppointmentId(1), patientsFile("patients.txt"), doctorsFile("doctors.txt"), appointmentsFile("appointments.txt") {
        loadPatients();
        loadDoctors();
        loadAppointments();
    }

    ~AppointmentManager() {
        savePatients();
        saveDoctors();
        saveAppointments();
    }

    void addDoctor() {
        string name, specialization, contact;
        cout << "Enter doctor name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter doctor specialization: ";
        getline(cin, specialization);

        while (true) {
            cout << "Enter doctor contact (10-digit number): ";
            cin >> contact;
            if (validateContact(contact)) break;
            cout << "Invalid contact number. Please enter a 10-digit number.\n";
        }

        Doctor newDoctor(nextDoctorId++, name, specialization, contact);
        string day, more;
        do {
            while (true) {
                cout << "Enter the day for schedule (e.g., Monday): ";
                cin >> day;
                if (validateDay(day)) break;
                cout << "Invalid day. Please enter a valid day (e.g., Monday).\n";
            }
            cout << "Enter the slots for " << day << " (HH:MM-HH:MM,HH:MM-HH:MM,...): ";
            string slots;
            cin >> slots;
            vector<pair<string, string> > slotList;
            stringstream ss(slots);
            string slot;
            while (getline(ss, slot, ',')) {
                stringstream slotStream(slot);
                string start, end;
                getline(slotStream, start, '-');
                getline(slotStream, end, '-');
                slotList.push_back(make_pair(start, end));
            }
            newDoctor.setSchedule(day, slotList);
            cout << "Add another day? (yes/no): ";
            cin >> more;
        } while (more == "yes");

        doctors.push_back(newDoctor);
        saveDoctors(); 
        cout << "Doctor added successfully.\n";
        waitForUser();
    }

    void viewDoctors() {
        for (size_t i = 0; i < doctors.size(); ++i) {
            const Doctor& doctor = doctors[i];
            cout << "ID: " << doctor.id << ", Name: " << doctor.name << ", Specialization: " << doctor.specialization
                 << ", Contact: " << doctor.contact << "\n";
            for (map<string, vector<pair<string, string> > >::const_iterator it = doctor.schedule.begin(); it != doctor.schedule.end(); ++it) {
                cout << "  " << it->first << ": ";
                for (size_t j = 0; j < it->second.size(); ++j) {
                    cout << it->second[j].first << "-" << it->second[j].second << " ";
                }
                cout << "\n";
            }
        }
        waitForUser();
    }

    void updateDoctor() {
        int id;
        string name, specialization, contact;
        cout << "Enter doctor ID to update: ";
        cin >> id;

        for (size_t i = 0; i < doctors.size(); ++i) {
            Doctor& doctor = doctors[i];
            if (doctor.id == id) {
                cout << "Enter new doctor name: ";
                cin.ignore();
                getline(cin, name);
                doctor.name = name;

                cout << "Enter new doctor specialization: ";
                getline(cin, specialization);
                doctor.specialization = specialization;

                while (true) {
                    cout << "Enter new doctor contact (10-digit number): ";
                    cin >> contact;
                    if (validateContact(contact)) break;
                    cout << "Invalid contact number. Please enter a 10-digit number.\n";
                }
                doctor.contact = contact;

                cout << "Doctor updated successfully.\n";
                saveDoctors(); // Save doctors to file
                waitForUser();
                return;
            }
        }
        cout << "Doctor not found.\n";
        waitForUser();
    }

    void deleteDoctor() {
        int id;
        cout << "Enter doctor ID to delete: ";
        cin >> id;
        doctors.erase(remove_if(doctors.begin(), doctors.end(), FindDoctorById(id)), doctors.end());
        saveDoctors(); 
        cout << "Doctor deleted successfully.\n";
        waitForUser();
    }

    void addAppointment() {
    int doctorId;
    string date, time, patientName, patientGender, patientContact, patientIssue;
    int patientAge;

    while (true) {
        cout << "Enter doctor ID: ";
        cin >> doctorId;

        if (find_if(doctors.begin(), doctors.end(), FindDoctorById(doctorId)) == doctors.end()) {
        cout << "Doctor ID doesn't exist. ";
        waitForUser();
        return;
    } else {
        break;
        }
    }

    while (true) {
        cout << "Enter appointment date (DD-MM-YYYY): ";
        cin >> date;
        if (validateDate(date)) {
            if (validateFutureDate(date)) break;
            cout << "Cannot book an appointment in the past. Please enter a future date.\n";
        } else {
            cout << "Invalid date format. Please enter in DD-MM-YYYY format.\n";
        }
    }

    string day = getDayOfWeek(date);
    Doctor& doctor = getDoctorById(doctorId);
    vector<pair<string, string> > slots = doctor.getSchedule(day);

    if (slots.empty()) {
        cout << "Doctor does not sit on " << day << ".\n";
        waitForUser();
        return;
    }

    cout << "Available slots on " << day << ":\n";
    for (size_t i = 0; i < slots.size(); ++i) {
        cout << slots[i].first << " - " << slots[i].second << "\n";
    }

    char sessionChoice;
    vector<pair<string, string> > chosenSlots;

    if (slots.size() > 1) {
        cout << "Book for morning or evening sitting (m/e): ";
        cin >> sessionChoice;

        if (sessionChoice == 'm' || sessionChoice == 'M') {
            chosenSlots.push_back(slots[0]);
        } else if (sessionChoice == 'e' || sessionChoice == 'E') {
            chosenSlots.push_back(slots[1]);
        } else {
            cout << "Invalid choice.\n";
            waitForUser();
            return;
        }
    } else {
        chosenSlots = slots;
    }

    displayBookedAppointments(doctorId, date, chosenSlots);

    while (true) {
        cout << "Enter appointment time (HH:MM): ";
        cin >> time;
        if (validateTime(time)) {
            bool validAppointment = validateAppointment(doctorId, date, time, chosenSlots);
            if (validAppointment) {
                break;
            } else {
                cout << "Doctor does not have availability at this time. Please enter a valid time.\n";
            }
        } else {
            cout << "Invalid time format. Please enter in HH:MM format.\n";
        }
    }

    while (true) {
        cout << "Enter patient name: ";
        cin.ignore();
        getline(cin, patientName);
        if (!patientName.empty()) break;
        cout << "Patient name cannot be empty.\n";
    }

    cout << "Enter patient age: ";
    while (!(cin >> patientAge)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid age: ";
    }

    while (true) {
        cout << "Enter patient gender (m/f/o): ";
        cin >> patientGender;
        if (patientGender == "m") {
            patientGender = "male";
            break;
        } else if (patientGender == "f") {
            patientGender = "female";
            break;
        } else if (patientGender == "o") {
            patientGender = "other";
            break;
        } else {
            cout << "Invalid gender. Please enter 'm', 'f', or 'o'.\n";
        }
    }

    while (true) {
        cout << "Enter patient contact (10-digit number): ";
        cin >> patientContact;
        if (validateContact(patientContact)) break;
        cout << "Invalid contact number. Please enter a 10-digit number.\n";
    }

    cout << "Enter patient issue (optional): ";
    cin.ignore();
    getline(cin, patientIssue);

    int patientId = nextPatientId++;
    Patient newPatient(patientId, patientName, patientAge, patientGender, patientContact, patientIssue);
    patients.push_back(newPatient);
    savePatients(); 

    Appointment newAppointment(nextAppointmentId++, patientId, doctorId, date, time);
    appointments.push_back(newAppointment);
    saveAppointments(); 

    cout << "Appointment added successfully.\n";
    waitForUser();
}

    void viewAllAppointments() {
        for (size_t i = 0; i < appointments.size(); ++i) {
            const Appointment& appointment = appointments[i];
            const Patient& patient = getPatientById(appointment.patientId);
            const Doctor& doctor = getDoctorById(appointment.doctorId);
            cout << "Appointment ID: " << appointment.id << ", Doctor: " << doctor.name
                 << ", Date: " << appointment.date << ", Time: " << appointment.time << "\n";
            cout << "Patient ID: " << patient.id << ", Name: " << patient.name << ", Age: " << patient.age
                 << ", Gender: " << patient.gender << ", Contact: " << patient.contact << "\n";
            if (!patient.issue.empty()) {
                cout << "Issue: " << patient.issue << "\n";
            }
            cout << "------------------------\n";
        }
        waitForUser();
    }

    void viewAllPatients() {
        for (size_t i = 0; i < patients.size(); ++i) {
            const Patient& patient = patients[i];
            cout << "ID: " << patient.id << ", Name: " << patient.name << ", Age: " << patient.age
                 << ", Gender: " << patient.gender << ", Contact: " << patient.contact << ", Issue: " << patient.issue << "\n";
            cout << "------------------------\n";
        }
        waitForUser();
    }

    void updateExistingAppointment() {
        updateAppointment();
    }

    void deleteExistingAppointment() {
        deleteAppointment();
    }

    void deleteExistingPatient() {
        deletePatient();
    }

    void viewAllDoctors() {
        viewDoctors();
    }

    void updateExistingDoctor() {
        updateDoctor();
    }

    void deleteExistingDoctor() {
        deleteDoctor();
    }

    void updateAppointment() {
        int id, doctorId, patientAge;
        string date, time, patientName, patientGender, patientContact, patientIssue;

        cout << "Enter appointment ID to update: ";
        cin >> id;

        for (size_t i = 0; i < appointments.size(); ++i) {
            Appointment& appointment = appointments[i];
            if (appointment.id == id) {
                cout << "Enter new doctor ID: ";
                cin >> doctorId;

                if (find_if(doctors.begin(), doctors.end(), FindDoctorById(doctorId)) == doctors.end()) {
                    cout << "Invalid doctor ID.\n";
                    waitForUser();
                    return;
                }

                while (true) {
                    cout << "Enter new appointment date (DD-MM-YYYY): ";
                    cin >> date;
                    if (validateDate(date)) {
                        if (validateFutureDate(date)) break;
                        cout << "Cannot book an appointment in the past. Please enter a future date.\n";
                    } else {
                        cout << "Invalid date format. Please enter in DD-MM-YYYY format.\n";
                    }
                }

                string day = getDayOfWeek(date);
                Doctor& doctor = getDoctorById(doctorId);
                vector<pair<string, string> > slots = doctor.getSchedule(day);

                if (slots.empty()) {
                    cout << "Doctor does not sit on " << day << ".\n";
                    waitForUser();
                    return;
                }

                cout << "Available slots on " << day << ":\n";
                for (size_t j = 0; j < slots.size(); ++j) {
                    cout << slots[j].first << " - " << slots[j].second << "\n";
                }

                char sessionChoice;
                vector<pair<string, string> > chosenSlots;

                if (slots.size() > 1) {
                    cout << "Book for morning or evening sitting (m/e): ";
                    cin >> sessionChoice;

                    if (sessionChoice == 'm' || sessionChoice == 'M') {
                        chosenSlots.push_back(slots[0]);
                    } else if (sessionChoice == 'e' || sessionChoice == 'E') {
                        chosenSlots.push_back(slots[1]);
                    } else {
                        cout << "Invalid choice.\n";
                        waitForUser();
                        return;
                    }
                } else {
                    chosenSlots = slots;
                }

                displayBookedAppointments(doctorId, date, chosenSlots);

                while (true) {
                    cout << "Enter new appointment time (HH:MM): ";
                    cin >> time;
                    if (validateTime(time) && validateAppointment(doctorId, date, time, chosenSlots)) break;
                    cout << "Invalid time or time within 5 minutes of another appointment. Please enter a valid time.\n";
                }

                while (true) {
                    cout << "Enter new patient name: ";
                    cin.ignore();
                    getline(cin, patientName);
                    if (!patientName.empty()) break;
                    cout << "Patient name cannot be empty.\n";
                }

                cout << "Enter new patient age: ";
                while (!(cin >> patientAge)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid age: ";
                }

                while (true) {
                    cout << "Enter new patient gender (m/f/o): ";
                    cin >> patientGender;
                    if (patientGender == "m") {
                        patientGender = "male";
                        break;
                    } else if (patientGender == "f") {
                        patientGender = "female";
                        break;
                    } else if (patientGender == "o") {
                        patientGender = "other";
                        break;
                    } else {
                        cout << "Invalid gender. Please enter 'm', 'f', or 'o'.\n";
                    }
                }

                while (true) {
                    cout << "Enter new patient contact (10-digit number): ";
                    cin >> patientContact;
                    if (validateContact(patientContact)) break;
                    cout << "Invalid contact number. Please enter a 10-digit number.\n";
                }

                cout << "Enter new patient issue (optional): ";
                cin.ignore();
                getline(cin, patientIssue);

                Patient& patient = getPatientById(appointment.patientId);
                patient.name = patientName;
                patient.age = patientAge;
                patient.gender = patientGender;
                patient.contact = patientContact;
                patient.issue = patientIssue;

                appointment.doctorId = doctorId;
                appointment.date = date;
                appointment.time = time;

                cout << "Appointment updated successfully.\n";
                saveAppointments(); // Save appointments to file
                waitForUser();
                return;
            }
        }
        cout << "Appointment not found.\n";
        waitForUser();
    }

    void deleteAppointment() {
        int id;
        cout << "Enter appointment ID to delete: ";
        cin >> id;
        appointments.erase(remove_if(appointments.begin(), appointments.end(), FindAppointmentById(id)), appointments.end());
        saveAppointments(); // Save appointments to file
        cout << "Appointment deleted successfully.\n";
        waitForUser();
    }

    void deletePatient() {
        int id;
        string name;
        cout << "Enter patient ID to delete: ";
        cin >> id;
        cout << "Enter patient name to delete: ";
        cin.ignore();
        getline(cin, name);
        vector<Patient>::iterator it = find_if(patients.begin(), patients.end(), FindPatientByIdAndName(id, name));
        if (it != patients.end()) {
            patients.erase(it);
            savePatients(); // Save patients to file
            cout << "Patient deleted successfully.\n";
        } else {
            cout << "Patient not found.\n";
        }
        waitForUser();
    }
};

void displayMenu() {
    cout << "\n--------------------------------------------------\n";
    cout << setw(43) << "Doctor Appointment Management System\n";
    cout << "--------------------------------------------------\n\n";
    cout << "1. Add Appointment\n";
    cout << "2. View Appointments\n";
    cout << "3. Update Appointment\n";
    cout << "4. Delete Appointment\n";
    cout << "5. View Patients\n";
    cout << "6. Delete Patient\n";
    cout << "7. Add Doctor\n";
    cout << "8. View Doctors\n";
    cout << "9. Update Doctor\n";
    cout << "10. Delete Doctor\n";
    cout << "0. Exit\n";
}

int main() {
    AppointmentManager manager;
    int choice;
    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addAppointment();
                break;
            case 2:
                manager.viewAllAppointments();
                break;
            case 3:
                manager.updateExistingAppointment();
                break;
            case 4:
                manager.deleteExistingAppointment();
                break;
            case 5:
                manager.viewAllPatients();
                break;
            case 6:
                manager.deleteExistingPatient();
                break;
            case 7:
                manager.addDoctor();
                break;
            case 8:
                manager.viewAllDoctors();
                break;
            case 9:
                manager.updateExistingDoctor();
                break;
            case 10:
                manager.deleteExistingDoctor();
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}