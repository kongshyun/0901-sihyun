#include "meal_manage.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unistd.h>

void mealmanage::loadFromCSV(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "파일을 열 수 없습니다: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) { // getline으로 토큰 단위로 읽어들임
        stringstream ss(line);
        string idStr, name, food, totalCaloriesStr;

        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, food, ',');
        getline(ss, totalCaloriesStr, ',');

        if (!idStr.empty() && !name.empty() && !food.empty() && !totalCaloriesStr.empty()) {
            int id = stoi(idStr);
            int totalCalories = stoi(totalCaloriesStr);
            meals.emplace_back(id, name, food, totalCalories);
        }
    }

    file.close();
}

void mealmanage::saveToCSV(const string& filename) { // 변경 내용 저장
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "파일을 열 수 없습니다: " << filename << endl;
        return;
    }

    for (const auto& meal : meals) {
        file << meal.id << "," << meal.name << "," << meal.food << "," << meal.totalCalories << endl;
    }

    file.close();
}

// void mealmanage::addMeal(const Customer& cust, const string& food, int totalCalories) {
//     // customer의 id와 name을 이용하여 meal 생성
//     meals.emplace_back(cust.id, cust.name, food, totalCalories);
// }

void mealmanage::addMeal(const Customer& cust, const string& food, int totalCalories) {
    // 같은 ID를 가진 비어있는 음식 정보를 찾음
    auto it = find_if(meals.begin(), meals.end(),
                      [&cust](const meal& m) {
                          return m.id == cust.id && m.food.empty() && m.totalCalories == 0;
                      });
    
    if (it != meals.end()) {
        // 비어있는 음식 정보를 찾았다면, 그 자리에 새 정보를 덮어씀
        it->food = food;
        it->totalCalories = totalCalories;
        cout << "ID " << cust.id << "번 고객님 식단 정보를 업데이트함." << endl;
    } else {
        // 비어있는 음식 정보가 없다면, 새로운 meal 객체를 추가
        meals.emplace_back(cust.id, cust.name, food, totalCalories);
        cout << "ID " << cust.id << "번 고객님 새로운 식단 정보를 추가함." << endl;
    }

    saveToCSV("meal.csv");
    sleep(2);
}

void mealmanage::modifyMeal(int id, const string& food, int totalCalories) {
    for (auto& m : meals) {
        if (m.id == id) {
            m.food = food;               // food 수정
            m.totalCalories = totalCalories; // totalCalories 수정
            cout << "수정 완료 되었습니다."<< endl;
            sleep(2);
            return;
        }
    }
    //sleep(2);
    //cout << "No meal found with ID: " << id << endl;
    saveToCSV("meal.csv");
}

// void mealmanage::deleteMeal(int id) { // 식사관리 삭제 기능
//     meals.erase(remove_if(meals.begin(), meals.end(), [id](const meal& m) { return m.id == id; }), meals.end());
// }

void mealmanage::deleteMeal(int id, const string& food) {

    auto it = find_if(meals.begin(), meals.end(),
                      [id, &food](const meal& m) {
                          return m.id == id && m.food == food;
                      });

    if (it != meals.end()) {
        it->food.clear();  // 운동 유형을 빈 문자열로 초기화
        it->totalCalories = 0;  // 운동 시간을 0으로 초기화

        cout << "ID " << id << " 번 고객님의 '" << food << "' 식단 정보가 삭제." << endl;
        saveToCSV("exercises.csv");
        sleep(2);
    } else {
        cout << "ID " << id << " 번 고객님의 '" << food << "' 식단 정보를 찾음." << endl;
    }
}


// void mealmanage::displayMeals() const { // csv에 저장되어있는 값 출력
//     for (const auto& meal : meals) {
//         meal.display();
//     }
// }

void mealmanage::displayMeals() const {
    bool hasExercises = false;
    for (const auto& meal : meals) {
        if (!meal.food.empty() && meal.totalCalories > 0) {
            cout << "ID: " << meal.id << "| 이름: " << meal.name 
                 << "| 음식 유형(?): " << meal.food 
                 << "| 음식 칼로리: " << meal.totalCalories << " Kcal" << endl;
            hasExercises = true;
        }
    }
    if (!hasExercises) {
        cout << "표시할 식단 기록이 없음." << endl;
    } else {
        cout << "\n총 " << count_if(meals.begin(), meals.end(), 
                                 [](const meal& m) { return !m.food.empty() && m.totalCalories > 0; })
             << "개의 식단 기록이 표시되었음." << endl;
    }
}

void mealmanage::displayAllCustomers() const {
    // 고객 정보가 있는지 확인
    if (meals.empty()) {
        cout << "저장된 고객 정보가 없습니다." << endl;
        return;
    }

    // 고객 목록 출력
    cout << "현재 저장된 고객 목록:" << endl;
    for (const auto& meal : meals) {
        cout << "ID: " << meal.id << "| 이름: " << meal.name 
        <<  "| 음식 유형: " << meal.food << " | 음식 칼로리: " << meal.totalCalories << " Kcal" << "\n\n";
    }
}
