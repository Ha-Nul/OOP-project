#include <iostream>
#include <string>

using namespace std;

class Bank;
class Account;


/*-------------------------------------ATM region-------------------------------------------------------------------------------*/

class ATM {
private:
	static long long atm_count;
	const long long serial_number;
	const long long admin_card_number;
	const long long atm_type; // Single bank ATM = 0, Multi-bank ATM = 1
	const long long language_type; // Unilingual = 0, Bilingual = 1
	long long availabe_cash = 10000000;
	long long receive_check;
	Bank* bank_list[5]; long long bank_list_size = 0;
	string file_name;
	long long Menu();
	void Admin();
	void Deposit();
	void Withdraw();
	void Transfer();
	void Cancle();
public:
	ATM(long long _language_type, long long _atm_type);
	void Start_ATM();
	void Add_bank_list(Bank* _bank);
};

long long ATM::Menu(){

}
void ATM::Admin(){

}
void ATM::Deposit(){

}
void ATM::Withdraw(){

}
void ATM::Transfer(){

}
void ATM::Cancle(){

}

ATM::ATM(long long  _language_type, long long  _atm_type)
	: serial_number{ atm_count + 100000 },
	language_type{ _language_type },
	atm_type{ _atm_type },
	admin_card_number{ atm_count + 100000 }
{
	atm_count++;
}

void ATM::Transfer() {
	
}

void ATM::Start_ATM() {
	/*=================== Enter the account number ===================*/
	long long _account_number;
	cout << "Please enter your card(account) number" << endl;
	cin >> _account_number;
	/*================================================================*/

	/*==================== Select the Language ====================*/
	long long language; // English = 1, Korean = 2
	// if ATM is bilingual
	if (language_type == 1) {
		cout << "Please select the language" << endl;
		cout << "1. English" << endl;
		cout << "2. 한국어" << endl;
		cin >> language;
	}
	// if ATM is unilingual
	else {
		language = 1; // Default is English
	}
	/*=============================================================*/

	if (_account_number == admin_card_number) {
		Admin();
		return;
	}

	while (1) {
		long long var = Menu();

		if (var == 1) {
			// deposit
			Deposit();
		}

		if (var == 2) {
			// withdrawal
			Withdraw();
		}

		/*=============================== Transfer ===============================*/
		if (var == 3) {
			/*==================== Select the Transfer Type ====================*/
			int transfer_type; // Cash transfer = 1, Account transfer = 2
			if (language == 1) {
				cout << "Please select the transfer type" << endl;
				cout << "1. Cash transfer" << endl;
				cout << "2. Account transfer" << endl;
			}
			else {
				cout << "이체 형식을 선택해주세요" << endl;
				cout << "1. 현금 이체" << endl;
				cout << "2. 계좌 이체" << endl;
			}
			cin >> transfer_type;
			/*==================================================================*/

			/*============= Enter the Destination Account Number ==============*/
			long long  destination_account_number;
			if (language == 1) {
				cout << "Please enter the destination account number" << endl;
			}
			else {
				cout << "도착 계좌 번호를 입력해주세요" << endl;
			}
			cin >> destination_account_number;
			/*=================================================================*/

			/*============================ Transfer ============================*/
			// if Cash transfer
			if (transfer_type == 1) {

			}
			// if Account transfer
			if (transfer_type == 2) {

			}
			/*==================================================================*/
		}
		/*========================================================================*/

		if (var == 4) {
			// Cancel
			Cancle();
		}
	}
}
void ATM::Add_bank_list(Bank* _bank){
	bank_list[bank_list_size++] = _bank;
}





/*-------------------------------------Bank region-------------------------------------------------------------------------------*/
class Bank {
private:
	const string bank_name;
	ATM* atm_list[5]; long long atm_list_size = 0;
	Account* account_list[10]; long long account_list_size = 0;

public:
	Bank(string _bank_name);

	string Call_bank_name();
	long long Call_balance(long long _account_num); // Use only after executing Exist account and Confirm_password functions. Use _account_num variable we used in Exist_account

	bool Exist_account(long long _account_num);
	bool Confirm_password(long long _account_num, long long _input_password);// Use only after executing Exist account function. Use _account_num variable we used in Exist_account
	void Input_balance(long long _account_num, long long _fund);//we should use at Deposit and Tranfer_in_Bank function
	bool Output_balance(long long _account_num, long long _fund);//we should use at Withdraw and Tranfer_in_Bank function. retun false when _fund is larger than balance.
	long long Transfer_in_Bank(long long _source_account, long long _destination_account);
	Account* Find_corresponding_account(long long _account_num);//function name is same as the explanation

	void Add_account_list(Account* _account);
	void Add_atm_list(ATM* _atm);

}; Bank* bank_list[5]; int bank_list_size = 0; //Global variables

Bank::Bank(string _bank_name) : bank_name(_bank_name) {
	bank_list[bank_list_size++] = this;
}


string Bank::Call_bank_name() {
	return bank_name;
}
long long Bank::Call_balance(long long _account_num){
	return Find_corresponding_account(_account_num)->Call_balance();
}


bool Bank::Exist_account(long long _account_num){
	for (int i = 0 ; i < account_list_size ; i++){
		if (account_list[i]->Call_account_number() == _account_num ){
			return true;
		}}
	return false;
}
bool Bank::Confirm_password(long long _account_num, long long _input_password){
	if (Find_corresponding_account(_account_num)->Call_password() == _input_password){
		return true;
	}
	else{
		return false;
	}
}
void Bank::Input_balance(long long _account_num, long long _fund){
	Find_corresponding_account(_account_num)->balance += _fund;
}
bool Bank::Output_balance(long long _account_num, long long _fund){
	Account* curr_acc = Find_corresponding_account(_account_num);
	if(curr_acc->Call_balance() < _fund ){
		return false;
	}
	else{
		curr_acc->balance -= _fund;
		return true;
	}
}
long long Bank::Transfer_in_Bank(long long _source_account, long long _destination_account){
	//implement it here
}
Account* Bank::Find_corresponding_account(long long _account_num){
	for (int i = 0 ; i < account_list_size ; i++){
		if (account_list[i]->Call_account_number() == _account_num ){//Assume that there must exist corresponding account
			return account_list[i];
		}}
}


void Bank::Add_account_list(Account* _account){
	account_list[account_list_size++] = _account;
}
void Bank::Add_atm_list(ATM* _atm){
	atm_list[atm_list_size++] = _atm;
}


/*-------------------------------------Account region-------------------------------------------------------------------------------*/
class Account {
private:
	static long long account_count;
	const string user_name;
	const Bank* bank;
	const long long account_number;
	const long long password;
	long long balance; //control only at Constructor, Input_balance and output_balance functions.
	friend void Bank::Input_balance(long long _account_num, long long _fund); //set freind function. control balance at this function of Bank class
	friend bool Bank::Output_balance(long long _account_num, long long _fund); //set freind function. control balance at this function of Bank class
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






/*-------------------------------------Main region-------------------------------------------------------------------------------*/

int main() {
	

	return 0;
}
