
# DocAppoint: Healthcare Scheduling System

DocAppoint is a healthcare scheduling system designed to streamline the process of booking and managing appointments with doctors. It allows for efficient handling of patient data, doctor schedules, and appointment bookings.



## Features


- **Add Doctors**: Add new doctors to the system with their contact details and schedule.
- **View Doctors**: View all doctors with their details and schedules.
- **Update Doctors**: Update the information of existing doctors.
- **Delete Doctors**: Remove doctors from the system.
- **Add Appointments**: Book appointments for patients with available doctors.
- **View Appointments**: View all scheduled appointments along with patient and doctor details.
- **Update Appointments**: Modify existing appointments.
- **Delete Appointments**: Remove appointments from the system.
- **View Patients**: View all registered patients with their details.
- **Delete Patients**: Remove patients from the system.


## Getting Started

### Prerequisites

- A C++ compiler (e.g., GCC)

### Installing

1. Clone the repository:
   ```sh
   git clone https://github.com/your-username/DocAppoint.git
   cd DocAppoint
2. Compile the code:
   ```sh
   g++ -o docappoint main.cpp
3. Run the executable:
   ```sh
   ./docappoint

## Usage

1. **Add a Doctor**: 
   - Before you can schedule any appointments, you must register doctors in the system.
   - Select the option `7. Add Doctor` from the menu.
   - Follow the prompts to enter the doctor's name, specialization, contact number, and available schedule.

2. **Book Appointments**: 
   - Once a doctor is registered, you can start scheduling appointments.
   - Select the option `1. Add Appointment` from the menu.
   - Enter the doctor ID, appointment date, and time.
   - Provide patient details including name, age, gender, contact number, and issue (optional).

3. **View Appointments**: 
   - To view all scheduled appointments, select the option `2. View Appointments` from the menu.
   - This will display a list of all appointments along with patient and doctor details.

4. **Update Appointments**: 
   - To modify an existing appointment, select the option `3. Update Appointment`.
   - Enter the appointment ID and follow the prompts to update the details.

5. **Delete Appointments**: 
   - To cancel an appointment, select the option `4. Delete Appointment`.
   - Enter the appointment ID to delete it from the system.

6. **View Patients**: 
   - To view all registered patients, select the option `5. View Patients`.
   - This will display a list of all patients with their details.

7. **Delete Patients**: 
   - To remove a patient from the system, select the option `6. Delete Patient`.
   - Enter the patient ID and name to confirm the deletion.

8. **View Doctors**: 
   - To view all registered doctors, select the option `8. View Doctors`.
   - This will display a list of all doctors with their details and schedules.

9. **Update Doctors**: 
   - To update a doctor's details, select the option `9. Update Doctor`.
   - Enter the doctor ID and follow the prompts to update the information.

10. **Delete Doctors**: 
   - To remove a doctor from the system, select the option `10. Delete Doctor`.
   - Enter the doctor ID to confirm the deletion.

11. **Exit**: 
   - To exit the program, select the option `0. Exit`.

Follow these steps to effectively manage doctors and appointments using the DocAppoint system.



## Data Persistence

- Patient data is stored in 'patients.txt'.
- Doctor data is stored in doctors.txt.
- Appointment data is stored in appointments.txt.
These files are loaded at the start of the program and saved upon exiting.
## Contributing

Contributions are always welcome!

1. Fork the repository.
2. Create your feature branch (`git checkout -b feature/your-feature`).
3. Commit your changes (`git commit -m 'Add some feature'`).
4. Push to the branch (`git push origin feature/your-feature`).
5. Open a pull request.
                              
## License

This project is licensed under the MIT License - see the LICENSE file for details.

