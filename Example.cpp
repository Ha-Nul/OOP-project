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
	int language; // English = 1, Korean = 2
	long long _account_number;
	long long availabe_cash = 10000000;
	long long receive_check;
	const Bank* primary_bank;
	Bank* bank_list[5];
	long long bank_list_size = 0;
	string file_name;
	long long Menu();
	void Admin();
	void Deposit();
	void Withdraw();
	void Transfer();
	void Cancle();
	bool Allowed_cash(long long cash);
	bool Allowed_check(long long check);
	bool Account_Exist(long long account_number);
	Bank* Call_Bank_of_account(long long account_number);
public:
	ATM(Bank* _primary_bank, long long _language_type, long long _atm_type);
	void Start_ATM();
	void Add_bank_list(Bank* _bank);
};

ATM::ATM(Bank* _primary_bank, long long  _language_type, long long  _atm_type)
	: serial_number{ atm_count + 100000 },
	language_type{ _language_type },
	atm_type{ _atm_type },
	admin_card_number{ atm_count + 100000 },
	primary_bank{ _primary_bank }
{
	atm_count++;
	_primary_bank->Add_atm_list(this);
	//Error message for preparing code demostration.
	if ((_language_type != 0)&&(_language_type != 1)){
		cout << "ERROR::Please input language_type either 0 or 1" << endl;
	}
	if((_atm_type!=0)&&(_atm_type!=1)){
		cout << "ERROR::Please input atm_type either 0 or 1" << endl;
	}
}

bool ATM::Allowed_cash(long long cash) {
	bool result = cash % 50000 % 10000 % 5000 % 1000 == 0;
	if (result == false) {
		if (language == 1) {
			cout << "The amount of cash is not allowed" << endl;
			cout << "Only the following types of cashes are allowed" << endl;
			cout << "KRW 1000, KRW 5000, KRW 10000, KRW 50000" << endl;
		}
		else {
			cout << "허용되지 않은 현금의 금액입니다" << endl;
			cout << "오직 다음과 같은 현금 종류만 허용됩니다" << endl;
			cout << "1000 원, 5000 원, 10000 원, 50000 원" << endl;
		}
	}
	return result;
}

bool ATM::Allowed_check(long long check) {
	bool result = check >= 100000;
	if (result == false) {
		if (language == 1) {
			cout << "The amount of check is not allowed" << endl;
			cout << "Any amout over KRW 100000 check is allowed" << endl;
		}
		else {
			cout << "허용되지 않은 수표의 금액입니다" << endl;
			cout << "100000 원 이상의 수표만 허용됩니다" << endl;
		}
	}
	return result;
}

bool ATM::Account_Exist(long long account_number) {
	for (int i = 0; i < bank_list_size; i++) {
		Bank* bank = bank_list[i];
		if (bank->Exist_account(account_number)) {
			return true;
		}
	}
	return false;
}

Bank* ATM::Call_Bank_of_account(long long account_number) {
	Bank* bank;
	for (int i = 0; i < bank_list_size; i++) {
		Bank* bank = bank_list[i];
		if (bank->Exist_account(account_number)) {
			return bank;
		}
	}
	return nullptr;
}

