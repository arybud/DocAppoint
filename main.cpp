#include <iostream>
#include <string>
#include <fstream>

using namespace std;

void DisplaySlot(char key,int hours,int status) {
cout<<" "<<key<<"->";
    
if(hours<10)   
    cout<<"0"<<hours<<":00 Hours";
else  
    cout<<hours<<":00 Hours";
    cout<<" - "; 
if(status==0)
    cout<<"Available"<<endl;
else
    cout<<"Booked"<<endl;
}

int BookAppointment() {
cout<<"\n\t\t\t\t Book Your Appointment\n";
cout<<"\tAvailable slots\n";
ifstream ifile;
ifile.open("appointment.txt");
int arr[13] ={0};
char key= 'A';
int hours= 9;
if(ifile) {
    string line;
while(getline(ifile,line)) {
    char temp= line[0];
    int index= (temp - 65);
    arr[index] = 1;
}
ifile.close();
}

for(int i=0;i<=12;i++) {
    DisplaySlot(key, hours, arr[i]);
    hours++;
    key++;
} 
char choice;
cout<<"Input your choice(A-M): ";
cin>>choice;
choice= toupper(choice);  
if (!(choice>='A' && choice<= 'M')) {
    cout<<"Invalid choice\n";
    cout<<"Please select a valid value from A-M";
    cout <<"\nPress enter key to continue...";
    getchar();
    getchar();
    return 0;
}

int index = (choice - 65);
if(arr[index]==1) {
    cout<<"Appointment already exists for this Time\n";
    cout<<"Please select a different time.";
    cout<<"\nPress enter key to continue...";
    getchar();
    getchar();
    return 0;
}

string name;
cin.ignore();
cout<<"Enter your First name: ";
getline(cin,name);
ofstream ofile;
ofile.open("appointment.txt", ios::app);
if(ofile) {
    ofile<<choice<<":"<<name<< " "<<endl;
    ofile.close();
    cout<<"Appointment booked for "<<(choice - 65) + 9<<":00 Hours successfully!";
} 
else {
    cout<<"Appointment not booked.";
}    
cout<<"\nPress enter key to continue...";
getchar();
return 0;
}

void ExistingAppointment() {
cout<<"----- Appointments Summary ----\n";

ifstream ifile;
ifile.open("appointment.txt");

int arr[13] ={0}; 
char key = 'A';
int hours = 9;

if(ifile) {
    string line;
while (getline(ifile,line)) {
    char temp=line[0];
    int index =(temp - 'A');
    arr[index] = 1; 
    }
    ifile.close();
}

for(int i=0;i<=12;i++) {
    DisplaySlot(key,hours,arr[i]);
    hours++;
    key++;
}

cout<<"Please press enter key to continue...";
getchar();
getchar();
}

void AppointmentDetails() {

char choice;
cout<<"Enter the appointment key (A-M) for which you want to see details: ";
cin>>choice;

cout<<"----- Appointment Details ------\n";

ifstream ifile;
ifile.open("appointment.txt");

if(ifile) {
    string line;
    choice = toupper(choice);  
    
    while (getline(ifile,line)) {
        if (line[0]==choice) {
            string key, name;
            key = line.substr(0,1);
            name = line.substr(2);
            cout<<"Appointment Key:"<<key<<endl;
            cout<<"Appointment Time: "<<(key[0] -'A') + 9<<":00 Hours\n";
            cout<<"Patient Name: "<< name << endl;
            ifile.close();
            cout<<"\nPress enter key to continue...";
            getchar();
            getchar();
            return;
         }
 } 
ifile.close();
} 
else {
    cout<<"Error trying to display appointment details .\n";
}

cout<<"No appointment found!";
cout<<"\nPress enter key to continue...";
getchar();
getchar();
}


