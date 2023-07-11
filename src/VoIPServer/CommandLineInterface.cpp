#include <iostream>
#include <thread>

#include "CommandLineInterface.h"
#include "session/SessionControl.h"
#include "../json/json.h"

CommandLineInterface* CommandLineInterface::instance = nullptr;


CommandLineInterface* CommandLineInterface::getInstance() {
	if (instance == nullptr) {
		instance = new CommandLineInterface();
	}
	return instance;
}

void CommandLineInterface::startCli()
{
	ContactDb* contactDb = ContactDb::getInstance();
	ConferenceDb* conferenceDb = ConferenceDb::getInstance();
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	while (true) {
		string input;
		std::cout << "======================================================" << std::endl;
		std::cout << " AONE VOIP Server" << std::endl;
		std::cout << "======================================================" << std::endl;
		std::cout << " 1. SHOW ALL CONTACTS" << std::endl;
		std::cout << " 2. SHOW ALL CONFERENCES" << std::endl;
		std::cout << " 3. ENABLE CONTACT" << std::endl;
		std::cout << " 4. DISABLE CONTACT" << std::endl;
		std::cout << " 5. DELETE CONTACT" << std::endl;
		std::cout << "======================================================" << std::endl;
		std::cout << "Choose (Q to Exit): " ;
		std::getline(std::cin, input);
		int selected = 0;
		try {
			selected = stoi(input);
			if (selected == 0) {
				continue;
			}		
		}
		catch (std::exception ex) {
			// Not number
			if (input == "Q") {
				break;
			}
			continue;
		}
		switch (selected) {
		case 1: //SHOW ALL CONTACTS
			std::cout << contactDb->get() << endl;
			break;

		case 2: //SHOW ALL CONFERENCES
			std::cout << conferenceDb->get() << endl;
			break;

		case 3: //ENABLE CONTACT
			std::cout << "ENABLE CONTACT" << std::endl;
			std::cout << "cid : ";
			std::getline(std::cin >> std::ws, input);
			contactDb->setEnable(input, true);
			break;

		case 4: //DISABLE CONTACT
			std::cout << "DISABLE CONTACT" << std::endl;
			std::cout << "cid : ";
			std::getline(std::cin >> std::ws, input);
			contactDb->setEnable(input, false);
			break;
		case 5: // DELETE CONTACT
			std::cout << "DELETE CONTACT" << std::endl;
			std::cout << "cid : ";
			std::getline(std::cin >> std::ws, input);
			contactDb->remove(input);
			break;
		default:			
			break;
		}		
	}
	std::cout << "CLI Terminated " << std::endl;
}
