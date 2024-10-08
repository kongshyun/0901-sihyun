
#include "customer_manage.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>
#include <unistd.h>

using namespace std;

// NEW customerManager 클래스의 멤버 함수 getCustomerById를 호출해 고객 ID를 받아오는 함수 추가
Customer* customerManager::getCustomerById(int id) {
    for (auto& cust : customers) { // 고객 목록에서 해당 ID를 가진 고객을 찾음
        if (cust.getId() == id) {
            return &cust; // 포인터로 반환
        }
    }
    return nullptr; // 찾지 못하면 nullptr 반환
}

// CSV 파일에서 고객 데이터를 읽어와 customers 벡터에 추가하는 함수
void customerManager::loadFromCSV(const string& filename) {
    ifstream file(filename); // 파일 스트림 생성

    // 파일이 열리지 않으면 에러 메시지 출력
    if (!file.is_open()) {
        cerr << "파일을 열 수 없습니다: " << filename << endl;
        return;
    }

    string line;

    // 파일의 각 줄을 읽어 처리한다.
    while (getline(file, line)) {
        stringstream ss(line); // 한 줄을 stringstream으로 변환
        string idStr, name, phoneNumber, gender;

        // 각 필드를 콤마로 구분하여 읽기
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, phoneNumber, ',');
        getline(ss, gender, ',');

        // 각 필드가 유효한지 확인하고 객체 생성
        if (!idStr.empty() && !name.empty() && !phoneNumber.empty() && !gender.empty()) {
            int id = stoi(idStr); // ID를 정수로 변환
            customers.emplace_back(id, name, phoneNumber, gender);
        }
    }
    file.close(); // 파일 닫기
}


// 고객 ID를 나열하여 출력하는 함수
void customerManager::printCustomerList() const {
    cout << "\n현재 고객 ID 목록 | ";

    if (customers.empty()) {
        cout << "등록된 고객이 없습니다." << endl;
        return;
    }

    for (const auto& customer : customers) {
        cout << customer.id << " ";
    }

    cout << "\n\n"; // 모든 ID 출력 후 줄바꿈 추가
}

// 새로운 고객을 추가하는 함수 (간단한 입력을 통해 정보 입력)
void customerManager::addCustomer() {
    int id;
    string name, phoneNumber, gender;

    if (customers.size() >= 10) {
        cout << "더 이상 고객을 추가할 수 없습니다. 일부 고객 정보를 삭제하십시오. 메뉴로 돌아갑니다." << endl;
        sleep(1); // 잠시 대기 후 메뉴로 돌아감
        return;
    }

    //--------상단 안내 메시지---------
    cout << "\n[ 고객 정보를 입력하세요. (취소는 0 입력) ]\n" << endl;

    //--------ID 입력---------
    while (true) {
        cout << "고객 ID >> ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << " -- 유효한 숫자를 입력하십시오." << endl;
            continue;
        }

        cin.ignore();

        if (id == 0) return;

        if (id < 1 || id > 100) {
            cout << " -- ID는 1에서 100 사이의 숫자여야 합니다. 다시 시도하십시오." << endl;
            continue;
        }

        bool idExists = false;
        for (const auto& customer : customers) {
            if (customer.id == id) {
                idExists = true; break;
            }
        }

        if (idExists) {
            cout << " -- 이미 존재하는 ID입니다. 다시 시도하십시오." << endl;
            continue;
        } else {
            break; // ID가 중복되지 않으면 다음으로 이동
        }
    }

    //--------이름 입력---------
    while (true) {
        cout << "고객 이름 >> ";
		getline(cin, name);
        if (name == "0") return;

        if (name.empty()) {
            cout << " -- 이름을 입력하십시오." << endl;
        } else {
            break; // 이름이 입력되면 다음으로 이동
        }
    }

    //--------전화번호 입력-----
    while (true) {
        cout << "고객 전화번호 >> ";
        getline(cin, phoneNumber);

        if (phoneNumber.empty()) {
            cout << " -- 전화번호를 입력하십시오." << endl;
        } else if (phoneNumber == "0") return;
        else break; // 전화번호가 입력되면 다음으로 이동
    }

    //--------성별 입력--------
    while (true) {
        cout << "고객 성별(M/F) >> ";
        getline(cin, gender);

        if (gender.empty()) {
            cout << " -- 성별을 입력하십시오." << endl;
        } else if (gender == "0") return;
        else if (gender == "M" || gender == "F") {
            break; // 성별이 M 또는 F로 올바르게 입력되면 다음 단계로 이동
        } else {
            cout << " -- 잘못된 성별입니다. M 또는 F를 입력하십시오." << endl;
        }
    }

    customers.emplace_back(id, name, phoneNumber, gender);
    cout << "\033[38;5;48m";
	cout << "╔════════════════════════════╗" << endl;
	cout << "║  고객이 추가되었습니다!    ║" << endl;
	cout << "╚════════════════════════════╝" << endl;
	cout << "\033[0m";
	sleep(2);



    saveToCSV("customers.csv"); // 데이터 저장
}


