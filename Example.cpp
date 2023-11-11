//Github Edited hehe

#include <iostream>
#include <string>

using namespace std;

class Bank;
class Account;


///////ATM Block////////////////////////////////////////////////////////////////////////////////////////////

class ATM {
private:
	static long long atm_count;
	const long long serial_number;
	const long long admin_card_number;
	const long long atm_type; // Single bank ATM = 0, Multi-bank ATM = 1
	const long long language_type; // Unilingual = 0, Bilingual = 1
	long long availabe_cash = 10000000;
	long long receive_check;
	Bank* bank_list[5];//construction emf 
	eo
	string file_name;
	long long Menu();
	void Admin();
	void Deposit();
	void Withdraw();
	void Transfer(); //Transper_in_Bank 활용
	void Cancle();// 동작 중간 중간에 cancle 누르면 cancle이 눌려서 종료된다는 말과 함께 session이 종료됨.
public:
	ATM(long long _language_type, long long _atm_type);
	void Start_ATM();
};


void ATM::Start_ATM() {
	// 카드 번호 받기
	long long _account_number;
	cin >> _account_number;

	if (language_type == 1) {
		//언어 선택 물어보고 바꾸기
		long long language;
		cin >> language;
	}

	if (_account_number == admin_card_number) {
		Admin();
		return;
	}

	while (1) {//사용자가 cancle button버튼 누르거나 exceptional situation일어나면 종료
		//메뉴창 띄우기
		long long var = Menu();

		if (var == 1) {
			// deposit
			Deposit();
		}

		if (var == 2) {
			// withdrawal
			Withdraw();
		}

		if (var == 3) {
			// transfer
			Transfer();
		}

		if (var == 4) {
			// Cancel
			Cancle();
		}
	}
}

ATM::ATM(long long  _language_type, long long  _atm_type)
	: serial_number{ atm_count + 100000 },
	language_type{ _language_type },
	atm_type{ _atm_type },
	admin_card_number{ atm_count + 100000 }
{
	atm_count++;
}





////////Bank Block///////////////////////////////////////////////////////////////////////////////////////////
class Bank {
private:
	const string bank_name;
	ATM* atm_list[5];
	Account* account_list[10];
public:
	Bank(string _bank_name);

	string Call_bank_name();

	bool Eixst_account(long long _account_num);
	bool Confirm_password(long long _account_num, long long _input_password);
	void Input_balance(long long _account_num, long long _fund);
	bool Output_balance(long long _account_num, long long _fund);
	long long Call_balance(long long _account_num);
	long long Transfer_in_Bank(long long _source_account, long long _destination_account);//Error num에 따라 에러가 생성되게 하기, 계좌이체만 Bank에서 총괄하는 방식

	void Add_account_list(Account* _account);
	void Add_atm_list///////////////////
};

Bank* bank_list[5];
int bank_list_size = 0;

Bank::Bank(string _bank_name) : bank_name(_bank_name) {
	bank_list[bank_list_size++] = this;


}

string Bank::Call_bank_name() {
	return bank_name;
}


////////Account Block///////////////////////////////////////////////////////////////////////////////////////////
class Account {
private:
	static long long account_count;
	const string user_name;
	const Bank* bank;
	const long long account_number;
	const long long password;
	long long balance;
public:
	Account(string _user_name, Bank* _bank, long long _password, long long _balance);
	
	string Call_user_name();
	const Bank* Call_bank();
	long long Call_account_number();
	long long Call_password();
	long long Call_balance();
};

long long atm_count = 0;
long long account_count = 0;


Account::Account(string _user_name, Bank* _bank, long long _password, long long _balance)
	: user_name{ _user_name },
	bank{ _bank },
	password{ _password },
	balance{ _balance },
	account_number{ 100000000000 + account_count }
{
	account_count++;
}

string Account::Call_user_name() {
	return user_name;
}
const Bank* Account::Call_bank() {
	return bank;
}
long long Account::Call_account_number() {
	return account_number;
}
long long Account::Call_password() {
	return password;
}
long long Account::Call_balance() {
	return balance;
}






///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	

	return 0;
}