void ATM::Start_ATM() {
	/*=================== Enter the account number ===================*/
	cout << "Please enter your card(account) number" << endl;
	cin >> _account_number;
	/*================================================================*/

	/*==================== Select the Language ====================*/
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

		// Deposit
		if (var == 1) {
			Deposit();
		}

		// Withdrawal
		if (var == 2) {
			
			Withdraw();
		}

		// Transfer
		if (var == 3) {
			Transfer();
		}
		// Cancel
		if (var == 4) {
			Cancle();
		}
	}
}

	long long ATM::Menu() {

	}
	void ATM::Admin() {

	}
	void ATM::Deposit() {

	}
	void ATM::Withdraw() {

	}

	void ATM::Transfer() {
		/*===================== Set the Source Bank =====================*/
		Bank* source_bank = Call_Bank_of_account(_account_number);
		/*===============================================================*/

		/*==================== Select the Transfer Type ====================*/
		int transfer_type; // Cash transfer = 1, Account transfer = 2
		while (1) {
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
			if (transfer_type == 1) {
				if (language == 1) {
					cout << "Cash transfer was selected" << endl;
				}
				else {
					cout << "현금 이체가 선택되었습니다" << endl;
				}
				break;
			}
			else if (transfer_type == 2) {
				
				if (language == 1) {
					cout << "Account transfer is selected" << endl;
					cout << "The withdrawal account is " << source_bank->Call_bank_name() << " " << _account_number << endl;
				}
				else {
					cout << "계좌 이체가 선택되었습니다" << endl;
					cout << "출금 계좌는 " << source_bank->Call_bank_name() << " " << _account_number << " 입니다" << endl;
				}
				break;
			}
			else {
				if (language == 1) {
					cout << "Please enter the number 1 or 2" << endl;
				}
				else {
					cout << "숫자 1 또는 2를 입력해주세요" << endl;
				}
				continue;
			}
		}
		/*==================================================================*/

		/*============= Enter the Destination Account Number ==============*/
		long long  destination_account_number;
		while (1) {
			if (language == 1) {
				cout << "Please enter the destination account number" << endl;
			}
			else {
				cout << "입금 계좌 번호를 입력해주세요" << endl;
			}
			cin >> destination_account_number;
			if (Account_Exist(destination_account_number)) {
				if (language == 1) {
					cout << "The destination account number is " << destination_account_number << endl;
				}
				else {
					cout << "입금 계좌 번호는 " << destination_account_number << " 입니다" << endl;
				}
				break;
			}
			else {
				if (language == 1) {
					cout << "The destination account number is not exist" << endl;
				}
				else {
					cout << "입금 계좌 번호가 존재하지 않습니다" << endl;
				}
				continue;
			}
		}
		Bank* destination_bank = Call_Bank_of_account(destination_account_number);
		/*=================================================================*/

		/*============================ Transfer ============================*/
		// if Cash transfer
		if (transfer_type == 1) {

			/*=================== Recieve Cash Transfer Payment ===================*/
			long long pay;
			long long cash_transfer_fee = 5000;
			while (1) {
				if (language == 1) {
					cout << "Please pay the amount that you want to cash transfer plus cash transfer fee KRW " << cash_transfer_fee << endl;
					cout << "Please enter the amount of cash that you want to pay" << endl;
				}
				else {
					cout << "현금 이체하실 금액에 현금 이체 수수료 " << cash_transfer_fee << " 원을 더하여 지불해주세요" << endl;
					cout << "지불하실 현금의 금액를 입력해주세요" << endl;
				}
				cin >> pay;
				if (Allowed_cash(pay) == false) {
					continue;
				}
				else if (pay <= cash_transfer_fee) {
					if (language == 1) {
						cout << "Your payment is less than or equal to the cash transfer fee" << endl;
					}
					else {
						cout << "지불하신 금액이 현금 이체 수수료보다 작거나 같습니다" << endl;
					}
					continue;
				}
				else {
					if (language == 1) {
						cout << "The total amount that you paid is KRW " << pay << endl;
						cout << "The amount of cash transfer: KRW " << pay - cash_transfer_fee << endl;
						cout << "Cash transfer fee: KRW " << cash_transfer_fee << endl;
					}
					else {
						cout << "지불하신 총 금액은 " << pay << " 원입니다" << endl;
						cout << "현금 이체 금액: " << pay << " 원" << endl;
						cout << "현금 이체 수수료: " << cash_transfer_fee << " 원" << endl;
					}
					availabe_cash += pay;
					break;
				}
			}
			/*=====================================================================*/

			/*============== Increase Balance of Destination Account ==============*/
			destination_bank->Input_balance(destination_account_number, pay - cash_transfer_fee);
			/*=====================================================================*/

			/*============================ End Session ============================*/
			if (language == 1) {
				cout << "The cash transfer of KRW " << pay - cash_transfer_fee << " to " << destination_bank->Call_bank_name() << " " << destination_account_number << " was performed successfully" << endl;
			}
			else {
				cout << pay - cash_transfer_fee << " 원이 " << destination_bank->Call_bank_name() << " " << destination_account_number << " 으로 현금 이체 되었습니다" << endl;
			}
			return;
			/*=====================================================================*/
		}

		// if Account transfer
		if (transfer_type == 2) {

			/*=================== Determine Account Transfer Fee ===================*/
			long long account_transfer_fee;
			if (source_bank == primary_bank) {
				if (destination_bank == primary_bank) {
					account_transfer_fee = 2000;
				}
				else {
					account_transfer_fee = 3000;
				}
			}
			if (source_bank != primary_bank) {
				if (destination_bank == primary_bank) {
					account_transfer_fee = 3000;
				}
				else {
					account_transfer_fee = 4000;
				}
			}
			/*======================================================================*/

			/*================== Enter the Amount of Account Transfer ==================*/
			long long amount_transfer;
			while (1) {
				if (language == 1) {
					cout << "The balance of source account: KRW " << source_bank->Call_balance(_account_number) << endl;
					cout << "In the case of account transfer, the account transfer fee KRW " << account_transfer_fee << " will be paid from the source account" << endl;
					cout << "Please enter the amount of transfer" << endl;
				}
				else {
					cout << "출금 계좌의 잔액: " << source_bank->Call_balance(_account_number) << " 원" << endl;
					cout << "계좌 이체의 경우 계좌 이체 수수료 " << account_transfer_fee << " 원이 출금 계좌에서 추가로 지불됩니다" << endl;
					cout << "송금하실 금액을 입력해주세요" << endl;
				}
				cin >> amount_transfer;
				if (source_bank->Call_balance(_account_number) < amount_transfer + account_transfer_fee) {
					if (language == 1) {
						cout << "The balance of source account is not sufficient" << endl;
					}
					else {
						cout << "출금 계좌의 잔액이 부족합니다" << endl;
					}
					continue;
				}
				else {
					if (language == 1) {
						cout << "The amount of account transfer: KRW " << amount_transfer << endl;
						cout << "Account transfer fee: KRW " << account_transfer_fee << endl;
					}
					else {
						cout << "송금 금액: " << amount_transfer << " 원" << endl;
						cout << "계좌 이체 수수료: " << account_transfer_fee << " 원" << endl;
					}
					break;
				}
			}
			/*==========================================================================*/

			/*=================== Decrease and Increase Balance ===================*/
			source_bank->Output_balance(_account_number, amount_transfer + account_transfer_fee);
			destination_bank->Input_balance(destination_account_number, amount_transfer);
			/*=====================================================================*/

			/*============================ End Session ============================*/
			if (language == 1) {
				cout << "The account transfer of KRW " << amount_transfer << " from " << source_bank->Call_bank_name() << " " << _account_number << " to " << destination_bank->Call_bank_name() << " " << destination_account_number << " was performed successfully" << endl;
			}
			else {
				cout << amount_transfer << " 원이 " << source_bank->Call_bank_name() << " " << _account_number << " 에서 " << destination_bank->Call_bank_name() << " " << destination_account_number << " 으로 계좌 이체 되었습니다" << endl;
			}
			return;
			/*=====================================================================*/

		}
		/*==================================================================*/
	}

	void ATM::Cancle() {

	}