// 고객을 삭제하는 함수(ID로 검색하여 삭제)
void customerManager::deleteCustomer() {
    int id;

    // 고객 목록 출력
    printCustomerList();
    cout << "삭제할 고객의 ID를 입력하세요 (모두 삭제하려면 -1 입력) >> ";
    cin >> id;

    if (id == -1) {
        customers.clear();
        cout << "모든 고객이 삭제되었습니다." << endl;
        saveToCSV("customers.csv");
        return;
    }

    for (auto it = customers.begin(); it != customers.end(); ++it) {
        if (it->id == id) {
            customers.erase(it);
            cout << "고객이 삭제되었습니다." << endl;
            saveToCSV("customers.csv");
            return;
        }
    }

    cout << "지정된 ID의 고객을 찾을 수 없습니다." << endl;

    saveToCSV("customers.csv"); // 데이터 저장
}

// 고객 정보를 수정하는 함수(ID로 검색하여 정보 수정)
void customerManager::modifyCustomer() {
    int id;

    // 고객 목록 출력
    printCustomerList();

    cout << "수정할 고객의 ID를 입력하세요 >> ";
    cin >> id;
    cin.ignore();

    for (auto it = customers.begin(); it != customers.end(); ++it) {
        if (it->id == id) {
            cout << "새 이름을 입력하세요 >> ";
            getline(cin, it->name);

            cout << "새 전화번호를 입력하세요 >> ";
            getline(cin, it->phoneNumber);

            cout << "새 성별을 입력하세요 >> ";
            getline(cin, it->gender);

            cout << "고객 정보가 업데이트되었습니다." << endl;
            saveToCSV("customers.csv");
            return;
        }
    }

    cout << "지정된 ID의 고객을 찾을 수 없습니다. 다시 시도하십시오." << endl;

    saveToCSV("customers.csv"); // 데이터 저장
}

// 고객 정보를 조회하는 함수(ID로 검색하여 정보 출력)
void customerManager::viewCustomer() const {
    int id;
    bool customerFound = false;
	// 고객 목록 출력
    printCustomerList();

    while (!customerFound) {
        cout << "조회할 고객의 ID를 입력하세요 (1~100) >> ";
        cin >> id;

        if (id == 0) {
            cout << "메인 메뉴로 돌아갑니다.\n" << endl;
            return;
        }

        for (auto it = customers.begin(); it != customers.end(); ++it) {
            if (it->id == id) {
                it->display();  // display() : 고객 정보를 출력하는 함수. customer.h에 정의됨
                cout << "\n3초 후 메뉴로 돌아갑니다.\n";
                sleep(3);
                customerFound = true;
                return;
            }
        }

        cout << "지정된 ID의 고객을 찾을 수 없습니다. 다시 시도하십시오.\n(종료하려면 0을 입력)" << endl;
    }
}

// 고객 데이터를 CSV 파일에 저장하는 함수
void customerManager::saveToCSV(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "파일을 열 수 없습니다: " << filename << endl;
        return;
    }
    for (const auto& customer : customers) {
        file << customer.id << ","
             << customer.name << ","
             << customer.phoneNumber << ","
             << customer.gender << endl;
    }
    file.close(); // 파일 닫기
}