void UpdateAppointment() {
cout<<"---- Update Appointment ----\n";

ifstream ifile;
ifile.open("appointment.txt");

int arr[13] ={0}; 
char key = 'A';
int hours = 9;

if (ifile) {
    string line;
    while(getline(ifile,line)) {
        char temp =line[0];
        int index =(temp - 65);
        arr[index] =1;
    }
    ifile.close();
}

for(int i=0;i<=12;i++) {
    DisplaySlot(key,hours,arr[i]);
    hours++;
    key++;
 }

char choice;
cout<<"Enter your existing appointment (A-M) to update: ";
cin>>choice;
choice =toupper(choice);  
if (choice<'A' || choice>'M') {
    cout<< "Invalid appointment slot chosen.\n";
    cout<<"Please enter a valid time slot (A-M).\n";
    cout<<"Press enter key to continue...";
    getchar();
    getchar();
    return;
}
    
int index =(choice - 65);
if(arr[index]==0) {
    cout<<"No appointment is found for the selected time slot.";
    cout<<"\nPress enter key to continue...";
    getchar();
    getchar();
    return;
    }

char newchoice;
cout<<"Enter the new appointment slot(A-M): ";
cin>>newchoice;
newchoice =toupper(newchoice);  
    
if(newchoice<'A' || newchoice>'M') {
    cout<<"Invalid new appointment slot chosen.\n";
    cout<<"Please enter a valid time slot (A-M).\n";
    cout<<"Press enter key to continue...";
    getchar();
    getchar();
    return;
}

int newIndex =(newchoice - 65);
if(arr[newIndex] == 1) {
    cout<<"The new Time slot is already booked.";
    cout<<"\nPress enter key to continue...";
    getchar();
    getchar();
    return;
}

ofstream ofile("appointment_temp.txt");  
fstream infile("appointment.txt");

if(!ofile || !infile) {
    cout<<"Issue in updating appointment.";
    cout<<"\nPress enter key to continue...";
    getchar();
    getchar();
    return;
}

string line;
while(getline(infile,line)) {
    if (line[0]==choice) {
        ofile<<newchoice<<":"<<line.substr(2)<<" "<<endl;
        cout<<"Appointment updated successfully!";
    } 
else 
    ofile<<line<<"\n";
}

infile.close();
ofile.close();
remove("appointment.txt");
rename("appointment_temp.txt", "appointment.txt");
cout<<"\nPress enter key to continue...";
getchar();
getchar();
}

void DeleteAppointment() {
cout<<"----- Delete Appointment ----\n";
ifstream ifile;
ifile.open("appointment.txt");

int arr[13] ={0}; 
char key = 'A';
int hours = 9;

if (ifile) {
    string line;
    while (getline(ifile,line)) {
        char temp =line[0];
        int index =(temp - 65);
        arr[index] =1;
    }
    ifile.close();
}

for (int i=0;i<=12;i++) {
    DisplaySlot(key,hours,arr[i]);
    hours++;
    key++;
}

char choice;
cout<<"Enter the appointment slot(A-M) to delete: ";
cin>>choice;
choice = toupper(choice);  

int index =(choice - 65);
if(arr[index] == 0) {
    cout<<"No booked appointment slot found for deletion.";
    cout<<"\n Press enter key to continue...";
    getchar();
    getchar();
    return;
}

ofstream ofile("appointment_temp.txt");  
ifstream infile("appointment.txt");

if(!ofile || !infile) {
    cout<<"Issue occured while trying to delete appointment.";
    cout<<"\nPress enter key to continue...";
    getchar();
    getchar();
    return;
}

string line;
    while (getline(infile,line)) {
        if(line[0] != choice) {
        ofile<< line<< " "<<endl;
        } 
    else 
    cout<<"Appointment deleted successfully!";
    }

infile.close();
ofile.close();
remove("appointment.txt");
rename("appointment_temp.txt", "appointment.txt");

cout<<"\nPress enter key to continue...";
getchar();
getchar();

}

int main() {

while (1) {
    cout<<"\t\t\t\tDoctor Appointment System\n";
    cout<<"\t\t\t-----------------------------------------\n";
    cout<<"1. Book Appointment\n";
    cout<<"2. Check Existing Appointment\n";
    cout<<"3. Appointment Details\n";
    cout<<"4. Update your Appointment time\n";
    cout<<"5. Delete Appointment\n";
    cout<<"6. Exit\n";
    
    int choice;
    cout<<"Enter your choice: ";
    cin >>choice;

       switch (choice) {
    case 1: BookAppointment();
                break;
    case 2: ExistingAppointment();
                break;
    case 3: AppointmentDetails();
                break;
    case 4: UpdateAppointment();
                break;
    case 5: DeleteAppointment();
                break;
    case 6:
    while (1) {
    cout<<"Are you sure, you want to exit? (y/n) : ";
    char option;
    cin>>option;
    if(option=='y' || option=='Y')
    exit(0);
    else if(option=='n' || option =='N') {
    break;
    } else {
    cout<<"Invalid choice !";
    getchar();
      }
    }
    break;
    default:
    cin.clear();
    cin.ignore();
    cout<<"Invalid choice.";
    cout<<"\nPlease Press enter key to continue...";
    getchar();
    getchar();
   }
 }
return 0;
}