void ATM::Add_bank_list(Bank* _bank){
	//Error message for preparing code demostration.
	if ((atm_type==0)&&(bank_list_size >= 1)){
		cout << "ERROR:This is SingleBank ATM. Please do not add more Bank. The adding Bank is ignored" << endl;
		return;
	}
	
	bank_list[bank_list_size++] = _bank;
	_bank->Add_atm_list(this);
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
	Account* Find_corresponding_account(long long _account_num);//function name is same as the explanation

	void Add_account_list(Account* _account);//This function is executed by Account constructor.
	void Add_atm_list(ATM* _atm);//This function is executed by  ATM::Add_bank_list function.

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
	_bank->Add_account_list(this);
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
	Bank* HN_bank =new Bank{"HN은행"};
	Bank* JS_bank = new Bank{"JS은행"};
	Bank* HG_bank = new Bank{"HG은행"};
	Bank* SH_bank = new Bank{"SH은행"};

	Account* account_1 = new Account("석하늘", HN_bank ,202321012, 1000000);
	Account* account_2 = new Account("김재석", JS_bank ,202111036, 2000000);
	Account* account_3 = new Account("김재석", JS_bank ,202111036, 0);//Second account at same bank
	Account* account_4 = new Account("성현규", HG_bank ,202111095, 3000000);
	Account* account_5 = new Account("조수호", SH_bank ,202111178, 4000000);


	ATM* atm_SingUnil = new ATM(HN_bank, 0, 0);
	ATM* atm_SingBili = new ATM(JS_bank, 0, 1);
	ATM* atm_MultUnil = new ATM(HG_bank, 1, 0);
	ATM* atm_MultBili = new ATM(SH_bank, 1, 1);

	atm_MultUnil->Add_bank_list(SH_bank);
	atm_MultBili->Add_bank_list(HG_bank);


	//Execute an atm what you want
	atm_MultBili->Start_ATM();


	return 0;
}
